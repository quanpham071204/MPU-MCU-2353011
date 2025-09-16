/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void display7SEG(int num);

/* USER CODE BEGIN 0 */
void display7SEG(int num) {
    // Bảng mã cho Common Anode (0 = sáng, 1 = tắt)
    const uint8_t segCode[10][7] = {
            {1,0,0,0,0,0,0}, // 0
            {1,0,0,1,1,1,1}, // 1
            {0,1,0,0,1,0,0}, // 2
            {0,0,0,0,1,1,0}, // 3
            {0,0,0,1,0,1,1}, // 4
            {0,0,1,0,0,1,0}, // 5
            {0,0,1,0,0,0,0}, // 6
            {1,0,0,0,1,1,1}, // 7
            {0,0,0,0,0,0,0}, // 8
            {0,0,0,0,0,1,0}  // 9
    };

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, segCode[num][0] ? GPIO_PIN_SET : GPIO_PIN_RESET); // a
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, segCode[num][1] ? GPIO_PIN_SET : GPIO_PIN_RESET); // b
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, segCode[num][2] ? GPIO_PIN_SET : GPIO_PIN_RESET); // c
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, segCode[num][3] ? GPIO_PIN_SET : GPIO_PIN_RESET); // d
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, segCode[num][4] ? GPIO_PIN_SET : GPIO_PIN_RESET); // e
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, segCode[num][5] ? GPIO_PIN_SET : GPIO_PIN_RESET); // f
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, segCode[num][6] ? GPIO_PIN_SET : GPIO_PIN_RESET); // g
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* Infinite loop */
  while (1)
  {
    // ---- Phase 1: Cụm1 xanh 5s, Cụm2 đỏ ----
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // RED1 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // YELLOW1 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // GREEN1 ON

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // RED2 ON
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // YELLOW2 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // GREEN2 OFF

    for (int t=5; t>0; t--) {
        display7SEG(t);
        HAL_Delay(1000);
    }

    // ---- Phase 2: Cụm1 vàng 2s, Cụm2 đỏ ----
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // GREEN1 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // YELLOW1 ON
    for (int t=2; t>0; t--) {
        display7SEG(t);
        HAL_Delay(1000);
    }

    // ---- Phase 3: Cụm1 đỏ 5s, Cụm2 xanh ----
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // RED1 ON
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // YELLOW1 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // GREEN1 OFF

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // RED2 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);   // YELLOW2 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);// GREEN2 ON

    for (int t=5; t>0; t--) {
        display7SEG(t);
        HAL_Delay(1000);
    }

    // ---- Phase 4: Cụm1 đỏ, Cụm2 vàng 2s ----
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);  // GREEN2 OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // YELLOW2 ON
    for (int t=2; t>0; t--) {
        display7SEG(t);
        HAL_Delay(1000);
    }
  }
}

/* GPIO init -----------------------------------------------------------------*/
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // --- Output cho đèn giao thông ---
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                           GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
                        GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // --- Output cho 7SEG ---
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|
                           GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|
                        GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* Clock config giữ nguyên mặc định */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}
