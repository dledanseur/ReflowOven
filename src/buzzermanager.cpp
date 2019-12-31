#include "buzzermanager.hpp"

BuzzerManager::BuzzerManager(uint8_t buzzerPin): buzzerPin(buzzerPin)
{
    channel = 0;
    resolution = 8;
    frequency = 2000;
    pinMode(buzzerPin, OUTPUT);
    ledcSetup(channel, frequency, resolution);
    ledcAttachPin(buzzerPin, channel);
}

void BuzzerManager::singleTone(unsigned int frequency, unsigned long duration) 
{
    ledcWriteTone(this->channel,frequency);
    delay(duration);
    ledcWriteTone(this->channel,0);
    
}

void BuzzerManager::triTone(unsigned int frequency, int increment, unsigned long singleDuration)
{
    ledcWriteTone(this->channel, frequency);
    delay(singleDuration);
    ledcWriteTone(this->channel,frequency + increment);
    delay(singleDuration);
    ledcWriteTone(this->channel,frequency + 2*increment);
    delay(singleDuration);
    ledcWriteTone(this->channel, 0);
}