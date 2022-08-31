/**
 * \file sw_i2c_master.c
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-08-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sw_i2c_master.h"

static void i2c_start(const I2CMaster* const device) {

    assert(device);
    device->config.sda_write(0);

}

static void i2c_stop(const I2CMaster* const device) {

    assert(device);
    device->config.sda_write(1);

}

static void i2c_master_write_bit(const I2CMaster* const dev, const bool bit) {

    assert(dev);

    uint16_t period = (1000000.0f / (float)dev->frequency);
    
    dev->config.scl_write(0);
    dev->config.delay(period / 2);
    dev->config.sda_write(bit);
    dev->config.scl_write(1);   
    dev->config.delay(period / 2);

}

static bool i2c_master_read_bit(const I2CMaster* const dev) {

    assert(dev);

    uint16_t period = (1000000.0f / (float)dev->frequency);

    dev->config.scl_write(0);    
    dev->config.delay(period / 2);
    dev->config.scl_write(1);    
    dev->config.delay(period / 2);

    return dev->config.sda_read();

}

static void i2c_master_write_byte(const I2CMaster* const dev, const uint8_t data) {

    assert(dev);

    for(uint8_t j = 0x80; j != 0; j >>= 1)
        i2c_master_write_bit(dev, data & j);

}

static uint8_t i2c_master_read_byte(const I2CMaster* const dev) {

    assert(dev);

    uint8_t data = 0;
    for(uint8_t i = 7; i != UINT8_MAX; i--)
        data |= i2c_master_read_bit(dev) << i;

    return data;
}

void i2c_master_write_bus(const I2CMaster* const dev, const void* const data, const uint16_t size) {

    assert(dev && data && size);

    for(uint16_t i = 0; i != size; i++) 
        i2c_master_write_byte(dev, ((uint8_t*)data)[i]);

}


uint16_t i2c_master_write(const I2CMaster* const dev, const uint8_t s_addr, const void* const data, const uint16_t size) {

    assert(dev && data && size);

    uint8_t addr = (s_addr << 1);

    i2c_start(dev);
    i2c_master_write_byte(dev, addr);
    if(i2c_master_read_bit(dev))
        return 0;

    uint8_t i = 0;
    for(; i != size; i++) {
        i2c_master_write_byte(dev, ((uint8_t*)data)[i]);
        if(i2c_master_read_bit(dev)) // if the slave didn't acknowledge then stop sending the data
            break;
    }
    
    i2c_stop(dev);

    return i; // return how many bytes were sent

}

uint16_t i2c_master_read(const I2CMaster* const dev, const uint8_t s_addr, void* const data, const uint16_t size) {

    assert(dev && data && size);

    uint8_t addr = (s_addr << 1) | 1;

    i2c_start(dev);
    i2c_master_write_byte(dev, addr);
    if(i2c_master_read_bit(dev)) // if there isn't a device that acknowledged then its a failure
        return 0;

    uint16_t i = 0;
    for(; i != size; i++) {
        ((uint8_t*)data)[i] = i2c_master_read_byte(dev);
        i2c_master_write_bit(dev, (i == size - 1) * I2C_NACK); // if its the last bit we send a NACK to the slave otherwise ack a byte
    }

    i2c_stop(dev);
    
    return i;

}

uint16_t i2c_master_read_reg(const I2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, void* const data, const uint16_t size) {

    assert(dev && data && size);

    i2c_start(dev);
    i2c_master_write_byte(dev, s_addr << 1);
    if(i2c_master_read_bit(dev) != I2C_ACK)
        return 0;

    i2c_master_write_byte(dev, reg_addr);
    if(i2c_master_read_bit(dev) != I2C_ACK)
        return 0;

    return i2c_master_read(dev, s_addr, data, size);

}

uint16_t i2c_master_write_reg(const I2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, const void* const data, const uint16_t size) {

    assert(dev && data && size);

    i2c_start(dev);
    i2c_master_write_byte(dev, s_addr << 1);
    if(i2c_master_read_bit(dev) != I2C_ACK)
        return 0;

    i2c_master_write_byte(dev, reg_addr);
    if(i2c_master_read_bit(dev) != I2C_ACK)
        return 0;
    
    uint16_t i = 0;
    for(; i != size; i++) {
        i2c_master_write_byte(dev, ((uint8_t*)data)[i]);
        if(i2c_master_read_bit(dev) != I2C_ACK)
            break;
    }

    i2c_stop(dev);

    return i;

}