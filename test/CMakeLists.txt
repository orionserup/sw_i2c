
if(IDF_VERSION_MAJOR GREATER_EQUAL 4)

    idf_component_register( SRCS "esp32/test_driver.c" "esp32/test.c" "test_main.c"
                            INCLUDE_DIRS "../include" "."
                            REQUIRES unity)

    if(IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
    if(CONFIG_TEST_ULP AND CONFIG_ESP32S2_ULP_COPROC_RISCV OR CONFIG_ESP32S3_ULP_COPROC_RISCV)

        set(ulp_app_name ulp_test_app)
        file(GLOB ULP_SOURCES "../src/*.c" "esp32/ulp/*.c")
        set(ulp_exp_dep_srcs "esp32/test_main.c")
        ulp_embed_binary(${ulp_app_name} "${ULP_SOURCES}" "${ulp_exp_dep_srcs}")

    endif()
    endif()

else()


endif()    


