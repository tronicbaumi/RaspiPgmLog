/**
 * Programm to run on Arduino. Communicates with a counterpart software on a raspberry pi.
 * Samples analog input at a given speed and sends it to the raspi
 */
#include <stdlib.h>
#include <string.h>

// intitialise some variables
unsigned long mstime;
unsigned long counter;
unsigned int interval;
char str[100]; // a char array used as string
char c;
char* index; // a char pointer to store the output of the strstr function
int DO[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // a int array to save the state of the digital pins of the Arduino

bool sampling;

//Code Initialization Starts
void setup() {
  //Setting the Baud to 230400 Bits/Second
  Serial.begin(230400);

  // intitialise 5 pwm pins of the Arduino Uno
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  
  // initialise all variables with standard values
  mstime = millis();
  counter = 1;
  sprintf(str, "");
  interval = 100;
  index = 0;

  sampling = false;
}

void loop() {
  // reset the string
  sprintf(str, "");
  // read incomming data
  while(Serial.available()){
    c = Serial.read();
    sprintf(str, "%s%c", str, c);
    index = strstr (str,"\n");
    // process input if the whole line is read
    if(index != NULL){
      Serial.println(str);
      if(strcmp(str, "start\n") == 0){// start logging, reset counter
        sampling = true;
        counter = 0;
      }else if(strcmp(str, "stop\n") == 0){ // stop logging
        sampling = false;
        Serial.println("stopped");
      }else if(strstr (str,"v")){ // set sample speed
        char* pch;
        unsigned int temp;
        // parse the speed information string
        pch = strtok (str,"|");
        while (pch != NULL)
        {
          pch = strtok (NULL, "|");
          if(pch != NULL){
            temp = atoi(pch);  
          }
        }
        // check if input is valid, then proceede
        if(temp > 0){
          interval = temp;
          counter = 0;
        }else{
          Serial.println("speederror");
        }
      }else if(strstr (str,"g")){ // set digital pins of the Arduino
        int i = 0;
        char* pch;
        int temp[2];
        // parse the pin information string
        pch = strtok (str,"|");
        while (pch != NULL)
        {
          pch = strtok (NULL, "|");
          if(pch != NULL){
            temp[i] = atoi(pch);
          }
          i++;
        }
        // check if input is valid, then proceede
        if(i == 3){
          DO[temp[0]] = temp[1]; 
        }else{
          Serial.println("pinerror");
        }
      }else{
        Serial.println("argerror");
      }
      // reset the string
      sprintf(str, "");
    }
  }

  // set the pins of the Arduino to the values, that are saved in the array
  for(int i = 0; i < 14; i++){
    if(DO[i] > 0){
      analogWrite(i, DO[i]);
    }else{
      digitalWrite(i, LOW);
    }
  }

  // send sample data if the interval is met
  if(((millis() - mstime) >= interval) && sampling){
    mstime = millis();
    sprintf(str, "%lu,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i", counter * interval, analogRead(A0), analogRead(A1), analogRead(A2), analogRead(A3), analogRead(A4), analogRead(A5), DO[5], DO[6], DO[9], DO[10], DO[11]);

    Serial.println(str);
    counter++;
  }
}

//End of the Program
