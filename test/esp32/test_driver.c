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
#include <driver/rtc_io.h>
#include <esp_rom_sys.h>
#include <esp_err.h>

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
    
    ESP_ERROR_CHECK(rtc_gpio_init(CONFIG_GPIO_SCL));    
    ESP_ERROR_CHECK(rtc_gpio_set_drive_capability(CONFIG_GPIO_SCL, GPIO_DRIVE_CAP_3));
    ESP_ERROR_CHECK(rtc_gpio_pullup_en(CONFIG_GPIO_SCL));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(CONFIG_GPIO_SCL)); 
    ESP_ERROR_CHECK(rtc_gpio_set_direction(CONFIG_GPIO_SCL, RTC_GPIO_MODE_INPUT_OUTPUT_OD));
    ESP_ERROR_CHECK(rtc_gpio_set_level(CONFIG_GPIO_SCL, 0));  
    
    esp_rom_delay_us(100);

    ESP_ERROR_CHECK(rtc_gpio_init(CONFIG_GPIO_SDA));    
    ESP_ERROR_CHECK(rtc_gpio_set_drive_capability(CONFIG_GPIO_SDA, GPIO_DRIVE_CAP_3));
    ESP_ERROR_CHECK(rtc_gpio_pullup_en(CONFIG_GPIO_SDA));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(CONFIG_GPIO_SDA));    
    ESP_ERROR_CHECK(rtc_gpio_set_direction(CONFIG_GPIO_SDA, RTC_GPIO_MODE_INPUT_OUTPUT_OD));
    ESP_ERROR_CHECK(rtc_gpio_set_level(CONFIG_GPIO_SDA, 0));

    esp_rom_delay_us(100);

    TEST_ASSERT_MESSAGE(rtc_gpio_set_level(CONFIG_GPIO_SDA, 1) == ESP_OK, "GPIO Couldn't Set the Level of the SDA");
    TEST_ASSERT_MESSAGE(rtc_gpio_set_level(CONFIG_GPIO_SCL, 1) == ESP_OK, "GPIO Couldn't Set the Level of the SCL");

    esp_rom_delay_us(100);

    TEST_ASSERT_MESSAGE(rtc_gpio_get_level(CONFIG_GPIO_SDA) == 1, "SDA Was Low When it Should Have been High");
    TEST_ASSERT_MESSAGE(rtc_gpio_get_level(CONFIG_GPIO_SCL) == 1, "SCL Was Low When it Should Have been High");
    
    esp_rom_delay_us(100);

}

void gpio_deinit() {

    rtc_gpio_deinit(CONFIG_GPIO_SCL);
    rtc_gpio_deinit(CONFIG_GPIO_SDA);

    gpio_reset_pin(CONFIG_GPIO_SCL);
    gpio_reset_pin(CONFIG_GPIO_SDA);

}

static bool read_sda() { return (bool)rtc_gpio_get_level(CONFIG_GPIO_SDA); }
static bool read_scl() { return (bool)rtc_gpio_get_level(CONFIG_GPIO_SCL); }
static void write_sda(const bool state) { rtc_gpio_set_level(CONFIG_GPIO_SDA, state); }
static void write_scl(const bool state) { rtc_gpio_set_level(CONFIG_GPIO_SCL, state); }
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