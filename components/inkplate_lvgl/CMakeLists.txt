file(GLOB_RECURSE LVGL_SRCS "src/lvgl/src/*.c")

if(CONFIG_INKPLATE_BOARD_INKPLATE10)
    set(BOARD_SRC "src/boards/Inkplate10.cpp")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE6)
    set(BOARD_SRC "src/boards/Inkplate6.cpp" "src/features/I2S.cpp")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE6FLICK)
    set(BOARD_SRC "src/boards/Inkplate6.cpp" "src/features/I2S.cpp")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE6COLOR)
    set(BOARD_SRC "src/boards/Inkplate6Color.cpp" "src/features/SPI.cpp")
    set(DITHER_SRC "src/graphics/ditheringColor/ditherAlgorithm.cpp")
    set(DITHER_INCLUDES "include/graphics/ditheringColor")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE13)
    set(BOARD_SRC "src/boards/Inkplate13.cpp" "src/features/SPI.cpp")
    set(DITHER_SRC "src/graphics/ditheringColor/ditherAlgorithm.cpp")
    set(DITHER_INCLUDES "include/graphics/ditheringColor")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE5)
    set(BOARD_SRC "src/boards/Inkplate5.cpp" "src/features/I2S.cpp")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE4)
    set(BOARD_SRC "src/boards/Inkplate4.cpp")
    set(SENSOR_SRCS
        "src/features/APDS9960/APDS9960.cpp"
        "src/features/APDS9960/libs/APDS9960Driver.cpp"
        "src/features/BME680/BME680.cpp"
        "src/features/BME680/libs/BME680Driver.cpp"
        "src/features/BQ27441/BQ27441.cpp"
        "src/features/BQ27441/libs/BQ27441Driver.cpp"
        "src/features/LSM6DS3/LSM6DS3.cpp"
        "src/features/LSM6DS3/libs/LSM6DS3Driver.cpp")
    set(SENSOR_INCLUDES
        "include/features/APDS9960"
        "include/features/APDS9960/libs"
        "include/features/BME680"
        "include/features/BME680/libs"
        "include/features/BQ27441"
        "include/features/BQ27441/libs"
        "include/features/LSM6DS3"
        "include/features/LSM6DS3/libs")
elseif(CONFIG_INKPLATE_BOARD_INKPLATE2)
    set(BOARD_SRC "src/boards/Inkplate2.cpp")
    set(DITHER_SRC "src/graphics/ditheringColor/ditherAlgorithm.cpp")
    set(DITHER_INCLUDES "include/graphics/ditheringColor")
endif()

if(NOT DEFINED DITHER_SRC)
    set(DITHER_SRC "src/graphics/ditheringGrayscale/ditherAlgorithm.cpp")
    set(DITHER_INCLUDES "include/graphics/ditheringGrayscale")
endif()

if(CONFIG_INKPLATE_BOARD_INKPLATE2)
idf_component_register(SRCS "src/features/WiFi.cpp" "src/features/SPI.cpp"
                            "src/boards/Inkplate.cpp" "src/features/I2C.cpp"
                            ${BOARD_SRC}
                            ${DITHER_SRC}
                            ${LVGL_SRCS}
                            "src/lvgl/custom_alocation_algorithm.c"
                       REQUIRES "esp_driver_gpio"
                                "esp_driver_spi"
                                "esp_timer"
                                "esp_wifi" "nvs_flash" "esp_http_client"
                       INCLUDE_DIRS "include"
                                    "include/features"
                                    "include/boards"
                                    "include/graphics"
                                    ${DITHER_INCLUDES}
                                    "src/lvgl"
                                    "src/lvgl/src"
                                    )
else()
idf_component_register(SRCS "src/features/I2C.cpp" "src/features/RTC.cpp" "src/features/Frontlight.cpp"
                            "src/features/PCAL.cpp" "src/features/TPS.cpp" "src/features/SPI.cpp"
                            "src/features/WiFi.cpp" "src/features/SDCard.cpp"
                            "src/boards/BoardCommon.cpp" "src/features/TouchCypress.cpp" "src/features/TouchElan.cpp"
                            "src/boards/Inkplate.cpp"
                            ${BOARD_SRC}
                            ${DITHER_SRC}
                            ${SENSOR_SRCS}
                            ${LVGL_SRCS}
                            "src/lvgl/custom_alocation_algorithm.c"
                       REQUIRES "esp_driver_i2c" "esp_driver_i2s" "esp_driver_gpio" "esp_timer" "esp_adc"
                                "esp_driver_spi" "fatfs" "sdmmc"
                                "esp_wifi" "nvs_flash" "esp_http_client"
                       INCLUDE_DIRS "include"
                                    "include/features"
                                    "include/boards"
                                    "include/graphics"
                                    ${DITHER_INCLUDES}
                                    ${SENSOR_INCLUDES}
                                    "src/lvgl"
                                    "src/lvgl/src"
                                    )
endif()
