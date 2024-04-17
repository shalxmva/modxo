
/*
SPDX short identifier: BSD-2-Clause
BSD 2-Clause License

Copyright (c) 2024, Shalx <Alejandro L. Huitron shalxmva@gmail.com>
*/
#ifndef _SUPERIO_H_
#define _SUPERIO_H_

typedef void (*SUPERIO_PORT_CALLBACK_T)(uint16_t address, uint8_t *data);
bool superio_add_handler(uint16_t port_base, uint16_t mask, SUPERIO_PORT_CALLBACK_T read_cback, SUPERIO_PORT_CALLBACK_T write_cback);

#endif