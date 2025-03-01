#ifndef BME280_H
#define BME280_H

#include "FreeRTOS.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <task.h>

//
// Calibration 1 Registers
//
typedef struct BmeCalibrationRegisters1 {
  // 0x88 / 0x89
  unsigned short t1;

  // 0x8A / 0x8B
  signed short t2;

  // 0x8C / 0x8D
  signed short t3;

  // 0x8E / 0x8F
  unsigned short p1;

  // 0x90 / 0x91
  signed short p2;

  // 0x92 / 0x93
  signed short p3;

  // 0x94 / 0x95
  signed short p4;

  // 0x96 / 0x97
  signed short p5;

  // 0x98 / 0x99
  signed short p6;

  // 0x9A / 0x9B
  signed short p7;

  // 0x9C / 0x9D
  signed short p8;

  // 0x9E / 0x9F
  signed short p9;

  // 0xA1
  uint8_t h1;

} CalibrationRegisters1;

//
// Calibration 2 Registers
//
typedef struct BmeCalibrationRegisters2 {
  // 0xE1 / 0xE2
  signed short h2;

  // 0xE3
  uint8_t h3;

  // 0xE4 / 0xE6
  union hum {

    uint32_t raw;
    struct {
      signed short h4 : 12;
      signed short h5 : 12;
      uint16_t spare : 8;
    } fields;
  } hum;

  // 0xE7
  int8_t h6;
} CalibrationRegisters2;

typedef union ctrlHum_t {

  uint8_t raw;
  struct {
    uint8_t osrs : 3;
    uint8_t spare1 : 5;
  } fields;

} ctrlHum_t;

typedef union status_t {

  uint8_t raw;
  struct {
    uint8_t imUpdate : 1;
    uint8_t spare2 : 2;
    uint8_t measuring : 1;
    uint8_t spare3 : 3;

  } fields;
} status_t;

typedef union ctrlMeas_t {

  uint8_t raw;
  struct {
    uint8_t mode : 2;
    uint8_t osrs_p : 3;
    uint8_t osrs_t : 3;
  } fields;
} ctrlMeas_t;

typedef union config_t {

  uint8_t raw;
  struct {
    uint8_t spiEn : 1;
    uint8_t spare4 : 1;
    uint8_t filter : 3;
    uint8_t tSb : 3;
  } fields;

} config_t;

typedef union pressureXlsb_t {

  uint8_t raw;
  struct {
    uint8_t spare5 : 4;
    uint8_t xlsb : 4;
  } fields;
} pressureXlsb_t;

typedef union tempXlsb_t {

  uint8_t raw;
  struct {
    uint8_t spare5 : 4;
    uint8_t xlsb : 4;
  } fields;
} tempXlsb_t;

// ADDRESSES
#define BME_DEVICE_ADDR 0x76
#define BME_CALIBRATION_REGISTERS1_ADDR 0x88
#define BME_CALIBRATION_REGISTERS2_ADDR 0xE1
#define BME_STATE_REGISTERS_ADDR 0xD0
#define BME_INFO_REGISTERS_ADDR 0xF2

// CHIP SPECIFIC
#define BME_I2C_HANDLE i2c0
#define BME_I2C_SDA_PIN 28
#define BME_I2C_SCL_PIN 29
// #define BME_I2C_HANDLE i2c0
// #define BME_I2C_SDA_PIN 28
// #define BME_I2C_SCL_PIN 29

// ADDRESS VALUES
#define BME_RESET_VALUE 0xB6

//
// State Registers
//
typedef struct BmeStateRegisters {
  uint8_t id;
  unsigned short spare[5];
  uint8_t reset;
} BmeStateRegisters;

//
// Info Registers
//
typedef struct BmeInfoRegister {
  ctrlHum_t ctrlHum;
  status_t status;
  ctrlMeas_t ctrlMeas;
  config_t config;
  uint8_t spare;
  uint8_t pressureMsb;
  uint8_t pressureLsb;
  pressureXlsb_t pressureXlsb;
  uint8_t tempMsb;
  uint8_t tempLsb;
  tempXlsb_t tempXlsb;
  uint8_t humLsb;
  uint8_t humMsb;
} BmeInfoRegister;

uint32_t BME_GetTemperatue(void);
uint32_t BME_GetPressure(void);
uint16_t BME_GetHumidity(void);

void BME_Init(void);
uint8_t BME_GetDeviceId(void);
void BME_Read(uint32_t *pressure, uint32_t *temperature, uint16_t *humidity);
void BME_ResetChip(void);
void BME_GetDeviceIdLoop(void *);
#endif
