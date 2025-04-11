# Project 2: Debugging and Humidity/Temperature Sensor with ESP32

**Part 2 of my IoT Lab Series using ESP32 and Raspberry Pi 4**

## Overview

This project builds on the previous setup by introducing hardware debugging and sensor data collection.
It covers two main objectives:
Using GDB to debug a RISC-V ELF binary and analyze function call registers.
Interfacing with the SHTC3 humidity and temperature sensor on the ESP32-C3 board and printing real-time data.

The goal is to strengthen skills in embedded debugging and sensor interfacing over I2C.

## Objectives

- ✅ Debug a RISC-V ELF file using GDB and OpenOCD
- ✅ Identify function arguments and return values from registers
- ✅ Read humidity and temperature from the SHTC3 sensor
- ✅ Power cycle the sensor correctly between reads
- ✅ Validate sensor reads using checksum verification
- ✅ Print readings in Celsius, Fahrenheit, and percentage humidity

## Project Structure
esp32-lab2-debugging-sensor/  
├── report.pdf # Lab report (required)  
├── lab2_debug/ # Lab 2.1: GDB Debugging  
│ ├── lab2_debug.elf  
│ ├── lab2_image.S  
│ └── gdbinit  
├── lab2_2/ # Lab 2.2: Humidity and Temperature Sensor  
│ ├── sdkconfig  
│ ├── CMakeLists.txt  
│ ├── README.md  
│ ├── hello.bin // not on git as file is too big to upload  
│ └── main/  
│ ├── CMakeLists.txt  
│ ├── main.c  
│ └── sensor.h  


## Setup Instructions

### 🐞 Lab 2.1: GDB Debugging

1. **Convert ELF to binary (if needed):**
riscv64-unknown-elf-objcopy -O binary lab2_debug.elf lab2_debug.bin

2. **Flash the binary manually:**
- Avoid idf.py flash as it may overwrite the ELF.
- Use the correct flashing method based on your board.

3. **Start OpenOCD:**
idf.py openocd

4. **Start GDB session with init file:**
riscv32-esp-elf-gdb -x gdbinit lab2_debug.elf

5. **Useful GDB commands:**
- target remote :3333
- set remote hardware-watchpoint-limit 2
- mon reset halt
- flushregs
- b *0x... (set breakpoints manually at addresses from disassembly)
- info registers (check parameter registers)
- Use objdump to find compute function and return addresses:
riscv64-unknown-elf-objdump -D lab2_debug.elf > lab2_image.S

6. **Answer in report.pdf:**
- What registers hold the 3 arguments to compute?
- Which register returns the result?
- What is the entry point address of compute?
- What is the address of the return instruction in compute?

### 🌡️ Lab 2.2: Humidity and Temperature Sensor

1. **Power cycle the sensor every 2 seconds:**  
- Power up before read, power down after read.

2. **Create separate functions:**  
- One for reading temperature (max 3 bytes read)
- One for reading humidity (max 3 bytes read)

3. **Use checksum to validate sensor data.**

4. **Print output every 2 seconds:**
Example:
Temperature is 22C (or 72F) with a 42% humidity
Temperature is 20C (or 68F) with a 44% humidity

5. **Verify output during project check-off.**

## Notes
- Exclude build/ directories when zipping the project.
- Submit the required files only.
- Document issues or learnings in report.pdf and subfolder README.md.
- All external code must follow APACHE or BSD-like licenses.
- Reference any helpful resources properly in report.pdf (No StackOverflow, Reddit).

### What I Learned
- GDB debugging with OpenOCD on embedded RISC-V
- Function call conventions and register usage in RISC-V
- Using objdump to disassemble binaries
- I2C communication protocol with SHTC3 sensor
- Power management for sensor devices
- Checksum validation for sensor data integrity

### Future Improvements
- Automate GDB setup with scripts
- Expand sensor application to log data over time
- Integrate cloud logging for remote sensor monitoring
- Explore additional sensors (e.g., motion, light)

### License
This project is for educational purposes.

Previous Project: [ESP32 LED Blinker](https://github.com/Inhle-C/Project-1-esp32-pi4-iot-lab-setup)  
(Part 1 of the series)

Next Project: [ESP32 LED Sensor Data Display](https://github.com/Inhle-C/Project-3-esp32-display-sensor)  
(To be uploaded as Part 3 of the series)


