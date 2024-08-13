#include <Servo.h> //This library allows an Arduino board to control servo motors
  
  //Variables to action the servo motor for the actuationTime
   unsigned char servoState = LOW; //Stores the physical state of the servo
   unsigned char State = LOW; //Stores wether a cycle was initiated by user input.
   unsigned char lastState = LOW; //Stores the previous state of the user input
   unsigned long servoCameOn = 0; //Stores the time that the servo came on
   
   unsigned long actuationTime = 1; //STORES THE ACTUATION TIME OF THE CAP (in seconds)
   
   int push; //Stores the number of cycles
   boolean EXIT = false; //Enables to exit ther loop of the actuation of the servo

  int timer = (actuationTime*10); //Amount of measurements per cycle 

  //Variable for sensors
  unsigned long previousMillis = 0;
  unsigned long Time;
  int interval1 = 100; //Constant time between measurements in cycles.
  
  //Array for to store the measurements of the volume
  float VolumeSensor[50]; 
  
  //Variables for the pressure measurements
  float AveragePress[50]; 
  float averageA1 = 0; //Stores the 1 pressure sensor

  //varibles for differential pressure mreasurements
  float AverageDiff[50];
  float averageA2A3 = 0; //Stores the average of the differences of the two pressure sensors

  int END = 1; //Used to end the arduino process once enough cycles have been done

int pos = 0; // variable to store the servo position

Servo myservo; //Creates a servo object called "myservo" 


//--------------------------------------------//--------------------------------------------

void setup() {
  //The setup program runs once:
  Serial.begin(9600); //readies the communication of the Serial Monitor at a data rate of 9600 bits per second
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

}

//--------------------------------------------//--------------------------------------------

void getTime(){
    
  //Get the number of seconds 
  delay(100); Serial.println("Press NEXT for cycle or END to store measurements:"); //Prompt User for Input
  
  while (Serial.available() == 0) { //Loops until the user enters a value to the Serial Monitor 
    
  }
  
  END = Serial.parseInt(); //END stores the value from the user input

  if(END == 9){ 
    //Serial.println("RECEIVED 9");
    FinalResults(); //calls the method to end the application
  }
  
  push = push + 1;
  State = HIGH;
  EXIT = false;
  
  Serial.println("--------------------------------------------------------");   delay(100);
  Serial.print("Cycle "); Serial.println(push);  delay(100);
  Serial.print("the cap will be pressed for "); Serial.print(actuationTime); Serial.println(" seconds");  delay(100);
  Serial.println("--------------------------------------------------------");
  delay(100);
  
  actuationTime = actuationTime * 1000; //This calculation is needed because the program counts in milliseconds
  }

//--------------------------------------------//--------------------------------------------//--------------------------------------------

void serialFlush(){ //Method that clears the Serial.monitor
  while(Serial.available() > 0) {
    char t = Serial.read();
  }

}

//--------------------------------------------//--------------------------------------------//--------------------------------------------

void ServoON(){
  do{
    // If the Servo has been on for longer than the actuation time seconds then turn it off.
    if(servoState == HIGH) //Checks if the servo is on
    {
      if(millis()-servoCameOn > actuationTime){ //Checks if the current time minus the time that it came on is greater than the actuationTime
      
        myservo.write(0); //Resets the servo position
        servoState = LOW; 
        EXIT = true;

        serialFlush(); //calls this method in order to clear the Serial.monitor (avoid errors of automatically inputing without user input)  
        lastState = LOW;
        State = LOW;
      }
    }
    
    if(State != lastState)
    {
      lastState = State;
      if((State == HIGH) && (servoState == LOW)) 
      {
        
        myservo.write(-180); //Servo is activated
        servoState = HIGH;
        servoCameOn = millis();

        //Method reads the sensors while the cap is pressed
        readSensors();
        myservo.write(0); //Resets the Servo to its original position

  
      }
    }
  }while(EXIT != true); //Loop while Exit = false 
}

//--------------------------------------------//--------------------------------------------//--------------------------------------------

void readSensors(){

  //pressure sensor that is by itself
  float PressAlone;
  long millisNow;
  //two pressure sensors
  float PressSensor1;
  float PressSensor2;
  float diffInPres;

  millisNow = millis(); //the time before the measurements 

do{
      //if half 0.1 (value of interval1) seconds has passed, we get the readings of A1, A2, A3
      if (millis() - previousMillis >= interval1) {
        
        previousMillis = millis(); //stores the time when the measurements start 
        
        //Stores the values of the pressure sensor that is alone A1
        PressAlone = analogRead(A1); 
        //Serial.println(PressAlone);
        averageA1 = averageA1 + PressAlone; //adds all the values together for the average
  
        
        //Stores the values of the two pressure sensors that will be differenced then added to a variable for the average.
        PressSensor1 = (analogRead(A2));
        PressSensor2 = (analogRead(A3));
        diffInPres = (PressSensor2 - PressSensor1);
        averageA2A3 = averageA2A3 + diffInPres; //adds all the values together for the average
      }

 }while(millis()-servoCameOn < actuationTime-10); //Loop while the servo has been on for less than or equal to the set time 
                                                  //actuationTime-10 to avoid an extra measurements
  //Stores the reading of the water sensor at the end of each cycle.
  VolumeSensor[push-1] = analogRead(A0); 
}

//--------------------------------------------//--------------------------------------------//--------------------------------------------

void CycleSummary(){
  
  
  //gets the average for the A1 sensor
  AveragePress[push-1] = (averageA1 / (timer));

  //gets the average for the difference in pressure A2 and A3
  AverageDiff[push-1] = (averageA2A3 / (timer));
  
  delay(200); Serial.println(); Serial.print("-------------  Cycle "); Serial.print(push);  Serial.println(" Completed Successfully --------------"); Serial.println(); delay(100);

  //puts back the timer to its original size after a cycle
  actuationTime = actuationTime/1000;
  averageA1 = 0;
  averageA2A3 = 0;

}

//--------------------------------------------//--------------------------------------------//--------------------------------------------

void FinalResults(){

  String Vol = "";
  String Pres = "";
  String Diff = "";
  
  if(AveragePress[0] == 0){ //If there have not been any cycles, then loop again
    Serial.println("no cycles have been made");
    loop();
  }  
  
  for(int c = 0; c < push; c++){    //Loop to create the Strings
      Vol = Vol + VolumeSensor[c] + " ";
      Pres = Pres + AveragePress[c] + " ";
      Diff = Diff + AverageDiff[c] + " ";
  }
  
  //Print the strings to the Serial monitor  
  Serial.print("Pres");
  for(int c = 0; c < push; c++){    //Loop to create the Strings
    Serial.print(AveragePress[c]); Serial.print(" ");

  }
  
  Serial.print("Dif");
  for(int c = 0; c < push; c++){    //Loop to create the Strings
    Serial.print(AverageDiff[c]); Serial.print(" ");
  }

  Serial.print("|Vol");
  for(int c = 0; c < push; c++){    //Loop to create the Strings
   Serial.print(VolumeSensor[c]); Serial.print(" ");

  }
  Serial.println("?");
  
  
  /*Serial.print(push);
  Serial.println("Pres" + Pres); 
  Serial.println("Dif" + Diff + "|");
  Serial.println("Vol" + Vol + "?");*/
    
  delay(100);
  exit(0);
}

//--------------------------------------------//--------------------------------------------//--------------------------------------------

void loop() {
  
  getTime();
  ServoON();
  CycleSummary();
  
}
