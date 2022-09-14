/**
 * \file sw_i2c_master.h
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-08-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SW_I2C_MASTER_H
#define SW_I2C_MASTER_H

#include "sw_i2c.h"

/// \todo add clock stretching compatibility

/// @brief Master Structure, represents an I2C bus master
typedef struct I2CMaster {

    I2CConfig config;   ///< The Hardware Configuration
    uint16_t frequency; ///< The Master Clock Frequency
    bool started;       ///< If The Communication is Started

} I2CMaster;

/**
 * \brief 
 * 
 * \param master
 * \param config
 * \param freq
 * \return I2CMaster* 
 */
I2CMaster* i2c_master_init(I2CMaster* const master, const I2CConfig* const config, const uint16_t freq);

/**
 * \brief 
 * 
 * \param master
 */
void i2c_master_deinit(I2CMaster* const master);

/**
 * \brief Tries to Connect to a Slave with the given address
 * 
 * \param[in] dev: Device to Connect to a Slave From
 * \param[in] s_addr: Address of the Slave To Connect To 
 * \param[in] iswriting: If you are connecting to the slave to Write to it
 * \return true: If the Slave Acknowledged the master 
 * \return false: If the Slave didn't acknowledge the master or there is no slave
 */
bool i2c_master_connect_slave(const I2CMaster* const dev, const uint8_t s_addr, const bool iswriting);

/**
 * \brief Writes to a slave on the i2c bus with address s_addr
 * 
 * \param[in] dev: Software I2C Device to Write/Read from
 * \param[in] s_addr: Slave to Write to
 * \param[in] data: Data to write to the slave
 * \param[in] size: How many bytes to write to the slave
 * \return uint16_t: How many bytes were written
 */
uint16_t i2c_master_write(const I2CMaster* const dev, const uint8_t s_addr, const void* const data, const uint16_t size);

/**
 * \brief Reads from a slave on the i2c bus
 * 
 * \param[in] dev: Software I2C Device to Write/Read with
 * \param[in] s_addr: The address of the slave to write to
 * \param[out] data: The place to put the data read
 * \param[in] size: How many bytes to read
 * \return uint16_t: How many bytes were transferred at the end of the transaction
 */
uint16_t i2c_master_read(const I2CMaster* const dev, const uint8_t s_addr, void* const data, const uint16_t size);

/**
 * \brief Reads the value of a register from a slave
 * 
 * \param[in] dev: Software I2C Device to write/read the data with
 * \param[in] s_addr: The slave to read from
 * \param[in] reg_addr: The Address of the register to read/start from
 * \param[out] data: The Place to put the data
 * \param[in] size: How many bytes to read
 * \return uint16_t: How many bytes were read
 */
uint16_t i2c_master_read_reg(const I2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, void* const data, const uint16_t size);

/**
 * \brief Write to a register on the i2c slave 
 * 
 * \param[in] dev: I2C Device Created in software to write/read from
 * \param[in] s_addr: The address of the slave to write to
 * \param[in] reg_addr: The address of the register to start at/write to
 * \param[in] data: The data to write to the register/sequence
 * \param[in] size: The number of bytes to write
 * \return uint16_t: The Number of bytes written
 */
uint16_t i2c_master_write_reg(const I2CMaster* const dev, const uint8_t s_addr, const uint8_t reg_addr, const void* const data, const uint16_t size);

#endif