#include "log.h"
#include "stdio.h"

static char printfBuf[1024];

int log_rtt_printf(unsigned BufferIndex, const char * sFormat, ...)
{
  int r;
  va_list ParamList;

  va_start(ParamList, sFormat);
  r = vsnprintf(printfBuf, sizeof(printfBuf), sFormat, ParamList);
	SEGGER_RTT_Write(0, printfBuf, r);
  va_end(ParamList);
  return r;
}

void log_printf_buf(char *str, u8 *buf, u32 len, u8 line, const char *fun)
{
	__disable_irq();
	LOG("%s:  %s(%u):", fun, str, len);
	for(u32 i=0;i<len;i++){
		if(!(i % line))LOG("\r\n");
		LOG("%02X ", buf[i]);
	}
	LOG("\r\n");
	__enable_irq();
}
