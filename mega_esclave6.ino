//I2C SLAVE CODE
//I2C Communication between Two Arduino
//CircuitDigest
//Pramoth.T
#include <Wire.h>  //Library for I2C Communication functions
#include "CytronMotorDriver.h"
#include "Servo.h"

CytronMD motor1(PWM_PWM, 3, 9);    // PWM 1A = Pin 3, PWM 1B = Pin 9.
CytronMD motor2(PWM_PWM, 10, 11);  // PWM 2A = Pin 10, PWM 2B = Pin 11.

Servo servo;  // création de l'objet "servo"

#define IR_LEFT A9
#define IR_MIDDLE A11
#define IR_RIGHT A13
#define PIN_SERVO 8


const int trig1 = 48;
const int echo1 = 49;
float lecture_echo1 = 0;
float distance1;

const int trig2 = 52;
const int echo2 = 53;
float lecture_echo2 = 0;
float distance2;

const int trig3 = 37;
const int echo3 = 36;
float lecture_echo3 = 0;
float distance3;

int nb_intersections = 0;
int a = 0;

int chambre_a_livrer;
int SlaveReceived[] = { -1, 0, 0, 0, 0, 0, false };
int intersection;
int i = 0;
int ancienne_chambre = 0;

bool chambre_trouvee = false;
bool validation = false;
bool ok = false;

void robotStop() {
  motor1.setSpeed(0);  // Motor 1 stops.
  motor2.setSpeed(0);  // Motor 2 stops.
}

void robotForward() {
  motor1.setSpeed(-130);  // Motor 1 runs forward.
  motor2.setSpeed(-130);   // Motor 2 runs forward.
}

void robotReverse() {
  motor1.setSpeed(50);   // Motor 1 runs backward.
  motor2.setSpeed(50);  // Motor 2 runs backward.
}

void robotTurnLeft() {
  motor1.setSpeed(140);  // Motor 1 runs forward.
  motor2.setSpeed(-140);  // Motor 2 runs backward.
}

void robotTurnRight() {
  motor1.setSpeed(-140);  // Motor 1 runs backward.
  motor2.setSpeed(140);  // Motor 2 runs forward.
}

void robotDemiTour() {
  motor1.setSpeed(-145);  // Motor 1 runs backward.
  motor2.setSpeed(175);  // Motor 2 runs forward.
}

void suivi_ligne() {
  digitalWrite(trig1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig1, LOW);
    lecture_echo1 = pulseIn(echo1, HIGH);
    distance1 = lecture_echo1 * 0.017;
  //distance1 = 20;
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);
  lecture_echo2 = pulseIn(echo2, HIGH);
  distance2 = lecture_echo2 * 0.017;

  digitalWrite(trig3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig3, LOW);
  lecture_echo3 = pulseIn(echo3, HIGH);
  distance3 = lecture_echo3 * 0.017;


  if ((distance1 < 10) || (distance2 < 10) || (distance3 < 10)) {
    Serial.println("capteur us passe");
    robotStop();
  } else {
    if (isOnWhite(IR_LEFT) && isOnBlack(IR_MIDDLE) && isOnWhite(IR_RIGHT)) {
      robotForward();
    }
    if (isOnBlack(IR_LEFT) && isOnWhite(IR_MIDDLE)) {
      robotTurnLeft();
    }
    if (isOnBlack(IR_RIGHT) && isOnWhite(IR_MIDDLE)) {
      robotTurnRight();
    }
    if (isOnBlack(IR_LEFT) && isOnBlack(IR_MIDDLE) && isOnBlack(IR_RIGHT)) {
      robotForward();
      nb_intersections = nb_intersections + 1;
      delay(250);
    }
    if (isOnWhite(IR_LEFT) && isOnWhite(IR_MIDDLE) && isOnWhite(IR_RIGHT)) {
      robotForward();
    }
    if (isOnBlack(IR_LEFT) && isOnBlack(IR_MIDDLE) && isOnWhite(IR_RIGHT)) {
      robotTurnLeft();
    }
    if (isOnBlack(IR_RIGHT) && isOnBlack(IR_MIDDLE) && isOnWhite(IR_LEFT)) {
      robotTurnRight();
    }
  }
}


bool isOnBlack(int sensor) {

  if (digitalRead(sensor) == HIGH) {
    return true;
  } else return false;
}


bool isOnWhite(int sensor) {
  if (digitalRead(sensor) == LOW) {
    return true;
  } else return false;
}


void service(int n) {
  Serial.println(n);
  robotStop();
  Serial.println("service");
  ancienne_chambre = SlaveReceived[n];
  SlaveReceived[n] = 0;
  //ouverture porte pendant que non validation
  while (validation == false) {
    servo.write(180);  // demande au servo de se déplacer à cette position
    delay(1000);       // attend 1000 ms entre changement de position

    validation = SlaveReceived[6];
    Serial.println("boucle");
    Serial.println(SlaveReceived[0]);
    Serial.println(SlaveReceived[1]);
    Serial.println(SlaveReceived[6]);
  }
  //quand validation: fermeture porte
  servo.write(90);
  delay(1000);
  //demi-tour
  robotDemiTour();
  delay(1200);
  while (isOnWhite(IR_LEFT)) {
    robotDemiTour();
  }
  nb_intersections = 0;
  //finir trajet
}


void setup() {
  Serial.begin(9600);  //Begins Serial Communication at 9600 baud rate

  servo.write(90);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_MIDDLE, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  servo.attach(PIN_SERVO);  // attache le servo au pin spécifié

  Wire.begin(8);                 //Begins I2C communication with Slave Address as 8 at pin (A4,A5)
  Wire.onReceive(receiveEvent);  //Function call when Slave receives value from master
  Wire.onRequest(requestEvent);  //Function call when Master request value from Slave
}


void loop(void) {

  Serial.println("Slave Received From Master:");  //Prints in Serial Monitor
  Serial.println(SlaveReceived[0]);
  chambre_a_livrer = SlaveReceived[0];

  while (SlaveReceived[i] != 0) {
    if (validation == false) {
      intersection = SlaveReceived[i];
      //Serial.println(intersection);
      //Serial.println(nb_intersections);
      Serial.print(digitalRead(IR_LEFT));
      Serial.print("  ");
      Serial.print(digitalRead(IR_MIDDLE));
      Serial.print("  ");
      Serial.println(digitalRead(IR_RIGHT));

      delay(500);

      while ((SlaveReceived[i] != -1) && (nb_intersections != (SlaveReceived[i] - ancienne_chambre))) {
        Serial.print("robot commence ");
        Serial.print(SlaveReceived[i]);
        intersection = SlaveReceived[i];
        Serial.println(intersection);

        Serial.println(SlaveReceived[0]);
        Serial.println(SlaveReceived[1]);
        Serial.println(SlaveReceived[2]);
        Serial.println(SlaveReceived[6]);

        suivi_ligne();
      }

      robotStop();
      delay(1000);

      if ((nb_intersections == SlaveReceived[i] - ancienne_chambre) && chambre_trouvee == false) {
        chambre_trouvee = true;
        robotTurnRight();
        delay(600);
        while (isOnWhite(IR_LEFT)) {
        }
        robotStop();
        nb_intersections = 0;
      }

      while ((chambre_trouvee == true) && (nb_intersections != 1)) {
        suivi_ligne();
      }

      while ((chambre_trouvee == true) && (nb_intersections == 1)) {
        service(i);
        nb_intersections = 0;
      }
    } else if (validation == true) {
      Serial.println("commande recuperee");
      robotStop();

      while ((ok == false) && (nb_intersections != 1)) {
        suivi_ligne();
      }

      if ((ok == false) && nb_intersections == 1) {
        robotStop();
        delay(1000);
        if (SlaveReceived[i + 1] == 0) {
          if (intersection <= 2) {
            robotTurnLeft();
            delay(600);
            while (isOnWhite(IR_MIDDLE)) {
              robotTurnLeft();
            }
            robotStop();
            delay(100);
            ok = true;
            nb_intersections = 0;

            while ((ok == true) && (nb_intersections != intersection)) {

              suivi_ligne();
            }

            robotStop();
            delay(1000);
            robotDemiTour();
            delay(1000);
            while (isOnWhite(IR_MIDDLE)) {
              robotDemiTour();
            }
            robotStop();
            delay(100);
          } else {
            robotTurnRight();
            delay(600);
            while (isOnWhite(IR_LEFT)) {
              robotTurnRight();
            }
            robotStop();
            delay(100);
            ok = true;
            nb_intersections = 0;

            while ((ok == true) && (nb_intersections != 6 - intersection)) {

              suivi_ligne();
            }

            robotStop();
          }
          validation = false;
          nb_intersections = 0;
          chambre_trouvee = false;
          ok = false;
          SlaveReceived[0] = -1;
          ancienne_chambre = 0;
          i = 0;

        } else {
          robotTurnRight();
          delay(600);
          while (isOnWhite(IR_LEFT)) {
            robotTurnRight();
          }
          robotStop();
          delay(200);
          i += 1;
          nb_intersections = 0;
          SlaveReceived[6] = false;
          chambre_trouvee = false;
          ok = false;
          validation = false;
        }
      }
    }
  }
}
void receiveEvent(int howMany)  //This Function is called when Slave receives value from master
{
  Wire.readBytes((uint8_t*)&SlaveReceived, 14);
}
void requestEvent()  //This Function is called when Master wants value from slave
{
  int potvalue = 5;      // Reads analog value from POT (0-5V)
                         // Convert potvalue digital value (0 to 1023) to (0 to 127)
  Wire.write(potvalue);  // sends one byte converted POT value to master
}