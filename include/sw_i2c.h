/**
 * \file sw_i2c.h
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-08-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SW_I2C_H
#define SW_I2C_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define I2C_ACK     0
#define I2C_NACK    1

/// Everything needed to bit-bang i2c
typedef struct SWI2CCONFIG {

    void (*sda_write)(const bool state);  ///< The Function for the SDA GPIO writing
    void (*scl_write)(const bool state);  ///< The Function for the SCL GPIO writing, only needed if a master

    bool (*sda_read)(void);  ///< The Function for the Reading of the SDA GPIO
    bool (*scl_read)(void);  ///< The Function for the Reading of the SCL GPIO, only necessary if a slave

    void (*delay)(const uint16_t useconds);    ///< A delay function for proper timing

} SWI2CConfig;


#endif