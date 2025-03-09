#include "GameSpy/gcdkeyclient.h"
#include "GameSpy/gcdkeyserver.h"
#include "GameSpy/ghttp.h"
#include "GameSpy/gqueryreporting.h"
#include "GameSpy/gs_md5.h"
#include "GameSpy/gs_patch_usage.h"
#include "GameSpy/gtypes.h"
#include "GameSpy/nonport.h"

int qr_init(qr_t*, const char*, unsigned short, const char*, const char*, qr_callback, qr_callback, qr_callback,
    qr_callback, void*)
{
    return 0;
}

void qr_shutdown(qr_t)
{
}

void qr_send_exiting(qr_t)
{
}

void qr_process_queries(qr_t)
{
}

void gcd_init_qr(qr_t, int)
{
}

void gcd_think()
{
}

int get_master_count()
{
    return 0;
}

void clear_master_list()
{
}

bool get_sockaddrin(const char* addr, WORD port, sockaddr_in* saddr, const char* protocol)
{
    return false;
}

void add_master(sockaddr_in* addr)
{
}

void ghttpThink()
{
}
