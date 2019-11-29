#ifndef WEATHER_STATION_DEVICE
#define WEATHER_STATION_DEVICE
#include <Arduino.h>
#include <ArduinoLog.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

class Device
{
    public:
        static void enableLogging(int level)
        {
            Log.begin(level, &Serial);
            Log.setSuffix([](Print* _logOutput){_logOutput->print('\n');});
        }
        
        static void enableWatchdog()
        {
            cli();
            wdt_reset();
            MCUSR &= ~(1<<WDRF);
            WDTCSR |= (1<<WDCE) | (1<<WDE);
            WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1);
            sei();
        }

        static void disableWatchdog()
        {
            cli();
            wdt_reset();
            MCUSR &= ~(1<<WDRF);
            WDTCSR |= (1<<WDCE) | (1<<WDE);
            WDTCSR = 0x00;
            sei();
        }
        
        static void sleep(uint64_t seconds, const volatile bool& shouldWakeUp)
        {
            ADCSRA = 0;
            power_all_disable();

            uint64_t sleepCycles = 0;
            while(sleepCycles < seconds && !shouldWakeUp)
            {
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_mode();
                sleepCycles++;
            }

            power_all_enable();
        }
};
#endif /* WEATHER_STATION_DEVICE */