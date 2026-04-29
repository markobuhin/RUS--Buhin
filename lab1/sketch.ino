const uint8_t LED_RED    = 23;
const uint8_t LED_YELLOW = 22;
const uint8_t LED_GREEN  = 21;

const uint8_t BTN1 = 13;
const uint8_t BTN2 = 12;

volatile bool highPressed = false;
volatile bool lowPressed = false;
volatile bool timerFlag = false;

bool highActive = false;
bool lowBlock = false;

unsigned long highEndTime = 0;
unsigned long lowEndTime = 0;

bool ledState = false;

hw_timer_t *timer = NULL;


void IRAM_ATTR onBtn1Press() {
  highPressed = true;
}

void IRAM_ATTR onBtn2Press() {
  lowPressed = true;
}

void IRAM_ATTR onTimer() {
  timerFlag = true;
}
void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);

  attachInterrupt(BTN1, onBtn1Press, FALLING);
  attachInterrupt(BTN2, onBtn2Press, FALLING);

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 500000, true, 0); // 0.5s
}
void loop() {

  unsigned long now = millis();
  if (highPressed) {
    highPressed = false;
    highActive = true;
    highEndTime = now + 2000; 
  }
  if (lowPressed) {
    lowPressed = false;
    lowBlock = true;
    lowEndTime = now + 5000; 
  }
  if (lowBlock) {
    if (now < lowEndTime) {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);
      return; 
    } else {
      lowBlock = false;
    }
  }
  if (highActive) {
    if (now < highEndTime || digitalRead(BTN1) == LOW) {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      return; 
    } else {
      highActive = false;
    }
  }
  if (timerFlag) {
    timerFlag = false;

    ledState = !ledState;
    digitalWrite(LED_YELLOW, ledState);
  }
}
