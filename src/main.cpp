#include <Arduino.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include "sensors/TemperatureAndHumidity.h"

const uint8_t TH_PIN=8;
const uint16_t BAUD_RATE=9600;
Sensors::TemperatureAndHumidity thSensor(TH_PIN);
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
    radio.printDetails();
  } else {
    Log.error("Failed");
  }
}

void loop()
{
  delay(1000);
  auto reading = thSensor.read();
  Log.verbose(F("Temperature %f *C and humidity %f %"), 
    reading.temperature, reading.humidity);
  Log.trace(F("Sending data"));
  char data[32] = "123";
  bool result = radio.write(data, sizeof(data));
  if(result)
  {
    Log.trace(F("Ack received"));
  }
}