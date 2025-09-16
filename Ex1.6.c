/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private prototypes --------------------------------------------------------*/
static void MX_GPIO_Init(void);
void led_cycle(void);

int main(void)
{
  HAL_Init();
  MX_GPIO_Init();

  while (1)
  {
    led_cycle();
  }
}

/* Cycle each LED PA4..PA15 one by one --------------------------------------*/
void led_cycle(void)
{
    /* array of pins PA4..PA15 (12 pins) */
    const uint16_t pins[12] = {
        GPIO_PIN_4,  GPIO_PIN_5,  GPIO_PIN_6,  GPIO_PIN_7,
        GPIO_PIN_8,  GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11,
        GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
    };

    for (int i = 0; i < 12; ++i)
    {
        /* Turn all off first (active-high wiring: SET = off, RESET = on optional) */
        HAL_GPIO_WritePin(GPIOA,
                          pins[0]|pins[1]|pins[2]|pins[3]|
                          pins[4]|pins[5]|pins[6]|pins[7]|
                          pins[8]|pins[9]|pins[10]|pins[11],
                          GPIO_PIN_RESET); // use RESET to turn OFF if you use active-low; see note below

        /* For typical active-HIGH (pin HIGH lights LED):
           - turn all pins LOW (RESET) to ensure off, then set current pin HIGH (SET).
           For active-LOW wiring (pin LOW lights LED) invert logic accordingly. */

        /* Example below assumes active-HIGH wiring (pin SET -> LED ON) */
        HAL_GPIO_WritePin(GPIOA,
                          pins[0]|pins[1]|pins[2]|pins[3]|
                          pins[4]|pins[5]|pins[6]|pins[7]|
                          pins[8]|pins[9]|pins[10]|pins[11],
                          GPIO_PIN_RESET); // ensure all low (off)
        HAL_GPIO_WritePin(GPIOA, pins[i], GPIO_PIN_SET); // light current LED

        HAL_Delay(300); // step delay (ms)
    }
}

/* GPIO init -----------------------------------------------------------------*/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIOA clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure PA4..PA15 as outputs */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                        GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
                        GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Set initial state: all OFF (active-HIGH wiring => pins LOW = OFF) */
  HAL_GPIO_WritePin(GPIOA,
                    GPIO_InitStruct.Pin,
                    GPIO_PIN_RESET);
}

/* Error handler (optional) */
void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}
