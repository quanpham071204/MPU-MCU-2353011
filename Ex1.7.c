/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private prototypes --------------------------------------------------------*/
static void MX_GPIO_Init(void);
void clearAllClock(void);
void led_cycle(void);

int main(void)
{
  HAL_Init();
  MX_GPIO_Init();

  clearAllClock(); // ban đầu tất cả tắt

  while (1)
  {
    led_cycle();   // chạy theo chiều kim đồng hồ
  }
}

/* Hàm tắt tất cả LED -------------------------------------------------------*/
void clearAllClock(void)
{
  HAL_GPIO_WritePin(GPIOA,
                    GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                    GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
                    GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,
                    GPIO_PIN_SET);   // SET = tắt
}

/* Hàm chạy LED theo chiều kim đồng hồ -------------------------------------*/
void led_cycle(void)
{
  const uint16_t pins[12] = {
      GPIO_PIN_4,  GPIO_PIN_5,  GPIO_PIN_6,  GPIO_PIN_7,
      GPIO_PIN_8,  GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11,
      GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
  };

  for (int i = 0; i < 12; i++)
  {
    clearAllClock();                          // tắt hết
    HAL_GPIO_WritePin(GPIOA, pins[i], GPIO_PIN_RESET); // bật LED hiện tại
    HAL_Delay(300);                           // chờ 300ms
  }
}

/* GPIO init ---------------------------------------------------------------*/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                        GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
                        GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  clearAllClock(); // khởi tạo: tắt hết
}
