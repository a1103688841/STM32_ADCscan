```c
#include "dbg_cmd.h" /* Switch command line functions */
#ifdef DBG_CMD_EN
#define CMD_NAME "input"

static void module_msg()
{
    const char InputName[][8] = {"V", "xA", "A",};

    DBG_CMD_PRN("Filter(%d)\r\n", obj.p_cfg->c_input_filter_num);
    DBG_CMD_PRN("Num InVal Name\r\n");
    for (int i = 0; i < obj.input_num; i++) {
        DBG_CMD_PRN("%d>:  (%x)  %s\r\n", i, obj.p_input[i].input_state, InputName[i]);
    }

}

static char dbg_cmd_func(void)
{
    if (dbg_cmd_exec("help", "", "")) {
        DBG_CMD_PRN("."CMD_NAME"\r\n");
        return false;
    }
    if (dbg_cmd_exec("."CMD_NAME, "", "")) {
        dbg_cmd_print_msg_en();
    }
    if (dbg_cmd_exec(CMD_NAME"Msg", "", "")) {
#ifdef MODULE_PRINT_INFO_EN
        DBG_CMD_PRN("PrnLevel  :%d\r\n", prn_level);
#endif
        module_msg();
        return true;
    }
#ifdef MODULE_PRINT_INFO_EN
    if (dbg_cmd_exec(CMD_NAME"Prf", "1", "<0~1> Prn Level")) {
        prn_level = get_param_char(0);
        return true;
    }
#endif
    return false;
}
#endif // DBG_CMD_EN
```

```c
static void input_init(void)
{
#ifdef DBG_CMD_EN
    dbg_cmd_add_list((long)dbg_cmd_func);
#endif
}
```

