#ifndef _UART_FRAME_H
#define _UART_FRAME_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET 15 //software reset will be performed
#define CAM_PIN_XCLK 27
#ifdef CONFIG_TIMER_CAMERA_X_F
#define CAM_PIN_SIOD 25
#else
#define CAM_PIN_SIOD 22
#endif
#define CAM_PIN_SIOC 23

#define CAM_PIN_D7 19
#define CAM_PIN_D6 36
#define CAM_PIN_D5 18
#define CAM_PIN_D4 39
#define CAM_PIN_D3 5
#define CAM_PIN_D2 34
#define CAM_PIN_D1 35
#define CAM_PIN_D0 32


#if defined(CONFIG_TIMER_CAMERA_X_F)
#define CAM_PIN_VSYNC 22
#else
#define CAM_PIN_VSYNC 25
#endif
#define CAM_PIN_HREF 26
#define CAM_PIN_PCLK 21

#if defined(CONFIG_TIMER_CAMERA_X_F)
#define BLINK_LED_PIN 2
#define BLINK_LED_LEDC_CHANNEL LEDC_CHANNEL_0

#define BAT_HOLD_PIN 33
#define BAT_ADC_PIN  38
#define BAT_ADC_CHANNEL ADC1_CHANNEL_2

#define PWR_BTN_PIN  37

#elif defined(CONFIG_UNIT_CAM)
#define BLINK_LED_PIN 2
#define BLINK_LED_LEDC_CHANNEL LEDC_CHANNEL_0

#define BAT_HOLD_PIN 33
#define BAT_ADC_PIN  38
#define BAT_ADC_CHANNEL ADC1_CHANNEL_2

#define PWR_BTN_PIN  37

#else
#define BLINK_LED_PIN 14
#define BLINK_LED_LEDC_CHANNEL LEDC_CHANNEL_0
#endif

typedef enum {
    IDLE = 0x00,
    WAIT_FINISH ,
    FINISH,
} frame_state_n;

extern volatile frame_state_n frame_state;

void uart_frame_init(int32_t rx, int32_t tx, int baud);
void uart_frame_send(uint8_t cmd, const uint8_t* frame, uint32_t len, bool wait_finish);
void uart_update_pin(int32_t rx, int32_t tx);

#endif