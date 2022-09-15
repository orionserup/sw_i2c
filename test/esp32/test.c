/**
 * \file test_driver.c
 * \author Orion Serup (orionserup@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-09-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "test_driver.h"

#include "unity.h"

// #if CONFIG_TEST_ULP_RISCV

// static void init_ulp_riscv();
// static void gpio_init_ulp_riscv();

// TEST_CASE("RISC-V Initialization", "[sw-i2c][ulp-riscv]") {



// }

// #endif

int app_main(void) {

    UNITY_BEGIN();  

    unity_run_tests_by_tag("[sw_i2c]", false);

    UNITY_END();

    unity_run_menu();

    return 1;

}