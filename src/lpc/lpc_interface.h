#ifndef _LPC_INTERFACE_H
#define _LPC_INTERFACE_H_

#include "hardware/pio.h"

int init_lpc_interface();
void lpc_poll_loop();
void lpc_printer();

#endif