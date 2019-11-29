#ifndef WEATHER_STATION_CONFIGURATOR
#define WEATHER_STATION_CONFIGURATOR

#include <Arduino.h>
#include <RF24.h>

#define EEPROM_CONFIGURED_POSITION 0
#define CONFIGURED_VALUE 69
#define EEPROM_GATEWAY_ADDRESS_POSITION 8
#define EEPROM_ID_POSITION 16
#define DEVICE_ADDRESS 0xDABDABDAB1LL

extern volatile bool configurationRequested;
extern volatile bool handlingConfiguration;

class Configurator
{
    private:
        uint8_t interruptPin;
        
        uint8_t id;
        uint64_t gatewayAddress;

        uint16_t maxConfigurationTime;
        RF24 radio;
        bool configurationAvailable;
    public:
        Configurator(uint8_t interruptPin, uint16_t maxConfigurationTime, RF24& radio)
            : interruptPin(interruptPin), maxConfigurationTime(maxConfigurationTime), radio(radio) {}

        void setup();
        void loop();

        uint8_t getDeviceId() const
        {
            return id;
        }
        
        uint64_t getGatewayAddress() const
        {
            return gatewayAddress;
        }

        bool isConfigurationAvailable() const
        {
            return configurationAvailable;
        }

        void clearConfigurationAvailable()
        {
            configurationAvailable = false;
        }

        ~Configurator()
        {
            detachInterrupt(interruptPin);
        }
};

#endif /* WEATHER_STATION_CONFIGURATOR */