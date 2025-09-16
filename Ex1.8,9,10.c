/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private prototypes --------------------------------------------------------*/
static void MX_GPIO_Init(void);
void clearAllClock(void);
void setNumberOnClock(int num);
void clearNumberOnClock(int num);

int main(void)
{
  HAL_Init();
  MX_GPIO_Init();

  clearAllClock(); // ban đầu tắt hết

  // Biến giả lập giờ, phút, giây
  int hour = 0, minute = 0, second = 0;

  while (1)
  {
    clearAllClock();  // tắt hết trước

    // Bật LED giờ, phút, giây
    setNumberOnClock(hour % 12);
    setNumberOnClock(minute % 12);
    setNumberOnClock(second % 12);

    HAL_Delay(100);  // delay 1 giây

    // Tăng thời gian
    second++;
    if (second >= 60) {
        second = 0;
        minute++;
    }
    if (minute >= 60) {
        minute = 0;
        hour++;
    }
    if (hour >= 12) {
        hour = 0;
    }
  }
}

/* ================= Các hàm hỗ trợ ================= */

// LED pins mapping (PA4..PA15)
const uint16_t clockPins[12] = {
    GPIO_PIN_4,  GPIO_PIN_5,  GPIO_PIN_6,  GPIO_PIN_7,
    GPIO_PIN_8,  GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};

/* Hàm tắt tất cả LED --------------------------------------------------------*/
void clearAllClock(void)
{
  HAL_GPIO_WritePin(GPIOA,
                    GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                    GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
                    GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,
                    GPIO_PIN_SET);   // SET = tắt (active-low wiring)
}

/* Exercise 8: bật LED theo số (0..11) ---------------------------------------*/
void setNumberOnClock(int num)
{
    if (num >= 0 && num < 12) {
        HAL_GPIO_WritePin(GPIOA, clockPins[num], GPIO_PIN_RESET); // RESET = sáng
    }
}

/* Exercise 9: tắt LED theo số (0..11) --------------------------------------*/
void clearNumberOnClock(int num)
{
    if (num >= 0 && num < 12) {
        HAL_GPIO_WritePin(GPIOA, clockPins[num], GPIO_PIN_SET);   // SET = tắt
    }
}

/* GPIO init -----------------------------------------------------------------*/
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

/* Error handler -------------------------------------------------------------*/
void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}
