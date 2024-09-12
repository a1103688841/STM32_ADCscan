#ifndef _LED_HANDLE_H_
#define _LED_HANDLE_H_
#include <stdio.h>

#ifdef _LED_HANDLE_C_
	#define LED_HANDLE_EXTERN
#else
	#define LED_HANDLE_EXTERN		extern
#endif

typedef struct led_func
{
    int cnt_max;
    int input1_cnt;
    int input2_cnt;
    int input3_cnt;
    void (*input1_func)(int);  // input1
    void (*input2_func)(int);  // input2
    void (*input3_func)(int);  // input3
}led_func_t;

LED_HANDLE_EXTERN led_func_t led_driver;

extern void led_init();
extern void led_handel();

#endif

