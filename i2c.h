#ifndef _I2C_H_
#define _I2C_H_

#include "stm8l15x.h"
#include "utils.h"

#include <string.h>

int requestBytes(uint8_t *buffer, int len);

void send_command_with_sub(uint8_t reg, uint16_t sub_command);

void send_command(uint8_t command);

int request2Bytes(uint8_t *buffer, int len);

uint16_t requestWord(void);

#endif
