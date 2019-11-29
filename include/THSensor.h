#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#include <DHT.h>
#include <ArduinoLog.h>

class THSensor
{
    public:
        class Reading {
            public:
                float temperature;
                float humidity;
                bool valid;
                void getAsString(char* buffer) const;
        };
    
    private:
        DHT dht;
        uint8_t dataPin;
        uint8_t powerPin;
        Reading reading;
    
    public:
        THSensor(uint8_t dataPin, uint8_t powerPin) 
            : dataPin(dataPin), powerPin(powerPin) {}
        
        void setup();
        void wake()
        {
            digitalWrite(powerPin, HIGH);
        }
        void sleep()
        {
            digitalWrite(powerPin, LOW);
        }
        const Reading& getReading(bool ignoreSamplingInterval = false);
};

#endif /* TEMP_HUM_SENSOR_H */