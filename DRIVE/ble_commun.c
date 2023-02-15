#include "ble_commun.h"
#include "hal_time.h"
#include "checkSum.h"
#include "log.h"
#include "ble.h"

#include <stdbool.h>
#include <string.h>

static u16 cnt;
static u32 timeout;
static u8 recvBuf[sizeof(ble_com_frame_t)];
static ble_com_frame_t frame;
static bool recvFlag = false;
volatile uint32_t com_pack_count = 0;

static void timecallback(void);
static bool checkFrame(ble_com_frame_t *frame, u16 len);


void ble_com_init(void)
{
	HalTimeRegisteringCallbacks(timecallback, 1);
	if(sizeof(ble_com_frame_t) != BLE_COM_FRAME_SIZE){
		LOG_D("ble_com_frame_t size error !!! %u != %u", sizeof(ble_com_frame_t), BLE_COM_FRAME_SIZE);
	}
}

void ble_com_send(ble_com_frame_t *f)
{
	f->length = 0x0F;
	f->sum = AccumulationCheck((u8 *)&f->length, f->length - 1);
	ble_send((u8 *)f, sizeof(ble_com_frame_t));
}

ble_com_frame_t *ble_com_recv(void)
{
	if(recvFlag == true){
		recvFlag = false;
		return &frame;
	} else {
		return NULL;
	}
}

void ble_com_pack_recv(u8 *buf, u16 len)
{
	timeout = 500;
	
	memcpy(&recvBuf[cnt], buf, len);
	cnt += len;
	com_pack_count ++;
	if(cnt == BLE_COM_FRAME_SIZE){
		ble_com_frame_t *f = (ble_com_frame_t *)recvBuf;
		if(checkFrame(f, len)){
			memcpy(&frame, recvBuf, sizeof(frame));
			recvFlag = true;
		}
	}
}

static bool checkFrame(ble_com_frame_t *frame, u16 len)
{
	u8 *buf = (u8 *)&frame->length;
	len -= sizeof(frame->uuid) + sizeof(frame->length);
	if(frame->length != len)return false;
	return (AccumulationCheck(buf, frame->length) == frame->sum) ? true : false;
}

static void timecallback(void)
{
	if(timeout){
		timeout--;
		if(timeout == 0){
			cnt = 0;
		}
	}
}
