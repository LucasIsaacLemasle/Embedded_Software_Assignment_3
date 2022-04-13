// ===============================
//    B31DG - EMBEDDED SOFTWARE
//         Assignment  3
//          Lucas Isaac
// ===============================

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// ==========================
//      VARIABLES & PINS
// ==========================

#define LED           15
#define error_LED     16
#define push_button   23
#define square_reader 35
#define analog_reader 14

float       analog_values[4] =  {0,0,0,0};
const int   half_max         = 3.3/2*1000; // 1 650 mV
bool        error_code       =          0;

// initialization of the queue 

static QueueHandle_t queue;


// ====================================
//      CREATION OF TASKS & DELAYS 
// ====================================

void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void task4(void *pvParameters);
void task5(void *pvParameters);
void task6(void *pvParameters);
void task7(void *pvParameters);
void task8(void *pvParameters);
void task9(void *pvParameters);

const TickType_t xDelay1 =    9 / portTICK_PERIOD_MS;
const TickType_t xDelay2 =  200 / portTICK_PERIOD_MS;
const TickType_t xDelay3 = 1000 / portTICK_PERIOD_MS;
const TickType_t xDelay4 =   42 / portTICK_PERIOD_MS;
const TickType_t xDelay5 =   42 / portTICK_PERIOD_MS;
const TickType_t xDelay6 =  100 / portTICK_PERIOD_MS;
const TickType_t xDelay7 =  333 / portTICK_PERIOD_MS;
const TickType_t xDelay8 =  333 / portTICK_PERIOD_MS;
const TickType_t xDelay9 = 5000 / portTICK_PERIOD_MS;




// ===================
//      STRUCTURE 
// ===================

struct s_data {
  byte         button_state         :   1;
  unsigned int frecuency_value      :  10; // another possibity is to put this on 9, and add 500
  unsigned int average_analog_value :  12; // analog value sotckable until 4096
};

// creation of the datas variable
s_data datas; 

// =============================
//            TASKS 
// =============================

// --------- TASK 1 --------- //

void task1(void *pvParameters){
  (void) pvParameters;
  for(;;){
    digitalWrite(LED, HIGH);
    delayMicroseconds(50);
    digitalWrite(LED, LOW);
    //Serial.println("Task 1");
    vTaskDelay(xDelay1);
  }
}

// --------- TASK 2 --------- //

void task2(void *pvParameters){
  /* use of a semaphore 
   *  to write in the struct
   *  for the button state
   */
  (void) pvParameters;
  for(;;){
    if(digitalRead(push_button)){
      datas.button_state = 1;
    }
    else{
      datas.button_state = 0;
    }
    //Serial.println("Task 2");
    //Serial.println(datas.button_state);
    vTaskDelay(xDelay2);
  }
}

// --------- TASK 3 --------- //

void task3(void *pvParameters){
  /* Use of a semaphore to write   
   * in the struct the square frequency 
   */
   (void) pvParameters;
  for(;;){
    //Serial.println("Task 3");
    vTaskDelay(xDelay3);
  }
}

// --------- TASK 4 --------- //

void task4(void *pvParameters){
  /* Use of a semaphore 
   *  to write the analog values
   */
  (void) pvParameters;
  for(;;){
    for(int i=1; i<4; i++){
      analog_values[i] = analog_values[i+1];
    }
    analog_values[4] = analogRead(analog_reader);
    //Serial.println("Task 4");
    vTaskDelay(xDelay4);
  }}


// --------- TASK 5 --------- //

void task5(void *pvParameters){ // use of a queue
  (void) pvParameters;
  for(;;){
    datas.average_analog_value = 0;
    for (int i=1; i<5; i++){
      datas.average_analog_value = datas.average_analog_value + analog_values[i];
    }
    datas.average_analog_value = datas.average_analog_value / 4;
    //Serial.println("Task 5");
    vTaskDelay(xDelay5);
  }
}


// --------- TASK 6 --------- //

void task6(void *pvParameters){
  (void) pvParameters;
  for(;;){
    for(int i=0; i<1000; i++){
      __asm__ __volatile__ ("nop");
    }
    //Serial.println("Task 6");
    vTaskDelay(xDelay6);
  }
}


// --------- TASK 7 --------- //

void task7(void *pvParameters){ // use of a queue
  (void) pvParameters;
  for(;;){
    if(analogRead(analog_reader) > half_max){
      error_code = 1;
    }
    else{
      error_code = 0;
    }
    //Serial.println("Task 7");
    vTaskDelay(xDelay7);
  }
}


// --------- TASK 8 --------- //

void task8(void *pvParameters){
  (void) pvParameters;
  for(;;){
    if(error_code){
      digitalWrite(error_LED,HIGH);
    }
    else{
      digitalWrite(error_LED,LOW);
    }
    //Serial.println("Task 8");
    vTaskDelay(xDelay8);
  }
}


// --------- TASK 9 --------- //

//use of a semaphore
void task9(void *pvParameters){ //print only the button is pressed, 
  (void) pvParameters;
  for(;;){
    if(digitalRead(push_button)){
      Serial.print("Pushbutton value : ");
      Serial.println(datas.button_state);
      Serial.print("Frecuency of the square signal = ");
      Serial.println(datas.frecuency_value);
      Serial.print("Filtered analogue input value = ");
      Serial.println(datas.average_analog_value);
    } 
    Serial.println("Task 9");
    vTaskDelay(xDelay9);
  }
}

// ============================ //
//        INITIALISATION        //
// ============================ //

void setup() {
  
  Serial.begin(115200);
  
  pinMode(          LED,OUTPUT);
  pinMode(  push_button, INPUT);
  pinMode(square_reader, INPUT);
  pinMode(analog_reader, INPUT);
  pinMode(    error_LED,OUTPUT);

  queue = xQueueCreate(4,sizeof(int)); // creation of a queue of 4 ints
  
  xTaskCreatePinnedToCore(task1,"task1",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE); //4096 is the size of the task, 1 is the priority
  xTaskCreatePinnedToCore(task2,"task2",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task3,"task3",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task4,"task4",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE); //4096 is the size of the task, 1 is the priority
  xTaskCreatePinnedToCore(task5,"task5",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task6,"task6",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task7,"task7",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE); //4096 is the size of the task, 1 is the priority
  xTaskCreatePinnedToCore(task8,"task8",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task9,"task9",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  
  // any function has a parameter, so the value is NULL

}


void loop() {  }
