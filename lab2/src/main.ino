#include "esp_sleep.h"

#define BUTTON_PIN 13
#define LED_PIN 23

RTC_DATA_ATTR int counter = 0;

void blink(int n, int t) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(t);
    digitalWrite(LED_PIN, LOW);
    delay(t);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.print("Button state: ");
  Serial.println(digitalRead(BUTTON_PIN));

  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
  if (reason == ESP_SLEEP_WAKEUP_EXT0) {
    counter++;
    Serial.println("📬 Dogadaj detektiran!");
    Serial.print("Broj: ");
    Serial.println(counter);
    blink(10, 200);
  } else {
    Serial.println("Prvo pokretanje");
    blink(3, 300);
  }
  Serial.println("Spavanje za 2 sekunde...");
  delay(2000);
  blink(3, 100);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);
  Serial.println("Sleep...\n");
  delay(500);
  esp_deep_sleep_start();
}

void loop() {}
