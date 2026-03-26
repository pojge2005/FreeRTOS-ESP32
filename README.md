# FreeRTOS-ESP32-Suspend and Resume Tasks
 suspend and resume a FreeRTOS task on the ESP32. We’ll create a task that blinks an LED, and use a pushbutton to control it. When the button is pressed, the task will be suspended to stop the blinking, and pressing it again will resume the task.
Parts Required
  For this example, you need the following parts:

    -ESP32 Board – read Best ESP32 Development Boards
    -LED
    -220 Ohm resistor (or similar values)
    -Pushubutton
    -Breadboard
    -Jumper wires
Wiring the Circuit
         -Add a pushbutton to your previous circuit. We’re connecting the pushbutton to GPIO 23, but you can use any              other suitable GPIO.

ESP32: Suspend and Resume FreeRTOS Tasks – Arduino Code
#define LED1_PIN 2
#define BUTTON_PIN 23

// Task handle
TaskHandle_t BlinkTaskHandle = NULL;

// Volatile variables for ISR
volatile bool taskSuspended = false;
volatile uint32_t lastInterruptTime = 0;
const uint32_t debounceDelay = 100; // debounce period

void IRAM_ATTR buttonISR() {
  // Debounce
  uint32_t currentTime = millis();
  if (currentTime - lastInterruptTime < debounceDelay) {
    return;
  }
  lastInterruptTime = currentTime;

  // Toggle task state
  taskSuspended = !taskSuspended;
  if (taskSuspended) {
    vTaskSuspend(BlinkTaskHandle);
    Serial.println("BlinkTask Suspended");
  } else {
    vTaskResume(BlinkTaskHandle);
    Serial.println("BlinkTask Resumed");
  }
}

void BlinkTask(void *parameter) {
  for (;;) { // Infinite loop
    digitalWrite(LED1_PIN, HIGH);
    Serial.println("BlinkTask: LED ON");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1000ms
    digitalWrite(LED1_PIN, LOW);
    Serial.println("BlinkTask: LED OFF");
    Serial.print("BlinkTask running on core ");
    Serial.println(xPortGetCoreID());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);


  // Initialize pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Internal pull-up resistor

  // Attach interrupt to button
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // Create task
  xTaskCreatePinnedToCore(
    BlinkTask,         // Task function
    "BlinkTask",       // Task name
    10000,             // Stack size (bytes)
    NULL,              // Parameters
    1,                 // Priority
    &BlinkTaskHandle,  // Task handle
    1                  // Core 1
  );
}

void loop() {
  // Empty because FreeRTOS scheduler runs the task
}




How Does the Code Work?
We already covered how to create tasks in the previous example. So, we’ll just cover the relevant parts for this section.

We define the GPIOs for the LED and the pushbutton.

#define LED1_PIN 2
#define BUTTON_PIN 23
We create volatile variables that will be used in the ISR (interrupt service routine for the pushbutton). The taskSuspended variable is used to determine whether the task is suspended or not, and the lastInterruptTime and debounceDelay are needed to debounce the pushbutton.

// Volatile variables for ISR
volatile bool taskSuspended = false;
volatile uint32_t lastInterruptTime = 0;
const uint32_t debounceDelay = 100; // debounce period
To detect pushbutton presses, we’re using interrupts. When we use interrupts, we need to define an interrupt service routine (a function that runs on the ESP32 RAM). In this case, we create the buttonISR() function. We must add IRAM_ATTR to the function definition to run it on RAM.

void IRAM_ATTR buttonISR() {
First, we debounce the pushbutton, and if a button press is detected, we toggle the state of the taskSuspended flag variable.

// Debounce
uint32_t currentTime = millis();
if (currentTime - lastInterruptTime < debounceDelay) {
  return;
}
lastInterruptTime = currentTime;

// Toggle task state
taskSuspended = !taskSuspended;
Suspend and Resume Tasks
Then, if the taskSuspended variable is true, we call the vTaskSuspend() function and pass the task handle as an argument. Here, you can see one of the uses of the Task Handle. It is a way to refer to the task to control it.

if (taskSuspended) {
  vTaskSuspend(BlinkTaskHandle);
If the taskSuspended variable is false, we call the vTaskResume() function to resume the execution of the task.

} else {
  vTaskResume(BlinkTaskHandle);
setup()
In the setup(), we must declare our pushbutton as an interrupt as follows:

attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

