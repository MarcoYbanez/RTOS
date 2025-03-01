#include "W25Q16.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "task.h"
#include <hardware/spi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void W25Q_Init(void) {
  W25Q_SpiInit();
  W25Q_ResetChip();
  vTaskDelay(100);
}

static void W25Q_SpiInit(void) {
  // SPI INIT
  // spi_init(W25Q_SPI_HANDLE, W25Q_SPI_BAUDRATE * 1000);
  spi_init(W25Q_SPI_HANDLE, W25Q_SPI_BAUDRATE * 1000);
  gpio_set_function(W25Q_SPI_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(W25Q_SPI_TX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(W25Q_SPI_SCK_PIN, GPIO_FUNC_SPI);

  gpio_init(W25Q_SPI_CS_PIN);
  gpio_set_dir(W25Q_SPI_CS_PIN, 1);
  gpio_put(W25Q_SPI_CS_PIN, 1);
}

/**
 * @brief return 8 bit array of 24 bit address
 */
static uint8_t *GetAddress(uint8_t blockId, uint8_t sectorId, uint8_t pageId) {
  uint8_t *addressBuf = pvPortCalloc(sizeof(uint8_t), 3);

  addressBuf[0] = (blockId * W25Q_BLOCK_SIZE);
  addressBuf[1] = (sectorId * W25Q_SECTOR_SIZE);
  addressBuf[2] = (pageId * W25Q_PAGE_SIZE);
  return addressBuf;
}

void W25Q_EnableChipSelect(void) { gpio_put(W25Q_SPI_CS_PIN, 0); }

void W25Q_DisableChipSelect(void) { gpio_put(W25Q_SPI_CS_PIN, 1); }

void W25Q_WriteEnable(void) {

  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){WRITE_ENABLE}, 1);
}

void W25Q_WriteDisable(void) {

  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){WRITE_DISABLE}, 1);
}

uint8_t *W25Q_GetDeviceId(void) {
  uint8_t *buf = pvPortCalloc(sizeof(uint8_t), 3);
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;

  W25Q_EnableChipSelect();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){GET_DEVICE_ID}, 1);
  spi_write_blocking(W25Q_SPI_HANDLE, buf, 3);
  spi_read_blocking(W25Q_SPI_HANDLE, 0, buf, 2);
  W25Q_DisableChipSelect();

  return buf;
}

/**
 * @brief W25Q IC flash only allows for entire 256 byte page writes at a time.
 * User must ensure 256 byte array is passed
 */
void W25Q_Write(uint8_t blockId, uint8_t sectorId, uint8_t pageId,
                uint8_t *dataBuf) {

  // get address for flash memory write
  uint8_t *addressBuf = GetAddress(blockId, sectorId, pageId);

  // begin data write
  W25Q_EnableChipSelect();
  W25Q_WriteEnable();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){WRITE_PAGE_PROGRAM}, 1);
  spi_write_blocking(W25Q_SPI_HANDLE, addressBuf, 3);
  spi_write_blocking(W25Q_SPI_HANDLE, dataBuf, W25Q_PAGE_SIZE);
  W25Q_WriteDisable();
  W25Q_DisableChipSelect();

  free(addressBuf);
}

void W25Q_ReadData(uint8_t blockId, uint8_t sectorId, uint8_t pageId,
                   uint8_t offset, uint8_t *dataBuf, uint32_t readCount) {

  uint8_t readStatusRegister = 0;
  uint8_t *addressBuf = GetAddress(blockId, sectorId, pageId);

  W25Q_EnableChipSelect();

  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){READ_STATUS_REGISTER1}, 1);
  spi_read_blocking(W25Q_SPI_HANDLE, 0, &readStatusRegister, 1);

  // continue with read if write not in progress
  if (readStatusRegister & (1 << BUSY_BIT) != 1) {
    spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){READ_DATA}, 1);
    spi_write_blocking(W25Q_SPI_HANDLE, addressBuf, 3);
    spi_read_blocking(W25Q_SPI_HANDLE, 0, dataBuf, readCount);
  }

  W25Q_DisableChipSelect();

  free(addressBuf);
}

void W25Q_FastReadData(uint8_t blockId, uint8_t sectorId, uint8_t pageId,
                       uint8_t addr, uint8_t *dataBuf, uint32_t readCount) {

  uint8_t readStatusRegister = 0;
  uint8_t *addressBuf = GetAddress(blockId, sectorId, pageId);

  W25Q_EnableChipSelect();

  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){READ_STATUS_REGISTER1}, 1);
  spi_read_blocking(W25Q_SPI_HANDLE, 0, &readStatusRegister, 1);

  // continue with read if write not in progress
  if (readStatusRegister & (1 << BUSY_BIT) != 1) {
    spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){FAST_READ_DATA}, 1);
    spi_write_blocking(W25Q_SPI_HANDLE, addressBuf,
                       4); // include dummy clocks for per command instruction
    spi_read_blocking(W25Q_SPI_HANDLE, 0, dataBuf, readCount);
  }

  W25Q_DisableChipSelect();
  free(addressBuf);
}

void W25Q_ResetChip(void) {

  W25Q_EnableChipSelect();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){ENABLE_RESET}, 1);
  W25Q_DisableChipSelect();

  W25Q_EnableChipSelect();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){RESET_DEVICE}, 1);
  W25Q_DisableChipSelect();
}

void W25Q_EraseSector(uint8_t blockId, uint8_t sectorId) {

  // get address for flash memory write
  uint8_t *addressBuf = GetAddress(blockId, sectorId, 0);

  W25Q_EnableChipSelect();
  W25Q_WriteEnable();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){ERASE_SECTOR}, 1);
  spi_write_blocking(W25Q_SPI_HANDLE, addressBuf, 3);
  W25Q_WriteDisable();
  W25Q_DisableChipSelect();

  free(addressBuf);
}

// block is 64KB, veify erase of bottom/top/inbetween portion
void W25Q_EraseBlock32(uint8_t blockId, uint8_t sectorId) {

  // get address for flash memory write
  uint8_t *addressBuf = GetAddress(blockId, sectorId, 0);

  W25Q_EnableChipSelect();
  W25Q_WriteEnable();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){ERASE_BLOCK32}, 1);
  spi_write_blocking(W25Q_SPI_HANDLE, addressBuf, 3);
  W25Q_WriteDisable();
  W25Q_DisableChipSelect();

  free(addressBuf);
}

void W25Q_EraseBlock64(uint8_t blockId) {

  // get address for flash memory write
  uint8_t *addressBuf = GetAddress(blockId, 0, 0);

  W25Q_EnableChipSelect();
  W25Q_WriteEnable();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){ERASE_BLOCK64}, 1);
  spi_write_blocking(W25Q_SPI_HANDLE, addressBuf, 3);
  W25Q_WriteDisable();
  W25Q_DisableChipSelect();

  free(addressBuf);
}

void W25Q_EraseChip(void) {

  W25Q_EnableChipSelect();
  W25Q_WriteEnable();
  spi_write_blocking(W25Q_SPI_HANDLE, &(uint8_t){ERASE_CHIP}, 1);
  W25Q_WriteDisable();
  W25Q_DisableChipSelect();
}

void W25Q_GetDeviceIdLoop(void *param) {
  W25Q_Init();

  for (;;) {
    vTaskDelay(300);
    uint8_t *buf = W25Q_GetDeviceId();
    printf("W25Q ID: %X\n", (buf[0] << 8) + (buf[1]));
    vPortFree(buf);
  }
}
