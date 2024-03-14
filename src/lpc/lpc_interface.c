/*
SPDX short identifier: BSD-2-Clause
BSD 2-Clause License

Copyright (c) 2024, Shalx <Alejandro Lopez alexwinger@gmail.com>

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

#include "lpc_interface.h"

#include "lpc_mem_read.pio.h"


#define LPC_BUS_PIN_START (0U)
#define LPC_BUS_PIN_COUNT (4U)
#define FLASH_ROM_START_ADDRESS ((uint8_t*)0x10040000)
#define FLASH_ROM_MASK_ADDRESS  ((uint32_t*)(FLASH_ROM_START_ADDRESS - (4*1024)))


static const uint32_t* flash_rom_mask_address = FLASH_ROM_MASK_ADDRESS;
static const uint8_t*  flash_rom_data = FLASH_ROM_START_ADDRESS;
uint32_t flash_rom_mask;

//PIO
static PIO pio = pio0;
static int lpc_mem_read_SM;


static void init_lpc_gpios() {    
    for(uint i = LPC_BUS_PIN_START; i < LPC_BUS_PIN_START + LPC_BUS_PIN_COUNT+3; i++) {
        gpio_init(i);
        gpio_disable_pulls(i);
    }
}

int init_lpc_interface() {
    uint offset;

    flash_rom_mask = *flash_rom_mask_address;
    init_lpc_gpios();

    int sm = pio_claim_unused_sm(pio, true);
    if(sm < 0){
        printf("Error: Cannot claim a free state machine\n");
        return -1;
    }

    if (pio_can_add_program(pio, &lpc_mem_read_program)) {
        offset = pio_add_program(pio, &lpc_mem_read_program);
    } else {
        printf("Error: pio program can not be loaded\n");
        return -2;
    }

    lpc_mem_read_program_init(pio, sm, offset);
    return sm;
}


void lpc_mem_read_handler(){

    register uint32_t address, mem_data, shifted;

    if(!pio_sm_is_rx_fifo_empty(pio, lpc_mem_read_SM)){
        address = pio->rxf[lpc_mem_read_SM];
        mem_data = (uint32_t)flash_rom_data[address & flash_rom_mask];
        shifted = (mem_data<<4);
        pio->txf[lpc_mem_read_SM]=shifted;
    }

}

