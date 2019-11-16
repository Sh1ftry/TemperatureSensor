#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoLog.h>

namespace Sensors {
    class TemperatureAndHumidity {
        private:
            DHT dht;
        public:
            TemperatureAndHumidity(uint8_t pin): dht(pin, DHT22) {}
            
            void init()
            {
                Log.notice(F("Initializing DHT sensor"));
                dht.begin();
            }

            float readTemperature()
            {
                Log.notice(F("Reading temperature"));
                float t = dht.readTemperature();
                if (isnan(t)) {
                    return -1.f;
                }
                return t;
            }

            float readHumidity()
            {
                Log.notice(F("Reading humidity"));
                float h = dht.readHumidity();
                if (isnan(h)) {
                    return -1.f; 
                }
                return h;
            }
    };
}
#endif /* TEMP_HUM_SENSOR_H */