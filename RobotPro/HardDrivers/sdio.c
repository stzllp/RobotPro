#include "stm32f2xx.h"
#include "sdio_sd.h"

/** 
  * @brief  Mask for errors Card Status R1 (OCR Register) 
  */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)


/** 
  * @brief  Masks for R6 Response 
  */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define SD_ALLZERO                      ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000)

#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO                     ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020)

void SD_sendCMD(uint32_t cmd,uint32_t Argument,uint32_t resp_len);


SDIO_CmdInitTypeDef SDIO_CmdInitStructure;


void SD_sendCMD(uint32_t cmd,uint32_t Argument,uint32_t resp_len)
{
	SDIO_CmdInitStructure.SDIO_Argument = Argument;
  SDIO_CmdInitStructure.SDIO_CmdIndex = cmd;   
  SDIO_CmdInitStructure.SDIO_Response = resp_len;  
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
}

//·¢ËÍCMD0,²»ÐèÒª response
SD_Error SD_SendCMD0(void)
{
	SD_Error errorstatus = SD_OK;
	  /*!< CMD0: GO_IDLE_STATE ---------------------------------------------------*/
  /*!< No CMD response required */
  SD_sendCMD(SD_CMD_GO_IDLE_STATE,0,SDIO_Response_No);
	while (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET)  //Ñ­»·µÈ´ýCMD0·¢ËÍÍê±Ï
  {
		;
  }
	SDIO_ClearFlag(SDIO_FLAG_CMDSENT);
	return errorstatus;
}


/*!< Send CMD2 ALL_SEND_CID */
SD_Error SD_SendCMD2(void)
{
	SD_Error errorstatus = SD_OK;
	uint32_t status;
	
	SD_sendCMD(SD_CMD_ALL_SEND_CID,0,SDIO_Response_Long);	
	
	status = SDIO->STA;
	while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}


 /*!< Send CMD3 SET_REL_ADDR with argument 0 */   //49Ò³
    /*!< SD Card publishes its RCA. */
//²ÎÊýRCAÓÃÓÚ½ÓÊÕsdcard·¢³öµÄ RCA
SD_Error SD_SendCMD3(uint16_t *prca)
{
	SD_Error errorstatus = SD_OK;
  uint32_t status;
  uint32_t response_r1;
	
	SD_sendCMD(SD_CMD_SET_REL_ADDR,0,SDIO_Response_Short);
	status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
	else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }
	
	 /*!< Check response received is of desired command */
  if (SDIO_GetCommandResponse() != SD_CMD_SET_REL_ADDR)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }
	
	  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	
	/*!< We have received response, retrieve it.  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);
	
	if (SD_ALLZERO == (response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
  {
    *prca = (uint16_t) (response_r1 >> 16);
    return(errorstatus);
  }

  if (response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & SD_R6_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (response_r1 & SD_R6_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  return(errorstatus);
}


  /*!< Send CMD7 SDIO_SEL_DESEL_CARD */
SD_Error SD_SendCMD7(uint32_t Argument)
{
	SD_sendCMD(SD_CMD_SEL_DESEL_CARD,Argument,SDIO_Response_Short);
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }
  SDIO->ICR = SDIO_STATIC_FLAGS;
  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);
}




/*!< CMD8: SEND_IF_COND ----------------------------------------------------*/
/*!< Send CMD8 to verify SD card interface operating condition */
  /*!< Argument: - [31:12]: Reserved (shall be set to '0')
               - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
               - [7:0]: Check Pattern (recommended 0xAA) */
SD_Error SD_SendCMD8(uint32_t Argument)
{
	uint32_t timeout,status;
	SD_Error errorstatus = SD_OK;
	
	SD_sendCMD(8,Argument,SDIO_Response_Short);
	timeout = SDIO_CMD0TIMEOUT;
	
	status = SDIO->STA;
	while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) && (timeout > 0))
  {
    timeout--;
    status = SDIO->STA;
  }
	
	if ((timeout == 0) || (status & SDIO_FLAG_CTIMEOUT))
  {
    /*!< Card is not V2.0 complient or card does not support the set voltage range */
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
	
	if (status & SDIO_FLAG_CMDREND)
  {
    /*!< Card is SD V2.0 compliant */
    SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    return(errorstatus);
  }
  return(errorstatus);
}

   /*!< Send CMD9 SEND_CSD with argument as card's RCA */
SD_Error SD_SendCMD9(uint16_t rca)
{
	SD_Error errorstatus = SD_OK;
  uint32_t status;
	
	SD_sendCMD(SD_CMD_SEND_CSD,(uint32_t)(rca << 16),SDIO_Response_Long);
	status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}


SD_Error SD_SendCMD17(uint32_t Argument)
{
	SD_sendCMD(SD_CMD_READ_SINGLE_BLOCK,Argument,SDIO_Response_Short);
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;

  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);
}




  /*!< Send CMD24 WRITE_SINGLE_BLOCK */
SD_Error SD_SendCMD24(uint32_t Argument)
{
	SD_sendCMD(SD_CMD_WRITE_SINGLE_BLOCK,Argument,SDIO_Response_Short);
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;
  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);
}




/*!< Send CMD32 SD_ERASE_GRP_START with argument as addr  */
SD_Error SD_SendCMD32(uint32_t Argument)
{
	SD_sendCMD(SD_CMD_SD_ERASE_GRP_START,Argument,SDIO_Response_Short);
	
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;

  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);

}



    /*!< Send CMD33 SD_ERASE_GRP_END with argument as addr  */
SD_Error SD_SendCMD33(uint32_t Argument)
{
	SD_sendCMD(SD_CMD_SD_ERASE_GRP_END,Argument,SDIO_Response_Short);
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;

  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);

}


/*!< Send CMD38 ERASE */
SD_Error SD_SendCMD38(void)
{
	SD_sendCMD(SD_CMD_ERASE,0,SDIO_Response_Short);
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;

  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);
}


  /*!< Send ACMD6 APP_CMD with argument  for wide bus mode */
SD_Error SD_SendACMD6(uint32_t Argument)
{
	SD_sendCMD(SD_CMD_APP_SD_SET_BUSWIDTH,Argument,SDIO_Response_Short);
	while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
  }

  SDIO->ICR = SDIO_STATIC_FLAGS;

  return (SD_Error)(SDIO->RESP1 &  SD_OCR_ERRORBITS);
}




//(ACMD41) »ñÈ¡SdCARD ÄÚ²¿ OCR¼Ä´æÆ÷µÄÄÚÈÝ
SD_Error SD_SendACMD41(uint32_t Argument)
{
	uint32_t status;
	SD_Error errorstatus = SD_OK;
	
	SD_sendCMD(SD_CMD_SD_APP_OP_COND,Argument,SDIO_Response_Short);
  status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return(errorstatus);
}








SD_Error SD_SendCMD55(uint32_t Argument)
{
	uint32_t timeout,status;
	SD_Error errorstatus = SD_OK;
	
	SD_sendCMD(SD_CMD_APP_CMD,Argument,SDIO_Response_Short);
	
	timeout = SDIO_CMD0TIMEOUT;
	status = SDIO->STA;
	while (!(status& (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
		status = SDIO->STA;
  }
	if ((timeout == 0) || (status & SDIO_FLAG_CTIMEOUT))
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
	if (status & SDIO_FLAG_CMDREND)
  {
    /*!< Card is SD V2.0 compliant */
    SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    return(errorstatus);
  }
  return(errorstatus);
}

/*          STM32 SDIO Pin assignment
  *          =========================    
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SDIO Pins            |     SD        |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          |      SDIO D2      PC10      |   D2          |    1        |
  *          |      SDIO D3      PC11      |   D3          |    2        |
  *          |      SDIO CMD     PD2       |   CMD         |    3        |
  *          |                             |   VCC         |    4 (3.3 V)|
  *          |      SDIO CLK     PC12      |   CLK         |    5        |
  *          |                             |   GND         |    6 (0 V)  |
  *          |      SDIO D0      PC8       |   D0          |    7        |
  *          |      SDIO D1      PC9       |   D1          |    8        |
						 |			CARD_DETECT	 PF11																9									 |  
  *          +-----------------------------+---------------+-------------+  
  *              
*/

SDIO_InitTypeDef SDIO_InitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;   

uint16_t rca;
uint32_t card_capacity;
uint32_t CSD_Tab[4];

void SD_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< GPIOC and GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB, ENABLE);

  /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);
	
  /*!< Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#define SDIO_TRANSFER_CLK_DIV ((uint8_t)0x00)
/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for data 
  *         transfer).
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_Init(void)
{
	uint32_t powerup;
	uint32_t count;
	
  SD_Error errorstatus = SD_OK;
 
  /* SDIO Peripheral Low Level Init */
  SD_LowLevel_Init();

	SDIO_DeInit();
	if(SD_Detect() == SD_NOT_PRESENT)
		return SD_ERROR;
	 /*!< Power ON Sequence -----------------------------------------------------*/
  /*!< Configure the SDIO peripheral */
  /*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV) */
  /*!< on STM32F2xx devices, SDIOCLK is fixed to 48MHz */
  /*!< SDIO_CK for initialization should not exceed 400 KHz */  
  /* Enable SDIO Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
	/* Configure the SDIO peripheral
	SDIO_CK = SDIOCLK /(2 + SDIO_TRANSFER_CLK_DIV)
	On STM32F2xx devices, SDIOCLK is fixed to 48MHz
	*/
	SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV;
	SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
	SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	SDIO_Init(&SDIO_InitStructure);
	
  /*!< Set Power State to ON */
  SDIO_SetPowerState(SDIO_PowerState_ON);
  /*!< Enable SDIO Clock */
  SDIO_ClockCmd(ENABLE);
	
	SD_SendCMD0();
	
	
	//·¢ËÍCMD8  ÏêÏ¸ËµÃ÷ÔÚÑ§Ï°Êé¼® ¡¶¡SSDIO Í¨Ñ¶¹æ·¶.pdf ¡· 16Ò³·
	/***************************************************************************************************/
  /*!< CMD8: SEND_IF_COND ----------------------------------------------------*/
  /*!< Send CMD8 to verify SD card interface operating condition */
  /*!< Argument: - [31:12]: Reserved (shall be set to '0')
               - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
               - [7:0]: Check Pattern (recommended 0xAA) */
  /*!< CMD Response: R7 */
	SD_SendCMD8((uint32_t)0x1AA);
	powerup = 0;count = 0;
	while ((!powerup) && (count < SD_MAX_VOLT_TRIAL))
	{
		/*!< SEND CMD55 APP_CMD with RCA as 0 */
		SD_SendCMD55(0);
		
		//SD_CMD_SD_APP_OP_COND(ACMD41)ÃüÁîµÄÏêÏ¸ÃèÊö¼û Ñ§Ï°Êé¼® ¡¶¡SSDIO Í¨Ñ¶¹æ·¶.pdf ¡· 55Ò³·
		  //ÔÚÕâÀïORÉÏ 0x40000000±íÊ¾host ´øÓÐ high capacity £¬SD_VOLTAGE_WINDOW_SDÖ¸³öHOSTµçÑ¹·¶Î§¹
			//(ACMD41) »ñÈ¡SdCARD ÄÚ²¿ OCR¼Ä´æÆ÷µÄÄÚÈÝ
		SD_SendACMD41(0x80100000 | 0x40000000);
		powerup = (SDIO_GetResponse(SDIO_RESP1)& 0x80000000);  //OCR¼Ä´æÆ÷µÄ×î¸ßÎ»Îª1Ö¸³öpower up¹ý³ÌÍê³É£¬ËùÒÔ²»¶Ï¶ÁÈ¡ sdcard OCR¼Ä´æÆ÷ÒÔÅÐ¶Ïpower upÍê³É¡£
		card_capacity = (SDIO_GetResponse(SDIO_RESP1)& 0x40000000); //OCR¼Ä´æÆ÷µÄ´Î×î¸ßÎ»Îª1Ö¸³ö HIGH capacity£¬¸Ã¿¨ÎªÒ»¸ö4G¿¨
		count++;
	}
	if (count >= SD_MAX_VOLT_TRIAL)
	{
		errorstatus = SD_INVALID_VOLTRANGE;
		return(errorstatus);
	}
	
	 /*!< Send CMD2 ALL_SEND_CID */   //49Ò³
	SD_SendCMD2();
	 /*!< Send CMD3 SET_REL_ADDR with argument 0 */   //49Ò³
    /*!< SD Card publishes its RCA. */
	SD_SendCMD3(&rca);
	/*!< Send CMD9 SEND_CSD with argument as card's RCA */
	SD_SendCMD9(rca);
	
		//ÇÐ»»µ½TRAN´«ÊäÆµÂÊ
	SDIO_InitStructure.SDIO_ClockDiv = 1; 
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&SDIO_InitStructure);
	
	 /*!< Send CMD7 SDIO_SEL_DESEL_CARD */
	SD_SendCMD7(rca<<16);
	
	//ÆôÓÃ4bit ´«Êä·½Ê½
	SD_SendCMD55(rca<<16);
	SD_SendACMD6(2);
	
	SDIO_InitStructure.SDIO_ClockDiv = 10; 
	SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;
	SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	SDIO_Init(&SDIO_InitStructure);
	
	return errorstatus;
}





/**
  * @brief  Allows to erase memory area specified for the given card.
  * @param  startaddr: the start address.
  * @param  endaddr: the end address.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr)
{
  SD_Error errorstatus = SD_OK;

	if(card_capacity != 0x40000000)
	{
		startaddr = startaddr*512;
		endaddr = endaddr *512;
  }
	SD_SendCMD32(startaddr);
  SD_SendCMD33(endaddr); 
	SD_SendCMD38();

	while((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0))  //sdcard ÔÚ½øÐÐ²Á³ý²Ù×÷ÆÚ¼ä»áÒ»Ö±À­µÍDAT0,Ò»Ö±µ½²Á³ýÍê³É.¼û¡¶SDcardÍ¨Ñ¶¹æ·¶¡· 22Ò³·
	{
	}
  return(errorstatus);
}





/**
  * @brief  Allows to write one block starting from a specified address in a card. 
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  WriteAddr: Address from where data are to be read.   
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WriteBlock(uint8_t *writebuff, uint32_t WriteAddr, uint16_t BlockSize)
{
  SD_Error errorstatus = SD_OK;
 
  uint32_t bytestransferred = 0, count = 0, restwords = 0;
  uint32_t *tempbuff = (uint32_t *)writebuff;
  SDIO->DCTRL = 0x0;

	if(card_capacity != 0x40000000)
	{
		WriteAddr = WriteAddr*512;
  }
	BlockSize = 512;
 
  /*!< Send CMD24 WRITE_SINGLE_BLOCK */
	SD_SendCMD24(WriteAddr);

  SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFFFF;
  SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;  //9<<4 = 144 = 0b10010000 ¾ßÌå×÷ÓÃ¼ûSTM32f103zet6 ²Î¿¼ÊÖ²á587Ò³
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /*!< In case of single data block transfer no need of stop command at all */

  while (!(SDIO->STA & (SDIO_FLAG_DBCKEND | SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
    {
      if ((512 - bytestransferred) < 32)
      {
        restwords = ((512 - bytestransferred) % 4 == 0) ? ((512 - bytestransferred) / 4) : (( 512 -  bytestransferred) / 4 + 1);
        for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
        {
          SDIO_WriteData(*tempbuff);
        }
      }
      else
      {
        for (count = 0; count < 8; count++)
        {
          SDIO_WriteData(*(tempbuff + count));
        }
        tempbuff += 8;
        bytestransferred += 32;
      }
    }
  }
  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
    errorstatus = SD_TX_UNDERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }
	
	while((GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0))  //sdcard ÔÚ½øÐÐ²Á³ý²Ù×÷ÆÚ¼ä»áÒ»Ö±À­µÍDAT0,Ò»Ö±µ½²Á³ýÍê³É.¼û¡¶SDcardÍ¨Ñ¶¹æ·¶¡· 22Ò³·
	{
	}

  return(errorstatus);
}



/**
  * @brief  Allows to read one block from a specified address in a card. 
  * @param  readbuff: pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data are to be read.  
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ReadBlock(uint8_t *readbuff, uint32_t ReadAddr, uint16_t BlockSize)
{
  SD_Error errorstatus = SD_OK;

	
  uint32_t count = 0, *tempbuff = (uint32_t *)readbuff;
	
	if(card_capacity != 0x40000000)
	{
		ReadAddr = ReadAddr*512;
		
  }
	BlockSize = 512;
  SDIO_DataInitStructure.SDIO_DataTimeOut = 0xFFFFFFFF;
  SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /*!< Send CMD17 READ_SINGLE_BLOCK */
	SD_SendCMD17(ReadAddr);
	
  /*!< In case of single block transfer, no need of stop transfer at all.*/
  /*!< Polling mode */
  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(tempbuff + count) = SDIO_ReadData();
      }
      tempbuff += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }
	
  while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
  {
    *tempbuff = SDIO_ReadData();
    tempbuff++;
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return(errorstatus);
}






/**
 * @brief  Detect if SD card is correctly plugged in the memory slot.
 * @param  None
 * @retval Return if SD is detected or not
 */
uint8_t SD_Detect(void)
{
   uint8_t status = SD_PRESENT;

  /*!< Check GPIO to detect SD */
  if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) != Bit_RESET)
  {
    status = SD_NOT_PRESENT;
  }
  return status;
}

