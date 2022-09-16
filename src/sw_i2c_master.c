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

#include "../include/sw_i2c_master.h"

#include <string.h>

void sw_i2c_start(SWI2CMaster* const device) {

    if(device == NULL)
        return;
    
    device->started = true;   
    device->config.sda_write(1);
    device->config.delay(5);
    device->config.sda_write(0);
    device->config.delay(5);
}

void sw_i2c_stop(SWI2CMaster* const device) {

    if(device == NULL)
        return;

    device->started = false;
    device->config.sda_write(0);
    device->config.delay(5);
    device->config.sda_write(1);
    device->config.delay(5);
    
}

static void sw_i2c_master_write_bit(const SWI2CMaster* const dev, const bool bit) {

    //assert(dev);

    uint16_t period = (1000000.0f / (float)dev->frequency);
    
    while(dev->config.scl_read() == 0);
    dev->config.scl_write(0);
    dev->config.delay(1);
    dev->config.sda_write(bit);
    dev->config.delay(period / 2);
    dev->config.scl_write(1);   
    dev->config.delay(period / 2);

}

static bool sw_i2c_master_read_bit(const SWI2CMaster* const dev) {

    //assert(dev);

    const uint16_t period = (1000000.0f / (float)dev->frequency);

    while(dev->config.scl_read() == 0);
    dev->config.scl_write(0);    
    dev->config.delay(period / 2);
    dev->config.scl_write(1);    
    dev->config.delay(period / 4);
    bool val = dev->config.sda_read();
    dev->config.delay(period / 4);

    return val;

}

static bool sw_i2c_master_ack_check(const SWI2CMaster* const master) {

    const uint16_t period = (1000000.0f / (float)master->frequency);

    master->config.scl_write(0);     
    master->config.delay(period / 4); 
    master->config.sda_write(1);
    master->config.delay(period / 4);
    master->config.scl_write(1);
    master->config.delay(period / 4);
    bool acked = master->config.sda_read() == I2C_ACK;
    master->config.delay(period / 4);

    return acked;

}

static void sw_i2c_master_write_byte(const SWI2CMaster* const dev, const uint8_t data) {

    //assert(dev);

    for(uint8_t j = 0x80; j != 0; j >>= 1)
        sw_i2c_master_write_bit(dev, data & j);

}

static uint8_t sw_i2c_master_read_byte(const SWI2CMaster* const dev) {

    //assert(dev);

    uint8_t data = 0;
    for(uint8_t i = 7; i != UINT8_MAX; i--)
        data |= sw_i2c_master_read_bit(dev) << i;

    return data;
}

void sw_i2c_master_write_bus(const SWI2CMaster* const dev, const void* const data, const uint16_t size) {

    //assert(dev && data && size);

    for(uint16_t i = 0; i != size; i++) 
        sw_i2c_master_write_byte(dev, ((uint8_t*)data)[i]);

}

SWI2CMaster* sw_i2c_master_init(SWI2CMaster* const master, const SWI2CConfig* const config, const uint16_t freq) {

    if(config->delay == NULL)
        return NULL;

    if(config->sda_read == NULL || config->scl_write == NULL || config->sda_write == NULL)
        return NULL;

    if(freq == 0)
        return NULL;

    memcpy(&master->config, config, sizeof(SWI2CConfig));
    master->frequency = freq;
    master->started = false;

    return master;

}

void sw_i2c_master_deinit(SWI2CMaster* const master) {

    if(master == NULL)
        return;

    if(master->started)
        sw_i2c_stop(master);
    
    memset(master, 0, sizeof(SWI2CMaster));

}

bool sw_i2c_master_connect_slave(const SWI2CMaster* const dev, const uint8_t s_addr, const bool iswriting) {

    if(dev == NULL || !dev->started)
        return false;

    sw_i2c_master_write_byte(dev, (s_addr << 1) | !iswriting);
    if(!sw_i2c_master_ack_check(dev))
        return false;

    return true;
    
}

uint16_t sw_i2c_master_write(SWI2CMaster* const dev, const uint8_t s_addr, const void* const data, const uint16_t size) {

    //assert(dev && data && size);
    sw_i2c_start(dev);
    if(!sw_i2c_master_connect_slave(dev, s_addr, true))
        return 0;

    uint8_t i = 0;
    for(; i != size; i++) {
        sw_i2c_master_write_byte(dev, ((uint8_t*)data)[i]);
        if(!sw_i2c_master_ack_check(dev)) // if the slave didn't acknowledge then stop sending the data
            break;
    }
    
    sw_i2c_stop(dev);

    return i; // return how many bytes were sent

}

uint16_t sw_i2c_master_read(SWI2CMaster* const dev, const uint8_t s_addr, void* const data, const uint16_t size) {

    //assert(dev && data && size);

    sw_i2c_start(dev);
    if(!sw_i2c_master_connect_slave(dev, s_addr, false))
        return 0;

    uint16_t i = 0;
    for(; i != size; i++) {
        ((uint8_t*)data)[i] = sw_i2c_master_read_byte(dev);
        sw_i2c_master_write_bit(dev, (i == size - 1)? I2C_NACK: I2C_ACK); // if its the last bit we send a NACK to the slave otherwise ack a byte
    }

    sw_i2c_stop(dev);
    
    return i;

}

uint16_t sw_i2c_master_read_reg(SWI2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, void* const data, const uint16_t size) {

    //assert(dev && data && size);

    sw_i2c_start(dev);
    if(!sw_i2c_master_connect_slave(dev, s_addr, true))
        return 0;

    sw_i2c_master_write_byte(dev, reg_addr);
    if(!sw_i2c_master_ack_check(dev))
        return 0;

    dev->config.delay(100);

    return sw_i2c_master_read(dev, s_addr, data, size);

}

uint16_t sw_i2c_master_write_reg(SWI2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, const void* const data, const uint16_t size) {

    //assert(dev && data && size);

    sw_i2c_start(dev);
    if(!sw_i2c_master_connect_slave(dev, s_addr, true))
        return 0;

    sw_i2c_master_write_byte(dev, reg_addr);
    if(!sw_i2c_master_ack_check(dev))
        return 0;
    
    uint16_t i = 0;
    for(; i != size; i++) {
        sw_i2c_master_write_byte(dev, ((uint8_t*)data)[i]);
        if(!sw_i2c_master_ack_check(dev))
            break;
    }

    sw_i2c_stop(dev);

    return i;

}