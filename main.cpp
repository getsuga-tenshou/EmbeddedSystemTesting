#include "mbed.h"
#include "board_freedom.h"
#include "adc.h"
#include "oled_ssd1322.h"
#include <cstdio>

#define MESSAGE_MAX_SIZE 50
#define TEMP_MIN 30.0
#define TEMP_MAX 35.0
#define TEMP_SAFE_MAX 40.0


DigitalOut greenLed(PTB3); // Green LED
DigitalOut redLed(PTB2); // Red LED
PwmOut heater_power(PTC2); // Heater
// MCP9701A


float readTemp(){
    uint16_t analog_in_value = adc_read(ADC0_SE16);
    float voltage = analog_in_value*3/65535.0;
    float temp = (voltage*1000 - 400)/19.5;
    return temp;
}

void displayMessage(const char* message){
    u8g2_ClearBuffer(&oled);
    u8g2_DrawUTF8(&oled, 10 , 10 , message);
    u8g2_SendBuffer(&oled);

}
void HeaterLedControl(float temp){
    if (temp<TEMP_MIN){
        heater_power = 1;
        redLed = 1;
        greenLed = 0;
        displayMessage("Heater Started");
    }
    else if (temp < TEMP_MAX && temp > TEMP_MIN ){
        heater_power = 0;
        redLed = 0;
        greenLed = 1;
        displayMessage("Temperature is in the desired range");
    }
    else if(temp > TEMP_SAFE_MAX){
        heater_power = 0;
        greenLed = 0;
        while(temp>TEMP_SAFE_MAX){
            redLed = !redLed;
            wait_ns(1000);
        }
        displayMessage("Overheating detected");
    }
    else {
        heater_power = 0;
        redLed= 1;
        greenLed = 0;
        displayMessage("Temperature higher than the desired range, turning off heater");
    }
}

int main(){
    board_init();
    u8g2_ClearBuffer(&oled);
    u8g2_SetFont(&oled, u8g2_font_6x12_mr);
    u8g2_SendBuffer(&oled);

    char message[MESSAGE_MAX_SIZE + 1];
    message[MESSAGE_MAX_SIZE] = '\0';

    while (true) {
        
        // uint16_t analog_in_value = adc_read(ADC0_SE16);
        // float voltage = analog_in_value * 3 / 65535.0 ;
        // float temp = (voltage * 1000 - 400) / 19.5 ;
        float temp = readTemp();
        // Read temperature sensor
        if (temp>=-40.0 && temp <= 125.0) { // MCP9701A operating range
            // Display temperature on LCD
            snprintf(message, MESSAGE_MAX_SIZE, "Temperature: %5.2f C", temp);
            // u8g2_ClearBuffer(&oled);
            // u8g2_DrawUTF8(&oled, 10, 10, message);
            // u8g2_SendBuffer(&oled);
            displayMessage(message);
            HeaterLedControl(temp);

            // // Control heater and LEDs based on temperature
            // if (temp < TEMP_MIN) {
            //     heater_power = 1; // Turn on heater
            //     redLed = 1; // Turn on red LED
            //     greenLed = 0; // Turn off green LED
            // } 
            // else if (temp > TEMP_MAX && temp < TEMP_SAFE_MAX) {
            //     heater_power = 0; // Turn off heater
            //     redLed = 0; // Turn off red LED
            //     greenLed = 0; // Turn off green LED
            // }
            // else if (temp >= TEMP_SAFE_MAX) {
            //     // Overheating detected, turn off heater and display error message
            //     heater_power = 0; // Turn off heater
            //     redLed = 0; // Turn off red LED
            //     greenLed = 0; // Turn off green LED
            //     snprintf(message, MESSAGE_MAX_SIZE, "Overheating detected!");
            //     u8g2_ClearBuffer(&oled);
            //     u8g2_DrawUTF8(&oled, 10, 10, message);
            //     u8g2_SendBuffer(&oled);
            // } 
            // else {
            //     heater_power = 0; // Turn off heater
            //     redLed = 0; // Turn off red LED
            //     greenLed = 1; // Turn on green LED
            // }

        } 
        else {
            // Display error message on LCD
            snprintf(message, MESSAGE_MAX_SIZE, "Temperature sensor not detected!");
            u8g2_ClearBuffer(&oled);
            u8g2_DrawUTF8(&oled, 10, 10, message);
            u8g2_SendBuffer(&oled);

            // Turn off heater and LEDs
            heater_power = 0;
            redLed = 0;
            greenLed = 0;
        }

       ThisThread::sleep_for(100ms);
    }
}
