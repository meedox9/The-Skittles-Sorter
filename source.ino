//including required header files
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <seeed-kit.h>

//defining each port in the color sensor

const int S0 = 8;
const int S1 = 9;
const int S2 = 10;
const int S3 = 11;
const int sensorOut = 12;

//initializing various global identifiers to zero

int frequency = 0;
int color = 0;
int servoAngles[] = {25, 50, 80, 110, 140}; //defining an array with 5 angle readings
int redCounter = 0;
int yellowCounter = 0;
int orangeCounter = 0;
int greenCounter = 0;
int purpleCounter = 0;
int emptyHole = 0;

//initializing servo motors
Servo topServo;
Servo bottomServo;

/*
   void setup is where the function declaration and port initializing
   is found. The code inside is executed only once at the beginning of
   the program and then never again.
*/
void setup() {
  /*
      the pinMode funtion is used to identify each port
      on the arduino board as an input or output port.
  */
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  lcdInit(); //initializes lcd screen


  /*
      according to the TC230 color sensor datasheet,
      setting pin S0 and S1 to HIGH will set the reading
      frequency scale to 100%
  */
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  //attaching each servo motor to certain digital port
  topServo.attach(5);
  bottomServo.attach(4);
  Serial.begin(9600);  // begins the serial monitor function to 9600 baud
}

/*
   the void loop function loops the code inside the block consecutively,
   which allows the program to change and respond. It is used to actively
   control the Arduino board.
*/
void loop() {

  int rotaryDial;
  /*
     The while loop is the main function of the program and connects
     everything together.
  */
  while (true) {

    rotaryDial = digitalRead(2); //the value of the rotary dial is constantly being read

    if (rotaryDial > 0) {
      lcdClear();  //clears the screen from previous output
      servoMovement();  //executes the servoMovement function below
    }
    /*
       the else function contains functions that are executed
       when the sorting mechanism is turned off.
    */
    else {
      warningScreen();  //executes the warning screen function
      valuesReset(redCounter, yellowCounter, orangeCounter
                  , greenCounter, purpleCounter, emptyHole); //resets the color count values to zero
      topServo.write(125); //moves the top servo to the middle
      bottomServo.write(90);  //moves the bottom servo to the middle
    }
  }

}
/*
   This function is what controls the servo motors based off
   the color reading.
*/
void servoMovement() {
  delay(1500);

  //
  for (int i = 125; i > 73; i--) {
    topServo.write(i);
    delay(2);
  }
  delay(1500);

  color = readColor(); //calls the readColor function which detects the color
  delay(10);
  /*
     The switch functions executes a certain set of code
     based off the color determined. Here, there are 6 cases.
  */
  switch (color) {
    case 1:
      redCounter++;  //increases the counter value by 1 each time it's detected
      bottomServo.write(servoAngles[0]);  //gets angle from array and moves servo
      lcdPrint("      RED");  //prints color name to the lcd
      lcdMoveCursor(0, 1);    // moves cursor to second line
      lcdPrint("skittles # ");
      lcdPrint(redCounter);  //prints out the number of skittles
      lcdBacklightColour(255, 0, 0);  //changes lcd backlight color
      delay(1500);  // sets a delay of 1.5 seconds
      lcdClear();  //clears lcd screen
      break;  //breaks the execution sequence for case 1

    case 2:
      orangeCounter++;
      bottomServo.write(servoAngles[1]);
      lcdPrint("     ORANGE");
      lcdMoveCursor(0, 1);
      lcdPrint("skittles # ");
      lcdPrint(orangeCounter);
      lcdBacklightColour(255, 100, 0);
      delay(1500);
      lcdClear();

      break;

    case 3:
      greenCounter++;
      bottomServo.write(servoAngles[2]);
      lcdPrint("     GREEN");
      lcdMoveCursor(0, 1);
      lcdPrint("skittles # ");
      lcdPrint(greenCounter);
      lcdBacklightColour(0, 255, 0);
      delay(1500);
      lcdClear();

      break;

    case 4:
      yellowCounter++;
      bottomServo.write(servoAngles[3]);
      lcdPrint("     YELLOW");
      lcdMoveCursor(0, 1);
      lcdPrint("skittles # ");
      lcdPrint(yellowCounter);
      lcdBacklightColour(255, 255, 0);
      delay(1500);
      lcdClear();

      break;

    case 5:
      purpleCounter++;
      bottomServo.write(servoAngles[4]);
      lcdPrint("     PURPLE");
      lcdMoveCursor(0, 1);
      lcdPrint("skittles # ");
      lcdPrint(purpleCounter);
      lcdBacklightColour(128, 0, 128);
      delay(1500);
      lcdClear();

      break;

    case 6:
      /*
         if the compartment where the skittles is supposed
         to be is empty for 2 runs, it will execute the a report
         for the number of skittles for each color
      */
      if (emptyHole == 2) {
        colorRunDown();
        delay(10000);
        lcdClear();
      }
      emptyHole++;
      bottomServo.write(servoAngles[2]);
      lcdMoveCursor(2, 0);
      lcdPrint("compartment");
      lcdMoveCursor(4, 1);
      lcdPrint("is empty");
      lcdBacklightColour(255, 255, 255);
      delay(1500);
      lcdClear();

    case 0:
      break;
  }
  delay(300);

  for (int i = 73; i > 20; i--) {
    topServo.write(i);
    delay(2);
  }
  delay(200);

  for (int i = 20; i < 125; i++) {
    topServo.write(i);
    delay(2);
  }
  color = 0;
}

/*
   The following function detects the color based off the
   frequency reading. The digitalWrite value depends on
   what color hue is being read from the data sheet.
*/
int readColor() {
  // Setting red filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Reading the output equency
  frequency = pulseIn(sensorOut, LOW);
  int R = frequency;
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  delay(50);

  // Setting Green filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  int G = frequency;
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  delay(50);

  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  int B = frequency;
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.println("  ");
  delay(50);

  /*
     These conditions are what determines the color under the sensor.
     There is a range for each condition because of the uncertainty
     and accuracy of the sensor.
  */
  if (R <= 25 & R >= 22 & G <= 39 & G >= 33 & B <= 28 & B >= 23) {
    color = 1; // Detects red
  }
  if (R <= 20 & R >= 16 & G <= 34 & G >= 28 & B <= 27 & B >= 21) {
    color = 2; // Detects Orange
  }
  if (R <= 30 & R >= 22 && G <= 30 & G >= 21 && B <= 26 & B >= 22) {
    color = 3; // Detects Green
  }
  if (R <= 20 & R >= 15 & G <= 25 & G >= 20 & B <= 24 & B >= 15) {
    color = 4; // Detects Yellow
  }
  if (R <= 32 & R >= 26 & G <= 43 & G >= 35 & B <= 31 & B >= 26) {
    color = 5; // Detects purple
  }
  if (R <= 38 & R >= 34 & G <= 43 & G >= 37 & B <= 31 & B >= 26)
    color = 6; // Detects Nothing

  return color; //return the color number to the servoMovement() function
}

// this function prints out to the screen when device is idle
void warningScreen() {

  lcdMoveCursor(4, 0);
  lcdPrint("WARNING:");
  lcdMoveCursor(0, 1);
  lcdPrint("STANDSTILL MODE!");

  /*
     the for loop is used to add a red fading affect to the lcd screen
  */
  for (int i = 255; i > 0; i--) //first for loop runs from 255 to 0
  {
    lcdBacklightColour(i, 0, 0);
  }
  for (int i = 0; i < 255; i++) //second for loop runs from 0 to 255
  {
    lcdBacklightColour(i, 0, 0);
  }

}
// function for printing out the color report at the end of each run
void colorRunDown() {
  lcdPrint("R=");
  lcdPrint(redCounter);
  lcdMoveCursor(6, 0);
  lcdPrint("G=");
  lcdPrint(greenCounter);
  lcdMoveCursor(13, 0);
  lcdPrint("Y=");
  lcdPrint(yellowCounter);
  lcdMoveCursor(4, 1);
  lcdPrint("P=");
  lcdPrint(purpleCounter);
  lcdMoveCursor(10, 1);
  lcdPrint("O=");
  lcdPrint(orangeCounter);
}
// This function is used to reset the counter values to zero after run is complete
void valuesReset(int& redCounter, int& yellowCoarunter, int& orangeCounter
                 , int& greenCounter, int& purpleCounter, int& emptyHole) {

  redCounter = 0;
  yellowCounter = 0;
  orangeCounter = 0;
  greenCounter = 0;
  purpleCounter = 0;
  emptyHole = 0;

}
