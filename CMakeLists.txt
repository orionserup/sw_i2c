cmake_minimum_required(VERSION 3.10)

if(IDF_VERSION_MAJOR GREATER_EQUAL 4)

    idf_component_register( SRC_DIRS src
                            INCLUDE_DIRS include)

else()

    project(SW_I2C LANGUAGES C VERSION 0.1)
    add_library(${PROJECT_NAME} STATIC src/sw_i2c_master.c src/sw_i2c_slave.c)
    target_include_directories(${PROJECT_NAME} PUBLIC include)
    target_compile_features(${PROJECT_NAME} PRIVATE c_std_99) # we use inline comments and a couple of other c99 things, so we need c99+

endif()