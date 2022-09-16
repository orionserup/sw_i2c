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

#include <sw_i2c_master.h>

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

    };
    gpio_config(&config);

    TEST_ASSERT_MESSAGE(gpio_set_level(CONFIG_GPIO_SDA, 1) == ESP_OK, "GPIO Couldn't Set the Level of the SDA");
    TEST_ASSERT_MESSAGE(gpio_set_level(CONFIG_GPIO_SCL, 1) == ESP_OK, "GPIO Couldn't Set the Level of the SCL");

    esp_rom_delay_us(1000);

    TEST_ASSERT_MESSAGE(gpio_get_level(CONFIG_GPIO_SDA) == 1, "SDA Was Low When it Should Have been High");
    TEST_ASSERT_MESSAGE(gpio_get_level(CONFIG_GPIO_SCL) == 1, "SCL Was Low When it Should Have been High");

}

void gpio_deinit() {

    gpio_reset_pin(CONFIG_GPIO_SDA);
    gpio_reset_pin(CONFIG_GPIO_SCL);

}

static bool read_sda() { return (bool)gpio_get_level(CONFIG_GPIO_SDA); }
static bool read_scl() { return (bool)gpio_get_level(CONFIG_GPIO_SCL); }
static void write_sda(const bool state) { gpio_set_level(CONFIG_GPIO_SDA, state); }
static void write_scl(const bool state) { gpio_set_level(CONFIG_GPIO_SCL, state); }
static void delay_us(const uint16_t us) { esp_rom_delay_us(us); }

void i2c_init(SWI2CMaster* const master) {

    TEST_ASSERT(master != NULL);

    const static SWI2CConfig config = {

        .sda_write = write_sda,
        .scl_write = write_scl,
        .scl_read = read_scl,
        .sda_read = read_sda,

        .delay = delay_us
    };

    SWI2CMaster* res = sw_i2c_master_init(master, &config, CONFIG_I2C_FREQUENCY);
    TEST_ASSERT_MESSAGE(res != NULL, "I2C Master did Not COnfigure Correctly");
    TEST_ASSERT_MESSAGE(res->config.sda_write == write_sda, "SDA Writing Function Isn't Correct");
    TEST_ASSERT_MESSAGE(res->config.sda_read == read_sda, "SDA Reading Function isn't Correct");
    TEST_ASSERT_MESSAGE(res->config.scl_write == write_scl, "SCL Writing Function Doesn't Line Up");
    TEST_ASSERT_MESSAGE(res->config.scl_read == read_scl, "SCL Reading Function Doesn't Line Up");
    TEST_ASSERT_MESSAGE(res->frequency == CONFIG_I2C_FREQUENCY, "I2C Frequency Wasn't the Same as Assigned");

}

uint8_t slave_reg_address_get() { return CONFIG_I2C_SLAVE_REG_ADDRESS; }

uint8_t slave_address_get() { return CONFIG_I2C_SLAVE_ADDRESS; }