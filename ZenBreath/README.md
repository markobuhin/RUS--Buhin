# ZenBreath – RPG Biofeedback Spirometar za Rehabilitaciju Pluća

## 📝 Opis Projekta
**ZenBreath** je napredni ugradbeni IoT sustav temeljen na **ESP32** mikrokontroleru, dizajniran za pomoć pacijentima u respiratornoj rehabilitaciji. Sustav simulira kreativni scenarij stabilizacije leta: pacijent dugim i kontroliranim izdisajem mora zadržati pokazivač unutar "idealne zone" na zaslonu. Sustav u realnom vremenu izračunava vršni protok zraka i procjenjuje **FVC (Forsirani vitalni kapacitet)** integracijom podataka kroz vrijeme.

---

## 🛠️ Tehničke Značajke i Kompleksnost
Projekt je strukturiran tako da maksimizira ocjenu prema sveučilišnim kriterijima ugradbenih sustava:
1. **Multithreading (FreeRTOS):** Sustav koristi rad u više dretvi na dvije jezge ESP32 čipa. Obrada senzora i grafika izvršavaju se na `Core 1`, dok se Wi-Fi i HTTP poslužitelj izvršavaju na `Core 0`, jamčeći rad u realnom vremenu.
2. **Wi-Fi i Web Server:** ESP32 podiže vlastitu pristupnu točku (Access Point). Pacijent se može spojiti mobitelom i pratiti napredak uživo putem AJAX web sučelja.
3. **Energetska Učinkovitost (Standby način rada):** Nakon 10 sekundi neaktivnosti, sustav gasi OLED zaslon i Wi-Fi perifernu jedinicu te prelazi u stanje mirovanja. Ponovno se aktivira pritiskom na gumb koji softverski reaktivira sve podsustave, čime se vjerno simulira stvarna logika uštede energije mikrokontrolera unutar simulacijskog okruženja.
4. **I2C Sabirnica:** Pouzdana komunikacija sa SSD1306 OLED zaslonom visoke brzine osvježavanja.

---

## 🔌 Shema Spajanja (Hardverska Arhitektura)

| Komponenta | Pin na komponenti | Pin na ESP32 | Opis |
| :--- | :--- | :--- | :--- |
| **SSD1306 OLED** | SDA | **GPIO 21** | I2C Podatkovna linija |
| | SCL | **GPIO 22** | I2C Takt linija |
| | VCC / GND | 3V3 / GND | Napajanje zaslona |
| **Senzor (Potenciometar)** | SIG (Srednji pin) | **GPIO 34** | ADC1 analogni ulaz |
| **Push Button** | Pin 1 | **GPIO 14** | RTC vanjski prekid (Wake-up) |
| | Pin 2 | GND | Uzemljenje |

---

## 🚀 Upute za Pokretanje (Wokwi Simulator)

Projekt je u potpunosti kompatibilan s **Wokwi** online simulatorom.
1. Kopirajte izvorni kod iz `src/ZenBreath.ino` u Wokwi editor.
2. U **Library Manageru** unutar Wokwija (ikona kocke s lijeve strane) dodajte sljedeće knjižnice:
   - `Adafruit SSD1306`
   - `Adafruit GFX Library`
3. Pokrenite simulaciju klikom na gumb **Play**.
4. Mišem pomičite klizač potenciometra kako biste simulirali jačinu izdisaja i promatrajte grafički biofeedback na OLED-u.
5. Projekt se također može i otvoriti sa linka: https://wokwi.com/projects/465941218625482753

---

## 👥 Podjela posla u timu
- **Student 1 (Buhin):** Implementacija FreeRTOS arhitekture, konfiguracija Wi-Fi pristupne točke i razvoj HTTP poslužitelja te AJAX nadzorne ploče.
- **Student 2 (Klaić):** Integracija I2C SSD1306 OLED zaslona, razvoj algoritma za numeričku integraciju volumena i implementacija Standby strategije uštede energije.
