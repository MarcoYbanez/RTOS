#ifndef W25Q16_H
#define W25Q16_H
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico.h>
#include <pico/stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// MAX COUNTS
#define MAX_BYTES_PER_PAGE 256
#define MAX_PAGES_PER_SECTOR 16
#define MAX_SECTORS_PER_BLOCK 16
#define W25Q_SPI_BAUDRATE 500 // MAX 133 MHz

// INSTRUCTIONS
#define GET_DEVICE_ID 0x90

#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0x04
#define WRITE_PAGE_PROGRAM UINT8_C(0x02)
#define READ_DATA 0x03
#define FAST_READ_DATA 0x0B

#define ENABLE_RESET 0x66
#define RESET_DEVICE 0x99

#define ERASE_SECTOR 0x20
#define ERASE_BLOCK32 0x52
#define ERASE_BLOCK64 0xD8
#define ERASE_CHIP 0x60

#define READ_STATUS_REGISTER1 0x05
#define WRITE_STATUS_REGISTER1 0x01
#define READ_STATUS_REGISTER2 0x35
#define WRITE_STATUS_REGISTER2 0x31
#define READ_STATUS_REGISTER3 0x15
#define WRITE_STATUS_REGISTER3 0x11

#define GLOBAL_BLOCK_LOCK 0x7E
#define GLOBAL_BLOCK_UNLOCK 0x98
#define READ_BLOCK_LOCK 0x3D
#define INDIVIDUAL_BLOCK_LOCK 0x36
#define INDIVIDUAL_BLOCK_UNLOCK 0x39

// DELAY TIMES
#define CHIP_RESET_TIME 30
#define WRTIE_STATUS_REGISTER_TIME 15 // ms
#define SECTOR_ERASE_TIME 400         // ms
#define BLOCK_ERASE_TIME32 1600       // ms
#define BLOCK_ERASE_TIME64 2000       // ms
#define CHIP_ERASE_TIME 25            // s

// SHIFT POSITIONS
// STATUS REGISTER-1
#define BUSY_BIT 0
#define WEL_BIT 1
#define BP0_BIT 2
#define BP1_BIT 3
#define BP2_BIT 4
#define TB_BIT 5
#define SEC_BIT 6
#define CMP_BIT 7

// CHIP SPECIFIC
#define W25Q_PAGE_SIZE 0x100
#define W25Q_SECTOR_SIZE 0x1000
#define W25Q_BLOCK_SIZE 0x10000

// CHIP PROGRAM OPTIONS
#ifdef W25Q_SPI1
#define W25Q_SPI_CS_PIN 29
#define W25Q_SPI_HANDLE spi1
#define W25Q_SPI_RX_PIN 8
#define W25Q_SPI_TX_PIN 15
#define W25Q_SPI_SCK_PIN 14
#else
#define W25Q_SPI_CS_PIN 5
#define W25Q_SPI_HANDLE spi0
#define W25Q_SPI_RX_PIN 0
#define W25Q_SPI_TX_PIN 3
#define W25Q_SPI_SCK_PIN 6
#endif // if W25Q_SPI1

// DRIVER PROTOTYPES
static void W25Q_SpiInit(void);

static uint8_t *GetAddress(uint8_t blockId, uint8_t sectorId, uint8_t pageId);

void W25Q_EnableChipSelect(void);

void W25Q_DisableChipSelect(void);

void W25Q_Init(void);

uint8_t *W25Q_GetDeviceId(void);

void W25Q_WriteEnable(void);

void W25Q_WriteDisable(void);

void W25Q_Write(uint8_t blockId, uint8_t sectorId, uint8_t pageId,
                uint8_t *dataBuf);

void W25Q_ReadData(uint8_t blockId, uint8_t sectorId, uint8_t pageId,
                   uint8_t addr, uint8_t *dataBuf, uint32_t readCount);

void W25Q_FastReadData(uint8_t blockId, uint8_t sectorId, uint8_t pageId,
                       uint8_t addr, uint8_t *dataBuf, uint32_t readCount);

void W25Q_ResetChip(void);

void W25Q_EraseSector(uint8_t blockId, uint8_t sectorId);

void W25Q_EraseBlock32(uint8_t blockId, uint8_t sectorId);

void W25Q_EraseBlock64(uint8_t blockId);

void W25Q_EraseChip(void);

void W25Q_GetDeviceIdLoop(void *param);
#endif
