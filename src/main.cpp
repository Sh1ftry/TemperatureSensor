#include <Arduino.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include "sensors/TemperatureAndHumidity.h"

const uint16_t BAUD_RATE=9600;
Sensors::TemperatureAndHumidity thSensor(3);
RF24 radio(9, 10);
const uint64_t address = 0xABCDABCD71LL;

void setup()
{
  Serial.begin(BAUD_RATE);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.setSuffix([](Print* _logOutput){_logOutput->print('\n');});
  thSensor.init();
  if(radio.begin()) {
    radio.setPALevel(RF24_PA_MIN);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(3, 5);
    radio.openWritingPipe(address);
    radio.stopListening();
  } else {
    Log.error("Radio configuration failed");
  }
}

struct Packet {
  byte address;
  byte type;
  float data;
} packet;

void loop()
{
  delay(5000);

  packet.address = 123;
  packet.type = 0;
  packet.data = thSensor.readTemperature();

  if(!radio.write(&packet, sizeof(packet)))
  {
    Log.warning(F("Ack not received"));
  }
}