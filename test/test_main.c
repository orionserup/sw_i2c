/**
 * \file test_main.c
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-09-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdint.h>
#include <stdbool.h>

#include <sw_i2c_master.h>

#include "test_driver.h"

#include "unity.h"

TEST_CASE("I2C Can Connect To Slave", "[sw_i2c]") {

    gpio_init();

    I2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    i2c_start(&master);
    TEST_ASSERT(i2c_master_connect_slave(&master, s_addr, true) == true);
    i2c_stop(&master);

    i2c_master_deinit(&master);

}

TEST_CASE("I2C Can Start and Stop", "[sw_i2c]") {

    gpio_init();
    I2CMaster master;
    i2c_init(&master);

    i2c_start(&master);
    TEST_ASSERT(master.config.sda_read() == 0);

    i2c_stop(&master);
    TEST_ASSERT(master.config.sda_read() == 1);

    i2c_master_deinit(&master);

}

TEST_CASE("I2C Can Write A Byte", "[sw_i2c]") {

    gpio_init();
    I2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    uint8_t byte = 0xAA;
    TEST_ASSERT(i2c_master_write(&master, s_addr, &byte, 1) == 1);

    i2c_master_deinit(&master);

}

TEST_CASE("I2C Can Read A Byte", "[sw_i2c]") {

    gpio_init();
    I2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    uint8_t byte;
    TEST_ASSERT(i2c_master_read(&master, s_addr, &byte, 1) == 1);

    i2c_master_deinit(&master);

}

TEST_CASE("I2C Can Write To A Register", "[sw_i2c]") {

    gpio_init();
    I2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    const uint8_t r_addr = slave_reg_address_get();
    uint8_t byte = 0xCC;
    TEST_ASSERT(i2c_master_write_reg(&master, s_addr, r_addr, &byte, 1) == 1);

    i2c_master_deinit(&master);

}

TEST_CASE("I2C Can Read From a Register", "[sw_i2c]") {

    gpio_init();
    I2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    const uint8_t r_addr = slave_reg_address_get();
    uint8_t byte;
    TEST_ASSERT(i2c_master_read_reg(&master, s_addr, r_addr, &byte, 1) == 1);

    i2c_master_deinit(&master);

}

TEST_CASE("I2C Can Read And Write To a Register", "[sw_i2c]") {

    gpio_init();
    I2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    const uint8_t r_addr = slave_reg_address_get();
    uint8_t byte_to_write = 0xAA;
    uint8_t byte_to_read = 0xCC;

    TEST_ASSERT(i2c_master_write_reg(&master, s_addr, r_addr, &byte_to_write, 1) == 1);
    TEST_ASSERT(i2c_master_read_reg(&master, s_addr, r_addr, &byte_to_read, 1) == 1);
    TEST_ASSERT(byte_to_read == byte_to_write);

    i2c_master_deinit(&master);

}