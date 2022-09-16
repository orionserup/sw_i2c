/**
 * \file test_driver.h
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-09-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sw_i2c_master.h"

/**
 * \brief 
 * 
 */
void gpio_init();

/**
 * \brief 
 * 
 */
void gpio_deinit();

/**
 * \brief 
 * 
 * \param master
 */
void i2c_init(SWI2CMaster* const master);

/**
 * \brief Get the slave address object
 * 
 * \return uint8_t 
 */
uint8_t slave_address_get(void);

/**
 * \brief Get the slave reg address object
 * 
 * \return uint8_t 
 */
uint8_t slave_reg_address_get(void);