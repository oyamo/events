/*Mysql only*/

-- == who wrote is vic ==--
drop schema if exists events;
create schema events;

use events;
set block_encryption_mode = 'aes-256-cbc';

-- =======USERS=================================================================

drop table if exists users;
create table users(
    id binary(16) UNIQUE,
    name varchar(100) NOT NULL,
    username varchar(100) NOT NULL UNIQUE,
    pass_word blob NOT NULL,
    salt binary(16),
    email varchar(100) NOT NULL UNIQUE,
    avater varchar(100) NULL,
    bio varchar(1000) NULL,
    about varchar(50) NULL,
    date_created timestamp default now(),
    date_modified timestamp default now() on update now(),
    

    constraint
		users_pk primary key(id)
);

/*=========PUBLISHED EVENTS================================*/

drop table if exists published;
create table published(
    id binary(16) UNIQUE,
    title varchar(200) NOT NULL,
    description varchar(1000) NOT NULL,
    venue varchar(50) NOT NULL,
    publisher_id binary(16)  NOT NULL,
    stars int unsigned default 0,
	date_created timestamp default now(),
    date_modified timestamp default now() on update now(),

    constraint 
		published_pk primary key(id),
    constraint 
		publisher_fk foreign key(publisher_id) references users(id)
);

/*===========STARS===============================*/



drop table if exists stars;
create table stars
(
    id binary(16)  UNIQUE,
    user_id binary(16) ,
    published_id binary(16) ,
	date_created timestamp,

    constraint 
		stars_pk primary key(id),
        
    constraint 
		stars_user_fk foreign key(user_id) references users(id)
    on delete cascade,
    
    constraint 
		stars_events_fk foreign key(published_id) references published(id)
    on delete cascade
);

-- =========PAYMENTS========================================================================

drop table if exists payments;
create table payments
(
    id  binary(16)  UNIQUE,
    user_id binary(16) ,
    published_id binary(16) ,
	date_created timestamp,

    constraint 
		payments_pk primary key(id),
        
    constraint 
		pay_user_fk foreign key(user_id) references users(id)
    on delete cascade,
    
    constraint
		pay_events_fk foreign key(published_id) references published(id)
    on delete cascade
);

-- ==============FOLLOWERS=============================================================

drop table if exists followers;
create table followers
(
    id binary(16) unique,
    user_id binary(16) NOT NULL,
    follower_id binary(16) NOT NULL,

    constraint 
		followers_pk primary key(id),
        
    constraint 
		follow_user_fk foreign key(user_id) references users(id)
    on delete cascade,
    
    constraint 
		follow_follower_fk foreign key(follower_id) references users(id)
    on delete cascade
);

-- =========SUBSCRIPTIONS==================================================================

drop table if exists subscriptions;
create table subscriptions
(
    id binary(16) UNIQUE,
    user_id binary(16)  not null,
    published_id binary(16) not null,
    paid bool default false,

    constraint 
		subscriptions_pk primary key(id),
        
    constraint 
		sub_user_fk foreign key(user_id) references users(id)
    on delete cascade,
    
    constraint 
		sub_events_fk foreign key(published_id) references published(id)
    on delete cascade
);

-- ===========================================================================
-- ===============procedures==================================================
-- ===========================================================================


-- ===========password enrypt ===========
delimiter #
drop function if exists encrypt_password #
create function encrypt_password ( in_password varchar(50), salt binary(16) ) returns blob
deterministic
begin
	set block_encryption_mode = 'aes-256-cbc';
    return aes_encrypt(in_password,sha2('key',512),salt);
end #
delimiter ;

-- select cast(encrypt_password('hello') as char);

-- select random_bytes(16);

-- 0773753482

-- select (aes_encrypt('yoh','key',random_bytes(16)))

-- =========password decrypt =========== ---

delimiter #
drop function if exists decrypt_password #
create function decrypt_password ( in_password blob, salt binary(16) ) returns varchar(50)
deterministic
begin
	set block_encryption_mode = 'aes-256-cbc';
	return aes_decrypt(in_password,sha2('key',512),salt);
end #
delimiter ;

-- ========== validate user ============ ---

delimiter #
drop procedure if exists validate_user #
create procedure validate_user (
	in_password varchar(50),
    in_username varchar(50),
    in_email varchar(50)
)

begin
	declare pass blob;
    declare v_salt binary(16);
    declare nf int default 0;
    
   declare continue handler for not found
		set nf = 1;
    
    if isnull(in_email) then
			select
				pass_word,salt
			into pass,v_salt
           -- email
			from users
			where username = in_username;
	else
			select
				pass_word,salt
			into pass,v_salt
            from users
            where email = in_email;
	end if;
	
		
    if nf = 0 then
		if decrypt_password(pass,v_salt) = in_password then
			select 1 ;
		else
			select 0 ;
		end if;
	else 
		select -1;
	end if;
end #
delimiter ;

-- =============== add user id ========== ---

delimiter #
drop trigger if exists add_users_id #
create trigger add_users_id
before insert on users
for each row
begin
	declare p_salt binary(16) default random_bytes(16);
    set new.id = uuid_to_bin(uuid());
    set new.pass_word = encrypt_password(new.pass_word,p_salt);
    set new.salt = p_salt;
end #
delimiter ;

-- ========= add published id ===== ---

delimiter #
drop trigger if exists add_published_id #
create trigger add_published_id
before insert on published
for each row
begin
    set new.id = uuid_to_bin(uuid());
end #
delimiter ;


delimiter #
drop trigger if exists add_star_id #
create trigger add_star_id
before insert on stars
for each row
begin
    set new.id = uuid_to_bin(uuid());
end #
delimiter ; 


delimiter #
drop trigger if exists add_follower_id #
create trigger add_follower_id
before insert on followers
for each row
begin
    set new.id = uuid_to_bin(uuid());
end #
delimiter ; 


delimiter #
drop trigger if exists add_payment_id #
create trigger add_payment_id
before insert on payments
for each row
begin
    set new.id = uuid_to_bin(uuid());
end #
delimiter ; 


select cast(bin_to_uuid(id) as char) into @muuid from users where username = 'my';

-- ==========get one user by id=========== --

delimiter #
drop procedure if exists get_user #
create procedure get_user(
	in_id varchar(256)
)
begin
	declare this_id binary(16) default uuid_to_bin(in_id);
	select
    cast( bin_to_uuid(id) as char) as id,
    name,
    username,
    email,
    avater,
    about,
    bio,
    date_created as join_date,
    (select count(*) from published where publisher_id = this_id) as published,
	(select count(*) from followers where  follower_id = this_id) as followers,
    (select count(*) from followers where  user_id = this_id) as followed,
    (select count(*) from stars where  user_id = this_id) as starred,
    ifnull((select sum(stars) from published where  publisher_id = this_id),0) as stars_earned
    from users
    where id = this_id;
end #
delimiter ;



delimiter #
drop trigger if exists add_stars #
create trigger add_stars
before insert on stars
for each row
begin
	update published p
    set stars = p.stars + 1
    where p.id = new.published_id;
end #
delimiter ;


delimiter #
drop trigger if exists sub_stars #
create trigger sub_stars
before delete on stars
for each row
begin
	update published p
    set stars = p.stars - 1
    where p.id = old.published_id;
end #
delimiter ;


delimiter #
drop procedure if exists get_one_published #
create procedure get_one_published(
	in_id varchar(256)
)
begin
	declare this_id binary(16) default uuid_to_bin(in_id);
    select 
		cast(bin_to_uuid(id) as char) as id,
        title,
        description,
        venue,
        cast(bin_to_uuid(publisher_id) as char) as id,
        stars,
        date_created,
        (select count(*) from subscriptions where id = this_id) as subscriptions
	from published;
end #
delimiter ;


call get_user(@muuid);

select * from published;

insert into users 
(
	name,username,email,pass_word
)
values
(
	'my','my','myemail2','my passwrd'
);

insert into published
(
	title,description,venue,publisher_id
)
values
('title1','description1','venue1',uuid_to_bin(@muuid));

select cast(bin_to_uuid(id) as char) into @muuid2 from published where title = 'title1';

call get_one_published(uuid());

insert into stars
(
	user_id,published_id
)
values
(uuid_to_bin(@muuid),uuid_to_bin(@muuid2));

/*

insert into users 
(
	name,username,email,password
)
values
(
	'my name','my username','myemail,'my passwrd'
);


insert into published
(
	title,description,venue,publisher
)
values
(
	'title','description',venue',
);


insert into subscriptions
(
	user_id,published_id
)
values
(
	''0','0'
);


insert into stars
(
	user_id,published_id
)
values
(
	''0','0'
);


insert into payments
(
	user_id,published_id
)
values
(
	''0','0'
);


insert into followers
(
	user_id,published_id
)
values
(
	''0','0'
);

==========================================================
insert into users 
(
	name,username,email,password
)
values
('vic','myuser1','myemail1@gmail.com','my passwrd'),
('vic','myuser2','myemail2@gmail.com','my passwrd'),
('vic','myuser3','myemail3@gmail.com','my passwrd'),
('vic','myuser4','myemail4@gmail.com','my passwrd'),
('vic','myuser5','myemail5@gmail.com','my passwrd'),
('vic','myuser6','myemail6@gmail.com','my passwrd'),
('vic','myuser7','myemail7@gmail.com','my passwrd'),
('vic','myuser8','myemail8@gmail.com','my passwrd'),
('vic','myuser9','myemail9@gmail.com','my passwrd'),
('vic','myuser10','myemail10@gmail.com','my passwrd')
;

===========================================================

insert into published
(
	title,description,venue,publisher
)
values
('title1','description1','venue1',31),
('title2','description2','venue2',31),
('title3','description3','venue3',33),
('title4','description4','venue4',34),
('title5','description5','venue5',35),
('title6','description6','venue6',36),
('title7','description7','venue7',37),
('title8','description8','venue8',38),
('title9','description9','venue9',39),
('title10','description10','venue10',40)
;

===========================================================


insert into subscriptions
(
	user_id,published_id
)
values
(31,1),
(32,2),
(33,3),
(34,4),
(35,5),
(36,6),
(37,7),
(38,8),
(39,9),
(40,10)
;

================================================================

insert into stars
(
	user_id,published_id
)
values
(31,1),
(32,2),
(33,3),
(34,4),
(35,5),
(36,6),
(37,7),
(38,8),
(39,9),
(40,10)
;

==================================================================

insert into followers
(
	user_id,follower_id
)
values
(31,40),
(32,31),
(33,32),
(34,33),
(35,34),
(36,35),
(37,36),
(38,37),
(39,38),
(40,39)
;

===================================================================
*/




