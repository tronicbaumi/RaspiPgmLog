/**
 * Programm to run on Arduino. Communicates with a counterpart software on a raspberry pi.
 * Samples analog input at a given speed and sends it to the raspi
 */
#include <stdlib.h>

unsigned long mstime;
unsigned long counter;
unsigned int interval;
String str;
char c;
int index;

bool sampling;

//Code Initialization Starts
void setup() {
  //Setting the Baud to 230400 Bits/Second
  Serial.begin(230400);
  // initialise all variables with standard values
  mstime = millis();
  counter = 1;
  str = "";
  c = "";
  interval = 100;
  index = 0;

  sampling = false;
}

void loop() {
  str = "";
  // read incomming data
  while(Serial.available()){
    c = Serial.read();
    str = str + c;
    index = str.indexOf("\n");
    // process input if the whole line is read
    if(index > -1){
      str.remove(index);
      Serial.println(str);
      if(str == "start"){// start logging, reset counter
        sampling = true;
        counter = 0;
      }else if(str == "stop"){ // stop logging
        sampling = false;
        Serial.println("stopped");
      }else if(str.indexOf("v") > -1){ // set sample speed
        // check if input is valid, then proceede
        unsigned int temp = str.substring(2).toInt();
        if(temp > 0){
          interval = temp;
          counter = 0;
        }else{
          Serial.println("speederror");
        }
      }else{
        Serial.println("argerror");
      }
      str = "";
    }
  }

  // send sample data if the interval is met
  if(((millis() - mstime) >= interval) && sampling){
    mstime = millis();
    str = String(counter * interval) + "," + String(analogRead(A0)) + "," + String((analogRead(A1) + 100)) + "," + String((analogRead(A2) + 300));
    Serial.println(str);
    counter++;
  }
}

//End of the Program
