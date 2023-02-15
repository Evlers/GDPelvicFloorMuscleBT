#ifndef _BLE_COMMUN_H_
#define _BLE_COMMUN_H_
#include "hal_sys.h"
#include "hal_usart.h"



#define BLE_COM_FRAME_SIZE					32

#pragma pack(1)

typedef struct
{
    u8 				uuid[16];                               //设备端ID
    uint8_t 	length;                                 //数据长度
    uint8_t 	type;                                   //数据类型
    uint8_t 	mode;                                   //工作模式
    uint8_t 	freq;                                   //治疗频率
    uint16_t 	pulse;                                  //治疗脉冲宽度
    uint16_t 	speed_inc;                              //脉冲加速时间
    uint8_t 	opt_time;                               //脉冲运行时间
    uint8_t 	rest_time;                              //脉冲休息时间
    uint8_t 	emsval;                                 //脉冲强度值
    uint8_t 	press_val;                              //压力强度值
    uint8_t 	ext_val;                                //其他扩展值
    uint8_t 	timer;                                  //剩余工作时间
    uint8_t 	bat_val;                                //电池电量
    uint8_t 	sum;                                    //累加结果
}ble_com_frame_t;

#pragma pack()

void ble_com_init(void);
void ble_com_pack_recv(u8 *buf, u16 len);
void ble_com_send(ble_com_frame_t *f);
ble_com_frame_t *ble_com_recv(void);
#endif
