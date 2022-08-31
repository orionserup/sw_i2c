/**
 * \file sw_i2c_slave.h
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-08-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sw_i2c.h"

/// @brief Device Structure to Represent A Designated Software I2C device
typedef struct I2CSlave {

    I2CConfig config;   ///< The Backend Hardware Configuration and Timing
    uint8_t address;    ///< The Slave Address of the Device

} I2CSlave;