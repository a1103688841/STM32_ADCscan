#define _LED_HANDLE_C_
#include "led_handle.h"
#include "dbg_cmd.h"

#ifdef DBG_CMD_EN
#define CMD_NAME "input"

static void demo_msg()
{
    DBG_CMD_PRN("demo test\r\n");
}

static char dbg_cmd_func(void)
{
    if (dbg_cmd_exec("help", "", ""))
    {
        DBG_CMD_PRN("." CMD_NAME "\r\n");
        return false;
    }
    if (dbg_cmd_exec("." CMD_NAME, "", ""))
    {
        dbg_cmd_print_msg_en();
    }
    if (dbg_cmd_exec(CMD_NAME "demo", "", ""))
    {
#ifdef MODULE_PRINT_INFO_EN
        DBG_CMD_PRN("PrnLevel  :%d\r\n", prn_level);
#endif
        demo_msg();
        return true;
    }
    if (dbg_cmd_exec( "s1", "", ""))
    {
        led_driver.input1_cnt = 0;
        led_driver.input1_func(0);
        return true;
    }
    if (dbg_cmd_exec( "s2", "", ""))
    {
        led_driver.input2_cnt = 0;
        led_driver.input2_func(0);
        return true;
    }
    if (dbg_cmd_exec( "s3", "", ""))
    {
        led_driver.input3_cnt = 0;
        led_driver.input3_func(0);
        return true;
    }
#ifdef MODULE_PRINT_INFO_EN
    if (dbg_cmd_exec(CMD_NAME "Prf", "1", "<0~1> Prn Level"))
    {
        prn_level = get_param_char(0);
        return true;
    }
#endif
    return false;
}
static void input_init(void)
{
    dbg_cmd_add_list((long)dbg_cmd_func);
}
#else   // DBG_CMD_EN

static void input_init(void) {}
#endif  // NORMAL

void led_init()
{
    input_init();
    led_driver.cnt_max = 300;
}

void led_handel()
{
    if (led_driver.input1_cnt < led_driver.cnt_max)
    {
        led_driver.input1_cnt++;
    }
    else if (led_driver.input1_cnt == led_driver.cnt_max)
    {
        led_driver.input1_cnt++;
        led_driver.input1_func(1);
    }
    if (led_driver.input2_cnt < led_driver.cnt_max)
    {
        led_driver.input2_cnt++;
    }
    else if (led_driver.input2_cnt == led_driver.cnt_max)
    {
        led_driver.input2_cnt++;
        led_driver.input2_func(1);
    }
    if (led_driver.input3_cnt < led_driver.cnt_max)
    {
        led_driver.input3_cnt++;
    }
    else if (led_driver.input3_cnt == led_driver.cnt_max)
    {
        led_driver.input3_cnt++;
        led_driver.input3_func(1);
    }
}