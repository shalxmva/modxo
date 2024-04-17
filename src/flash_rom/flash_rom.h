/*
SPDX short identifier: BSD-2-Clause
BSD 2-Clause License

Copyright (c) 2024, Shalx <Alejandro L. Huitron shalxmva@gmail.com>
*/
#ifndef _FLASH_MEM_H_
#define _FLASH_MEM_H_

#include <stdint.h>

bool flashrom_init(void);
extern uint32_t flash_rom_mask;

#endif