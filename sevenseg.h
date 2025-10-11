#ifndef INC_SEVENSEG_H_
#define INC_SEVENSEG_H_

#include "main.h"
#include <stdint.h>

void sevenseg_init(void);
void sevenseg_display_number(uint16_t num);
void sevenseg_update_multiplex(void);

#endif /* INC_SEVENSEG_H_ */
