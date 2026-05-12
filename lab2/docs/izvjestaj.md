## Platforma
ESP32

## Sleep mode
Deep Sleep

## Wakeup
GPIO interrupt

## Opis rada
ESP32 većinu vremena provodi u Deep Sleep modu.

Pritiskom na tipkalo:
- sustav se budi
- pali LED
- povećava brojač događaja
- vraća se u sleep

## RTC memorija
RTC_DATA_ATTR koristi se za spremanje broja događaja između sleep ciklusa.

## Zaključak
Projekt demonstrira event-driven energy management sustav pomoću svjetleće LED lampice.
