#include "stm32f2xx.h"
#include "stdio.h"
#include "sdio_sd.h"
#include "diskio.h"	


static volatile DSTATUS Stat = STA_NOINIT;	/* Physical drive status */


/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
	uint32_t status;
	status = SD_Init();
	if(SD_OK != status)
	{
		Stat = STA_NOINIT;
		return STA_NOINIT;
	}
	Stat = 0;
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (BYTE drv)		/* Physical drive number (0) */
{
	if (drv) return STA_NOINIT;		/* Supports only drive 0 */

	return Stat;	/* Return disk status */
}




/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	BYTE* buf_offset;
	buf_offset  = buff;
	
	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */
	
	while(count--)
	{
		SD_ReadBlock(buf_offset,sector++,512);
		buf_offset +=512;
	}

	return RES_OK;
}

DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Ponter to the data to write */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	
	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat & STA_PROTECT) return RES_WRPRT;	/* Check write protect */
	
	while(count--)
	{
		SD_Erase(sector,sector);
		SD_WriteBlock((uint8_t*)buff,sector,512);
		sector++;
	}
	return RES_OK;
}



DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
)
{
	return RES_OK;

}


DWORD get_fattime (void)
{
	return 0x00ffffff;
}

