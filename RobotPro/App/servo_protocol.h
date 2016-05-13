#ifndef __H_SERVO_POTOCOL_H__
#define __H_SERVO_POTOCOL_H__

#include <stdint.h>

extern uint8_t currId;
extern uint8_t revFlag;
extern uint16_t revPara[2];
extern uint8_t servoState[16];
extern uint16_t servoAngle[16];
extern uint16_t servoSpeed[16];

//ÃüÁî
typedef enum
{
    INST_NULL = 0,
    INST_SET_ANGLE_SPEED,
    INST_SET_ANGLE,
    INST_SET_SPEED,
    INST_SET_ID,
    INST_RESET,
    INST_HAND_ON,
    INST_HAND_OFF,
}EnumInst;

//ÄÚÈÝ
typedef enum
{
    CONTENT_NULL = 0,
    CONTENT_CUR_ANGLE,
    CONTENT_CUR_SPEED,
    CONTENT_CUR_TORQUE,
    CONTENT_VERSION,
}EnumContent;

void servoInit(void);
void servoSendMotion(void);
void servoSendRead(uint8_t id, EnumContent content);
void servoSendHandValue(uint8_t handValue);
uint8_t checkSum(uint8_t *buf, uint32_t len);

#endif

