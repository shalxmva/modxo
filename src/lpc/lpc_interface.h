#ifndef _LPC_INTERFACE_H
#define _LPC_INTERFACE_H_

#include "hardware/pio.h"

int init_lpc_interface();
void lpc_mem_read_handler();
extern uint32_t flash_rom_mask;

#endif