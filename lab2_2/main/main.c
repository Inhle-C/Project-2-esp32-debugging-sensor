
#include "driver/i2c.h"
#include <stdio.h>

#define I2C_MASTER_SCL_IO 8        // GPIO for SCL
#define I2C_MASTER_SDA_IO 10       // GPIO for SDA
#define I2C_MASTER_NUM I2C_NUM_0   // I2C port number
#define I2C_MASTER_FREQ_HZ 100000  // Frequency of I2C
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define SHTC3_SENSOR_ADDR 0x70     // I2C address for SHTC3

// Updated SHTC3 command codes
#define SHTC3_POWER_UP 0x3517      // Updated power-up command
#define SHTC3_POWER_DOWN 0xB098    // Power down command
#define SHTC3_READ_TEMP_HUMID 0x7CA2 // Updated read command

// I2C Master initialization
void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// Power-up function with updated command
void powerUpSensor() {
    uint8_t power_up_cmd[] = { (SHTC3_POWER_UP >> 8), (SHTC3_POWER_UP & 0xFF) };
    i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR, power_up_cmd, sizeof(power_up_cmd), 1000 / portTICK_PERIOD_MS);
}

// Power-down function
void powerDownSensor() {
    uint8_t power_down_cmd[] = { (SHTC3_POWER_DOWN >> 8), (SHTC3_POWER_DOWN & 0xFF) };
    i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR, power_down_cmd, sizeof(power_down_cmd), 1000 / portTICK_PERIOD_MS);
}

// CRC-8 checksum validation
uint8_t checkCRC(uint16_t data, uint8_t crc) {
    uint8_t crc_calc = 0xFF;  
    crc_calc ^= (data >> 8);  
    for (int i = 0; i < 8; ++i) crc_calc = (crc_calc & 0x80) ? (crc_calc << 1) ^ 0x31 : (crc_calc << 1);
    crc_calc ^= (data & 0xFF);  
    for (int i = 0; i < 8; ++i) crc_calc = (crc_calc & 0x80) ? (crc_calc << 1) ^ 0x31 : (crc_calc << 1);
    return crc_calc == crc;
}

// Function to read sensor data and validate it
uint16_t readSensorData(uint16_t *humidity) {
    uint8_t read_cmd[] = { (SHTC3_READ_TEMP_HUMID >> 8), (SHTC3_READ_TEMP_HUMID & 0xFF) };
    uint8_t data[6]; // 2 bytes temp, 1 byte temp CRC, 2 bytes humidity, 1 byte humidity CRC
   
    i2c_master_write_read_device(I2C_MASTER_NUM, SHTC3_SENSOR_ADDR, read_cmd, sizeof(read_cmd), data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    uint16_t rawTemp = (data[0] << 8) | data[1];
    uint16_t rawHumid = (data[3] << 8) | data[4];
   
    // Validate CRCs
    if (checkCRC(rawTemp, data[2]) && checkCRC(rawHumid, data[5])) {
        *humidity = rawHumid;
        return rawTemp;
    } else {
        printf("CRC Check failed!\n");
        return 0xFFFF; // Invalid value
    }
}

// Function to calculate temperature from raw data
float calculateTemperature(uint16_t rawTemp) {
    return -45.0 + 175.0 * ((float)rawTemp / 65535.0);
}

// Function to calculate humidity from raw data
float calculateHumidity(uint16_t rawHumid) {
    return 100.0 * ((float)rawHumid / 65535.0);
}

void app_main() {
    i2c_master_init();  // Initialize I2C

    while (1) {
        // Power up the sensor
        powerUpSensor();
        vTaskDelay(1 / portTICK_PERIOD_MS);  // Small delay for power-up

        // Read temperature and humidity data
        uint16_t rawHumid;
        uint16_t rawTemp = readSensorData(&rawHumid);
       
        // Only calculate if data is valid
        if (rawTemp != 0xFFFF) {
            // Convert raw data
            float temperatureC = calculateTemperature(rawTemp);
            float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;
            float humidity = calculateHumidity(rawHumid);

            // Print temperature and humidity
            printf("Temperature: %.2f°C (%.2f°F), Humidity: %.2f%%\n", temperatureC, temperatureF, humidity);
        }

        // Power down sensor
        powerDownSensor();

        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Wait 2 seconds
    }
}

