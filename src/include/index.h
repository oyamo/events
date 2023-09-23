#pragma once

#include "./requests.h"
#include "./lib.h"
#include "./map.h"
#include "./dbc.h"

void login(char *url, int sock);
void sign_up(int sock, char *json_load);
void add_event(int sock,char *json_load);