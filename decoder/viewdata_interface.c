#include "viewdata_interface.h"

int viewdata_connect_and_init(const char *ip, int port)
{
	int res=0;
	res=viewdata_init_screen();
	if (res<0) return res;
	res=viewdata_connect(ip, port);
	if (res<0) return res;
	return res;
};

int viewdata_handle_stuff(const int input)
{
	return viewdata_handle(input);
};

