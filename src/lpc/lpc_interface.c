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
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/bus_ctrl.h"

#include "../flash_mem/flash_mem.h"
#include "lpc_interface.h"

#include "lpc_comm.pio.h"

#define POLLING

#define LPC_BUS_PIN_START (0U)
#define LPC_BUS_PIN_COUNT (4U)

void start_mem_read_sm(void);
//PIO
static PIO pio = pio0;
static int lpc_mem_read_SM;

uint32_t buffer[1024];
uint32_t idx;
uint32_t pidx;
int count=0;

bool disable_internal_flash=true;

static void __not_in_flash_func(gpio_set_max_drivestrength)(uint gpio) {
    hw_write_masked(
            &padsbank0_hw->io[gpio],
            PADS_BANK0_GPIO0_DRIVE_VALUE_12MA<<PADS_BANK0_GPIO0_DRIVE_LSB,
            PADS_BANK0_GPIO0_DRIVE_BITS
    );
}

static void __not_in_flash_func(pio_custom_init)(PIO pio, uint sm, uint initial_pc, const pio_sm_config *config, uint32_t op){
        valid_params_if(PIO, initial_pc < PIO_INSTRUCTION_COUNT);
    // Halt the machine, set some sensible defaults
    pio_sm_set_enabled(pio, sm, false);

    if (config) {
        pio_sm_set_config(pio, sm, config);
    } else {
        pio_sm_config c = pio_get_default_sm_config();
        pio_sm_set_config(pio, sm, &c);
    }

    pio_sm_clear_fifos(pio, sm);

    // Clear FIFO debug flags
    const uint32_t fdebug_sm_mask =
            (1u << PIO_FDEBUG_TXOVER_LSB) |
            (1u << PIO_FDEBUG_RXUNDER_LSB) |
            (1u << PIO_FDEBUG_TXSTALL_LSB) |
            (1u << PIO_FDEBUG_RXSTALL_LSB);
    pio->fdebug = fdebug_sm_mask << sm;

    // Finally, clear some internal SM state
    pio_sm_restart(pio, sm);
    pio_sm_clkdiv_restart(pio, sm);
    
    pio->txf[sm]=op;
    
    pio_sm_exec(pio, sm, pio_encode_mov(pio_pins, pio_null));
    pio_sm_exec(pio, sm, pio_encode_pull(false, true));
    pio_sm_exec(pio, sm, pio_encode_mov(pio_y, pio_osr));
    pio_sm_exec(pio, sm, pio_encode_jmp(initial_pc));
}


static void __not_in_flash_func(lpc_read_irq_handler)(void){
    register uint32_t address,mem_data,shifted,pushed;
    address = pio->rxf[lpc_mem_read_SM];
    pushed = pio->rxf[lpc_mem_read_SM];
    mem_data = mem_read_handler(address);

    shifted = 0xF000;
    shifted |= (mem_data<<4);
    pio->txf[lpc_mem_read_SM]=shifted;
    pio->txf[lpc_mem_read_SM] = 8;

    buffer[idx] = address;
    buffer[idx+1] = pushed;
    if(idx < 1024)
        idx+=2;
    
    pio_interrupt_clear(pio, 0);//pio->irq=1;
}

void __not_in_flash_func(enable_pio_interrupts)(){
    pio_set_irq0_source_enabled(pio, pis_interrupt0, true);
    irq_set_exclusive_handler(PIO0_IRQ_0, lpc_read_irq_handler);
    //irq_set_exclusive_handler(PIO0_IRQ_1, lpc_read_irq_handler);
    irq_set_enabled(PIO0_IRQ_0, true);
    //irq_set_enabled(PIO0_IRQ_1, true);
}

int __not_in_flash_func(init_lpc_interface)() {
    uint offset;

    int sm = pio_claim_unused_sm(pio, true);
    if(sm < 0){
        printf("Error: Cannot claim a free state machine\n");
        return -1;
    }

    if (pio_can_add_program(pio, &lpc_read_request_program)) {
        offset = pio_add_program(pio, &lpc_read_request_program);
    } else {
        printf("Error: pio program can not be loaded\n");
        return -2;
    }
    

    pio_sm_config c =lpc_read_request_init(pio, sm, offset,32, disable_internal_flash);
    
    
    gpio_init(D0_PIN);
    gpio_disable_pulls(D0_PIN);

    if(disable_internal_flash){
        gpio_set_dir(D0_PIN, GPIO_OUT);
        gpio_put(D0_PIN, 0);
        gpio_set_max_drivestrength(D0_PIN);
        gpio_set_max_drivestrength(LFRAME_PIN);
    }else{
        gpio_set_dir(D0_PIN, GPIO_IN);
    }
    
    //lpc_disable_tsop(disable_internal_flash);

    pio_custom_init(pio, sm, offset, &c, 0x4);
    //Enable State Machines
    
    gpio_set_max_drivestrength(5);
    gpio_set_max_drivestrength(6);
    
    pio_sm_set_enabled(pio, sm, true);


    pio->txf[lpc_mem_read_SM] = 8;
    //pio->txf[lpc_mem_read_SM] = 0xCFFFFF00;
    idx = 0;
    pidx = 0;

    
    #ifndef POLLING
    enable_pio_interrupts();
    //enable_dma_interrupts(pio, sm);
    #endif


    return sm;
}

void __not_in_flash_func(lpc_poll_loop)(){
 while(true){
    #ifdef POLLING
    if((pio->fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + lpc_mem_read_SM))) == 0){
        lpc_read_irq_handler();
    }
    #endif
 }
}

void __not_in_flash_func(lpc_printer)(){
    while(true){
        //uint32_t compare = pidx>idx?(idx+0x400):idx;
        while(pidx < idx)
        {
            printf("\n\t%04d - %04d | %02d:[%08X] [%08X]",pidx,idx,count++,buffer[pidx],buffer[pidx+1]);
            pidx+=2;
            if(count == 20){
                count=0;
                printf("\n=========================================================");
            }
        }
    }
}

