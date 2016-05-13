#include "pc_protocol.h"
#include "util.h"
#include "usbd_cdc_vcp.h"
#include "servo_protocol.h"
#include "xmodem_hw.h"
#include <string.h>
#include "delay.h"

//#include "printf.h"

//发送返回数据包到PC，包含所有舵机的状态和当前角度
#define RETURN_SIZE	83
void sendReturnData(void)
{
	uint8_t buf[RETURN_SIZE];
	uint8_t i;
	uint16_t counter = 0;
	buf[counter++] = 0XFF;
	buf[counter++] = 0XFF;
	for(i=0; i<16; i++){
		buf[counter++] = servoState[i];
		buf[counter++] = servoAngle[i]>>8;
		buf[counter++] = servoAngle[i];
		buf[counter++] = servoSpeed[i]>>8;
		buf[counter++] = servoSpeed[i];
	}
	buf[counter++] = checkSum(&buf[2], RETURN_SIZE-3);
	VCP_DataTx(buf, RETURN_SIZE);
}

//虚拟串口数据接收回调函数
#define REV_SIZE_CMD 	68
#define REV_SIZE_READ 	4
void VCP_DataHandler(uint8_t *buf, uint32_t len)
{
	uint32_t i;
	static uint32_t length = 0;
	static uint8_t rev[100] = {0};
	memcpy(&rev[length], buf, len);
	length += len;
	if(rev[0] != 0XFF){
		length = 0;
		return;
	}
	if(length == 4 && rev[1] == 0XFE){
		length = 0;
		//读数据帧
		servoSendHandValue(100);
		delay_ms(10);
		for(i=0; i<16; i++){
			servoSendRead(i, CONTENT_CUR_ANGLE);
			xm_timer_start(100);
			while(1){
				if(xm_is_timeout()){
					servoAngle[i] = 0XFFFF;
					break;
				}
				if(revFlag){
					xm_timer_stop();
					revFlag = 0;
					servoAngle[i] = revPara[0];
					break;
				}
			}
		}
		sendReturnData();
	}
	else if(length == 68 && rev[1] == 0XFF){
		length = 0;
		//命令帧
		for(i=0; i<16; i++){
			servoAngle[i] = rev[3+4*i]<<8|rev[4+4*i];
			servoSpeed[i] = rev[5+4*i]<<8|rev[6+4*i];
		}
		servoSendMotion();
	}
	else if(length > 68){
		length = 0;
	}
}


