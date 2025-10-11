#include "input_processing.h"
#include "main.h"
#include "sevenseg.h"
#include "input_reading.h"

#define LED_ON  1
#define LED_OFF 0

// Các trạng thái của FSM
#define INIT            0
#define RED_GREEN       1
#define RED_YELLOW      2
#define GREEN_RED       3
#define YELLOW_RED      4

int status = INIT;
// ====== BỔ SUNG CHẾ ĐỘ ĐIỀU KHIỂN NÚT ======
#define MODE_BUTTON  0   // PB0
#define INC_BUTTON   1   // PB1
#define SET_BUTTON   2   // PB2

#define AUTO_MODE       0
#define SET_RED_TIME    1
#define SET_YELLOW_TIME 2
#define SET_GREEN_TIME  3

int system_mode = AUTO_MODE;

// Thời gian từng pha (đơn vị: 10ms tick)
int red_duration = 500;     // 5s
int yellow_duration = 200;  // 2s
int green_duration = 500;   // 5s
int timer1 = 0;

// Giá trị countdown hiển thị
int counter_NS = 0; // đếm cho hướng Bắc–Nam
int counter_WE = 0; // đếm cho hướng Tây–Đông

// ====== LED setup ======
// Mỗi hướng có 3 đèn liên tiếp: đỏ–vàng–xanh
// PA0–PA2: Hướng trái
// PA3–PA5: Hướng phải
// PA6–PA8: Hướng trên
// PA9–PA11: Hướng dưới
void set_leds(int base, int red, int yellow, int green){
    uint16_t red_pin, yellow_pin, green_pin;

    switch(base){
        case 0: red_pin = GPIO_PIN_0; yellow_pin = GPIO_PIN_1; green_pin = GPIO_PIN_2; break;  // trái
        case 1: red_pin = GPIO_PIN_3; yellow_pin = GPIO_PIN_4; green_pin = GPIO_PIN_5; break;  // phải
        case 2: red_pin = GPIO_PIN_6; yellow_pin = GPIO_PIN_7; green_pin = GPIO_PIN_8; break;  // trên
        case 3: red_pin = GPIO_PIN_9; yellow_pin = GPIO_PIN_10; green_pin = GPIO_PIN_11; break; // dưới
        default: return;
    }

    // Common Anode → LED bật khi ghi mức LOW
    HAL_GPIO_WritePin(GPIOA, red_pin,     red    ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, yellow_pin,  yellow ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, green_pin,   green  ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

// ====== Khởi tạo ban đầu ======
void init_roads(void){
    status = RED_GREEN;
    timer1 = green_duration;
    counter_NS = 0;
    counter_WE = green_duration / 100; // 1s = 100 tick

    // Trái–Phải: đỏ, Trên–Dưới: xanh
    set_leds(0, LED_ON, LED_OFF, LED_OFF); // trái
    set_leds(1, LED_ON, LED_OFF, LED_OFF); // phải
    set_leds(2, LED_OFF, LED_OFF, LED_ON); // trên
    set_leds(3, LED_OFF, LED_OFF, LED_ON); // dưới
}

// ====== FSM chính ======
void fsm_for_input_processing(void){
    switch(status){
    case INIT:
        init_roads();
        break;

    case RED_GREEN:
        if(timer1 <= 0){
            status = RED_YELLOW;
            timer1 = yellow_duration;
            counter_WE = yellow_duration / 100;

            // chuyển pha sang vàng cho hướng đang xanh
            set_leds(0, LED_ON, LED_OFF, LED_OFF);
            set_leds(1, LED_ON, LED_OFF, LED_OFF);
            set_leds(2, LED_OFF, LED_ON, LED_OFF);
            set_leds(3, LED_OFF, LED_ON, LED_OFF);
        }
        break;

    case RED_YELLOW:
        if(timer1 <= 0){
            status = GREEN_RED;
            timer1 = green_duration;
            counter_NS = green_duration / 100;

            // chuyển pha: trái–phải xanh, trên–dưới đỏ
            set_leds(0, LED_OFF, LED_OFF, LED_ON);
            set_leds(1, LED_OFF, LED_OFF, LED_ON);
            set_leds(2, LED_ON, LED_OFF, LED_OFF);
            set_leds(3, LED_ON, LED_OFF, LED_OFF);
        }
        break;

    case GREEN_RED:
        if(timer1 <= 0){
            status = YELLOW_RED;
            timer1 = yellow_duration;
            counter_NS = yellow_duration / 100;

            set_leds(0, LED_OFF, LED_ON, LED_OFF);
            set_leds(1, LED_OFF, LED_ON, LED_OFF);
            set_leds(2, LED_ON, LED_OFF, LED_OFF);
            set_leds(3, LED_ON, LED_OFF, LED_OFF);
        }
        break;

    case YELLOW_RED:
        if(timer1 <= 0){
            status = RED_GREEN;
            timer1 = green_duration;
            counter_WE = green_duration / 100;

            // quay lại pha đầu
            set_leds(0, LED_ON, LED_OFF, LED_OFF);
            set_leds(1, LED_ON, LED_OFF, LED_OFF);
            set_leds(2, LED_OFF, LED_OFF, LED_ON);
            set_leds(3, LED_OFF, LED_OFF, LED_ON);
        }
        break;

    default:
        break;
    }

    // ===== Hiển thị số đếm =====
    int display_num = 0;
    if(status == RED_GREEN || status == RED_YELLOW)
        display_num = counter_WE;
    else
        display_num = counter_NS;

    sevenseg_display_number(display_num);
    // ===== XỬ LÝ NÚT NHẤN =====
    switch(system_mode){
    case AUTO_MODE:
        if(is_button_pressed(MODE_BUTTON)){
            system_mode = SET_RED_TIME;
        }
        break;

    case SET_RED_TIME:
        sevenseg_display_number(red_duration / 100);
        if(is_button_pressed(INC_BUTTON)){
            red_duration += 100; // +1s
            if(red_duration > 9900) red_duration = 100; // reset nếu vượt
        }
        if(is_button_pressed(MODE_BUTTON)){
            system_mode = SET_YELLOW_TIME;
        }
        if(is_button_pressed(SET_BUTTON)){
            system_mode = AUTO_MODE;
            init_roads(); // áp dụng lại thời gian mới
        }
        break;

    case SET_YELLOW_TIME:
        sevenseg_display_number(yellow_duration / 100);
        if(is_button_pressed(INC_BUTTON)){
            yellow_duration += 100;
            if(yellow_duration > 9900) yellow_duration = 100;
        }
        if(is_button_pressed(MODE_BUTTON)){
            system_mode = SET_GREEN_TIME;
        }
        if(is_button_pressed(SET_BUTTON)){
            system_mode = AUTO_MODE;
            init_roads();
        }
        break;

    case SET_GREEN_TIME:
        sevenseg_display_number(green_duration / 100);
        if(is_button_pressed(INC_BUTTON)){
            green_duration += 100;
            if(green_duration > 9900) green_duration = 100;
        }
        if(is_button_pressed(MODE_BUTTON)){
            system_mode = AUTO_MODE;
        }
        if(is_button_pressed(SET_BUTTON)){
            system_mode = AUTO_MODE;
            init_roads();
        }
        break;
    }

}

// ====== Gọi mỗi 10ms trong ngắt timer ======
void app_tick_10ms(void){
    if(timer1 > 0) timer1--;

    static int tick_count = 0;
    tick_count++;

    if(tick_count >= 100){  // 100 tick * 10ms = 1s
        tick_count = 0;

        if(status == RED_GREEN || status == RED_YELLOW){
            if(counter_WE > 0) counter_WE--;
        } else {
            if(counter_NS > 0) counter_NS--;
        }
    }
}
