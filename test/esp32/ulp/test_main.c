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

#include <sdkconfig.h>
#include <ulp_riscv/ulp_riscv.h>
#include <ulp_riscv/ulp_riscv_gpio.h>
#include <ulp_riscv/ulp_riscv_utils.h>

#include "sw_i2c_master.h"

volatile uint32_t test_num;
volatile uint8_t address;
volatile uint32_t result = 0;
volatile bool running = false;

typedef uint32_t (*test)(void);

static uint32_t test_init();
static uint32_t test_write();
static uint32_t test_read();

static bool read_sda() { return (bool)ulp_riscv_gpio_get_level(CONFIG_GPIO_SDA); }
static bool read_scl() { return (bool)ulp_riscv_gpio_get_level(CONFIG_GPIO_SCL); }
static void write_sda(const bool state) { ulp_riscv_gpio_output_level(CONFIG_GPIO_SDA, state); }
static void write_scl(const bool state) { ulp_riscv_gpio_output_level(CONFIG_GPIO_SCL, state); }
static void delay_us(const uint16_t us) { ulp_riscv_delay_cycles(us * ULP_RISCV_CYCLES_PER_US); }

static bool inited = false;
static I2CMaster master;

int main() {

    if(!inited) {

    }

    static tests[] = { test_init, test_read, test_write };
    running = true;
    result = tests[test_num]();
    running = false;

    return 0;
}

static uint32_t test_init();
static uint32_t test_write();
static uint32_t test_read();