#pragma once

#include <winsock.h>

#include "gtypes.h"

void gcd_init_qr(qr_t, int);
void gcd_think();

int get_master_count();
void clear_master_list();

bool get_sockaddrin(const char *addr, WORD port, sockaddr_in *saddr, const char *protocol);
void add_master(sockaddr_in *addr);
