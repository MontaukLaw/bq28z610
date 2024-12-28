#ifndef _COMM_H_
#define _COMM_H_

#include "stm8l15x.h"
#include "bq28z610.h"

extern uint8_t uart_data;
extern uint8_t ifGetData;

void handle_comm(uint8_t data);

#endif
