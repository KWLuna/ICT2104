add_executable(${PROJECT_NAME}
        main.c
        mapping.c
        Nav_Map.c
        comms_uart.c
        comms_i2c.c
        encoder.c
        motor.c
        ultrasonic.c
        mpu.c
)

# Pull in common dependencies
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        pico_binary_info
        hardware_i2c
        hardware_pwm
        hardware_gpio
        hardware_clocks
        hardware_timer
)

# Enable USB serial
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

example_auto_set_url(${PROJECT_NAME})
