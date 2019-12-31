#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ArduinoLog.h>
#include "touchmanager/touch_manager.hpp"
#include "commands.hpp"
#include "temperaturemanager.hpp"
#include "buzzermanager.hpp"
#include "stop.h"
#include "start.h"
#include "up.h"
#include "down.h"
#include "touchmanager/calibrate.h"

#define TEMP_CS 15
#define TEMP_CLK -1
#define TEMP_MISO -1

#define SSR_PIN 26
#define BUZZER_PIN 27
#define PEN_IRQ GPIO_NUM_5

TFT_eSPI    tft = TFT_eSPI();         // Declare object "tft"
TouchManager touchManager(tft, TFT_GREEN, true);
TemperatureManager temperatureManager(TEMP_CLK, TEMP_CS, TEMP_MISO);
BuzzerManager buzzerManager(BUZZER_PIN);
SolderManager solderManager(temperatureManager, buzzerManager, SSR_PIN);

State state;
StartCommand startCmd(solderManager, state);
StopCommand stopCmd(solderManager);
UpCommand upCmd(state);
DownCommand downCmd(state);
 
Button* startButton;
Button* stopButton;
Button* upButton;
Button* downButton;

Label* profileLabel;
Label* temperatureLabel;
Label* stateLabel;
Label* timeLabel;

void callback() {
  Log.verbose("IRQ" CR);
  noInterrupts();
}

void setup() {
  Serial.begin(115200);
  
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
  pinMode(PEN_IRQ, INPUT);
  touchManager.init();
  touchManager.rotate(1);
  delay(2000);
  upButton = &touchManager.createButton(up_map,10,10,32,32, upCmd);
  downButton = &touchManager.createButton(down_map,10,86,32,32, downCmd);
  startButton = &touchManager.createButton(start_map,110,40,48,48, startCmd);
  stopButton = &touchManager.createButton(stop_map,110,40,48,48, stopCmd, true);
  touchManager.createLabel("Profile", 52, 10);
  profileLabel = &touchManager.createLabel("", 52, 20);
  touchManager.createLabel("Temp.", 52, 40);
  temperatureLabel = &touchManager.createLabel("", 52, 50);
  touchManager.createLabel("State", 52, 70);
  stateLabel = &touchManager.createLabel("", 52, 80);
  touchManager.createLabel("Time", 52, 100);
  timeLabel = &touchManager.createLabel("", 52, 110);

  uint16_t calData[5] = { 248, 3871, 214, 3943, 0 };
  touchManager.calibrateTouch(calData[0], calData[1], calData[2], calData[3]);
}

void updateScreenStatus() {
  double temp = temperatureManager.getThermcplTemp();
  temperatureLabel->setText(String(temp));
  profileLabel->setText(state.currentProfile->name);

  if (solderManager.isFinished()) {
    startButton->setHidden(false);
    stopButton->setHidden(true);
    upButton->setHidden(false);
    downButton->setHidden(false);
  }
  else {
    startButton->setHidden(true);
    stopButton->setHidden(false);
    upButton->setHidden(true);
    downButton->setHidden(true);
  }
  stateLabel->setText(solderManager.getSolderStateString());
  timeLabel->setText(solderManager.getTimeString());

}
void loop() {
  vTaskDelay(200);
  temperatureManager.read();
  vTaskDelay(200);
  //uint16_t x,y;
  //tft.getTouchRaw(&x, &y);
  //Log.notice("Raw touch %i, %i" CR, x, y);
  touchManager.loop();
  solderManager.loop();
  updateScreenStatus();
  touchManager.redraw();
  touchManager.flip();

}