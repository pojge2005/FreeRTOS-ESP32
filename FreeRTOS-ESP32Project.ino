#include <WiFi.h>

// ----------- CONFIG -----------
#define LED_PIN 2
#define BUTTON_PIN 4

// WiFi (optional)
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";

// ----------- RTOS HANDLES -----------
QueueHandle_t sensorQueue;
SemaphoreHandle_t buttonSemaphore;

// ----------- ISR -----------
void IRAM_ATTR buttonISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);
}

// ----------- TASK 1: SENSOR -----------
void SensorTask(void *pvParameters) {
  int sensorValue;

  while (1) {
    sensorValue = random(20, 40);  // simulate temperature
    xQueueSend(sensorQueue, &sensorValue, portMAX_DELAY);

    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// ----------- TASK 2: LED CONTROL -----------
void LEDTask(void *pvParameters) {
  int receivedValue;

  while (1) {
    if (xQueueReceive(sensorQueue, &receivedValue, portMAX_DELAY)) {
      
      if (receivedValue > 30) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }

      Serial.println("LED Updated based on sensor");
    }

    // Check button interrupt
    if (xSemaphoreTake(buttonSemaphore, 0)) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      Serial.println("Button Pressed -> LED Toggled");
    }
  }
}

// ----------- TASK 3: WIFI -----------
void WiFiTask(void *pvParameters) {

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  Serial.println("\nConnected to WiFi");

  while (1) {
    Serial.println("Sending data to server...");
    // Here you can add MQTT / HTTP

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

// ----------- SETUP -----------
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Create Queue
  sensorQueue = xQueueCreate(5, sizeof(int));

  // Create Semaphore
  buttonSemaphore = xSemaphoreCreateBinary();

  // Attach Interrupt
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // Create Tasks
  xTaskCreate(SensorTask, "Sensor Task", 2048, NULL, 1, NULL);
  xTaskCreate(LEDTask, "LED Task", 2048, NULL, 2, NULL);
  xTaskCreate(WiFiTask, "WiFi Task", 4096, NULL, 1, NULL);
}

void loop() {
  // Empty (RTOS handles everything)
}
