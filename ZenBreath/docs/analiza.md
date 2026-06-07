# Analiza Sustava – ZenBreath Biofeedback Spirometar

## 1. Uvod i Svrha Sustava
ZenBreath je ugradbeni medicinsko-rehabilitacijski uređaj namijenjen pacijentima s respiratornim poteškoćama (npr. post-COVID oporavak, astma, KOPB). Svrha sustava je motivirati korisnika na izvođenje pravilnih, dugotrajnih i kontroliranih izdisaja kroz interaktivni grafički biofeedback.

Sustav rješava ključni problem klasične spirometrije: pacijenti često ne znaju modulirati snagu daha, što dovodi do netočnih rezultata ili neučinkovite vježbe. ZenBreath vizualizira izdisaj u realnom vremenu i vodi pacijenta kroz idealne respiratorne parametre.

---

## 2. Analiza Toka Podataka i Arhitekture (Data Flow)
Arhitektura sustava temelji se na asinkronom asimetričnom multiprocesiranju (AMP) unutar FreeRTOS okruženja na dvojezgrenom ESP32 mikrokontroleru.

### Podjela po jezgrama mikrokontrolera:
1. **Core 1 (Glavna petlja - Mjerni podsustav):**
   - **Uzorkovanje:** Svakih 100 ms sustav čita sirovu vrijednost s analognog ulaza (ADC) na GPIO 34.
   - **Filtriranje šuma:** Uveden je programski prag od 8% (`currentFlow > 8`). Sve vrijednosti ispod praga tretiraju se kao šum okoline i sustav ostaje u stanju pripravnosti.
   - **Algoritam integracije:** Volumen se računa diskretnom numeričkom integracijom u vremenu prema formuli gdje je trenutni postotak protoka pomnožen s vremenskim korakom (0.1 s) i kalibracijskim koeficijentom pretvorbe u mililitre (mL).
   - **I2C Osvježavanje:** Renderiranje grafičkih elemenata na SSD1306 OLED zaslonu (128x64 px) putem `Wire` protokola.

2. **Core 0 (FreeRTOS dretva - Komunikacijski podsustav):**
   - **Mrežni stog:** ESP32 podiže samostalnu Wi-Fi mrežu (Access Point) zaštićenu lozinkom.
   - **Asinkroni Web Server:** Dretva `TaskServer` neprekidno sluša i obrađuje klijentske HTTP zahtjeve bez blokiranja mjerne jezgre na drugoj dretvi.
   - **API Podaci:** Ruta `/data` vraća strukturirani niz u obliku stringa (`currentFlow,maxFlow,totalVolume`) koji se koristi za brzu asinkranu AJAX komunikaciju s klijentskim web sučeljem.

---

## 3. Analiza Potrošnje Energije (Power Management)
Jedan od ključnih zahtjeva za ugradbene IoT sustave je energetska učinkovitost. ZenBreath koristi napredne strategije uštede energije:

- **Aktivni način rada:** Mikrokontroler i OLED zaslon rade u punom profilu potrošnje (~120-160 mA) tijekom aktivnog testiranja.
- **Automatski Sleep Tajmer:** Sustav neprekidno evaluira varijablu `lastActivityTime`. Ako unutar 30 sekundi sustav ne detektira dah ili interakciju, automatski se aktivira sekvenca gašenja periferije.
- **Deep Sleep Mod:** Pozivom funkcije `esp_deep_sleep_start()`, glavna jezgra, Wi-Fi radio i memorija se gase, spuštajući potrošnju na mikroampersku razinu (< 15 µA).
- **RTC Domena i Eksterno Buđenje:** Prije odlaska u san, postavlja se prekid `esp_sleep_enable_ext0_wakeup` na GPIO pin 14. Jedino interna RTC (Real-Time Clock) domena ostaje pod naponom i osluškuje hardverski gumb za ponovno podizanje (boot) sustava.

---

## 4. Analiza Robusnosti i Sigurnosnih Mjera
- **Validacija ulaza:** Hardverski ADC ulaz (0-4095) se softverski mapira na linearnu skalu od 0-100% pomoću funkcije `map()`, sprječavajući preljev podataka ili nepredviđene matematičke pogreške u proračunu volumena.
- **Robusnost hardverske inicijalizacije:** Provjera uspješnosti pokretanja I2C zaslona (`if(!display.begin)`) sprječava "zamrzavanje" mikrokontrolera u beskonačnoj petlji bez povratne informacije, ispisujući grešku na serijski port.
- **Mrežna sigurnost:** Pristup pacijentovom panelu zaštićen je WPA2 sigurnosnim protokolom s predefiniranim mrežnim ključem čime se onemogućuje presretanje medicinskih podataka od strane neautoriziranih uređaja u dometu.
