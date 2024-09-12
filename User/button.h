#ifndef __BUTTON_H__
#define __BUTTON_H__

// default
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>  //memset() strcmp() fuction

#ifndef boolean_t
#define boolean_t bool
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef LOW
#define LOW 0
#endif
#ifndef HIGHT
#define HIGHT 1
#endif

//==================================modify define/typedef/enum=================================

/* 	按键消抖时间40ms, 建议调用周期为20ms
    只有连续检测到40ms状态不变才认为有效，包括弹起和按下两种事件*/
#define CONTINUOS_TRIGGER         0  // 是否支持连续触发，连发的话就不要检测单双击与长按了

/* 	是否支持单击&双击同时存在触发，如果选择开启宏定义的话，单双击都回调，只不过单击会延迟响应，
    因为必须判断单击之后是否触发了双击否则，延迟时间是双击间隔时间 BUTTON_DOUBLE_TIME。
    而如果不开启这个宏定义，建议工程中只存在单击/双击中的一个，否则，在双击响应的时候会触发一次单击，
    因为双击必须是有一次按下并且释放之后才产生的 */
#define SINGLE_AND_DOUBLE_TRIGGER 0

/* 是否支持长按释放才触发，如果打开这个宏定义，那么长按释放之后才触发单次长按，
   否则在长按指定时间就一直触发长按，触发周期由 BUTTON_LONG_CYCLE 决定 */
#define LONG_FREE_TRIGGER         0

#define BUTTON_CYCEL              20
#define BUTTON_LEVEL_STABLE       20 / BUTTON_CYCEL    // 消抖时间      (1)*调用周期
#define BUTTON_LONG_TIME          2000 / BUTTON_CYCEL  /* 持续n秒((n-1)*调用周期 ms)，认为长按事件 */
#define BUTTON_LONG_CYCLE         2000 / BUTTON_CYCEL  // 长按触发周期时间  (n-1)*调用周期

#define BUTTON_CONTINUOS_CYCLE    1   // 连按触发周期时间  (n-1)*调用周期
#define BUTTON_DOUBLE_TIME        15  // 双击间隔时间  (n-1)*调用周期  建议在200-600ms

// 备注：长按一定会连续触发

////按键名字属性
// button name str max lens
#define BTN_NAME_MAX              5

//==========================================================================================
typedef enum
{
    SW_NONE = 0,
    SW0_DOWN,
    SW0_LONG,
    SW1_DOWN,
    SW1_LONG,
    SW2_DOWN,
    SW2_LONG,
    SW3_DOWN,
    SW3_LONG,
    SW_OTHER,
} SW_RECORDCODING_E;
// 按键记录参数修改
// sw record's bin len = btn_idx+1
#define BTNRECORDMAX 4
// record sw event
typedef struct
{  // btn_len = btn_idx+1
    SW_RECORDCODING_E bin[BTNRECORDMAX + 1];
    int16_t           i;
} SW_RECORDFIFO_S;

extern SW_RECORDFIFO_S sw_record;

//=========================================================================================

// btn->CallBack_Function[event]是的函数的指针。简称函数指针数组。
#define TRIGGER_CB(event)              \
    if (btn->CallBack_Function[event]) \
    btn->CallBack_Function[event]((button_t*)btn)
// function pointer
typedef void (*Button_CallBack)(void*);

// trigger event array
typedef enum
{
    BUTTON_DOWN = 0,
    BUTTON_UP,
    BUTTON_DOUBLE_WAIT,
    BUTTON_DOUBLE,
    BUTTON_LONG,
    BUTTON_LONG_FREE,
    BUTTON_CONTINUOS,
    // 这个没写
    BUTTON_CONTINUOS_FREE,
    BUTTON_ALL_RIGGER,
    number_of_event,
    NONE_TRIGGER,
} Button_Event;

/*
    每个按键对应1个全局的结构体变量。
    其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct button
{
    /* 下面是一个函数指针，指向判断按键手否按下的函数 */
    uint8_t (*Read_Button_Level)(void); /* 读取按键电平函数，需要用户实现 */

    char Name[BTN_NAME_MAX];

    uint8_t Button_State;             /* 按键当前状态（按下还是弹起） */
    uint8_t Button_Trigger_Level : 2; /* 按键触发电平 */
    uint8_t Button_Last_Level : 2;    /* 按键当前电平 */

    uint8_t Button_Trigger_Event; /* 按键触发事件，单击，双击，长按等 */

    Button_CallBack CallBack_Function[number_of_event]; /*这是一组函数指针数组，用于存放触发事件的函数头地址*/

    uint8_t cycle_timercnt;  // 连续触发计数
    uint8_t level_timercnt;  // 电平消抖计时
    uint8_t double_timecnt;  // 双击间隔计数
    uint8_t down_timecnt;    // 按键按下持续计数

    struct button* Next;
} button_t;

/* ------------------------------------------- varivable ----------------------------------------- */
#ifdef __BUTTON_C__
#define BUTTON_EXT
#else
#define BUTTON_EXT extern
#endif
BUTTON_EXT button_t sw0;
BUTTON_EXT button_t sw1;
/* ------------------------------------------- funtion ----------------------------------------- */
extern void Button_Attach_ini(void);
extern void Button_Process(void);
/*-------------------------------------------- funtion --------------------------------------------*/
extern boolean_t Button_Attach(button_t* btn, Button_Event btn_event, Button_CallBack btn_callback);
extern boolean_t Button_Create(const char* name, button_t* btn, uint8_t (*read_btn_level)(void));
extern void      Add_Button(button_t* btn);
extern void      Button_Delete(button_t* btn);

#endif /*  __BUTTON_H__  */
