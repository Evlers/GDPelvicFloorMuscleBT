#include "at.h"
#include "string.h"
#include "Malloc.h"
#include "ucos_ii.h"
#include "hal_usart.h"
#include "delay.h"

//AT指令处理

at_t at;

static void at_receive_handle(u8 *buf,u16 len);

//初始化
void at_init(u32 baud)
{
	HalUSARTInit(AT_UART_PORT,baud,at_receive_handle);
	memset(&at,0,sizeof(at));
}

//设置波特率
//重新初始化串口
void at_set_baud(u32 baud)
{
	HalUSARTInit(AT_UART_PORT,baud,at_receive_handle);
}

//清除所有的AT指令应答
void at_clear_all(void)
{
	for(u8 i=0;i<AT_RSP_CMD_MAX_NUM;i++)
		if(at.at_rsp[i].len)
		{
			Free(at.at_rsp[i].buf);//释放内存
			memset(&at.at_rsp[i],0,sizeof(at_rsp_t));
		}
}

//AT指令处理 
void at_processing(void)
{
	u8 i,index;//关键字的缓存级首地址
	
	for(i=0;i<AT_RSP_CMD_MAX_NUM;i++)//删除超时的应答指令
		if(at.at_rsp[i].timeout == RESET && at.at_rsp[i].len)//如果该缓冲级的数据已经超时
		{
			Free(at.at_rsp[i].buf);//释放内存
			memset(&at.at_rsp[i],0,sizeof(at_rsp_t));
		}
		
	for(i=0;i<AT_WAIT_CMD_MAX_NUM;i++)//处理所有等待的AT应答
		if(at.at_wait[i].sta == AT_HAND_WAIT)//如果此缓冲内有AT指令在等待应答
		{
			if(at_search_cmd(at.at_wait[i].cmd,&index) == AT_DONE)//如果AT缓冲里出现等待的AT指令应答
			{
				at.at_wait[i].index = index;//复制出现关键字的缓存的索引
				at.at_wait[i].sta = AT_HAND_DONE;//标志等待的指令已经应答 让等待的进程去处理这个应答指令
			}
			else if(at.at_wait[i].timeout == RESET)//这次的检查没有找到等待的应答 如果这条等待的指令已经超时
				at.at_wait[i].sta = AT_HAND_TIMEOUT;//标记这条指令已经等待超时
		}
}

//需要回调的AT指令处理
void at_cmd_cb_hand(void)
{
	u8 i,index;
	AT_CB:
	for(i=0;i<AT_CB_CMD_MAX_NUM;i++)//检查有没有出现需要回调的AT指令
		if(at.cb[i].cb != RESET)//如果这个回调成立 
		{
			if(at_search_cmd(at.cb[i].cmd,&index) == AT_DONE)//如果AT缓冲里出现需要回调的AT指令
			{
				at.cb[i].cb(at_buf_get(index),at_buf_len_get(index));//回调
				at_free_buf(index);//删除此条AT指令
			}
		}
		
		
	for(i=0;i<AT_IGNORE_CMD_MAX_NUM;i++)//处理被忽略的AT指令集的应答
		if(at.ignore[i] != NULL)//如果此缓冲有地址则需要处理
		{
			if(at_search_cmd(at.ignore[i],&index) == AT_DONE)//如果AT应答缓冲级内有被忽略的AT指令出现
			{
				for(i=0;i<AT_CB_CMD_MAX_NUM;i++)//删除该指令之前先检查需要删除之后后面有没有出现需要回调的AT指令
					if(at.cb[i].cb != RESET)//如果这个回调成立 
					{
						if(strstr(at_buf_get(index),at.cb[i].cmd) != NULL)//如果里面搜索到有需要回调的指令
							goto AT_CB;//回去处理回调
					}
				at_free_buf(index);//删除此条AT指令
			}
		}
}

//发送AT指令
void at_send_cmd(char *fmt,...)  
{
	va_list ap;  
	char *string;
	string = Malloc(512);
	if(string == NULL)
		return ;
	va_start(ap,fmt);  
	vsprintf(string,fmt,ap);  
	HalUSARTWriteData(AT_UART_PORT,(u8 *)string,strlen(string));  
	va_end(ap);  
	Free(string);
}
//发送数据 
void at_send_data(u8 *buf,u16 len)
{
	HalUSARTWriteData(AT_UART_PORT,buf,len);  
}

//注册一个被忽视的AT指令 
//收到被注册过的指令后直接清除留出缓冲级空间
at_error at_cmd_ignore_register(char *cmd)
{
	for(u8 i=0;i<AT_IGNORE_CMD_MAX_NUM;i++)
	{
		if(at.ignore[i] == NULL)//如果此缓冲没被注册
		{
			at.ignore[i] = Malloc(strlen(cmd)+1);
			if(at.ignore[i] == NULL)return AT_ERROR;
			memset(at.ignore[i],0,strlen(cmd)+1);
			memcpy(at.ignore[i],cmd,strlen(cmd));
			return AT_DONE;
		}
	}
	return AT_FOUND;//没有找到空的缓冲级
}

//注销需要被忽略的AT指令
at_error at_cmd_ignore_cancel(char *cmd)
{
	for(u8 i=0;i<AT_IGNORE_CMD_MAX_NUM;i++)
	{
		if(at.ignore[i] != NULL)
		{
			if(memcmp(at.ignore[i],cmd,strlen(cmd)) == 0)//如果搜索到此AT指令 
			{
				Free(at.ignore[i]);
				at.ignore[i] = NULL;
				return AT_DONE;
			}
		}
	}
	return AT_FOUND;
}

//AT指令回调注册
//cmd 需要被回调的AT指令集 
//cb 需要回调的函数地址 
at_error at_cmd_cb_register(char *cmd,at_cmd_cb_t cb)
{
	u8 i;
	for(i=0;i<AT_CB_CMD_MAX_NUM;i++)//寻找没有回调的缓存级
	{
		if(at.cb[i].cb == NULL)//如果这个缓存级没有回调
		{
			at.cb[i].cmd = Malloc(strlen(cmd)+1);//为此缓存级指令地址分配内存
			if(at.cb[i].cmd == NULL)return AT_WAIT_FILL;
			memset(at.cb[i].cmd,0,strlen(cmd)+1);//清除内存
			memcpy(at.cb[i].cmd,cmd,strlen(cmd));//复制需要回调的AT指令
			at.cb[i].cb = cb;//复制回调函数地址
			return AT_DONE;
		}
	}
	return AT_FOUND;//没有找到空的缓存级
}

//AT指令回调注销
at_error at_cmd_cb_cancel(at_cmd_cb_t cb)
{
	u8 i;
	for(i=0;i<AT_CB_CMD_MAX_NUM;i++)//寻找已经被注册的回调
	{
		if(at.cb[i].cb == cb)//如果这个缓存级的回调一致
		{
			Free(at.cb[i].cmd);//收回保存指令的内存
			memset(&at.cb[i],0,sizeof(at_cb_t));//清除这个缓存级
			return AT_DONE;
		}
	}
	return AT_ERROR;
}

//等待两个指令 
//如果出现其中一个立刻退出并返回AT缓冲的index  配合at_buf_get函数查询缓冲首地址
at_error at_wait2_cmd(char *s,char *s2,u16 timeout,u8 *index)
{
	u8 one = 0xFF,two = 0xFF;//提取两个等待缓冲级
	at_error error;
	for(u8 i=0;i<AT_WAIT_CMD_MAX_NUM;i++)//寻找没有在等待应答的缓冲级
	{
		if(at.at_wait[i].sta == AT_HAND_NOT)//如果此级缓冲没有在等待应答
		{
			if(one == 0xFF)
			{
				at.at_wait[i].cmd = Malloc(strlen(s)+1);//分配内存
				if(at.at_wait[i].cmd == NULL)//如果没内存
					return AT_WAIT_FILL;
				memset(at.at_wait[i].cmd,0,strlen(s)+1);//清除内存
				memcpy(at.at_wait[i].cmd,s,strlen(s));//复制需要等待的AT应答 让esp task去处理
				at.at_wait[i].timeout = timeout;//设置设置指令的等待超时
				at.at_wait[i].sta = AT_HAND_WAIT;//标志等待状态
				one = i;
				break;
			}
		}
	}
	for(u8 i=0;i<AT_WAIT_CMD_MAX_NUM;i++)//寻找没有在等待应答的缓冲级
	{
		if(at.at_wait[i].sta == AT_HAND_NOT)//如果此级缓冲没有在等待应答
		{
			if(two == 0xFF)
			{
				at.at_wait[i].cmd = Malloc(strlen(s)+1);//分配内存
				if(at.at_wait[i].cmd == NULL)//如果没内存
					return AT_WAIT_FILL;
				memset(at.at_wait[i].cmd,0,strlen(s)+1);//清除内存
				memcpy(at.at_wait[i].cmd,s,strlen(s));//复制需要等待的AT应答 让esp task去处理
				at.at_wait[i].timeout = timeout;//设置设置指令的等待超时
				at.at_wait[i].sta = AT_HAND_WAIT;//标志等待状态
				two = i;
				break;
			}
		}
	}
	if(one != 0xFF && two != 0xFF)//如果有两个等待缓冲级
	{
		while(at.at_wait[one].sta == AT_HAND_WAIT && at.at_wait[two].sta == AT_HAND_WAIT)AT_DLY(1);//等待AT应答或者超时退出
		if(at.at_wait[one].sta != AT_HAND_WAIT)//如果one等待已经出现 
		{
			if(at.at_wait[one].sta == AT_HAND_DONE)//如果AT指令已经应答
			{
				if(index == NULL)//如果不需要索引
					at_free_buf(at.at_wait[one].index);//删除这个缓冲级的数据
				else
					*index = at.at_wait[one].index;//传递出现AT应答的缓冲级索引
				error = AT_DONE;
			}
			else if(at.at_wait[one].sta == AT_HAND_TIMEOUT)//如果AT指令等待超时
				error = AT_TIMEOUT;
		}
		else if(at.at_wait[two].sta != AT_HAND_WAIT)//如果two已经出现
		{
			if(at.at_wait[two].sta == AT_HAND_DONE)//如果AT指令已经应答
			{
				if(index == NULL)//如果不需要索引
					at_free_buf(at.at_wait[two].index);//删除这个缓冲级的数据
				else
					*index = at.at_wait[two].index;//传递出现AT应答的缓冲级索引
				error = AT_DONE;
			}
			else if(at.at_wait[two].sta == AT_HAND_TIMEOUT)//如果AT指令等待超时
				error = AT_TIMEOUT;
		}
		Free(at.at_wait[one].cmd);//释放内存
		memset(&at.at_wait[one],0,sizeof(at_wait_t));//清除这个等待缓冲
		Free(at.at_wait[two].cmd);//释放内存
		memset(&at.at_wait[two],0,sizeof(at_wait_t));//清除这个等待缓冲
		return error;
	}
	else //没有两个缓冲级  
	{//如果有一个申请了内存需要释放
		if(one != 0xff)
		{
			Free(at.at_wait[one].cmd);//释放内存
			memset(&at.at_wait[one],0,sizeof(at_wait_t));//清除这个等待缓冲
		}
		if(two != 0xFF)
		{
			Free(at.at_wait[two].cmd);//释放内存
			memset(&at.at_wait[two],0,sizeof(at_wait_t));//清除这个等待缓冲
		}
	}
	return AT_WAIT_FILL;//没有剩下的等待缓冲级
}

//等待AT指令应答
//如果有等到应答 index将是应答的AT指令缓冲级的索引 如果index是NULL则立刻删除AT缓冲级里的应答指令
at_error at_wait_cmd(char *s,u16 timeout,u8 *index)
{
	u8 i;
	at_error error;
	for(i=0;i<AT_WAIT_CMD_MAX_NUM;i++)//寻找没有在等待应答的缓冲级
	{
		if(at.at_wait[i].sta == AT_HAND_NOT)//如果此级缓冲没有在等待应答
		{
			at.at_wait[i].cmd = Malloc(strlen(s)+1);//分配内存
			if(at.at_wait[i].cmd == NULL)//如果没内存
				return AT_WAIT_FILL;
			memset(at.at_wait[i].cmd,0,strlen(s)+1);//清除内存
			memcpy(at.at_wait[i].cmd,s,strlen(s));//复制需要等待的AT应答 让esp task去处理
			at.at_wait[i].timeout = timeout;//设置设置指令的等待超时
			at.at_wait[i].sta = AT_HAND_WAIT;//标志等待状态
			while(at.at_wait[i].sta == AT_HAND_WAIT)AT_DLY(1);//等待AT应答或者超时退出
			if(at.at_wait[i].sta == AT_HAND_DONE)//如果AT指令已经应答
			{
				if(index == NULL)//如果不需要索引
					at_free_buf(at.at_wait[i].index);//删除这个缓冲级的数据
				else
					*index = at.at_wait[i].index;//传递出现AT应答的缓冲级索引
				error = AT_DONE;
			}
			else if(at.at_wait[i].sta == AT_HAND_TIMEOUT)//如果AT指令等待超时
				error = AT_TIMEOUT;
			Free(at.at_wait[i].cmd);//释放内存
			memset(&at.at_wait[i],0,sizeof(at_wait_t));//清除这个等待缓冲
			return error;
		}
	}
	return AT_WAIT_FILL;//没有剩下的等待缓冲级
}

//搜索AT缓冲里的数据
//参数keyword是写入需要搜索的关键词
//参数index 如果找到关键词则返回此关键词出现的AT指令应答的缓冲级索引
at_error at_search_cmd(char *keyword,u8 *index)
{
	u8 i;
	char *k_addr;
	for(i=0;i<AT_RSP_CMD_MAX_NUM;i++)//到每级的缓冲去寻找对应的AT指令
	{
		if(at.at_rsp[i].len != RESET)//检查此级缓冲是否有数据
		{
			k_addr = strstr(at.at_rsp[i].buf,keyword);//查找此级缓存是否有等待应答的AT指令
			if(k_addr != NULL)//如果此级缓冲里有在等待AT应答
			{
				*index = i;//返回在等待应答的AT指令的缓存级索引
				return AT_DONE;
			}
		}
	}
	return AT_FOUND;//没有搜寻到正再等待的AT应答
}

//ESP超时计数 1ms
void at_time_task(void)
{
	u8 i;
	for(i=0;i<AT_WAIT_CMD_MAX_NUM;i++)
	{
		if(at.at_wait[i].timeout)
			at.at_wait[i].timeout--;
	}
	for(i=0;i<AT_RSP_CMD_MAX_NUM;i++)
	{
		if(at.at_rsp[i].timeout)
			at.at_rsp[i].timeout--;
	}
}

//释放AT缓存级
void at_free_buf (u8 index)
{
	Free(at.at_rsp[index].buf);
	memset(&at.at_rsp[index],0,sizeof(at_rsp_t));
}

//查询缓冲级首地址
char *at_buf_get(u8 index)
{
	return at.at_rsp[index].buf;
}

//查询缓存级的长度
u16 at_buf_len_get(u8 index)
{
	return at.at_rsp[index].len;
}

//接收处理
static void at_receive_handle(u8 *buf,u16 len)
{
	u8 i;
	if(len < AT_BUF_LEN_MAX)
	{
		for(i=0;i<AT_RSP_CMD_MAX_NUM;i++)
		{
			if(at.at_rsp[i].len == RESET)//如果这个缓冲级没有被使用
			{
				at.at_rsp[i].buf = Malloc(len+1);
				if(at.at_rsp[i].buf == NULL)//如果没内存
					return ;
				memset(at.at_rsp[i].buf,0,len+1);
				memcpy(at.at_rsp[i].buf,buf,len);
				at.at_rsp[i].len = len;
				at.at_rsp[i].timeout = AT_RSP_TIMEOUT_TIME;
				break;
			}
		}
	}
}
