#include "servo_protocol.h"
#include "util.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>

//#include "printf.h"

uint8_t currId = 0;
uint8_t revFlag = 0;
uint16_t revPara[2] = {3, 4};

uint8_t servoState[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
uint16_t servoAngle[16] = {0};
uint16_t servoSpeed[16] = {0};

#define REV_SIZE	8
void servoDataHandler(uint8_t dat)
{
	static u8 rev[REV_SIZE] = {0};
	static u8 count = 0;
	rev[count] = dat;

	if(rev[0] != 0XFF && count == 0){
		return;
	}
	count++;
	if(count == 2 && rev[1] != 0XFF){
		count = 0;
		return;
	}
	if(count == REV_SIZE){
		count = 0;
		revPara[0] = rev[3]<<8|rev[4];
		revPara[1] = rev[5]<<8|rev[6];
		revFlag = 1;
	}
}

void servoInit(void)
{
	USART1_HalfDuplexInit(115200, servoDataHandler);
}

#define MOTION_FRAME_SIZE	256
void servoSendMotion(void)
{
	uint16_t i;
	uint16_t counter = 0;
	uint8_t buf[MOTION_FRAME_SIZE] = {0};
	uint16_t select = 0XFFFF;
	uint8_t instruction = 0X41;
	uint16_t index;
	
	USART_SetTxMode(USART1);
	buf[counter++] = 0XFF;
	buf[counter++] = 0XFF;
	buf[counter++] = 0XD2;				//~N/N
	buf[counter++] = select>>8;			//select hight
	buf[counter++] = select;			//select low
	buf[counter++] = (~select)>>8;		//~select hight
	buf[counter++] = (~select);			//~select low
	buf[counter++] = instruction;		//instruction
	buf[counter++] = ~instruction;		//~instruction
	for(i=0; i<16; i++){
		index = counter;
		buf[counter++] = servoAngle[i]>>8;
		buf[counter++] = servoAngle[i];
		buf[counter++] = servoSpeed[i]>>8;
		buf[counter++] = servoSpeed[i];
		buf[counter++] = checkSum(&buf[index], 4);
	}
	USART_Send(USART1, buf, counter);
	USART_SetRxMode(USART1);
}

void servoSendHandValue(uint8_t handValue)
{
	uint16_t i;
	uint16_t counter = 0;
	uint8_t buf[MOTION_FRAME_SIZE] = {0};
	uint16_t select = 0XFFFF;
	uint8_t instruction = 0X40|INST_HAND_ON;
	
	USART_SetTxMode(USART1);
	buf[counter++] = 0XFF;
	buf[counter++] = 0XFF;
	buf[counter++] = 0XD2;				//~N/N
	buf[counter++] = select>>8;			//select hight
	buf[counter++] = select;			//select low
	buf[counter++] = (~select)>>8;		//~select hight
	buf[counter++] = (~select);			//~select low
	buf[counter++] = instruction;		//instruction
	buf[counter++] = ~instruction;		//~instruction
	for(i=0; i<16; i++){
		buf[counter++] = handValue;
		buf[counter++] = 0;
		buf[counter++] = 0;
		buf[counter++] = 0;
		buf[counter++] = ~handValue;
	}
	USART_Send(USART1, buf, counter);
	USART_SetRxMode(USART1);
}

//void servoTest(uint16_t angle)
//{
//	uint16_t i;
//	uint16_t counter = 0;
//	uint8_t buf[MOTION_FRAME_SIZE] = {0};
//	uint16_t select = 0XFFFF;
//	uint8_t instruction = 0X41;
//	uint16_t index;
//	
//	USART_SetTxMode(USART1);
//	buf[counter++] = 0XFF;
//	buf[counter++] = 0XFF;
//	buf[counter++] = 0XD2;				//~N/N
//	buf[counter++] = select>>8;			//select hight
//	buf[counter++] = select;			//select low
//	buf[counter++] = (~select)>>8;		//~select hight
//	buf[counter++] = (~select);			//~select low
//	buf[counter++] = instruction;		//instruction
//	buf[counter++] = ~instruction;		//~instruction
//	for(i=0; i<16; i++){
//		index = counter;
//		buf[counter++] = angle>>8;
//		buf[counter++] = angle;
//		buf[counter++] = 200>>8;
//		buf[counter++] = 200;
//		buf[counter++] = checkSum(&buf[index], 4);
//	}
//	USART_Send(USART1, buf, counter);
//	USART_SetRxMode(USART1);
//}

#define READ_FRAM_SIZE	16
void servoSendRead(uint8_t id, EnumContent content)
{
	uint8_t buf[READ_FRAM_SIZE] = {0};
	uint16_t counter = 0;
	uint8_t contentVal = 0X40|((uint8_t)content);
	
	USART_SetTxMode(USART1);
	buf[counter++] = 0XFF;
	buf[counter++] = 0XFE;
	buf[counter++] = id;
	buf[counter++] = ~id;
	buf[counter++] = contentVal;
	buf[counter++] = ~contentVal;
	USART_Send(USART1, buf, counter);
	USART_SetRxMode(USART1);
}

