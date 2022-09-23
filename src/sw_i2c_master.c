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

void sw_i2c_start(SWI2CMaster* const device) {
    
    device->started = true;   
    device->config.sda_write(1);
    device->config.sda_write(0);
    device->config.delay(5);

}

void sw_i2c_restart(SWI2CMaster* const device) {
    
    device->config.delay(5);
    device->config.scl_write(0);
    device->config.delay(5);
    device->config.sda_write(1);
    device->config.delay(5);
    device->config.scl_write(1);
    device->config.delay(5);
    device->config.sda_write(0);
    device->config.delay(5);
}

void sw_i2c_stop(SWI2CMaster* const device) {

    device->started = false;
    device->config.delay(5);
    device->config.scl_write(0);
    device->config.sda_write(0);
    device->config.delay(5);
    device->config.scl_write(1);
    device->config.delay(5);
    device->config.sda_write(1);
    device->config.delay(5);
    
}

void sw_i2c_master_write_bit(const SWI2CMaster* const dev, const bool bit) {
    
    dev->config.scl_write(0);
    dev->config.sda_write(bit);  
    dev->config.delay(dev->period_us / 2); 
    dev->config.scl_write(1);    
    dev->config.delay(dev->period_us / 2);

}

bool sw_i2c_master_read_bit(const SWI2CMaster* const dev) {

    dev->config.scl_write(0);
    dev->config.sda_write(1);// let the slave drive the data 
    dev->config.delay(dev->period_us / 2);
    dev->config.scl_write(1);
    dev->config.delay(dev->period_us / 2);    
    return dev->config.sda_read();

}

bool sw_i2c_master_ack_check(const SWI2CMaster* const master) {

    return sw_i2c_master_read_bit(master) == I2C_ACK;

}

bool sw_i2c_master_write_byte(const SWI2CMaster* const dev, const uint8_t data) {

    for(uint8_t j = 0x80; j != 0; j >>= 1)
        sw_i2c_master_write_bit(dev, (data & j) != 0);

    if(!sw_i2c_master_ack_check(dev))
        return 0;

    return 1;

}

uint8_t sw_i2c_master_read_byte(const SWI2CMaster* const dev, const bool ack) {

    uint8_t data = 0;
    for(uint8_t i = 7; i != UINT8_MAX; i--)
        data |= (sw_i2c_master_read_bit(dev) << i);

    sw_i2c_master_write_bit(dev, ack);
    if(dev->config.sda_read() != ack)
        return 0xff;

    return data;

}

uint16_t sw_i2c_master_write_bus(const SWI2CMaster* const dev, const void* const data, const uint16_t size) {

    uint16_t i;
    for(i = 0; i != size; i++) {
        if(!sw_i2c_master_write_byte(dev, ((uint8_t*)data)[i]))
            break;
    }
    return i;
}

uint16_t sw_i2c_master_read_bus(const SWI2CMaster* const dev, void* const data, const uint16_t size) {

    uint16_t i = 0;
    for(; i != size; i++) {
        bool ack = (i == size - 1)? I2C_NACK: I2C_ACK;
        ((uint8_t*)data)[i] = sw_i2c_master_read_byte(dev, ack);
    }
    return i;
}


SWI2CMaster* sw_i2c_master_init(SWI2CMaster* const master, const SWI2CConfig* const config, const uint32_t freq) {

    if(config->delay == NULL)
        return NULL;

    if(config->sda_read == NULL || config->scl_write == NULL || config->sda_write == NULL)
        return NULL;

    if(freq == 0)
        return NULL;

    master->config.delay = config->delay;
    master->config.scl_read = config->scl_read;
    master->config.sda_read = config->sda_read;
    master->config.sda_write = config->sda_write;
    master->config.scl_write = config->scl_write;

    master->frequency = freq;    
    float period = 1.0f / (float)freq;
    master->period_us = (uint16_t)(100000.0f * period); 
    master->started = false;

    return master;

}

void sw_i2c_master_deinit(SWI2CMaster* const master) {

    if(master == NULL)
        return;

    if(master->started)
        sw_i2c_stop(master);
    
    master->config.delay = NULL;
    master->config.scl_read = NULL;
    master->config.sda_read = NULL;
    master->config.scl_write = NULL;
    master->config.sda_write = NULL;

}

bool sw_i2c_master_connect_slave(const SWI2CMaster* const dev, const uint8_t s_addr, const bool iswriting) {

    if(dev == NULL || !dev->started)
        return false;

    return sw_i2c_master_write_byte(dev, (s_addr << 1) | (iswriting? 0: 1));
    
}

uint16_t sw_i2c_master_write(SWI2CMaster* const dev, const uint8_t s_addr, const void* const data, const uint16_t size) {

    //assert(dev && data && size);
    sw_i2c_start(dev);
    if(!sw_i2c_master_connect_slave(dev, s_addr, true))
        return 0;

    uint16_t val = sw_i2c_master_write_bus(dev, data, size);
    
    sw_i2c_stop(dev);

    return val; // return how many bytes were sent

}

uint16_t sw_i2c_master_read(SWI2CMaster* const dev, const uint8_t s_addr, void* const data, const uint16_t size) {

    sw_i2c_start(dev);

    if(!sw_i2c_master_connect_slave(dev, s_addr, false))
        return 0;

    uint16_t i = sw_i2c_master_read_bus(dev, data, size);

    sw_i2c_stop(dev);
    
    return i;

}

uint16_t sw_i2c_master_read_reg(SWI2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, void* const data, const uint16_t size) {

    sw_i2c_start(dev);
    if(!sw_i2c_master_connect_slave(dev, s_addr, true))
        return 0;

    if(!sw_i2c_master_write_byte(dev, reg_addr))
        return 0;

    sw_i2c_restart(dev);

    if(!sw_i2c_master_connect_slave(dev, s_addr, false))
        return 0;

    uint16_t i = sw_i2c_master_read_bus(dev, data, size);
    
    sw_i2c_stop(dev);

    return i;

}

uint16_t sw_i2c_master_write_reg(SWI2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, const void* const data, const uint16_t size) {

    sw_i2c_start(dev);

    if(!sw_i2c_master_connect_slave(dev, s_addr, true))
        return 0;

    if(!sw_i2c_master_write_byte(dev, reg_addr))
        return 0;
    
    uint16_t i = sw_i2c_master_write_bus(dev, data, size);

    sw_i2c_stop(dev);

    return i;

}