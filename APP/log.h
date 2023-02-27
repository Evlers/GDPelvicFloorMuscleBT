#ifndef _LOG_H_
#define _LOG_H_
#include "hal_sys.h"
#include "SEGGER_RTT.h"

#ifndef NDEBUG

#define _LOG(M, ...)	log_rtt_printf(0, M"\r\n", __VA_ARGS__)
#define __LOG(M, ...)	log_rtt_printf(0, M, __VA_ARGS__)
#define LOG(...)			__LOG(__VA_ARGS__, "")
#define LOG_D(...)		_LOG(__VA_ARGS__, "");
#define LOG_B(str, buf, len, line)	log_printf_buf(str, buf, len, line, __FUNCTION__)

#else

#define _LOG(M, ...)
#define __LOG(M, ...)
#define LOG(...)
#define LOG_D(...)
#define LOG_B(str, buf, len, line)

#endif

int log_rtt_printf(unsigned BufferIndex, const char * sFormat, ...);
void log_printf_buf(char *str, u8 *buf, u32 len, u8 line, const char *fun);
#endif
