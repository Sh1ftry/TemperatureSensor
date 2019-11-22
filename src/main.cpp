#include <Arduino.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>
#include <RF24.h>
#include "sensors/TemperatureAndHumidity.h"

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

const uint16_t BAUD_RATE=9600;
Sensors::TemperatureAndHumidity thSensor(3);
RF24 radio(9, 10);
const uint64_t address = 0xABCDABCD71LL;

void setupWatchdog()
{
  cli();
  wdt_reset();
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0);
  sei();
}

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
    Log.error("F(Radio configuration failed)");
  }

  setupWatchdog();
}

void sleep(int desiredSleepCycles)
{
  int sleepCycles = 0;
  while(sleepCycles < desiredSleepCycles)
  {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
    sleepCycles++;
  }
}

ISR(WDT_vect)
{
  wdt_reset();
}

struct Packet
{
  uint8_t address;
  char payload[31];
} packet;

void loop()
{
  packet.address = 123;
  
  char tempBuffer[8] = {0};
  char humBuffer[8] = {0};
  dtostrf(thSensor.readTemperature(), 3, 2, tempBuffer);
  dtostrf(thSensor.readHumidity(), 3, 2, humBuffer);

  memset(packet.payload, 0, sizeof(packet.payload));
  sprintf(packet.payload, "%s,%s", tempBuffer, humBuffer);

  if(!radio.write(&packet, sizeof(packet)))
  {
    Log.warning(F("Ack not received"));
  }

  Serial.flush();
  sleep(2);
}