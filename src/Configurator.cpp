#include <EEPROM.h>
#include <ArduinoLog.h>
#include <inttypes.h>
#include "../include/Device.h"
#include "../include/Configurator.h"
#include "../include/Packet.h"

extern Packet packet;
volatile bool configurationRequested = false;
volatile bool handlingConfiguration = false;

void Configurator::setup()
{
    int value;
    EEPROM.get(EEPROM_CONFIGURED_POSITION, value);
    if(value == CONFIGURED_VALUE)
    {
        Log.notice(F("Configuration in EEPROM"));
        EEPROM.get(EEPROM_ID_POSITION, id);
        EEPROM.get(EEPROM_GATEWAY_ADDRESS_POSITION, gatewayAddress);
        configurationAvailable = true;
    }

    attachInterrupt(digitalPinToInterrupt(interruptPin), [&](){
        if(!handlingConfiguration)
        {
            configurationRequested=true;
        }
    }, FALLING);
}

void Configurator::loop()
{
    if(configurationRequested)
    {
        Device::disableWatchdog();
        handlingConfiguration = true;
        Log.notice(F("Configuration mode"));
        configurationRequested = false;
        
        radio.powerUp();
        radio.openReadingPipe(1, DEVICE_ADDRESS);
        radio.startListening();
        
        bool packetRead = false;
        unsigned long startTime = millis();
        unsigned long lastTime = startTime;
        while(startTime + maxConfigurationTime > millis())
        {
            if(lastTime + 5000 < millis())
            {
                Log.notice(F("Waiting for configuration..."));
                lastTime = millis();
            }
            if(radio.available())
            {
                Log.notice(F("Configuration provided"));
                radio.read(&packet, sizeof(packet));
                Log.trace(F("Recieved Packet(id=%d,payload='%s')"), packet.id, packet.payload);
                packetRead = true;
                break;
            }
        }
        
        radio.stopListening();
        radio.closeReadingPipe(1);
        radio.powerDown();
        
        if(packetRead)
        {
            Log.trace(F("Saving configuration to EEPROM"));
            id = packet.id;
            
            char half[9] = {0};
            
            snprintf(half, 9, "%s", packet.payload);
            uint32_t upper = strtoul(half, NULL, 16);
            half[0] = '\0';
            
            snprintf(half, 9, "%s", packet.payload+8);
            uint32_t lower = strtoul(half, NULL, 16);
            gatewayAddress = upper;
            gatewayAddress <<= 32;
            gatewayAddress += lower;
            
            EEPROM.put(EEPROM_ID_POSITION, id);
            EEPROM.put(EEPROM_GATEWAY_ADDRESS_POSITION, gatewayAddress);
            EEPROM.put(EEPROM_CONFIGURED_POSITION, CONFIGURED_VALUE);
            
            configurationAvailable = true;
            radio.openWritingPipe(gatewayAddress);
        }
        else
        {
            Log.warning(F("Configuration not recieved"));
        }

        memset(packet.payload, 0, sizeof(packet.payload));

        Device::enableWatchdog();
        handlingConfiguration = false;
    }
}