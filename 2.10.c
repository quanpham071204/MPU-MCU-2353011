/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
const int MAX_LED = 4;
int led_index = 0;
int led_buffer[4] = {1, 6, 5, 9};   // khởi tạo VD: 16:59
int hour = 16, minute = 59, second = 50;
int timer1_counter = 0;
int timer1_flag = 0;
/* Software timer */
int timer0_counter = 0;
int timer0_flag = 0;
const int TIMER_CYCLE = 10;   // timer interrupt period = 10ms

const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;
uint8_t matrix_buffer[8] = {0x00, 0xFC, 0x12, 0x11, 0x11, 0x12, 0xFC, 0x00}; // Mẫu ban đầu của "A"
uint8_t shift_counter = 0; // Đếm số lần dịch
const int SHIFT_DELAY = 5; // Số chu kỳ (50ms) giữa mỗi lần dịch

int timer2_counter = 0;
int timer2_flag = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN 0 */
/* Bảng mã 7 đoạn cho common anode (0 = sáng, 1 = tắt) */
const uint8_t segCode[10][7] = {
    {0,0,0,0,0,0,1}, // 0
    {1,0,0,1,1,1,1}, // 1
    {0,0,1,0,0,1,0}, // 2
    {0,0,0,0,1,1,0}, // 3
    {1,0,0,1,1,0,0}, // 4
    {0,1,0,0,1,0,0}, // 5
    {0,1,0,0,0,0,0}, // 6
    {0,0,0,1,1,1,1}, // 7
    {0,0,0,0,0,0,0}, // 8
    {0,0,0,0,1,0,0}  // 9
};

/* Software timer functions */
void setTimer0(int duration) {
    timer0_counter = duration / TIMER_CYCLE;
    timer0_flag = 0;
}
void setTimer1(int duration){
    timer1_counter = duration / TIMER_CYCLE;
    timer1_flag = 0;
}
void setTimer2(int duration){
    timer2_counter = duration / TIMER_CYCLE;
    timer2_flag = 0;
}
void timer_run(){
    if(timer0_counter > 0){
        timer0_counter--;
        if(timer0_counter == 0) timer0_flag = 1;
    }
    if(timer1_counter > 0){
        timer1_counter--;
        if(timer1_counter == 0) timer1_flag = 1;
    }
    if(timer2_counter > 0){
        timer2_counter--;
        if(timer2_counter == 0) timer2_flag = 1;
    }
}

/* Hiển thị số lên 7 đoạn */
void display7SEG(int num) {
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin, segCode[num][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin, segCode[num][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG2_Pin, segCode[num][2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG3_Pin, segCode[num][3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG4_Pin, segCode[num][4] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG5_Pin, segCode[num][5] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SEG6_Pin, segCode[num][6] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* Multiplexing 7SEG */
void update7SEG(int index) {
    HAL_GPIO_WritePin(GPIOA, EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin, GPIO_PIN_SET);
    switch (index) {
        case 0: display7SEG(led_buffer[0]); HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET); break;
        case 1: display7SEG(led_buffer[1]); HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); break;
        case 2: display7SEG(led_buffer[2]); HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET); break;
        case 3: display7SEG(led_buffer[3]); HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET); break;
    }
}

/* Cập nhật dữ liệu clock vào buffer */
void updateClockBuffer(void) {
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}

void updateLEDMatrix(int index) {
    uint32_t all_columns = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIOA->ODR &= ~all_columns;

    GPIOB->ODR = (GPIOB->ODR & 0x00FF) | ((uint32_t)matrix_buffer[index] << 8);

    uint16_t column_pins[8] = {GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};
    GPIOA->ODR |= column_pins[index];
}

void shiftLeftMatrix() {
    if (shift_counter >= SHIFT_DELAY) {
        // Dịch trái toàn bộ mảng
        for (int i = 0; i < MAX_LED_MATRIX - 1; i++) {
            matrix_buffer[i] = matrix_buffer[i + 1];
        }
        matrix_buffer[MAX_LED_MATRIX - 1] = 0x00; // Thêm cột trống bên phải
        shift_counter = 0; // Đặt lại đếm

        // Kiểm tra nếu "A" đã dịch hết, khôi phục mẫu ban đầu
        int all_zero = 1;
        for (int i = 0; i < MAX_LED_MATRIX; i++) {
            if (matrix_buffer[i] != 0x00) {
                all_zero = 0;
                break;
            }
        }
        if (all_zero) {
            for (int i = 0; i < MAX_LED_MATRIX; i++) {
                matrix_buffer[i] = (i == 1 || i == 5) ? 0xFC : (i == 2 || i == 4) ? 0x12 : (i == 3) ? 0x11 : 0x00;
            }
        }
    }
    shift_counter++;
}

/* USER CODE END 0 */

/* ---- Ngắt Timer2 ---- */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM2){
        timer_run();   // chỉ gọi timer_run
    }
}

/* ---- Main ---- */
int main(void){
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    HAL_TIM_Base_Start_IT(&htim2);

    setTimer0(1000);    // 1s cho clock
    setTimer1(250);     // 250ms cho quét 7SEG
    setTimer2(10);      // 10ms cho quét matrix

    while(1){
        if(timer0_flag){   // mỗi 1s
            second++;
            if(second>=60){ second=0; minute++; }
            if(minute>=60){ minute=0; hour++; }
            if(hour>=24){ hour=0; }
            updateClockBuffer();

            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);      // DOT blink
            HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin);     // LED_RED blink
            setTimer0(1000);
        }

        if(timer1_flag){   // mỗi 250ms
            update7SEG(led_index);
            led_index = (led_index+1)%MAX_LED;
            setTimer1(250);
        }

        if(timer2_flag){   // mỗi 10ms
            shiftLeftMatrix(); // Gọi hàm dịch trái
            updateLEDMatrix(index_led_matrix);
            index_led_matrix = (index_led_matrix + 1) % MAX_LED_MATRIX;
            setTimer2(10);
        }
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* -------------------- TIM2 Init -------------------- */
static void MX_TIM2_Init(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;   // 8MHz / 8000 = 1kHz (1ms tick)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;         // 10ms interrupt
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, LED_RED_Pin|EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin|GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin|SEG4_Pin|SEG5_Pin|SEG6_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = LED_RED_Pin|EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin|SEG4_Pin|SEG5_Pin|SEG6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Initialize matrix columns (PA2, PA3, PA10-PA15)
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Initialize matrix rows (PB8-PB15)
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void SystemClock_Config(void) {
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
