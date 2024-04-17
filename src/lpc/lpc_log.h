/*
SPDX short identifier: BSD-2-Clause
BSD 2-Clause License

Copyright (c) 2024, Shalx <Alejandro L. Huitron shalxmva@gmail.com>
*/
#ifndef LPC_LOG_H_
#define LPC_LOG_H_
#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint32_t address;
    uint8_t  cyc_type;
    uint8_t  data;
}log_entry;

void enqueue(log_entry item);
bool dequeue(log_entry *out);
#endif