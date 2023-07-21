# CAB202-Alarm-Clock
Alarm Clock made for Arduino 

# AVR Microcontroller Project - README

This is the README file for an AVR microcontroller project written in C. The project uses various AVR libraries and functionalities to control LEDs, read temperature, and communicate with the Liquid Crystal Display (LCD). Below are the details of the project and its functionalities.

## Project Overview

The AVR microcontroller project aims to control LEDs, read temperature from a sensor, and display information on an LCD. The project involves the use of various AVR libraries and low-level configurations to achieve the desired functionalities.

## Prerequisites

To compile and upload this code to an AVR microcontroller, you'll need the following:

- AVR microcontroller (e.g., ATmega328P)
- AVR toolchain and compiler (e.g., AVR-GCC)
- AVR programmer (e.g., USBasp)
- Liquid Crystal Library

## Code Description

The `main.c` file contains the main code logic and sets up various functionalities of the microcontroller. Below are the key sections of the code:

### Libraries and Definitions

The code includes necessary AVR libraries, such as `avr/io.h`, `avr/interrupt.h`, and `util/delay.h`. Additionally, some macros and pin definitions are provided for ease of use.

### LCD Initialization

The LCD initialization is performed using the Liquid Crystal Library. The LCD interface pins are mapped to specific Arduino pins, and the `LiquidCrystal` class is used for communication with the LCD.

### Global Variables

Several global variables are defined to store time, temperature, and other data. The `mode` variable represents the state of the application (RGB select mode or temperature display mode).

### UART Initialization and Communication

The UART is initialized using `uart_init()` function, and functions `uart_putchar()` and `uart_getchar()` are used to send and receive characters via the UART interface. The project provides serial communication for interacting with the microcontroller.

### Timer Setup and Timekeeping

The `setup_timer()` function sets up Timer1 for counting the clock, and the interrupt service routine `ISR(TIMER1_OVF_vect)` is used for clock counting. The `current_time()` function calculates and returns the current time based on the number of timer overflows and counter value.

### Analog-to-Digital Conversion (ADC)

The code includes functions for setting up ADC (`setup_adc()`) and reading an analog channel (`adc(channel)`). The temperature sensor's analog voltage is read using the ADC, and the temperature is calculated and displayed on the LCD.

### PWM Setup and LED Control

The `pwm_setup()` function sets up Timer0 for PWM generation, which controls the LEDs' brightness. The code uses the PWM duty cycle to create a flashing ambiance light effect for the LEDs.

### Button Input and Mode Switching

The `toggle_mode()` function reads the left and right buttons using ADC. Depending on the button press, the `mode` variable is updated to switch between RGB select mode and temperature display mode.

### LCD Display Functions

The `what_to_print()` function displays information on the bottom line of the LCD based on the current mode (RGB select mode or temperature display mode). The `displayfunction()` function displays time on the top line of the LCD.

### Main Loop

The `main()` function contains the main loop, which continuously updates the display, checks for button presses, handles mode switching, and communicates with the UART when in RGB select mode.

## Getting Started

1. Connect the AVR microcontroller to your computer using a programmer.
2. Set up the AVR toolchain and compiler on your computer.
3. Compile the code using AVR-GCC and upload it to the microcontroller using the AVR programmer.
4. Connect the LCD and temperature sensor to the appropriate pins of the microcontroller.
5. Power on the microcontroller and interact with the project via UART and buttons.

## Note

Make sure to include the required libraries and set the correct pin configurations according to your specific hardware setup.

---