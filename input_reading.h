#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

void button_reading(void);
uint8_t is_button_pressed(uint8_t index);
uint8_t is_button_pressed_1s(uint8_t index);

#endif /* INC_INPUT_READING_H_ */
