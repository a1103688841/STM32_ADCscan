/******************************************
 * @Author:RNF
 * @Date:2024-07-28 00:24:30
 * @LastEditTime:2024-07-28 15:27:54
 * @LastEditors:XG
 * @Description: Button handling functions for an STM32-based system
 * @FilePath:\STM32Fans\User\button.c
 ******************************************/

#define __BUTTON_C__

/* ------------------------------------------- include ----------------------------------------- */
#include "button.h"

/* ------------------------------------- define/typedef/enum ----------------------------------- */

/* ------------------------------------------- variable ---------------------------------------- */
// Button linked list head pointer
static struct button* Head_Button = NULL;

/* -------------------------------------------static function ----------------------------------------- */
static char* StrnCopy(char* dst, const char* src, unsigned int n);
static void  Button_Cycle_Process(button_t* btn);

/* --------------------------------- Button initialization and callbacks --------------------------------- */
#include "../Core/Inc/gpio.h"

// Read the level of button SW0
static uint8_t Read_SW0_Level(void)
{
    // GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    // return (pinState == GPIO_PIN_SET) ? 1 : 0;
    return 1;
}

// Read the level of button SW1
static uint8_t Read_SW1_Level(void)
{
    // GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    // return (pinState == GPIO_PIN_SET) ? 1 : 0;
    return 1;
}

extern void pause_button_down_callcak();
extern void rst_button_down_callcak();

// Initialize and attach button event callbacks
void Button_Attach_ini(void)
{
    // Initialize and attach callbacks for button SW0
    Button_Create("pause", &sw0, Read_SW0_Level);
    Button_Attach(&sw0, BUTTON_DOWN, pause_button_down_callcak);

    // Initialize and attach callbacks for button SW1
    Button_Create("reset", &sw1, Read_SW1_Level);
    Button_Attach(&sw1, BUTTON_DOWN, rst_button_down_callcak);
}
void Button_Process(void)
{
    struct button* pass_btn;
    for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
    {
        Button_Cycle_Process(pass_btn);
    }
}

/* ------------------------------------ internal functions ----------------------------------------- */
/************************************************************
 * @brief   Button state machine process for cycling through button states
 * @param   btn: The button object
 * @return  None
 ***********************************************************/
void Button_Cycle_Process(button_t* btn)
{
    // Read the current button level
    uint8_t current_level = (uint8_t)btn->Read_Button_Level();
    if ((current_level != btn->Button_Last_Level) && (++(btn->level_timercnt) > BUTTON_LEVEL_STABLE))
    {
        btn->Button_Last_Level = current_level;
        btn->level_timercnt    = 0;

        // Update button state based on the level
        if (btn->Button_State == NONE_TRIGGER && btn->Button_Last_Level == btn->Button_Trigger_Level)
        {
            btn->Button_State = BUTTON_DOWN;
        }
        else if (btn->Button_State == BUTTON_DOWN)
        {
            btn->Button_State = BUTTON_UP;
        }
#if SINGLE_AND_DOUBLE_TRIGGER == 1
        else if (btn->Button_State == BUTTON_DOUBLE_WAIT)
        {
            btn->Button_State = BUTTON_DOWN;
        }
#endif
    }
    else if (current_level == btn->Button_Last_Level)
    {
        btn->level_timercnt = 0;
    }

    // Handle button state transitions
    switch (btn->Button_State)
    {
    case BUTTON_DOWN: {
        if (btn->Button_Last_Level == btn->Button_Trigger_Level)
        {
#if CONTINUOS_TRIGGER
            // Handle continuous button press events
            if (++(btn->cycle_timercnt) >= BUTTON_CONTINUOS_CYCLE)
            {
                btn->cycle_timercnt       = 0;
                btn->Button_Trigger_Event = BUTTON_CONTINUOS;
                TRIGGER_CB(BUTTON_CONTINUOS);
            }
#else
            btn->down_timecnt++;
            if (btn->down_timecnt < BUTTON_LONG_TIME && btn->Button_Trigger_Event != BUTTON_DOUBLE_WAIT)
            {
                btn->Button_Trigger_Event = BUTTON_DOWN;
            }
#if LONG_FREE_TRIGGER == 1
            else if (btn->down_timecnt == BUTTON_LONG_TIME)
            {
                btn->Button_Trigger_Event = BUTTON_LONG_FREE;
            }
#else
            else if (btn->down_timecnt == BUTTON_LONG_TIME)
            {
                btn->Button_Trigger_Event = BUTTON_LONG;
                TRIGGER_CB(BUTTON_LONG);
            }
            else if (btn->down_timecnt > BUTTON_LONG_TIME)
            {
                if (++(btn->cycle_timercnt) >= BUTTON_LONG_CYCLE)
                {
                    btn->cycle_timercnt       = 0;
                    btn->Button_Trigger_Event = BUTTON_LONG;
                    TRIGGER_CB(BUTTON_LONG);
                }
            }
#endif  // End of LONG_FREE_TRIGGER
            if (btn->down_timecnt == 0xFF)
            {
                btn->down_timecnt = BUTTON_LONG_TIME + 1;
            }
#endif  // End of CONTINUOS_TRIGGER
        }
        break;
    }
    case BUTTON_UP: {
        // Handle button release events
        if (btn->Button_Trigger_Event == BUTTON_DOWN)
        {
#if (SINGLE_AND_DOUBLE_TRIGGER == 0)
            TRIGGER_CB(BUTTON_DOWN);
            btn->down_timecnt         = 0;
            btn->Button_State         = NONE_TRIGGER;
            btn->Button_Trigger_Event = NONE_TRIGGER;
#else
            if (btn->Button_Trigger_Event == BUTTON_DOWN)
            {
                btn->down_timecnt         = 0;
                btn->cycle_timercnt       = 0;
                btn->double_timecnt       = 0;
                btn->Button_State         = BUTTON_DOUBLE_WAIT;
                btn->Button_Trigger_Event = BUTTON_DOUBLE_WAIT;
            }
            else if (btn->Button_Trigger_Event == BUTTON_DOUBLE_WAIT)
            {
                btn->down_timecnt   = 0;
                btn->cycle_timercnt = 0;
                btn->double_timecnt = 0;
                TRIGGER_CB(BUTTON_DOUBLE);
                btn->Button_Last_State    = BUTTON_DOUBLE;
                btn->Button_State         = NONE_TRIGGER;
                btn->Button_Trigger_Event = NONE_TRIGGER;
            }
#endif
        }
        else if (btn->Button_Trigger_Event == BUTTON_CONTINUOS)
        {
            btn->down_timecnt         = 0;
            btn->cycle_timercnt       = 0;
            btn->Button_State         = NONE_TRIGGER;
            btn->Button_Trigger_Event = NONE_TRIGGER;
        }
        else if (btn->Button_Trigger_Event == BUTTON_LONG)
        {
            TRIGGER_CB(BUTTON_LONG_FREE);
            btn->down_timecnt         = 0;
            btn->cycle_timercnt       = 0;
            btn->Button_State         = NONE_TRIGGER;
            btn->Button_Trigger_Event = NONE_TRIGGER;
        }
#if LONG_FREE_TRIGGER == 1
        else if (btn->Button_Trigger_Event == BUTTON_LONG_FREE)
        {
            TRIGGER_CB(BUTTON_LONG_FREE);
            btn->down_timecnt         = 0;
            btn->cycle_timercnt       = 0;
            btn->Button_Last_State    = BUTTON_LONG_FREE;
            btn->Button_State         = NONE_TRIGGER;
            btn->Button_Trigger_Event = NONE_TRIGGER;
        }
#endif
#if CONTINUOS_TRIGGER
        else if (btn->Button_Trigger_Event == BUTTON_CONTINUOS)
        {
            btn->Long_Time = 0;
            TRIGGER_CB(BUTTON_CONTINUOS_FREE);
            btn->Button_State      = NONE_TRIGGER;
            btn->Button_Last_State = BUTTON_CONTINUOS;
        }
#endif

        break;
    }
#if (SINGLE_AND_DOUBLE_TRIGGER == 1)
    case BUTTON_DOUBLE_WAIT: {
        // Handle double click events
        btn->double_timecnt++;
        if (btn->double_timecnt >= BUTTON_DOUBLE_TIME)
        {
            btn->double_timecnt       = 0;
            btn->Button_Trigger_Event = BUTTON_DOWN;
            TRIGGER_CB(BUTTON_DOWN);
            btn->Button_State         = NONE_TRIGGER;
            btn->Button_Trigger_Event = NONE_TRIGGER;
        }
        break;
    }
#endif
    default: {
        break;
    }
    }  // end of switch
}  // end of void Button_Cycle_Process(button_t* btn)

/**************************************************************************************************
 * function	: Add_Button
 * brief	: Add a button to the linked list
 **************************************************************************************************/
void Add_Button(button_t* btn)
{
    struct button* pass_btn = Head_Button;

    while (pass_btn)
    {
        pass_btn = pass_btn->Next;
    }

    btn->Next   = Head_Button;
    Head_Button = btn;
}

/**************************************************************************************************
 * function	: Button_Delete
 * brief	: Remove a button from the linked list
 **************************************************************************************************/
void Button_Delete(button_t* btn)
{
    struct button** curr;
    for (curr = &Head_Button; *curr;)
    {
        struct button* entry = *curr;
        if (entry == btn)
        {
            *curr = entry->Next;
        }
        else
        {
            curr = &entry->Next;
        }
    }
}

/**************************************************************************************************
 * function	: Button_Create
 * brief	: Create a new button and add it to the list
 **************************************************************************************************/
boolean_t Button_Create(const char* name, button_t* btn, uint8_t (*read_btn_level)(void))
{
    if (btn == NULL)
    {
        return FALSE;
    }

    // Initialize button properties
    memset(btn, 0, sizeof(struct button));
    StrnCopy(btn->Name, name, BTN_NAME_MAX);

    btn->Button_State         = NONE_TRIGGER;
    btn->Button_Trigger_Event = NONE_TRIGGER;
    btn->Read_Button_Level    = read_btn_level;
    btn->Button_Trigger_Level = HIGHT;
    btn->Button_Last_Level    = btn->Read_Button_Level();
    btn->level_timercnt       = 0;

    // Add to the button list
    Add_Button(btn);

    return TRUE;
}

/**************************************************************************************************
 * function	: Button_Attach
 * brief	: Attach a callback to a button event
 **************************************************************************************************/
boolean_t Button_Attach(button_t* btn, Button_Event btn_event, Button_CallBack btn_callback)
{
    uint8_t i = 0;
    if (btn == NULL)
    {
        return FALSE;
    }

    if (BUTTON_ALL_RIGGER == btn_event)
    {
        for (i = 0; i < number_of_event - 1; i++)
            btn->CallBack_Function[i] = btn_callback;
    }
    else
    {
        btn->CallBack_Function[btn_event] = btn_callback;
    }
    return TRUE;
}

/**************************************************************************************************
 * function	: StrnCopy
 * brief	: Safely copy a string with a given length
 **************************************************************************************************/
static char* StrnCopy(char* dst, const char* src, uint32_t n)
{
    if (n != 0)
    {
        char*       d = dst;
        const char* s = src;
        do
        {
            if ((*d++ = *s++) == 0)
            {
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }
    return (dst);
}
