/**
 * \file test_driver.c
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-09-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <sdkconfig.h>
#include <driver/gpio.h>
#include <esp_rom_sys.h>

#include "unity.h"

#if (CONFIG_GPIO_SDA < 0) | (CONFIG_GPIO_SCL < 0)
#error "GPIO for SDA and SCL Need to to Be Legitimate"
#endif

#if (CONFIG_TEST_ULP_RISCV) & ((CONFIG_GPIO_SCL > 21) | (CONFIG_GPIO_SDA > 21))
#error "GPIO for SCL and SDA Need to be between 0 and 21 in order for ULP-RISCV Testing to Work"
#endif

#if (CONFIG_I2C_FREQUENCY <= 8) | !defined(CONFIG_I2C_SLAVE_ADDRESS) | !defined(CONFIG_I2C_SLAVE_REG_ADDRESS)
#error "We need the Frequency to Be in the Range [8, 50000] and the Slave and Register Address to Be Legit"
#endif

void gpio_init() {

    const static gpio_config_t config = {

        .pin_bit_mask = (1 << CONFIG_GPIO_SDA) | (1 << CONFIG_GPIO_SCL),
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD

    }
    gpio_config(&config);

    TEST_ASSERT(gpio_get_level(CONFIG_GPIO_SDA) == 1);
    TEST_ASSERT(gpio_get_level(CONFIG_GPI_SCL) == 1);

}

static bool read_sda() { return (bool)gpio_get_level(CONFIG_GPIO_SDA); }
static bool read_scl() { return (bool)gpio_get_level(CONFIG_GPIO_SCL); }
static void write_sda(const bool state) { TEST_ASSERT(gpio_set_level(CONFIG_GPIO_SDA, state) == ESP_OK); }
static void write_scl(const bool state) { TEST_ASSERT(gpio_set_level(CONFIG_GPIO_SCL, state) == ESP_OK); }
static void delay_us(const uint16_t us) { TEST_ASSERT(esp_rom_delay_us(us) == ESP_OK); }

void i2c_init(I2CMaster* const master) {

    TEST_ASSERT(master != NULL);

    static I2CCConfig config = {

        .sda_write = write_sda,
        .scl_write = write_scl,
        .scl_read = read_scl,
        .sda_read = read_sda,

        .delay = delay_us
    };

    I2CMaster* res = i2c_master_init(master, &config, CONFIG_I2C_FREQUENCY);
    TEST_ASSERT(res != NULL);
    TEST_ASSERT(res->config.sda_write == write_sda);
    TEST_ASSERT(res->config.sda_read == read_sda);
    TEST_ASSERT(res->config.scl_write == write_scl);
    TEST_ASSERT(res->config.scl_read == read_scl);
    TEST_ASSERT(res->freq == CONFIG_I2C_FREQUENCY);

    return res;

}

uint8_t slave_reg_address_get() { return CONFIG_I2C_SLAVE_REG_ADDRESS; }

uint8_t slave_address_get() { return CONFIG_I2C_SLAVE_ADDRESS; }