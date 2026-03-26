#define LED1_PIN 2
#define BUTTON_PIN 23

TaskHandle_t BlinkTaskHandle = NULL;

volatile bool toggleRequest = false;

void IRAM_ATTR buttonISR() {
  toggleRequest = true;   // only set flag (SAFE)
}

void BlinkTask(void *parameter) {
  bool suspended = false;

  for (;;) {

    // Check ISR request
    if (toggleRequest) {
      toggleRequest = false;

      if (suspended) {
        vTaskResume(NULL);  // resume self
        Serial.println("Task Resumed");
      } else {
        Serial.println("Task Suspended");
        suspended = true;
        vTaskSuspend(NULL); // suspend itself safely
      }
    }

    digitalWrite(LED1_PIN, HIGH);
    Serial.println("LED ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    digitalWrite(LED1_PIN, LOW);
    Serial.println("LED OFF");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  xTaskCreatePinnedToCore(
    BlinkTask,
    "BlinkTask",
    2048,
    NULL,
    1,
    &BlinkTaskHandle,
    1
  );
}

void loop() {}
