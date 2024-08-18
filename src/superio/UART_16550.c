/*
SPDX short identifier: BSD-2-Clause
BSD 2-Clause License

Copyright (c) 2024, Shalx <Alejandro L. Huitron shalxmva@gmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/multicore.h"
#include "hardware/structs/bus_ctrl.h"

#include "lpc/superio.h"
#include "tusb.h"

void uart_16550_port_write(uint16_t address, uint8_t* data){
    //UART Ports
    if((address == 0x3F8)){
        if(tud_cdc_connected())
            tud_cdc_write(data,1);
    }
}

void uart_16550_port_read(uint16_t address, uint8_t* data){
    //UART Ports
    if(tud_cdc_connected()){  //If usb serial port is open
        if(address == 0x3FD){
            *data = (tud_cdc_write_available()?0x20:0x00)|(tud_cdc_available()?0x01:0x00);
        }

        if(address == 0x3F8){
            tud_cdc_read(data,1);
        }
     }else{
        *data = 0;
     }
}

void uart_16550_init(void){
    superio_add_handler(0x03F8, 0xFFF8, uart_16550_port_read,   uart_16550_port_write);     //16550 Uart port emulation
}