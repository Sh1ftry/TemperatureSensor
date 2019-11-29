#include <Arduino.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>
#include <RF24.h>

#include "../include/THSensor.h"
#include "../include/Device.h"
#include "../include/Configurator.h"
#include "../include/Packet.h"

#define DEBUG true
#define BAUD_RATE 9600
#define TH_POWER_PIN 6
#define TH_DATA_PIN 7
#define RADIO_CE_PIN 8
#define RADIO_CS_PIN 9
#define RADIO_RETRY_DELAY_MS 3
#define RADIO_RETRIES 5
#define GATEWAY_ADDRESS 0xABCDABCD71LL
#define DEVICE_ID 123
#define CONFIGURATION_INTERRUPT_PIN 3
#define MAX_CONFIGURATION_TIME_MS 30000

Packet packet = Packet();
THSensor thSensor(TH_DATA_PIN, TH_POWER_PIN);

RF24 radio(RADIO_CE_PIN, RADIO_CS_PIN);
Configurator configurator(CONFIGURATION_INTERRUPT_PIN, MAX_CONFIGURATION_TIME_MS, radio);

void setup()
{
  Serial.begin(BAUD_RATE);
  Device::enableLogging(LOG_LEVEL_VERBOSE);
  Device::enableWatchdog();
  
  pinMode(INPUT_PULLUP, CONFIGURATION_INTERRUPT_PIN);
  configurator.setup();
  
  thSensor.setup();
  
  if(radio.begin())
  {
    radio.setPALevel(RF24_PA_MIN);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(RADIO_RETRY_DELAY_MS, RADIO_RETRIES);
  }
  else
  {
    Log.error("F(Radio configuration failed)");
  }

  while(!configurator.isConfigurationAvailable())
  {
    #if DEBUG==true
    Serial.flush();
    #endif
    Device::sleep(UINT64_MAX, configurationRequested);
    configurator.loop();
  }

  radio.openWritingPipe(configurator.getGatewayAddress());
  radio.stopListening();
}

void loop()
{
  thSensor.wake();
  
  #if DEBUG==true
  Serial.flush();
  #endif
  
  Device::sleep(2, configurationRequested);
  configurator.loop();
  
  radio.powerUp();
  
  auto reading = thSensor.getReading(true);
  thSensor.sleep();
  
  if(reading.valid)
  {
    packet.id = configurator.getDeviceId();
    reading.getAsString(packet.payload);
    Log.trace(F("Sending Packet(id=%d,payload='%s')"), packet.id, packet.payload);
    radio.write(&packet, sizeof(packet));
  }

  radio.powerDown();
  memset(packet.payload, 0, sizeof(packet.payload));
  
  #if DEBUG==true
  Serial.flush();
  #endif
  
  Device::sleep(8, configurationRequested);
  configurator.loop();
}