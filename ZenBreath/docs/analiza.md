# Analiza Sustava – ZenBreath Biofeedback Spirometar

## 1. Uvod i Svrha Sustava
ZenBreath je ugradbeni medicinsko-rehabilitacijski uređaj namijenjen pacijentima s respiratornim poteškoćama (npr. post-COVID oporavak, astma, KOPB). Svrha sustava je motivirati korisnika na izvođenje pravilnih, dugotrajnih i kontroliranih izdisaja kroz interaktivni grafički biofeedback.

Sustav rješava ključni problem klasične spirometrije: pacijenti često ne znaju modulirati snagu daha, što dovodi do netočnih rezultata ili neučinkovite vježbe. ZenBreath vizualizira izdisaj u realnom vremenu i vodi pacijenta kroz idealne respiratorne parametre.

---

## 2. Analiza Toka Podataka i Arhitekture (Data Flow)
Arhitektura sustava temelji se na asinkronom asimetričnom multiprocesiranju (AMP) unutar FreeRTOS okruženja na dvojezgrenom ESP32 mikrokontroleru.

### Podjela po jezgrama mikrokontrolera:
1. Core 1 (Glavna petlja - Mjerni podsustav):
   - Uzorkovanje: Svakih 100 ms sustav čita sirovu vrijednost s analognog ulaza (ADC) na GPIO 34.
   - Filtriranje šuma: Uveden je programski prag od 8% (currentFlow > 8). Sve vrijednosti ispod praga tretiraju se kao šum okoline i sustav ostaje u stanju pripravnosti.
   - Algoritam integracije: Volumen se računa diskretnom numeričkom integracijom u vremenu prema formuli gdje je trenutni postotak protoka pomnožen s vremenskim korakom (0.1 s) i kalibracijskim koeficijentom pretvorbe u mililitre (mL).
   - I2C Osvježavanje: Renderiranje grafičkih elemenata na SSD1306 OLED zaslonu (128x64 px) putem Wire protokola.

2. Core 0 (FreeRTOS dretva - Komunikacijski podsustav):
   - Mrežni stog: ESP32 podiže samostalnu Wi-Fi mrežu (Access Point) zaštićenu lozinkom.
   - Asinkroni Web Server: Dretva TaskServer neprekidno sluša i obrađuje klijentske HTTP zahtjeve bez blokiranja mjerne jezgre na drugoj dretvi.
   - API Podaci: Ruta /data vraća strukturirani niz u obliku stringa (currentFlow,maxFlow,totalVolume) koji se koristi za brzu asinkranu AJAX komunikaciju s klijentskim web sučeljem.

---

## 3. Analiza Potrošnje Energije (Power Management i Standby Logika)
Jedan od ključnih zahtjeva za ugradbene IoT sustave je energetska učinkovitost. U ovom projektu, strategija uštede energije prilagođena je stabilnom radu unutar simulacijskog okruženja (Wokwi) pomoću softverskog upravljanja stanjima (State Machine Standby):

- Aktivni način rada: Mikrokontroler i OLED zaslon rade u punom profilu potrošnje tijekom aktivnog testiranja i sinkronizacije podataka s web poslužiteljem.
- Automatski tajmer mirovanja: Sustav neprekidno evaluira varijablu lastActivityTime. Ako unutar 10 sekundi sustav ne detektira protok zraka ili interakciju, automatski se pokreće sekvenca prelaska u stanje mirovanja (vrijeme je optimizirano radi bržeg testiranja u simulatoru).
- Softverski Standby mod: Aktivacijom stanja sustavSpava, mikrokontroler programski gasi OLED zaslon (čišćenje međuspremnika i gašenje piksela) te potpuno isključuje Wi-Fi radijski modul (WiFi.mode(WIFI_OFF)). Time se simulira drastično smanjenje potrošnje energije gašenjem energetski najzahtjevnijih perifernih jedinica.
- Blokada petlje i reaktivacija: Tijekom stanja mirovanja, glavna dretva obustavlja izvršavanje algoritama za spirometriju i obradu klijentskih zahtjeva web poslužitelja, čime se resursi procesora oslobađaju. Sustav u brzim ciklusima nadzire isključivo stanje na GPIO pinu 14. Pritiskom na gumb (pad napona na LOW razinu), sustav trenutno izlazi iz standby stanja, ponovno podiže Wi-Fi pristupnu točku i vraća uređaj u punu operativnost.

---

## 4. Analiza Robusnosti i Sigurnosnih Mjera
- Validacija ulaza: Hardverski ADC ulaz (0-4095) se softverski mapira na linearnu skalu od 0-100% pomoću funkcije map(), sprječavajući preljev podataka ili nepredviđene matematičke pogreške u proračunu volumena.
- Robusnost hardverske inicijalizacije: Provjera uspješnosti pokretanja I2C zaslona (if(!display.begin)) sprječava "zamrzavanje" mikrokontrolera u beskonačnoj petlji bez povratne informacije, ispisujući grešku na serijski port.
- Mrežna sigurnost: Pristup pacijentovom panelu zaštićen je WPA2 sigurnosnim protokolom s predefiniranim mrežnim ključem čime se onemogućuje presretanje medicinskih podataka od strane neautoriziranih uređaja u dometu.
