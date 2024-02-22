#include <Arduino.h>

int trigPin1 = 13; //D7
int echoPin1 = 15; //D8
int trigPin2 = 12; //D6
int echoPin2 = 14; //D5
long duration1, duration2;
int distance1, distance2;
char distance1Arr[4];
char distance2Arr[4];
const char clearSerial = 'c';
char serialArr[28]; //s#1<1234>#2<1234>#3<magnet>#4<track> + '\r' + '\n'

//timer related 
bool isTimerSet = false;
bool cancelTimer = false;
unsigned long previousTime =0;
unsigned long eventInterval = 500;
unsigned long startMillis;  //some global variables available anywhere in the program

//int magneticPin = D3;//0 - on, 1 - 0ff //for esp32
int magneticPin = 3;//0 - on, 1 - 0ff //for arduino
int magneticState = 0; 
//int trackingPin = D0;//0 - on, 1 - 0ff //for esp32
int trackingPin = 0;//0 - on, 1 - 0ff //for arduino
int trackingState = 0;

void setup() {
  Serial.begin(2000000); //115200
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(magneticPin, INPUT);
  pinMode(trackingPin, INPUT);
  startMillis = millis();
}

void loop() {
    int magnet = digitalRead(magneticPin);

    if(magnet == LOW){
      //on
      magneticState = 1;
    }else{
      //off
      magneticState = 0;
    }

    int track = digitalRead(trackingPin);
    if(track == LOW){
      trackingState = 1; //on
    }else{
      trackingState = 0; //off
    }

    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = duration1 * 0.034 / 2; 
    if(distance1 > 10000){
      distance1 = 9999;
    }

    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = duration2 * 0.034 / 2; 
    if(distance2 > 10000){
      distance2 = 9999;
    }
    //Serial.print("data");
  

  
  /*unsigned long currentTime = millis();

  if(currentTime - previousTime >= eventInterval){
    CreateSerialArray();
    Serial.write(serialArr);
    previousTime = currentTime;
  }*/
}

void sendData(){
  if(isTimerSet){
    return;
  }else{
    unsigned long currentTime = millis();

    if(currentTime - startMillis >= eventInterval){
      CreateSerialArray();
      Serial.write(serialArr);
      startMillis = currentTime;
    }
  }
  
}

void reciveData(){
  while(Serial.available() > 0){
    
    //Serial.print("Avaliable");

    char inByte = Serial.read();
    if(inByte == 'c'){
      CreateSerialArray();
      Serial.write(serialArr);
      inByte = 'r';
    }

  }
}

void setTimer(){

}

void CreateSerialArray(){
  serialArr[0] = 'd';
  serialArr[1] = '#';
  serialArr[2] = '1';
  serialArr[3] = '<';
  AddDistance(distance1, 4);
  serialArr[8] = '>';
  serialArr[9] = '#';
  serialArr[10] = '2';
  serialArr[11] = '<';
  AddDistance(distance2, 12);
  serialArr[16] = '>';
  //serialArr[17] = '\n';
  serialArr[17] = '#';
  serialArr[18] = '3';
  serialArr[19] = '<';
  serialArr[20] = digitalRead(magneticPin) + '0'; //magneticState + '0'
  serialArr[21] = '>';
  serialArr[22] = '#';
  serialArr[23] = '4';
  serialArr[24] = '<';
  serialArr[25] = digitalRead(trackingPin) + '0'; //trackingState + '0'
  serialArr[26] = '>';
  serialArr[27] = '\0';
}

void AddDistance(int distance, int from){
  for (int i = 3; i >= 0; i--) {
		if (distance / 10 == 0 && distance % 10 == 0)
		{
			serialArr[i+from] = '-';
		}
		else {
			serialArr[i+from] = distance % 10 + '0';
		}
		distance /= 10;	
	}
}
