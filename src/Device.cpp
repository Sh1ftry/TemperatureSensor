#include <Device.h>

ISR(WDT_vect)
{
  wdt_reset();
}