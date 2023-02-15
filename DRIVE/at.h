#ifndef _AT_H_
#define _AT_H_
#include "stm32f10x.h"
#define AT_UART_PORT										USART_PORT1
#define AT_WAIT_CMD_MAX_NUM							10//最多同时等待应答的AT指令
#define AT_RSP_CMD_MAX_NUM							15//最大的应答命令的缓冲数量
#define AT_CB_CMD_MAX_NUM								10//最大需要回调的AT指令数量
#define AT_IGNORE_CMD_MAX_NUM						10//最多被忽视的AT指令数量

#define AT_RSP_TIMEOUT_TIME							30000//缓冲超时时间
#define AT_BUF_LEN_MAX									1024//缓冲最大长度
#define AT_DLY													OSTimeDly//AT等待时候的会不断回调这个函数

typedef void (*at_cmd_cb_t)(void *buf,u16 len);

typedef enum{
AT_DONE=0,//完成
AT_ERROR,//错误的
AT_UNKNOWN,//未知
AT_TIMEOUT,//超时
AT_WAIT_FILL,//AT等待缓冲已满
AT_FOUND,//没有找到相关的AT指令
}at_error;

typedef enum{
AT_HAND_NOT = 0,//没有要等待的AT指令
AT_HAND_WAIT = 1,//正在等待应答
AT_HAND_DONE = 2,//等待的AT指令已经应答
AT_HAND_TIMEOUT = 3,//等待的AT指令已经超时
}at_sta;

typedef struct{
u16 timeout;//每个缓冲级都有个超时 未处理的则清除
char *buf;//AT指令缓冲
u16 len;//AT指令长度
}at_rsp_t;//应答的AT指令

typedef struct{
u16 timeout;//超时倒计时
char *cmd;//用来保存正在等待应答的AT指令(最多同时等待10条AT应答)
at_sta sta;//此AT指令的状态
u8 index;//如果等待完成 这个结构体将成为出现应答的缓存级索引
}at_wait_t;//保存正在等待应答的指令

typedef struct{
char *cmd;//需要被回调的指令
at_cmd_cb_t cb;//AT指令回调 (例如出现+IPD时回调)
}at_cb_t;

typedef struct{
at_rsp_t at_rsp[AT_RSP_CMD_MAX_NUM];//保存接收到的AT应答
at_wait_t at_wait[AT_WAIT_CMD_MAX_NUM]; //正在等待应答的AT指令
at_cb_t cb[AT_CB_CMD_MAX_NUM];//AT指令回调
char *ignore[AT_IGNORE_CMD_MAX_NUM];//被忽视的AT指令  (收到被忽视的指令后立即清除)
}at_t;


void at_init(u32 baud);
void at_set_baud(u32 baud);
void at_processing(void);
void at_cmd_cb_hand(void);
void at_time_task(void);

void at_free_buf (u8 index);
char *at_buf_get(u8 index);
u16 at_buf_len_get(u8 index);
at_error at_search_cmd(char *keyword,u8 *index);

void at_clear_all(void);
void at_send_cmd(char *fmt,...);
void at_send_data(u8 *buf,u16 len);
at_error at_wait_cmd(char *s,u16 timeout,u8 *index);
at_error at_wait2_cmd(char *s,char *s2,u16 timeout,u8 *index);

at_error at_cmd_cb_cancel(at_cmd_cb_t cb);
at_error at_cmd_cb_register(char *cmd,at_cmd_cb_t cb);
at_error at_cmd_ignore_register(char *cmd);
at_error at_cmd_ignore_cancel(char *cmd);
#endif
