#ifndef _konf_net_private_h
#define _konf_net_private_h

#include "target_utils/konf/net.h"

struct konf_client_s {
	int sock;
	char *path;
};

#endif
