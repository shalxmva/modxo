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

////////////////////////////   LOG SECTION   //////////////////////////
#include "lpc_log.h"

#include "pico/stdlib.h"
#define LOG_SIZE 1024

struct{
	int rear;
	int front;
	log_entry array[LOG_SIZE];
}queue = {
    .rear = LOG_SIZE -1,
    .front = LOG_SIZE -1,
};

void enqueue(log_entry item){
	queue.rear = (queue.rear + 1) % LOG_SIZE;
	if(queue.rear == queue.front){
		queue.rear--;
	}else{
		queue.array[queue.rear] = item;
	}
}

bool dequeue(log_entry *out){
	
	if(queue.front != queue.rear ){
		queue.front = (queue.front + 1) % LOG_SIZE;
		*out = queue.array[queue.front];
		return true;
	}

	return false;
}
 
/////////////////////////////////////////////////////////////////////