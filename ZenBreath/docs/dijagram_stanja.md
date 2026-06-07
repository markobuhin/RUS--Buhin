stateDiagram-v2
    [*] --> INICIJALIZACIJA : Uključenje / Reset
    
    INICIJALIZACIJA --> STANJE_PRIPRAVNOSTI : Hardver OK / AP Podignut
    INICIJALIZACIJA --> GRESKA_HARDVERA : I2C Inicijalizacija Zakazala
    
    state STANJE_PRIPRAVNOSTI {
        [*] --> CekajDah
        CekajDah --> BrojiNeaktivnost : TrenutniProtok <= 8%
    }
    
    STANJE_PRIPRAVNOSTI --> DEEP_SLEEP : Neaktivnost > 30s
    STANJE_PRIPRAVNOSTI --> AKTIVNO_TESTIRANJE : Detektiran izdisaj (Protok > 8%)
    
    state AKTIVNO_TESTIRANJE {
        [*] --> MjerenjeURealnomVremenu
        MjerenjeURealnomVremenu --> IntegracijaVolumena : Svakih 100ms
        IntegracijaVolumena --> OsvjeziOLED_i_WebServer
    }
    
    AKTIVNO_TESTIRANJE --> PRIKAZ_IZVJESTAJA : Protok padne ispod 8% (Trajanje > 2s)
    
    PRIKAZ_IZVJESTAJA --> STANJE_PRIPRAVNOSTI : Istekao prikaz (6 sekundi)
    
    DEEP_SLEEP --> INICIJALIZACIJA : Pritisak na Gumb (RTC Wake-up na GPIO 14)
    GRESKA_HARDVERA --> [*] : Beskonačna petlja
