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
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "lpc/lpc_interface.h"

// Set the system frequency to 33 MHz x 6 to make time for handling LPC frames.
#define SYS_FREQ_IN_KHZ (270 * 1000)

#define LED_STATUS_PIN PICO_DEFAULT_LED_PIN 

int main(){
    set_sys_clock_khz(SYS_FREQ_IN_KHZ, true);
    stdio_init_all();

    gpio_init(LED_STATUS_PIN);
    gpio_set_dir(LED_STATUS_PIN, GPIO_OUT);

    if (init_lpc_interface() < 0) return -1;

    printf("Flashrom Mask [%08X]",flash_rom_mask);
    
    if(flash_rom_mask == 0xFFFFFFFF)
    {
        while(true)
        {
            gpio_put(LED_STATUS_PIN, 1);
            sleep_ms(250);
            printf("Flash rom not programmed\n");
            gpio_put(LED_STATUS_PIN, 0);
            sleep_ms(250);
        }
    }

    gpio_put(LED_STATUS_PIN, 1);

    /* TODO: Implement IRQ insted of polling and run from Core1*/
    while(true){
        lpc_mem_read_handler();
    }
}