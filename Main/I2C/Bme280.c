#include "BME280.h"
// unsigned 20-bit pressure and temperature
// unsigned 16-bit humidity
// Recommended to read 0xF7 - 0xFC
//

void BME_Init(void) {

  i2c_init(BME_I2C_HANDLE, 100 * 1000);
  gpio_set_function(BME_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(BME_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(BME_I2C_SDA_PIN);
  gpio_pull_up(BME_I2C_SCL_PIN);

  uint8_t addr = BME_INFO_REGISTERS_ADDR + offsetof(BmeInfoRegister, ctrlMeas);
  ctrlMeas_t measureVal;
  measureVal.fields.mode = 0x1;
  measureVal.fields.osrs_p = 0x1;
  measureVal.fields.osrs_t = 0x1;

  uint8_t buf[2];
  buf[0] = addr;
  buf[1] = measureVal.raw;

  i2c_write_blocking(BME_I2C_HANDLE, BME_DEVICE_ADDR, &addr, 1, false);
}

uint8_t BME_GetDeviceId(void) {
  uint8_t id = BME_STATE_REGISTERS_ADDR + offsetof(BmeStateRegisters, id);

  i2c_write_blocking(BME_I2C_HANDLE, BME_DEVICE_ADDR, &id, 1, true);
  i2c_read_blocking(BME_I2C_HANDLE, BME_DEVICE_ADDR, &id, 1, false);

  return id;
}

void BME_ResetChip(void) {

  uint8_t addr = BME_STATE_REGISTERS_ADDR + offsetof(BmeStateRegisters, reset);

  uint8_t buf[2];
  buf[0] = addr;
  buf[1] = BME_RESET_VALUE;

  i2c_write_blocking(BME_I2C_HANDLE, BME_DEVICE_ADDR, buf, 2, false);
}

void BME_Read(uint32_t *pressure, uint32_t *temperature, uint16_t *humidity) {

  uint8_t buf[6];
  uint8_t addr =
      BME_INFO_REGISTERS_ADDR + offsetof(BmeInfoRegister, pressureMsb);

  i2c_write_blocking(BME_I2C_HANDLE, BME_DEVICE_ADDR, &addr, 1, true);
  i2c_read_blocking(BME_I2C_HANDLE, BME_DEVICE_ADDR, buf, 6, false);

  *pressure = (buf[0] << 12) | (buf[1] << 4) | (buf[2] & 0x0F);
  *temperature = (buf[3] << 12) | (buf[4] << 4) | (buf[5] & 0x0F);
}

void BME_GetDeviceIdLoop(void *param) {
  BME_Init();
  for (;;) {
    vTaskDelay(310);
    printf("BME ID: %X\n", BME_GetDeviceId());
  }
}
