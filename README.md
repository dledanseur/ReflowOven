# Reflow Oven

This project aims at creating a reflow oven from the Tristar OV-1431 oven. The preparation of the device is heavily inspired from the project written by @TomasTT7: http://tt7hab.blogspot.com/2018/06/the-reflow-oven.html.

As such, this page will not describe the insulation process required for this model.

The software has been entirely rewritten however, relying on an ESP32. The device expose an API and a Vue.js application, in order to have a graph view of the soldering process.

## Hardware

This project is based on the following hardware:

| Pin Number           | Description                                                                                                              |
| -------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| SPI Touch LCD        | https://www.banggood.com/1_8-Inch-LCD-Screen-SPI-Serial-Port-Module-TFT-Color-Display-Touch-Screen-ST7735-p-1414465.html |
| Buzzer               | https://aliexpress.com/item/4000203106698.html                                                                           |
| 5V to 3.3V regulator | https://aliexpress.com/item/32774098119.html                                                                             |
| Type K thermocouple  | https://fr.aliexpress.com/item/32568236755.html                                                                          |

## Pinout

| Pin Number     | Target                                                |
| -------------- | ----------------------------------------------------- |
| 26             | Solid State Relay +                                   |
| 27             | Buzzer +                                              |
| 14             | Thermocouple SCK                                      |
| 12             | Thermocouple MISO                                     |
| 15             | Thermocouple CS                                       |
| 2              | TFT DC                                                |
| 4              | TFT RESET                                             |
| 16             | TFT CS (CS1)                                          |
| 18             | TFT CLK                                               |
| 19             | TFT MISO                                              |
| 21             | TFT Touch CS (CS2)                                    |
| 23             | TFT MOSI                                              |
| V5             | Regulator VIN                                         |
| GND            | Regulator GND / TFT GND / Thermocouple GND / Buzzer - |
| Regulator Vout | TFT Vcc / Thermocouple VCC                            |


## Development environment

This project has been built with Platform IO. You'll need it in order to deploy this project.

## Compiling and deploying on the ESP 32

The data folder contains a built version of the Vue.js application. It's build from the sources in the reflow-vue folder. You therefore need to deploy it via SPIFFS, thanks to the following command:

`pio run -t uploadfs`

After that, you can deploy the project on the ESP32.

