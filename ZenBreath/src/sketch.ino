#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SPIROMETER_PIN 34 
#define WAKE_BUTTON_PIN 14 

// --- Wi-Fi i Web Server ---
const char* ssid = "ZenBreath-Spirometer";
WebServer server(80);
volatile int currentFlow = 0;
volatile int maxFlow = 0;
volatile float totalVolume = 0.0;
unsigned long testStartTime = 0;
volatile bool isTesting = false;
unsigned long lastActivityTime = 0;

// --- HTML i JavaScript za Web Sučelje (Biofeedback Dashboard) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ZenBreath Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #f0f4f8; color: #333; }
        .container { max-width: 500px; margin: auto; padding: 20px; }
        .card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); margin-bottom: 20px; }
        .bar-container { background: #ddd; border-radius: 20px; height: 30px; width: 100%; overflow: hidden; }
        .bar { background: #4caf50; height: 100%; width: 0%; transition: width 0.1s; }
        h1 { color: #2c3e50; }
    </style>
    <script>
        // Budući da ne koristimo web-sockete radi stabilnosti knjižnica, 
        // JavaScript svakih 200ms traži osvježene podatke s ESP32 (AJAX tehnika)
        setInterval(function() {
            fetch('/data').then(response => response.text()).then(data => {
                var values = data.split(',');
                document.getElementById('flowVal').innerText = values[0] + " %";
                document.getElementById('flowBar').style.width = values[0] + "%";
                document.getElementById('maxFlow').innerText = values[1];
                document.getElementById('volume').innerText = values[2];
            });
        }, 200);
    </script>
</head>
<body>
    <div class="container">
        <h1>ZenBreath Pacijent Panel</h1>
        <div class="card">
            <h3>Trenutni Protok Zraka</h3>
            <div class="bar-container"><div id="flowBar" class="bar"></div></div>
            <h2 id="flowVal">0 %</h2>
        </div>
        <div class="card">
            <h3>Statistika Testiranja</h3>
            <p>Maksimalni protok: <strong id="maxFlow">0</strong> %</p>
            <p>Volumen (FVC): <strong id="volume">0</strong> mL</p>
        </div>
    </div>
</body>
</html>
)rawliteral";

// Rute za Web Server
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

void handleData() {
  String data = String(currentFlow) + "," + String(maxFlow) + "," + String((int)totalVolume);
  server.send(200, "text/plain", data);
}
void TaskServer(void * pvParameters) {
  for(;;) {
    server.handleClient(); // Procesuiraj zahtjeve s mobitela
    vTaskDelay(10 / portTICK_PERIOD_MS); // Kratka pauza za stabilnost OS-a
  }
}

void provjeriPotrosnjuEnergije() {
  if (millis() - lastActivityTime > 30000) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Zzz... Deep Sleep");
    display.display();
    delay(2000);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_BUTTON_PIN, 0); 
    esp_deep_sleep_start();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);
  lastActivityTime = millis();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("OLED zakazao!"); 
    for(;;); 
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    display.setCursor(0,0);
    display.println("ZenBreath Probudjen!");
    display.display();
    delay(2000);
  }
  WiFi.softAP(ssid, "zenbreath123");
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  xTaskCreatePinnedToCore(TaskServer, "TaskServer", 4096, NULL, 1, NULL, 0);
}

void loop() {
  int rawValue = analogRead(SPIROMETER_PIN);
  currentFlow = map(rawValue, 0, 4095, 0, 100); 

  if (currentFlow > 8) {
    lastActivityTime = millis();
    if (!isTesting) {
      isTesting = true;
      testStartTime = millis();
      maxFlow = 0;
      totalVolume = 0;
    }
    totalVolume += (currentFlow * 0.1) * 4.5; 
    if (currentFlow > maxFlow) maxFlow = currentFlow;
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Protok: "); display.print(currentFlow); display.println(" %");
    
    display.drawRect(0, 14, 128, 12, SSD1306_WHITE);
    display.fillRect(40, 15, 40, 10, SSD1306_WHITE); 
    
    int markerX = map(currentFlow, 0, 100, 0, 124);
    display.fillCircle(markerX, 20, 4, SSD1306_WHITE);

    display.setCursor(0, 35);
    if (currentFlow >= 40 && currentFlow <= 70) {
      display.println("STABILNO! Drzi tako.");
    } else if (currentFlow < 40) {
      display.println("PRESLABO! Puhni jace.");
    } else {
      display.println("PREJAKO! Lakse malo.");
    }
    
    display.print("Volumen: "); display.print((int)totalVolume); display.println(" mL");
    display.display();

  } else {
    // Kraj testa
    if (isTesting && (millis() - testStartTime > 2000)) {
      isTesting = false;
      prikaziZavrsniIzvjestaj();
    }

    // Prikaz u stanju čekanja
    display.clearDisplay();
    display.setCursor(15, 10);
    display.println("=== ZenBreath ===");
    display.setCursor(0, 30);
    display.println("Sustav spreman...");
    display.println("Puhnite za pocetak.");
    display.display();
    
    provjeriPotrosnjuEnergije();
  }

  delay(100);
}

void prikaziZavrsniIzvjestaj() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("=== REZULTATI TESTA ===");
  display.print("Vrsni protok: "); display.print(maxFlow); display.println("%");
  display.print("FVC Volumen:  "); display.print((int)totalVolume); display.println(" mL");
  
  display.setCursor(0, 40);
  if (totalVolume > 2000) {
    display.println("Status: IZVRSNO!");
  } else {
    display.println("Status: Potrebna vjezba");
  }
  display.display();
  delay(6000); 
  lastActivityTime = millis(); 
}
