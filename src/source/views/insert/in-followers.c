#include "../../../include/views/insert/in-followers.h"

void add_follower(int sock, char *json_load)
{
    // write_404(sock);
    json_object *jobj = json_tokener_parse(json_load);
    json_object *user_id, *follower_id;

    if (!json_object_object_get_ex(jobj, "user_id", &user_id))
    {
        write_BAD(sock);
        // todo
        return;
    }

    if (!json_object_object_get_ex(jobj, "follower_id", &follower_id))
    {
        write_BAD(sock);
        // todo
        return;
    }
    // write_404(sock);

    map_t *res = map_create();
    json_object *j_res = NULL;

    if (!insert_into_followers(json_object_get_string(user_id),
                               json_object_get_string(follower_id)))
    {
        map_add(res, "success", "false");
        j_res = create_json_object_from_map(res);
        write_json(j_res, sock);
        goto clean_up;
    }

    map_add(res, "success", "true");
    j_res = create_json_object_from_map(res);
    write_json(j_res, sock);

clean_up:
    json_object_put(j_res);
}