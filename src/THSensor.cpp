#include "THSensor.h"
#include <DHT.h>
#include <ArduinoLog.h>

void THSensor::setup()
{
    pinMode(6, OUTPUT);
    digitalWrite(6, HIGH);
    Log.notice(F("Initializing DHT sensor"));
    dht.setup(dataPin, DHT::DHT22);
}

const THSensor::Reading& THSensor::getReading(bool ignoreSamplingInterval = false)
{
    if(ignoreSamplingInterval)
    {
        dht.resetTimer();
    }

    reading.temperature = dht.getTemperature();
    reading.humidity = dht.getHumidity();
    reading.valid = dht.getStatus() == 0 ? true : false;
    Log.trace(F("Getting reading status is %s"), dht.getStatusString());
    return reading;
}

void THSensor::Reading::getAsString(char* buffer) const
{
    char tempBuffer[8] = {0};
    char humBuffer[8] = {0};
    dtostrf(temperature, 3, 2, tempBuffer);
    dtostrf(humidity, 3, 2, humBuffer);
    sprintf(buffer, "%s,%s", tempBuffer, humBuffer);
}