#include "../include/requests.h"

char *send_http_request(map_t *map, char *url)
{
    // char *PORT = "3000";
    int socketfd;
    struct addrinfo hints;
    struct addrinfo *server_info, *p;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // 0 for success
    int status = getaddrinfo(url, "2000", &hints, &server_info);

    if (status != 0)
    {
        fprintf(stderr, "Error : %s\n", gai_strerror(status));
        return NULL;
    }

    char address[INET6_ADDRSTRLEN];
    // int port = 0;

    for (p = server_info; p != NULL; p = p->ai_next)
    {

        if ((socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client socket");
            continue;
        }

        if ((status = connect(socketfd, p->ai_addr, p->ai_addrlen)) == -1)
        {
            close(socketfd);
            perror("client connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "Failed to connect\n");
        return NULL;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              address,
              sizeof address);

    printf("Connecting to %s\n", address);

    char recv_buf[2];
    int bytes_received;

    char *message = "GET /song.mp3 HTTP/1.1\r\nContent-Type: text\r\n";

    string_t *b = string_create();

    if ((send(socketfd, message, strlen(message), 0)) == -1)
    {
        perror("send");
        return NULL;
    }

    /*if((bytes_received = recv(socketfd,&recv_buf,1023,0)) == -1)
    {
        perror("recv");
        //printf("%d ",bytes_received);
        return NULL;
    }*/
    char end_of_header[] = "\r\n\r\n";
    int marker = 0;

    FILE *ptr = fopen("readfile.mp3", "a");
    bool file_reached = false;

    while ((bytes_received = recv(socketfd, recv_buf, 1, 0)))
    {
        if (bytes_received == -1)
        {
            perror("recv");
        }

        if (bytes_received == 0)
        {
            break;
        }

        if (!file_reached)
        {
            string_append(b, recv_buf[0]);
        }
        else
        {
            fwrite(recv_buf, 1, sizeof recv_buf - 1, ptr);
        }

        if (recv_buf[0] == end_of_header[marker])
        {
            marker++;
        }
        else
        {
            marker = 0;
        }

        if (marker == 4)
        {
            puts("end found");
            map_print(parse_http_req(b->chars));
            file_reached = true;
        }
        bzero(&recv_buf, sizeof recv_buf);
    }

    //printf("%s\n", b->chars);

    // write()
    close(socketfd);
    fclose(ptr);
    free(server_info);
    return NULL;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

map_t *parse_http_req(char *req)
{
    int len = strlen(req);
    list_t *lines;
    req = string_removechar('\r', req, len);
    lines = split('\n', req, strlen(req));
    list_popback(lines);
    list_popback(lines);

    list_t *vl = lines;
    // printf("\n%s\n",vl->value);
    list_t *vc = split(' ', vl->value, strlen(vl->value));

    if(list_len(vc) != 3){
        fprintf(stderr,"Invalid request\n");
        return NULL;
    }

    map_t *map = map_create();

    map_add(
        map,
        "method",
        list_get(vc, 0));

    map_add(
        map,
        "url",
        list_get(vc, 1));

    map_add(
        map,
        "Http-Version",
        list_get(vc, 2));

    vl = vl->next;

    list_destroy(vc);

    while (vl != NULL)
    {
        vc = split_lim(':', vl->value, strlen(vl->value), 2);

        if(list_len(vc) != 2) continue;

        map_add(
            map,
            string_removechar(' ',
                              list_get(vc, 0),
                              strlen(list_get(vc, 0))),

            string_removechar(' ',
                              list_get(vc, 1),
                              strlen(list_get(vc, 1))));

        list_destroy(vc);

        vl = vl->next;
    }
    free(req);
    return map;
}