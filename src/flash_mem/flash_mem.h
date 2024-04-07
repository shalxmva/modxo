#ifndef _FLASH_MEM_H
#define _FLASH_MEM_H

#include <stdint.h>

bool init_flash_mem(void);
uint8_t mem_read_handler(uint32_t address);
extern uint32_t flash_rom_mask;

#endif