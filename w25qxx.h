

#ifndef INC_W25QXX_H_
#define INC_W25QXX_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include "main.h"


#define _W25QXX_SPI                  hspi2
#define _W25QXX_CS_GPIO              GPIOC
#define _W25QXX_CS_PIN               GPIO_PIN_1
#define _W25QXX_USE_FREERTOS          0
#define _W25QXX_DEBUG                 1



	typedef enum
	{
		W25Q10,
		W25Q20,
		W25Q40,
		W25Q80,
		W25Q16 = 1 ,
		W25Q32,
		W25Q64,
		W25Q128,
		W25Q256,
		W25Q512,

	} W25QXX_ID_t;

	typedef struct
	{
		W25QXX_ID_t ID;
		uint8_t UniqID[8];
		uint16_t PageSize;
		uint32_t PageCount;
		uint32_t SectorSize;
		uint32_t SectorCount;
		uint32_t BlockSize;
		uint32_t BlockCount;
		uint32_t CapacityInKiloByte;
		uint8_t StatusRegister1;
		uint8_t StatusRegister2;
		uint8_t StatusRegister3;
		uint8_t Lock;

	} w25qxx_t;

	extern w25qxx_t w25qxx;
	//############################################################################
	// in Page,Sector and block read/write functions, can put 0 to read maximum bytes
	//############################################################################



//############################################################################
#ifdef __cplusplus
}
#endif


#if (_W25QXX_DEBUG == 1)
#include <stdio.h>
#endif


w25qxx_t w25qxx;
extern SPI_HandleTypeDef  _W25QXX_SPI;
#if (_W25QXX_USE_FREERTOS == 1)
#define W25qxx_Delay(delay) osDelay(delay)
#include "cmsis_os.h"
#else
#define W25qxx_Delay(delay) HAL_Delay(delay)
#endif
//###################################################################################################################


////////////////////////////////////////// MACROS FOR COMMANDS ///////////////////////////////////////////////////////////////////////////////////////////////////

#define Write_Enable                          0x06
#define Volatile_SR_Write_Enable              0x50
#define Write_Disable                         0x04
#define Release Power-down 	                  0xAB
#define JEDEC_ID                              0x9F
#define Read_Unique_ID                        0x4B
#define Read_Data                             0x03
#define Fast_Read                             0x0B
#define Page_Program                          0x02
#define Sector_Erase_4KB                      0x20
#define Block_Erase_32KB                      0x52
#define Block_Erase_64KB                      0xD8
#define Chip_Erase                            0xC7
#define Read_Status_Register_1                0x05
#define Write_Status_Register_1               0x01
#define Read_Status_Register_2                0x35
#define Write_Status_Register_2               0x31
#define Read_Status_Register_3                0x15
#define Write_Status_Register_3               0x11
#define Read_SFDP_Register                    0x5A
#define Erase_Security_Register               0x44
#define Program_Security_Register             0x42
#define Read_Security_Register                0x48
#define Global_Block_Lock                     0x7E
#define Global_Block_Unlock                   0x98
#define Read_Block_Lock                       0x3D
#define Individual_Block_lock                 0x36
#define Individual_Block_Unlock               0x39
#define Program_Suspend                       0x75
#define Program_Resume                        0x7A
#define Power_Down                            0xB9
#define Enable_Reset                          0x66
#define Reset_Device                          0x99





uint8_t W25qxx_Spi_transmit(uint8_t Data)
{

	HAL_SPI_Transmit(&_W25QXX_SPI,&Data,sizeof(Data),100);
	return 0;
}


uint8_t W25qxx_Spi_receive()
{
	uint8_t ret1;
	HAL_SPI_Receive(&_W25QXX_SPI,&ret1,sizeof(ret1),100);
	return ret1;

}


//###################################################################################################################
  uint32_t W25qxx_ReadID(void)
  {
	uint32_t jedec_id;
	uint8_t device_id[3];
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	W25qxx_Spi_transmit(JEDEC_ID);
    for(int i =0;i<=2;i++)
    {
       device_id[i]=W25qxx_Spi_receive();
    }
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	jedec_id = (device_id[0] << 16) | (device_id[1] << 8) | device_id[2];
	return jedec_id;
}

//###################################################################################################################
void W25qxx_ReadUniqID()
{
	uint8_t dummy_bytes[4];
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	W25qxx_Spi_transmit(Read_Unique_ID);
	for (uint8_t i = 0; i < 4; i++)
	{
		dummy_bytes[i]= W25qxx_Spi_receive();
	}
	for (uint8_t j = 0;j<8;j++)
	{
		w25qxx.UniqID[j] = W25qxx_Spi_receive();

	}
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);


}
//###################################################################################################################
void W25qxx_WriteEnable(void)
{
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	W25qxx_Spi_transmit(Write_Enable);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_Delay(1);
}
//###################################################################################################################
void W25qxx_WriteDisable(void)
{
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	W25qxx_Spi_transmit(Write_Disable);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_Delay(1);
}
//###################################################################################################################
uint8_t W25qxx_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
	uint8_t status = 0;
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	if (SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi_transmit(Read_Status_Register_1);
		status = W25qxx_Spi_receive();
		w25qxx.StatusRegister1 = status;
	}
	else if (SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi_transmit(Read_Status_Register_2);
		status = W25qxx_Spi_receive();
		w25qxx.StatusRegister2 = status;
	}
	else
	{
		W25qxx_Spi_transmit(Read_Status_Register_3);
		status = W25qxx_Spi_receive();
		w25qxx.StatusRegister3 = status;
	}
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	return status;
}
//###################################################################################################################
void W25qxx_WriteStatusRegister(uint8_t SelectStatusRegister_1_2_3, uint8_t Data)
{
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	W25qxx_WriteEnable();
	if (SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi_transmit(Write_Status_Register_1);
		w25qxx.StatusRegister1 = Data;
	}
	else if (SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi_transmit(Write_Status_Register_2);
		w25qxx.StatusRegister2 = Data;
	}
	else
	{
		W25qxx_Spi_transmit(Write_Status_Register_3);
		w25qxx.StatusRegister3 = Data;
	}
	W25qxx_Spi_transmit(Data);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
}
//###################################################################################################################
void W25qxx_WaitForWriteEnd(void)
{
	int stat=1;
	W25qxx_Delay(1);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	while(stat)
	{
	  stat = W25qxx_ReadStatusRegister(1);
	  stat = stat & 0x01;
	}
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
}
//###################################################################################################################
bool W25qxx_Init(void)
{
	w25qxx.Lock = 1;
	while (HAL_GetTick() < 100)
		W25qxx_Delay(1);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_Delay(100);
	uint32_t id;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx Init Begin...\r\n");
#endif

	id = W25qxx_ReadID();

#if (_W25QXX_DEBUG == 1)
	printf("w25qxx ID:0x%X\r\n", id);
#endif
	switch (id & 0x000000FF)
	{
	case 0x20: // 	w25q512
		w25qxx.ID = W25Q512;
		w25qxx.BlockCount = 1024;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q512\r\n");
#endif
		break;
	case 0x19: // 	w25q256
		w25qxx.ID = W25Q256;
		w25qxx.BlockCount = 512;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q256\r\n");
#endif
		break;
	case 0x18: // 	w25q128
		w25qxx.ID = W25Q128;
		w25qxx.BlockCount = 256;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q128\r\n");
#endif
		break;
	case 0x17: //	w25q64
		w25qxx.ID = W25Q64;
		w25qxx.BlockCount = 128;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q64\r\n");
#endif
		break;
	case 0x16: //	w25q32
		w25qxx.ID = W25Q32;
		w25qxx.BlockCount = 64;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q32\r\n");
#endif
		break;
	case 0x15: //	w25q16
		w25qxx.ID = W25Q16;
		w25qxx.BlockCount = 32;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q16\r\n");
#endif
		break;
	case 0x14: //	w25q80
		w25qxx.ID = W25Q80;
		w25qxx.BlockCount = 16;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q80\r\n");
#endif
		break;
	case 0x13: //	w25q40
		w25qxx.ID = W25Q40;
		w25qxx.BlockCount = 8;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q40\r\n");
#endif
		break;
	case 0x12: //	w25q20
		w25qxx.ID = W25Q20;
		w25qxx.BlockCount = 4;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q20\r\n");
#endif
		break;
	case 0x11: //	w25q10
		w25qxx.ID = W25Q10;
		w25qxx.BlockCount = 2;
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Chip: w25q10\r\n");
#endif
		break;
	default:
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx Unknown ID\r\n");
#endif
		w25qxx.Lock = 0;
		return false;
	}

	w25qxx.PageSize = 256; // 256 bytes //
	w25qxx.SectorSize = 4096; // 4096 bytes - almost equal to 4KB //
	w25qxx.SectorCount = w25qxx.BlockCount * 16;
	w25qxx.PageCount = (w25qxx.SectorCount * w25qxx.SectorSize) / w25qxx.PageSize;
	w25qxx.BlockSize = w25qxx.SectorSize * 16;
	w25qxx.CapacityInKiloByte = (w25qxx.SectorCount * w25qxx.SectorSize) / 1024;
	W25qxx_ReadUniqID();
	W25qxx_ReadStatusRegister(1);
	W25qxx_ReadStatusRegister(2);
	W25qxx_ReadStatusRegister(3);
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx Page Size: %d Bytes\r\n", w25qxx.PageSize);
	printf("w25qxx Page Count: %d\r\n", w25qxx.PageCount);
	printf("w25qxx Sector Size: %d Bytes\r\n", w25qxx.SectorSize);
	printf("w25qxx Sector Count: %d\r\n", w25qxx.SectorCount);
	printf("w25qxx Block Size: %d Bytes\r\n", w25qxx.BlockSize);
	printf("w25qxx Block Count: %d\r\n", w25qxx.BlockCount);
	printf("w25qxx Capacity: %d KiloBytes\r\n", w25qxx.CapacityInKiloByte);
	printf("w25qxx Init Done\r\n");
#endif
	w25qxx.Lock = 0;
	return true;

}
//###################################################################################################################
void W25qxx_EraseChip(void)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("w25qxx EraseChip Begin...\r\n");
#endif
	W25qxx_WriteEnable();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
    W25qxx_Spi_transmit(Chip_Erase);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx EraseBlock done after %d ms!\r\n", HAL_GetTick() - StartTime);
#endif
	W25qxx_Delay(10);
	w25qxx.Lock = 0;
}
//###################################################################################################################
void W25qxx_EraseSector_4KB(uint32_t SectorAddr)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("w25qxx EraseSector %d Begin...\n", SectorAddr);
#endif
	W25qxx_WaitForWriteEnd();
	//SectorAddr = SectorAddr * w25qxx.SectorSize;
	printf("sectoraddr : %d\n",SectorAddr);
	W25qxx_WriteEnable();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0x21); // Sector_Erase command for upper models of W25Q256 //
		W25qxx_Spi_transmit((SectorAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Sector_Erase_4KB);
	}
	W25qxx_Spi_transmit((SectorAddr >> 16) & 0xFF);
	W25qxx_Spi_transmit((SectorAddr >> 8) & 0xFF);
	W25qxx_Spi_transmit(SectorAddr & 0xFF);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx EraseSector done after %d ms \n", HAL_GetTick() - StartTime);
#endif
	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}
//###################################################################################################################
void W25qxx_EraseBlock_64KB(uint32_t BlockAddr)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx EraseBlock %d Begin...\r\n", BlockAddr);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	W25qxx_WaitForWriteEnd();
	BlockAddr = BlockAddr * w25qxx.SectorSize * 16;
	W25qxx_WriteEnable();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0xDC);
		W25qxx_Spi_transmit((BlockAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Block_Erase_64KB);
	}
	W25qxx_Spi_transmit((BlockAddr >> 16) & 0xFF);
	W25qxx_Spi_transmit((BlockAddr >> 8) & 0xFF);
	W25qxx_Spi_transmit(BlockAddr & 0xFF);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx EraseBlock done after %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	W25qxx_Delay(1);
	w25qxx.Lock = 0;


}

void W25qxx_EraseBlock_32KB(uint32_t BlockAddr)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx EraseBlock %d Begin...\r\n", BlockAddr);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	W25qxx_WaitForWriteEnd();
	//BlockAddr = BlockAddr * w25qxx.SectorSize * 16;
	W25qxx_WriteEnable();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0xDC);
		W25qxx_Spi_transmit((BlockAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Block_Erase_32KB);
	}
	W25qxx_Spi_transmit((BlockAddr >> 16) & 0xFF);
	W25qxx_Spi_transmit((BlockAddr >> 8) & 0xFF);
	W25qxx_Spi_transmit(BlockAddr & 0xFF);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx EraseBlock done after %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	W25qxx_Delay(1);
	w25qxx.Lock = 0;


}


//###################################################################################################################
uint32_t W25qxx_PageToSector(uint32_t PageAddress)
{
	return ((PageAddress * w25qxx.PageSize) / w25qxx.SectorSize);
}
//###################################################################################################################
uint32_t W25qxx_PageToBlock(uint32_t PageAddress)
{
	return ((PageAddress * w25qxx.PageSize) / w25qxx.BlockSize);
}
//###################################################################################################################
uint32_t W25qxx_SectorToBlock(uint32_t SectorAddress)
{
	return ((SectorAddress * w25qxx.SectorSize) / w25qxx.BlockSize);
}
//###################################################################################################################
uint32_t W25qxx_SectorToPage(uint32_t SectorAddress)
{
	return (SectorAddress * w25qxx.SectorSize) / w25qxx.PageSize;
}
//###################################################################################################################
uint32_t W25qxx_BlockToPage(uint32_t BlockAddress)
{
	return (BlockAddress * w25qxx.BlockSize) / w25qxx.PageSize;
}
//###################################################################################################################
bool W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
	if (((NumByteToCheck_up_to_PageSize + OffsetInByte) > w25qxx.PageSize) || (NumByteToCheck_up_to_PageSize == 0))
		NumByteToCheck_up_to_PageSize = w25qxx.PageSize - OffsetInByte;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckPage:%d, Offset:%d, Bytes:%d begin...\r\n", Page_Address, OffsetInByte, NumByteToCheck_up_to_PageSize);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = OffsetInByte; i < w25qxx.PageSize; i += sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Page_Address * w25qxx.PageSize);
		if (w25qxx.ID >= W25Q256)
		{
			W25qxx_Spi_transmit(0x0C);
			W25qxx_Spi_transmit((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25qxx_Spi_transmit(Read_Data);
		}
		W25qxx_Spi_transmit((WorkAddress >> 16) & 0xFF);
		W25qxx_Spi_transmit((WorkAddress >> 8) & 0xFF);
		W25qxx_Spi_transmit(WorkAddress & 0xFF);
		W25qxx_Spi_transmit(0);
		HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, sizeof(pBuffer), 100);
		HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((w25qxx.PageSize + OffsetInByte) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < w25qxx.PageSize; i++)
		{
			HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Page_Address * w25qxx.PageSize);
			W25qxx_Spi_transmit(Read_Data);
			if (w25qxx.ID >= W25Q256)
			{
				W25qxx_Spi_transmit(0x0C);
				W25qxx_Spi_transmit((WorkAddress & 0xFF000000) >> 24);
			}
			else
			{
				W25qxx_Spi_transmit(Read_Data);
			}
			W25qxx_Spi_transmit((WorkAddress >> 16) & 0xFF);
			W25qxx_Spi_transmit((WorkAddress >> 8) & 0xFF);
			W25qxx_Spi_transmit(WorkAddress & 0xFF);
			//W25qxx_Spi_transmit(0); //  Not - Needed //
			HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, 1, 100);
			HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckPage is Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	w25qxx.Lock = 0;
	return true;
NOT_EMPTY:
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckPage is Not Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	w25qxx.Lock = 0;
	return false;
}
//###################################################################################################################
bool W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
	if ((NumByteToCheck_up_to_SectorSize > w25qxx.SectorSize) || (NumByteToCheck_up_to_SectorSize == 0))
		NumByteToCheck_up_to_SectorSize = w25qxx.SectorSize;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckSector:%d, Offset:%d, Bytes:%d begin...\r\n", Sector_Address, OffsetInByte, NumByteToCheck_up_to_SectorSize);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = OffsetInByte; i < w25qxx.SectorSize; i += sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Sector_Address * w25qxx.SectorSize);
		if (w25qxx.ID >= W25Q256)
		{
			W25qxx_Spi_transmit(0x0C);
			W25qxx_Spi_transmit((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25qxx_Spi_transmit(Read_Data);
		}
		W25qxx_Spi_transmit((WorkAddress >> 16) & 0xFF);
		W25qxx_Spi_transmit((WorkAddress >> 8) & 0xFF);
		W25qxx_Spi_transmit(WorkAddress & 0xFF);
		W25qxx_Spi_transmit(0);
		HAL_SPI_Receive(&_W25QXX_SPI, pBuffer,sizeof(pBuffer), 100);
		HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((w25qxx.SectorSize + OffsetInByte) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < w25qxx.SectorSize; i++)
		{
			HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Sector_Address * w25qxx.SectorSize);
			if (w25qxx.ID >= W25Q256)
			{
				W25qxx_Spi_transmit(0x0C);
				W25qxx_Spi_transmit((WorkAddress & 0xFF000000) >> 24);
			}
			else
			{
				W25qxx_Spi_transmit(Read_Data);
			}
			W25qxx_Spi_transmit((WorkAddress >> 16) & 0xFF);
			W25qxx_Spi_transmit((WorkAddress >> 8) & 0xFF);
			W25qxx_Spi_transmit(WorkAddress & 0xFF);
			//W25qxx_Spi_transmit(0);
			HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, 1, 100);
		    HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckSector is Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	w25qxx.Lock = 0;
	return true;
NOT_EMPTY:
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckSector is Not Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	w25qxx.Lock = 0;
	return false;
}
//###################################################################################################################
bool W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
	if ((NumByteToCheck_up_to_BlockSize > w25qxx.BlockSize) || (NumByteToCheck_up_to_BlockSize == 0))
		NumByteToCheck_up_to_BlockSize = w25qxx.BlockSize;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckBlock:%d, Offset:%d, Bytes:%d begin...\r\n", Block_Address, OffsetInByte, NumByteToCheck_up_to_BlockSize);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = OffsetInByte; i < w25qxx.BlockSize; i += sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Block_Address * w25qxx.BlockSize);

		if (w25qxx.ID >= W25Q256)
		{
			W25qxx_Spi_transmit(0x0C);
			W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25qxx_Spi_transmit(Read_Data);
		}
		W25qxx_Spi((WorkAddress >> 16) & 0xFF);
		W25qxx_Spi((WorkAddress >> 8) & 0xFF);
		W25qxx_Spi(WorkAddress & 0xFF);
		//W25qxx_Spi(0);
		HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, sizeof(pBuffer), 100);
		HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((w25qxx.BlockSize + OffsetInByte) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < w25qxx.BlockSize; i++)
		{
			HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Block_Address * w25qxx.BlockSize);

			if (w25qxx.ID >= W25Q256)
			{
				W25qxx_Spi_transmit(0x0C);
				W25qxx_Spi_transmit((WorkAddress & 0xFF000000) >> 24);
			}
			else
			{
				W25qxx_Spi_transmit(Read_Data);
			}
			W25qxx_Spi_transmit((WorkAddress >> 16) & 0xFF);
			W25qxx_Spi_transmit((WorkAddress >> 8) & 0xFF);
			W25qxx_Spi_transmit(WorkAddress & 0xFF);
			//W25qxx_Spi_transmit(0); Not-needed
			HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, 1, 100);
			HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckBlock is Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	w25qxx.Lock = 0;
	return true;
NOT_EMPTY:
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx CheckBlock is Not Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	W25qxx_Delay(100);
#endif
	w25qxx.Lock = 0;
	return false;
}
//###################################################################################################################
void W25qxx_WriteByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes)
{
	while(w25qxx.Lock == 1)
	{
	  W25qxx_Delay(1);
	}
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("w25qxx WriteByte 0x%02X at address %d begin...", pBuffer, WriteAddr_inBytes);
#endif
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);

	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0x12);
		W25qxx_Spi_transmit((WriteAddr_inBytes & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Page_Program);
	}
	W25qxx_Spi_transmit((WriteAddr_inBytes >> 16) & 0xFF);
	W25qxx_Spi_transmit((WriteAddr_inBytes >> 8) & 0xFF);
	W25qxx_Spi_transmit(WriteAddr_inBytes & 0xFF);
	W25qxx_Spi_transmit(pBuffer);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx WriteByte done after %d ms\r\n", HAL_GetTick() - StartTime);
#endif
	w25qxx.Lock = 0;
}

void W25qxx_WriteBytes(uint8_t *pBuffer,uint32_t WriteAddr_inBytes,uint32_t NumByteToWrite)
{
	while(w25qxx.Lock == 1)
		  W25qxx_Delay(1);
   w25qxx.Lock = 1;
  #if (_W25QXX_DEBUG == 1)
		  uint32_t StartTime = HAL_GetTick();
  #endif
  W25qxx_WaitForWriteEnd();
  W25qxx_WriteEnable();
  HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
  if(w25qxx.ID >= W25Q256)
  {
	  W25qxx_Spi_transmit(0x12);
	  W25qxx_Spi_transmit((WriteAddr_inBytes & 0xFF000000) >> 24);
  }
  else
  {
	  W25qxx_Spi_transmit(Page_Program);
  }
  W25qxx_Spi_transmit((WriteAddr_inBytes >> 16) & 0xFF);
  W25qxx_Spi_transmit((WriteAddr_inBytes >> 8) & 0xFF);
  W25qxx_Spi_transmit(WriteAddr_inBytes & 0xFF);
  HAL_SPI_Transmit(&_W25QXX_SPI,pBuffer,NumByteToWrite,100);
  HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN, GPIO_PIN_SET);
  W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx WriteBytes done after %d ms\r\n", HAL_GetTick() - StartTime);
#endif
	w25qxx.Lock = 0;

}

//###################################################################################################################
void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
	if (((NumByteToWrite_up_to_PageSize + OffsetInByte) > w25qxx.PageSize) || (NumByteToWrite_up_to_PageSize == 0))
		NumByteToWrite_up_to_PageSize = w25qxx.PageSize - OffsetInByte;
	if ((OffsetInByte + NumByteToWrite_up_to_PageSize) > w25qxx.PageSize)
		NumByteToWrite_up_to_PageSize = w25qxx.PageSize - OffsetInByte;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n", Page_Address, OffsetInByte, NumByteToWrite_up_to_PageSize);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	//Page_Address = (Page_Address * w25qxx.PageSize) + OffsetInByte;
	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0x12);
		W25qxx_Spi_transmit((Page_Address & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Page_Program);
	}
	W25qxx_Spi_transmit((Page_Address >> 16) & 0xFF);
	W25qxx_Spi_transmit((Page_Address >> 8) & 0xFF);
	W25qxx_Spi_transmit(Page_Address & 0xFF);
	HAL_SPI_Transmit(&_W25QXX_SPI, pBuffer, NumByteToWrite_up_to_PageSize, 100);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
	W25qxx_WaitForWriteEnd();
#if (_W25QXX_DEBUG == 1)
	StartTime = HAL_GetTick() - StartTime;
	for (uint32_t i = 0; i < NumByteToWrite_up_to_PageSize; i++)
	{
		if ((i % 8 == 0) && (i > 2))
		{
			printf("\r\n");
			W25qxx_Delay(10);
		}
		printf("0x%02X,", pBuffer[i]);
	}
	printf("\r\n");
	printf("w25qxx WritePage done after %d ms\r\n", StartTime);
	W25qxx_Delay(100);
#endif
	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}
//###################################################################################################################
void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
	if ((NumByteToWrite_up_to_SectorSize > w25qxx.SectorSize) || (NumByteToWrite_up_to_SectorSize == 0))
		NumByteToWrite_up_to_SectorSize = w25qxx.SectorSize;
#if (_W25QXX_DEBUG == 1)
	printf("+++w25qxx WriteSector:%d, Offset:%d ,Write %d Bytes, begin...\r\n", Sector_Address, OffsetInByte, NumByteToWrite_up_to_SectorSize);
	W25qxx_Delay(100);
#endif
	if (OffsetInByte >= w25qxx.SectorSize)
	{
#if (_W25QXX_DEBUG == 1)
		printf("---w25qxx WriteSector Failed!\r\n");
		W25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToWrite_up_to_SectorSize) > w25qxx.SectorSize)
		BytesToWrite = w25qxx.SectorSize - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_SectorSize;
	//StartPage = Sector_Address + (OffsetInByte / w25qxx.PageSize);
	StartPage = Sector_Address ;
	LocalOffset = OffsetInByte % w25qxx.PageSize;
	do
	{
		W25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
		StartPage++;
		BytesToWrite -= w25qxx.PageSize - LocalOffset;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);
#if (_W25QXX_DEBUG == 1)
	printf("---w25qxx WriteSector Done\r\n");
	W25qxx_Delay(100);
#endif
}
//###################################################################################################################
void W25qxx_WriteBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
	if ((NumByteToWrite_up_to_BlockSize > w25qxx.BlockSize) || (NumByteToWrite_up_to_BlockSize == 0))
		NumByteToWrite_up_to_BlockSize = w25qxx.BlockSize;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx WriteBlock:%d, Offset:%d ,Write %d Bytes, begin...\r\n", Block_Address, OffsetInByte, NumByteToWrite_up_to_BlockSize);
	W25qxx_Delay(100);
#endif
	if (OffsetInByte >= w25qxx.BlockSize)
	{
#if (_W25QXX_DEBUG == 1)
		printf("---w25qxx WriteBlock Failed!\r\n");
		W25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToWrite_up_to_BlockSize) > w25qxx.BlockSize)
		BytesToWrite = w25qxx.BlockSize - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_BlockSize;
	StartPage = W25qxx_BlockToPage(Block_Address) + (OffsetInByte / w25qxx.PageSize);
	//StartPage = Block_Address + (OffsetInByte / w25qxx.PageSize);
	LocalOffset = OffsetInByte % w25qxx.PageSize;
	do
	{
		W25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
		StartPage++;
		BytesToWrite -= w25qxx.PageSize - LocalOffset;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);
#if (_W25QXX_DEBUG == 1)
	printf("---w25qxx WriteBlock Done\r\n");
	W25qxx_Delay(100);
#endif
}
//###################################################################################################################
void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("w25qxx ReadByte at address %lu begin...\r\n", Bytes_Address);
#endif
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);

	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0x0C);
		W25qxx_Spi_transmit((Bytes_Address & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Read_Data);  // Mistake - Fast Read not needed //
	}
	W25qxx_Spi_transmit((Bytes_Address >> 16) & 0xFF);
	W25qxx_Spi_transmit((Bytes_Address >> 8) & 0xFF);
	W25qxx_Spi_transmit(Bytes_Address & 0xFF);
	//W25qxx_Spi_transmit(0);   // Not needed //
	*pBuffer = W25qxx_Spi_receive();
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx ReadByte 0x%02X done after %d ms\r\n", *pBuffer, HAL_GetTick() - StartTime);
#endif
	w25qxx.Lock = 0;
}
//###################################################################################################################
void W25qxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
#if (_W25QXX_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("w25qxx ReadBytes at Address:%lu, %lu Bytes  begin...\r\n", ReadAddr, NumByteToRead);
#endif
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	if (w25qxx.ID >= W25Q256)
	{
        W25qxx_Spi_transmit(0x0C);
		W25qxx_Spi_transmit((ReadAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi_transmit(Read_Data);
	}
	W25qxx_Spi_transmit((ReadAddr >> 16) & 0xFF);
	W25qxx_Spi_transmit((ReadAddr >> 8) & 0xFF);
	W25qxx_Spi_transmit(ReadAddr & 0xFF);
	//W25qxx_Spi_Transmit(0);
	HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, NumByteToRead, 2000);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
#if (_W25QXX_DEBUG == 1)
	StartTime = HAL_GetTick() - StartTime;
	for (uint32_t i = 0; i < NumByteToRead; i++)
	{
		if ((i % 8 == 0) && (i > 2))
		{
			printf("\r\n");
			W25qxx_Delay(10);
		}
		printf("0x%02X,", pBuffer[i]);
	}
	printf("\r\n");
	printf("w25qxx ReadBytes done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
#endif
	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}
//###################################################################################################################
void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
	while (w25qxx.Lock == 1)
		W25qxx_Delay(1);
	w25qxx.Lock = 1;
	if ((NumByteToRead_up_to_PageSize > w25qxx.PageSize) || (NumByteToRead_up_to_PageSize == 0))
		NumByteToRead_up_to_PageSize = w25qxx.PageSize;
	if ((OffsetInByte + NumByteToRead_up_to_PageSize) > w25qxx.PageSize)
		NumByteToRead_up_to_PageSize = w25qxx.PageSize - OffsetInByte;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx ReadPage:%lu, Offset:%lu ,Read %lu Bytes, begin...\r\n", Page_Address, OffsetInByte, NumByteToRead_up_to_PageSize);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif

	//Page_Address = Page_Address * w25qxx.PageSize + OffsetInByte;
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_RESET);
	if (w25qxx.ID >= W25Q256)
	{
		W25qxx_Spi_transmit(0x0C);
		W25qxx_Spi_transmit((Page_Address & 0xFF000000) >> 24);
	}
	else
	{
	    W25qxx_Spi_transmit(Read_Data);
	}
	W25qxx_Spi_transmit((Page_Address >> 16) & 0xFF);
	W25qxx_Spi_transmit((Page_Address >> 8) & 0xFF);
	W25qxx_Spi_transmit(Page_Address & 0xFF);
	//W25qxx_Spi_Transmit(0); // Not-needed //
	HAL_SPI_Receive(&_W25QXX_SPI, pBuffer, NumByteToRead_up_to_PageSize, 100);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO, _W25QXX_CS_PIN, GPIO_PIN_SET);
#if (_W25QXX_DEBUG == 1)
	StartTime = HAL_GetTick() - StartTime;
	for (uint32_t i = 0; i < NumByteToRead_up_to_PageSize; i++)
	{
		if ((i % 8 == 0) && (i > 2))
		{
			printf("\r\n");
			W25qxx_Delay(10);
		}
		printf("0x%02X,", pBuffer[i]);
	}
	printf("\r\n");
	printf("w25qxx ReadPage done after %d ms\r\n", StartTime);
	W25qxx_Delay(100);
#endif
	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}
//###################################################################################################################
void W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize)
{
	if ((NumByteToRead_up_to_SectorSize > w25qxx.SectorSize) || (NumByteToRead_up_to_SectorSize == 0))
		NumByteToRead_up_to_SectorSize = w25qxx.SectorSize;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx ReadSector:%lu, Offset:%lu ,Read %lu Bytes, begin...\r\n", Sector_Address, OffsetInByte, NumByteToRead_up_to_SectorSize);
	W25qxx_Delay(100);
#endif
	if (OffsetInByte >= w25qxx.SectorSize)
	{
#if (_W25QXX_DEBUG == 1)
		printf("---w25qxx ReadSector Failed!\r\n");
		W25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToRead_up_to_SectorSize) > w25qxx.SectorSize)
		BytesToRead = w25qxx.SectorSize - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_SectorSize;
	//StartPage = Sector_Address + (OffsetInByte / w25qxx.PageSize);
	StartPage = Sector_Address;
	LocalOffset = OffsetInByte % w25qxx.PageSize;
	do
	{
		W25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= w25qxx.PageSize - LocalOffset;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx ReadSector Done\r\n");
	W25qxx_Delay(100);
#endif
}
//###################################################################################################################
void W25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize)
{
	if ((NumByteToRead_up_to_BlockSize > w25qxx.BlockSize) || (NumByteToRead_up_to_BlockSize == 0))
		NumByteToRead_up_to_BlockSize = w25qxx.BlockSize;
#if (_W25QXX_DEBUG == 1)
	printf("w25qxx ReadBlock:%lu, Offset:%lu ,Read %lu Bytes, begin...\r\n", Block_Address, OffsetInByte, NumByteToRead_up_to_BlockSize);
	W25qxx_Delay(100);
#endif
	if (OffsetInByte >= w25qxx.BlockSize)
	{
#if (_W25QXX_DEBUG == 1)
		printf("w25qxx ReadBlock Failed!\r\n");
		W25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToRead_up_to_BlockSize) > w25qxx.BlockSize)
		BytesToRead = w25qxx.BlockSize - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_BlockSize;
	StartPage =  Block_Address + (OffsetInByte / w25qxx.PageSize);
	LocalOffset = OffsetInByte % w25qxx.PageSize;
	do
	{
		W25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= w25qxx.PageSize - LocalOffset;
		pBuffer += w25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);
#if (_W25QXX_DEBUG == 1)
	printf("---w25qxx ReadBlock Done\r\n");
	W25qxx_Delay(100);
#endif
}


#endif /* INC_W25QXX_H_ */
