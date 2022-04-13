// ===================================
//      B31DG - EMBEDDED SOFTWARE
//           Assignment  3
//            Lucas Isaac
// ===================================

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// =================================================
//      VARIABLES & PINS & RESOURCES MANAGEMENT 
// =================================================

#define LED           15
#define error_LED     16
#define push_button   23
#define square_reader 35
#define analog_reader 34

unsigned const int half_max   = 3.3/2*1000; // 1 650 mV
bool               error_code =          0;
unsigned int       reception1 =          0;
unsigned int       reception2 =          0;
unsigned int       reception3 =          0;
unsigned int       reception4 =          0;

// creation of the queue and the semaphore

static QueueHandle_t     queue; 
static SemaphoreHandle_t mutex;

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
const TickType_t xDelay9 = 1000 / portTICK_PERIOD_MS;




// ===================
//      STRUCTURE 
// ===================

typedef struct s_data {
  byte         button_state         :   1;
  unsigned int frequency_value      :  10; // another possibity is to put this on 9, and add 500
  unsigned int average_analog_value :  12; // analog value sotckable until 4096
};

s_data datas; 

// =============================
//            TASKS 
// =============================

// -------------------------- //
// --------- TASK 1 --------- //
// -------------------------- //

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

// -------------------------- //
// --------- TASK 2 --------- //
// -------------------------- //

void task2(void *pvParameters){
  
  (void) pvParameters;
  for(;;){
    xSemaphoreTake(mutex, portMAX_DELAY); // use of a semaphore to protect the data from the structure ( wait function of the semaphore)
    if(digitalRead(push_button)){
      datas.button_state = 1;
    }
    else{
      datas.button_state = 0;
    }
    xSemaphoreGive(mutex); // end of the use of the semaphore (signal function of the semaphore)
    //Serial.println("Task 2");
    vTaskDelay(xDelay2);
  }
}

// -------------------------- //
// --------- TASK 3 --------- //
// -------------------------- //

void task3(void *pvParameters){
  /* Use of a semaphore to write   
   * in the struct the square frequency 
   */
  (void) pvParameters;
  
  for(;;){

    unsigned long frequency;
    unsigned long half_period_high = pulseIn(square_reader, HIGH);
    unsigned long half_period_low  = pulseIn(square_reader, LOW);
    
    if((half_period_high + half_period_low) == 0){ // in the case there is no readed signal, we set the frequency at 0 to avoid a division by 0
      frequency = 0;
    }
    else{
      frequency = 1000000/(half_period_high + half_period_low);  
    }
    xSemaphoreTake(mutex, portMAX_DELAY);
    datas.frequency_value = frequency;
    xSemaphoreGive(mutex);
    //Serial.println("Task 3");
    /*
    Serial.print("Value of the frequency of the square : ");
    Serial.println(datas.frequency_value);*/
    vTaskDelay(xDelay3);
  }
}

// -------------------------- //
// --------- TASK 4 --------- //
// -------------------------- //

void task4(void *pvParameters){
  /* Use of a semaphore 
   * to write the analog values
   */
  (void) pvParameters;
  int analog = 0; 

  for(;;){
    analog = analogRead(analog_reader);
    xQueueSend(queue,&analog,1); // send the value inside the queue
    /*
    Serial.print("Analog value one reading = ");
    Serial.println(analog);
    */
    //Serial.println("Task 4");
    vTaskDelay(xDelay4);
    }
  }

// -------------------------- //
// --------- TASK 5 --------- //
// -------------------------- //

void task5(void *pvParameters){ // use of a queue
  (void) pvParameters;
  int reception1;
  int reception2;
  int reception3;
  int reception4;
  
  for(;;){
    reception4 = reception3; // save the values of the precedent variables to welcome the last variable of the queue
    reception3 = reception2;
    reception2 = reception1;
    xQueueReceive(queue,&reception1,portMAX_DELAY); // reception of the value in the queue to protect the data
    xSemaphoreTake(mutex,portMAX_DELAY);
    datas.average_analog_value = (reception1 + reception2 + reception3 + reception4)/4;
    xSemaphoreGive(mutex);
    /*
    Serial.print("Average value = ");
    Serial.println(datas.average_analog_value);
    */
    //Serial.println("Task 5");
    vTaskDelay(xDelay5);
  }
}

// -------------------------- //
// --------- TASK 6 --------- //
// -------------------------- //

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

// -------------------------- //
// --------- TASK 7 --------- //
// -------------------------- //

void task7(void *pvParameters){ 
  (void) pvParameters;
  for(;;){
    xSemaphoreTake(mutex,portMAX_DELAY); // protection of the average_analog_value 
    if(datas.average_analog_value > half_max){
      error_code = 1;
    }
    else{
      error_code = 0;
    }
    xSemaphoreGive(mutex);
    //Serial.println("Task 7");
    vTaskDelay(xDelay7);
  }
}

// -------------------------- //
// --------- TASK 8 --------- //
// -------------------------- //

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

// -------------------------- //
// --------- TASK 9 --------- //
// -------------------------- //

//use of a semaphore
void task9(void *pvParameters){ //print only if the button is pressed, 
  (void) pvParameters;
  for(;;){
    if(digitalRead(push_button)){
      xSemaphoreTake(mutex, portMAX_DELAY); //protection of shared data
      Serial.print  ("Pushbutton value : ");
      Serial.println(datas.button_state);
      Serial.print  ("frequency of the square signal = ");
      Serial.println(datas.frequency_value);
      Serial.print  ("Filtered analogue input value = ");
      Serial.println(datas.average_analog_value);
      xSemaphoreGive(mutex);
    } 
    //Serial.println("Task 9");
    vTaskDelay(xDelay9);
  }
}

// ==================================== //
//        INITIALISATION & SETUP        //
// ==================================== //

void setup() {
  
  Serial.begin(115200);
  
  pinMode(           LED, OUTPUT);
  pinMode(   push_button,  INPUT);
  pinMode( square_reader,  INPUT);
  pinMode( analog_reader,  INPUT);
  pinMode(     error_LED, OUTPUT);

  queue = xQueueCreate(4,sizeof(int)); // creation of a queue of 4 ints
  mutex = xSemaphoreCreateMutex(); // initialisation of the semaphore as mutual exclusion, so the initial value is 1
  
  xTaskCreatePinnedToCore(task1,"task1",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE); //4096 is the size of the task, 1 is the priority
  xTaskCreatePinnedToCore(task2,"task2",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task3,"task3",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task4,"task4",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE); 
  xTaskCreatePinnedToCore(task5,"task5",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task6,"task6",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task7,"task7",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE); 
  xTaskCreatePinnedToCore(task8,"task8",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task9,"task9",4096,NULL,1,NULL,ARDUINO_RUNNING_CORE);
  
  // any function has a parameter, so the value is NULL

}


void loop() {  }
