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

TEST_CASE("GPIO Works Properly", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    master.config.scl_write(1);
    master.config.delay(5);
    TEST_ASSERT_EQUAL_MESSAGE(1, master.config.scl_read(), "Clock Isn't High");
    master.config.delay(5);
    master.config.scl_write(0);
    master.config.delay(5);
    TEST_ASSERT_EQUAL_MESSAGE(0, master.config.scl_read(), "Clock Isn't Low");
    master.config.delay(5);

    master.config.sda_write(1);
    master.config.delay(5);
    TEST_ASSERT_EQUAL_MESSAGE(1, master.config.sda_read(), "Data Isn't High");
    master.config.delay(5);
    master.config.sda_write(0);
    master.config.delay(5);
    TEST_ASSERT_EQUAL_MESSAGE(0, master.config.sda_read(), "Data Line Isnt Low");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Start and Stop", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    sw_i2c_start(&master);
    TEST_ASSERT_EQUAL_MESSAGE(0, master.config.sda_read(), "Start Didn't Pull the Data Line High");

    sw_i2c_stop(&master);
    TEST_ASSERT_EQUAL_MESSAGE(1, master.config.sda_read(), "Stop Didn't Pull the Data Line Low");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Write And Read a Bit", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    master.config.scl_write(0);
    
    sw_i2c_master_write_bit(&master, 0);
    TEST_ASSERT_EQUAL_MESSAGE(0, master.config.sda_read(), "Master Didn't Drive the Channel to 0");

    sw_i2c_master_write_bit(&master, 1);
    TEST_ASSERT_EQUAL_MESSAGE(1, master.config.sda_read(), "Master Didn't Write the Bit Appropriately");

    TEST_ASSERT_EQUAL_MESSAGE(1, sw_i2c_master_read_bit(&master), "Master Couldn't Read The 1 Bit Correctly");
    
    sw_i2c_master_deinit(&master);
    gpio_deinit();


}

TEST_CASE("I2C Can Write a Byte Correctly", "[sw_i2c]") {

    #include <stdio.h>

    uint8_t data = 0x55;

    printf("Outputting %hu\n", data);

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    master.config.delay(1000);

    for(uint8_t i = 0x80; i != 0; i >>= 1) {
        sw_i2c_master_write_bit(&master, (data & i) != 0);
        TEST_ASSERT_EQUAL_MESSAGE(((data & i) != 0), master.config.sda_read(), "Master Didn't Drive the Channel to The Correct level");
    }
    
    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Connect To Slave", "[sw_i2c]") 
{

    gpio_init();

    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    sw_i2c_start(&master);

    TEST_ASSERT_MESSAGE(sw_i2c_master_connect_slave(&master, s_addr, true), "Could Not Connect to Slave");
    
    sw_i2c_stop(&master);

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Write A Byte", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    uint8_t byte = 0x55;
    TEST_ASSERT_EQUAL_MESSAGE(1, sw_i2c_master_write(&master, s_addr, &byte, 1), "Couldn't Write a Byte to the Slave, Likely Bad ACK");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Do a Long Continuous Write", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    static uint8_t byte[255] = {0};
    TEST_ASSERT_EQUAL_MESSAGE(255, sw_i2c_master_write(&master, s_addr, &byte, 255), "Couldn't Send Long Byte String, Probably Bad ACK");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Read A Byte", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    uint8_t byte;
    TEST_ASSERT_EQUAL_MESSAGE(1, sw_i2c_master_read(&master, s_addr, &byte, 1), "Couldn't Read a Byte from the Slave");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Write To A Register", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    const uint8_t r_addr = slave_reg_address_get();
    uint8_t data[] = { 0x00, 0x08 };
    TEST_ASSERT_EQUAL_MESSAGE(2, sw_i2c_master_write_reg(&master, s_addr, r_addr, &data, 2), "Couldn't Write Bytes to Device Register");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Read From a Register", "[sw_i2c]") {

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = 0x41;
    const uint8_t r_addr = 0xE;
    uint16_t data = 0;

    TEST_ASSERT_EQUAL_MESSAGE(2, sw_i2c_master_read_reg(&master, s_addr, r_addr, &data, 2), "Couldn't Read a Bytes From the Device Register");

    #include <stdio.h>
        
    printf("ID is %u\n", data);

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}

TEST_CASE("I2C Can Read And Write To a Register", "[sw_i2c]") 
{

    gpio_init();
    SWI2CMaster master;
    i2c_init(&master);

    const uint8_t s_addr = slave_address_get();
    const uint8_t r_addr = slave_reg_address_get();
    uint16_t bytes_to_write = 0x5555;
    uint16_t bytes_to_read = 0xAAAA;

    TEST_ASSERT_EQUAL_MESSAGE(2, sw_i2c_master_write_reg(&master, s_addr, r_addr, &bytes_to_write, 2), "Coudn't Write 2 bytes to device register");

    TEST_ASSERT_EQUAL_MESSAGE(2, sw_i2c_master_read_reg(&master, s_addr, r_addr, &bytes_to_read, 2), "Couldn't Read 2 Bytes from the device register");
    
    TEST_ASSERT_EQUAL_MESSAGE(bytes_to_write, bytes_to_read, "The Bytes Written to the Device Register Don't Match the Bytes Read from the Register");

    sw_i2c_master_deinit(&master);
    gpio_deinit();

}