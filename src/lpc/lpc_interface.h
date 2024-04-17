/*
SPDX short identifier: BSD-2-Clause
BSD 2-Clause License

Copyright (c) 2024, Shalx <Alejandro L. Huitron shalxmva@gmail.com>
*/
#ifndef _LPC_INTERFACE_H_
#define _LPC_INTERFACE_H_

#include "hardware/pio.h"

typedef enum {
    LPC_OP_IO_READ    = 0,
    LPC_OP_IO_WRITE   = 1,
    LPC_OP_MEM_WRITE  = 2,
    LPC_OP_MEM_READ   = 3,
    LPC_OP_TOTAL      = 4
}LPC_OP_TYPE;


typedef void (*lpc_handler_cback)(uint32_t address, uint8_t* data);

void lpc_set_callback(LPC_OP_TYPE op, lpc_handler_cback cback);
void init_lpc_interface(PIO pio);
void set_disable_onboard_flash(bool disable);

#endif