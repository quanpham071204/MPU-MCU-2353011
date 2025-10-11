#include "sevenseg.h"

#define NUM_DIGITS 4

// Segment pins (a–g)
static const uint16_t seg_pins[7] = {
    GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6,
    GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9
};
#define seg_port GPIOB

// Digit select pins (common anode)
static const uint16_t digit_pins[NUM_DIGITS] = {
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
};
#define digit_port GPIOA

// Segment bitmask cho các số 0–9 (bit0→a ... bit6→g)
static const uint8_t seg_digits[10] = {
    0b1111110, // 0
    0b0110000, // 1
    0b1101101, // 2
    0b1111001, // 3
    0b0110011, // 4
    0b1011011, // 5
    0b1011111, // 6
    0b1110000, // 7
    0b1111111, // 8
    0b1111011  // 9
};

// buffer hiển thị
static uint8_t digits[NUM_DIGITS] = {0};
static uint8_t current_digit = 0;

void sevenseg_init(void){
    // Tắt toàn bộ digit
    for(int i = 0; i < NUM_DIGITS; i++)
        HAL_GPIO_WritePin(digit_port, digit_pins[i], GPIO_PIN_RESET); // OFF

    // Tắt tất cả segment
    for(int i = 0; i < 7; i++)
        HAL_GPIO_WritePin(seg_port, seg_pins[i], GPIO_PIN_SET); // OFF
}

// Common Anode: segment ON = LOW, OFF = HIGH
static void set_segments(uint8_t val){
    uint8_t mask = seg_digits[val];
    for(int i = 0; i < 7; i++){
        if(mask & (1 << (6 - i)))
            HAL_GPIO_WritePin(seg_port, seg_pins[i], GPIO_PIN_RESET); // sáng
        else
            HAL_GPIO_WritePin(seg_port, seg_pins[i], GPIO_PIN_SET);   // tắt
    }
}

// Cập nhật số hiển thị (gọi khi cần đổi giá trị)
void sevenseg_display_number(uint16_t num){
    // Clear buffer trước
    for(int i = 0; i < NUM_DIGITS; i++) digits[i] = 0xFF; // 0xFF = tắt hết segment

    int i = 0;
    do {
        digits[i++] = num % 10;
        num /= 10;
    } while(num > 0 && i < NUM_DIGITS);
}


// Quét 7-seg (gọi mỗi 2–5ms)
void sevenseg_update_multiplex(void){
    // Tắt digit hiện tại
    HAL_GPIO_WritePin(digit_port, digit_pins[current_digit], GPIO_PIN_RESET);

    // Chuyển sang digit kế
    current_digit++;
    if(current_digit >= NUM_DIGITS)
        current_digit = 0;

    // Hiển thị giá trị mới
    set_segments(digits[current_digit]);

    // Bật digit hiện tại (Common Anode → HIGH = bật)
    HAL_GPIO_WritePin(digit_port, digit_pins[current_digit], GPIO_PIN_SET);
}
