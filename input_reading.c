#include "input_reading.h"
#include "main.h"

#define N0_OF_BUTTONS 3
#define DURATION_FOR_AUTO_INCREASING 100  // 100 * 10ms = 1s
#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

// --- Biến lưu trạng thái nút ---
static GPIO_PinState buttonBuffer[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer1[N0_OF_BUTTONS];
static GPIO_PinState debounceButtonBuffer2[N0_OF_BUTTONS];
static uint8_t flagForButtonPress[N0_OF_BUTTONS];
static uint8_t flagForButtonPress1s[N0_OF_BUTTONS];
static uint16_t counterForButtonPress1s[N0_OF_BUTTONS];

// --- Cấu hình chân nút ---
typedef struct { GPIO_TypeDef* port; uint16_t pin; } Btn_t;
static const Btn_t btns[N0_OF_BUTTONS] = {
    {GPIOB, GPIO_PIN_0},   // MODE
    {GPIOB, GPIO_PIN_1},   // INC
    {GPIOB, GPIO_PIN_2}    // SET
};

// --- Đọc nút, gọi mỗi 10ms trong ngắt timer ---
void button_reading(void){
    for(uint8_t i = 0; i < N0_OF_BUTTONS; i++){
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(btns[i].port, btns[i].pin);

        // Khi ổn định 2 lần đọc liên tiếp → xác nhận giá trị
        if(debounceButtonBuffer1[i] == debounceButtonBuffer2[i]){
            if(buttonBuffer[i] != debounceButtonBuffer1[i]){
                buttonBuffer[i] = debounceButtonBuffer1[i];
                if(buttonBuffer[i] == BUTTON_IS_PRESSED){
                    flagForButtonPress[i] = 1; // đánh dấu nhấn 1 lần
                }
            }
        }

        // Xử lý giữ 1s
        if(buttonBuffer[i] == BUTTON_IS_PRESSED){
            if(counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING){
                counterForButtonPress1s[i]++;
                flagForButtonPress1s[i] = 0;
            } else {
                flagForButtonPress1s[i] = 1;
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i] = 0;
        }
    }
}

// --- Kiểm tra nút nhấn 1 lần ---
uint8_t is_button_pressed(uint8_t index){
    if(index >= N0_OF_BUTTONS) return 0;
    if(flagForButtonPress[index] == 1){
        flagForButtonPress[index] = 0;
        return 1;
    }
    return 0;
}

// --- Kiểm tra nút nhấn giữ > 1s ---
uint8_t is_button_pressed_1s(uint8_t index){
    if(index >= N0_OF_BUTTONS) return 0;
    return flagForButtonPress1s[index];
}
