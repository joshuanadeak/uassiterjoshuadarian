#include <Arduino_FreeRTOS.h>
#include <stdio.h>
#include <Servo.h>

void TaskTraffic( void *pvParameters );
void TaskTrain( void *pvParameters );

Servo Servo1; //Calling Servo1 for servo output

const uint8_t TrigPin = 10;   //Trigger pin on Ultrasonic sensor is hooked to Digital PWM pin 10 on Arduino
const uint8_t EchoPin = 9;    //Echo pin on Ultrasonic sensor is hooked to Digital PWM pin 9 on Arduino
const uint8_t BuzzPin = 8;    //Pin connecting from Buzzer's Vcc pin is hooked to Digital PWM pin 8
const uint8_t ServoPin = D4;  //Signal connection pin on servo is hooked to Digital PWM pin 11 on Arduino
const uint8_t red = D5;
const uint8_t yellow = D6;
const uint8_t green = D7;
int flagDetect = 0;
int trainNumber = 0;


float duration, distance; /* float duration variable to calculate the time taken for ultrasound to reach back to sensor,
                             distance will be obtained by calculating d = v/t */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

// Now set up two tasks to run independently.  
  xTaskCreate(
    TaskTraffic
    ,  "Traffic"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskTrain
    ,  "Train"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
    
}

void loop()
{
  // Empty. Things are done in Tasks.
}

void changeLights() {
// put your main code here, to run repeatedly:
  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);
  delay(3000);

// turn off yellow, then turn red on for 5 seconds
  digitalWrite(yellow, LOW);
  digitalWrite(red, HIGH);
  delay(8000);

// red and yellow on for 2 seconds (red is already on though)
  digitalWrite(yellow, HIGH);
  delay(2000);

// turn off red and yellow, then turn on green
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  delay(5000);  
}

void sensor() {
  noTone(BuzzPin);              //Initiates buzzer to not make sounds before a condition is met (distance < 50.00)
  Servo1.writeMicroseconds(0);  //Initiates servo to be in 0 degree position before a condition is met (distance < 50.00)
  if(distance < 50.00 && flagDetect == 0){
    flagDetect = 1;
    trainNumber++;
    Servo1.write(90);   //Servo rotates 90 degrees when the detected distance is less than 50

    for(int i = 0; i < 10; i++){
    Serial.println("1\n");
    tone(BuzzPin, 400); //
    delay(500);         // Codes in lines 51 to 54 allow the buzzer to create a 2-tone sound with an interval of 0.5 s
    tone(BuzzPin, 100); // when the detected distance is less than 50
    delay(500);   
    }      //
  
    flagDetect = 0;
  }
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskTraffic(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  for (;;)
  {
    changeLights();
    delay(18000); // wait for 18 seconds
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskTrain(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(BuzzPin, OUTPUT);
  Servo1.attach(ServoPin);  //Connecting the Servo1 servo output to ServoPin

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(TrigPin, LOW);

    duration = pulseIn(EchoPin, HIGH);
    distance = duration*0.0343/2;
    /*
      Source for making the ultrasonic distance measure:
      https://projecthub.arduino.cc/Isaac100/7cabe1ec-70a7-4bf8-a239-325b49b53cd4
      and https://www.youtube.com/watch?v=ZejQOX69K5M
    */
    sensor(); //Calling out void sensor() function
    Serial.println("0\n");
    //Serial.print("Distance: ");
    //Serial.print(distance);
    //Serial.println(" cm");
    delay(100);
  }
}
