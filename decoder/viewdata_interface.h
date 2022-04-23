#pragma once

#include "viewdata_network.h"
#include "viewdata_screen.h"

int viewdata_connect_and_init(const char *ip, int port);
int viewdata_handle_stuff(const int input);
