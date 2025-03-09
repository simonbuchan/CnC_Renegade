#pragma once

#include "gtypes.h"

typedef void (*qr_callback)(char*, int, void*);
int qr_init(
    qr_t*,
    const char*,
    unsigned short,
    const char*,
    const char*,
    qr_callback,
    qr_callback,
    qr_callback,
    qr_callback,
    void*);
void qr_shutdown(qr_t);
void qr_send_exiting(qr_t);
void qr_process_queries(qr_t);
