# Dijagram Stanja Sustava – ZenBreath

Ovaj dokument opisuje stanja sustava i uvjete prelaza unutar konačnog automata (FSM) ugradbene aplikacije ZenBreath.

## 1. Grafički prikaz stanja (Mermaid format)

```mermaid
stateDiagram-v2
    [*] --> INICIJALIZACIJA : Uključenje / Reset
    
    INICIJALIZACIJA --> STANJE_PRIPRAVNOSTI : Hardver OK / AP Podignut
    INICIJALIZACIJA --> GRESKA_HARDVERA : I2C Inicijalizacija Zakazala
    
    state STANJE_PRIPRAVNOSTI {
        [*] --> CekajDah
        CekajDah --> BrojiNeaktivnost : TrenutniProtok <= 8%
        BrojiNeaktivnost --> DeepSleep : Neaktivnost > 30s
    }
    
    STANJE_PRIPRAVNOSTI --> AKTIVNO_TESTIRANJE : Detektiran izdisaj (Protok > 8%)
    
    state AKTIVNO_TESTIRANJE {
        [*] --> MjerenjeURealnomVremenu
        MjerenjeURealnomVremenu --> IntegracijaVolumena : Svakih 100ms
        IntegracijaVolumena --> OsvjeziOLED_i_WebServer
    }
    
    AKTIVNO_TESTIRANJE --> PRIKAZ_IZVJESTAJA : Protok padne ispod 8% (Trajanje > 2s)
    
    PRIKAZ_IZVJESTAJA --> STANJE_PRIPRAVNOSTI : Istekao prikaz (6 sekundi)
    
    DeepSleep --> INICIJALIZACIJA : Pritisak na Gumb (RTC Wake-up na GPIO 14)
    GRESKA_HARDVERA --> [*] : Beskonačna petlja
