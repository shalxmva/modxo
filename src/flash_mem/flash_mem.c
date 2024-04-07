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

#include "flash_mem.h"
#include "../lpc/lpc_interface.h"

#define FLASH_ROM_START_ADDRESS ((uint8_t*)0x10040000)
#define FLASH_ROM_MASK_ADDRESS  ((uint32_t*)(FLASH_ROM_START_ADDRESS - (4*1024)))
uint32_t flash_rom_mask;

static const uint32_t* flash_rom_mask_address = FLASH_ROM_MASK_ADDRESS;
static const uint8_t*  flash_rom_data = FLASH_ROM_START_ADDRESS;
static uint8_t mem_read_count=0;


uint8_t __not_in_flash_func(mem_read_handler)(uint32_t address){
    register uint32_t mem_data;
    mem_data = (uint32_t)flash_rom_data[address & flash_rom_mask];

    if(mem_read_count < 25){
        mem_read_count++;
    }else if(mem_read_count == 25){
        mem_read_count++;
    }

    return mem_data;
}

bool __not_in_flash_func(init_flash_mem)(){
    flash_rom_mask = *flash_rom_mask_address;
    return (flash_rom_mask != 0xFFFFFFFF);
}