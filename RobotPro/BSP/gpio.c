#include "gpio.h"

void GPIO_Config(GPIO_TypeDef *gpio, 
				 uint32_t gpio_pin, 
				 GPIOMode_TypeDef mode, 
				 GPIOOType_TypeDef otype,
				 GPIOPuPd_TypeDef pupd,
				 GPIOSpeed_TypeDef speed)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpio_pin;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = otype;
	GPIO_InitStructure.GPIO_PuPd = pupd;
	GPIO_InitStructure.GPIO_Speed = speed;
	GPIO_Init(gpio, &GPIO_InitStructure);
}

//�����������
void GPIO_PpUpOut(GPIO_TypeDef *gpio, uint32_t gpiopin)
{
	GPIO_Config(gpio, gpiopin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
}

//���������������
void GPIO_AfPpUpOut(GPIO_TypeDef *gpio, uint32_t gpiopin)
{
	GPIO_Config(gpio, gpiopin, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
}

//��������
void GPIO_UpIn(GPIO_TypeDef *gpio, uint32_t gpiopin)
{
	GPIO_Config(gpio, gpiopin, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
}

//��©�������
void GPIO_OdUpOut(GPIO_TypeDef *gpio, uint32_t gpiopin)
{
	GPIO_Config(gpio, gpiopin, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_100MHz);
}
