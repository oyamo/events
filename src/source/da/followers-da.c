#include "../../include/da/followers-da.h"
#include "../../include/lib/files.h"
#include "../../include/da/db.h"

json_object *get_followers_by_user_id(char *id_,char *last_time)
{
    MYSQL *conn = cpool_get_connection(cpool);

    if (conn == NULL)
    {
        puts("failed to connect to db");
        goto exit;
    }

    char *query = "call get_followers(?,?)";

    unsigned long id_l = strlen(id_),time_l = strlen(last_time);

    MYSQL_BIND bind[2];
    MYSQL_STMT *stmt;

    stmt = mysql_stmt_init(conn);

    if (!stmt)
    {
        puts("out of memory");
        cpool_drop_connection(conn,cpool);
        return NULL;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        puts(mysql_stmt_error(stmt));
        goto exit;
    }

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].is_null = 0;
    bind[0].length = &id_l;
    bind[0].buffer = id_;
    bind[0].buffer_length = 100;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].is_null = 0;
    bind[1].length = &time_l;
    bind[1].buffer = last_time;
    bind[1].buffer_length = 100;


    if (mysql_stmt_bind_param(stmt, bind))
    {
        puts(mysql_stmt_error(stmt));
        goto exit;
    }

    if (mysql_stmt_execute(stmt))
    {
        puts(mysql_stmt_error(stmt));
        goto exit;
    }

    // printf("%d\n",column_count);

    MYSQL_BIND bind_results[5];
    memset(bind_results, 0, sizeof bind_results);

    ////////
    char id[100],
        user_id[100],
        name[100],
        username[100],
        date_created[100];

    bool is_null[5];
    bool error[5];
    unsigned long length[5];

    bind_results[0].buffer = id;
    bind_results[1].buffer = user_id;
    bind_results[2].buffer = name;
    bind_results[3].buffer = username;
    bind_results[4].buffer = date_created;

    for (int i = 0; i < 5; i++)
    {
        bind_results[i].length = &length[i];
        bind_results[i].buffer_length = i == 2 ? 1000 : 100;
        bind_results[i].error = &error[i];
        bind_results[i].is_null = &is_null[i];
        bind_results[i].buffer_type = MYSQL_TYPE_STRING;
    }

    if (mysql_stmt_bind_result(stmt, bind_results))
    {
        puts("bind failed");
        goto exit;
    }

    json_object * res = json_object_new_array();

    while (!mysql_stmt_fetch(stmt))
    {
        json_object * this_row = json_object_new_object();

        json_object_object_add(this_row,"id",json_object_new_string(id));
        json_object_object_add(this_row,"user_id",json_object_new_string(user_id));
        json_object_object_add(this_row,"name",json_object_new_string(name));
        json_object_object_add(this_row,"username",json_object_new_string(username));
        json_object_object_add(this_row,"last_time",json_object_new_string(date_created));

        json_object_array_add(res,json_object_get(this_row));

        json_object_put(this_row);
    }

    mysql_stmt_close(stmt);
    cpool_drop_connection(conn,cpool);
    return res;

exit:
    mysql_stmt_close(stmt);
    cpool_drop_connection(conn,cpool);
    return NULL;
}


json_object *get_following_by_user_id(char *id_,char *last_time)
{
    MYSQL *conn = cpool_get_connection(cpool);

    if (conn == NULL)
    {
        puts("failed to connect to db");
        goto exit;
    }

    char *query = "call get_following(?,?)";

    unsigned long id_l = strlen(id_),time_l = strlen(last_time);

    MYSQL_BIND bind[2];
    MYSQL_STMT *stmt;

    stmt = mysql_stmt_init(conn);

    if (!stmt)
    {
        puts("out of memory");
        goto exit;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        puts(mysql_stmt_error(stmt));
        goto exit;
    }

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].is_null = 0;
    bind[0].length = &id_l;
    bind[0].buffer = id_;
    bind[0].buffer_length = 100;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].is_null = 0;
    bind[1].length = &time_l;
    bind[1].buffer = last_time;
    bind[1].buffer_length = 100;


    if (mysql_stmt_bind_param(stmt, bind))
    {
        puts(mysql_stmt_error(stmt));
        goto exit;
    }

    if (mysql_stmt_execute(stmt))
    {
        puts(mysql_stmt_error(stmt));
        goto exit;
    }

    // printf("%d\n",column_count);

    MYSQL_BIND bind_results[5];
    memset(bind_results, 0, sizeof bind_results);

    ////////
    char id[100],
        user_id[100],
        name[100],
        username[100],
        date_created[100];

    bool is_null[5];
    bool error[5];
    unsigned long length[5];

    bind_results[0].buffer = id;
    bind_results[1].buffer = user_id;
    bind_results[2].buffer = name;
    bind_results[3].buffer = username;
    bind_results[4].buffer = date_created;

    for (int i = 0; i < 5; i++)
    {
        bind_results[i].length = &length[i];
        bind_results[i].buffer_length = i == 2 ? 1000 : 100;
        bind_results[i].error = &error[i];
        bind_results[i].is_null = &is_null[i];
        bind_results[i].buffer_type = MYSQL_TYPE_STRING;
    }

    if (mysql_stmt_bind_result(stmt, bind_results))
    {
        puts("bind failed");
        goto exit;
    }

    json_object * res = json_object_new_array();


    while (!mysql_stmt_fetch(stmt))
    {
        json_object * this_row = json_object_new_object();

        json_object_object_add(this_row,"id",json_object_new_string(id));
        json_object_object_add(this_row,"user_id",json_object_new_string(user_id));
        json_object_object_add(this_row,"name",json_object_new_string(name));
        json_object_object_add(this_row,"username",json_object_new_string(username));
        json_object_object_add(this_row,"last_time",json_object_new_string(date_created));

        json_object_array_add(res,json_object_get(this_row));

        json_object_put(this_row);
    }

    mysql_stmt_close(stmt);
    cpool_drop_connection(conn,cpool);
    return res;

exit:
    mysql_stmt_close(stmt);
    cpool_drop_connection(conn,cpool);
    return NULL;
}


bool insert_into_followers(const char *user_id, const char *follower_id)
{
    MYSQL *conn = cpool_get_connection(cpool);

    if (conn == NULL)
    {
        puts("failed to connect to db");
        return false;
    }

    char *query = "insert into followers (user_id,follower_id) values (uuid_to_bin(?),uuid_to_bin(?))";

    unsigned long user_l = strlen(user_id),
                  fol_l = strlen(follower_id);

    MYSQL_BIND bind[2];
    MYSQL_STMT *stmt;

    stmt = mysql_stmt_init(conn);

    if (!stmt)
    {
        cpool_drop_connection(conn,cpool);
        return false;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        goto exit_with_error;
    }

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].is_null = 0;
    bind[0].length = &user_l;
    bind[0].buffer_length = 100;
    bind[0].buffer = user_id;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].is_null = 0;
    bind[1].length = &fol_l;
    bind[1].buffer_length = 100;
    bind[1].buffer = follower_id;

    if (mysql_stmt_bind_param(stmt, bind))
    {
        puts("An error occured");
        goto exit_with_error;
    }

    if (mysql_stmt_execute(stmt))
    {
        goto exit_with_error;
    }


    mysql_stmt_close(stmt);
    cpool_drop_connection(conn,cpool);
    return true;

exit_with_error:
    puts(mysql_stmt_error(stmt));
    mysql_stmt_close(stmt);
    cpool_drop_connection(conn,cpool);
    return false;
}

