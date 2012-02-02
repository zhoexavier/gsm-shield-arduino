#include "SIM900.h"
#include <NewSoftSerial.h>
#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to start a connection as server.

InetGSM inet;
//CallGSM call;
//SMSGSM sms;

char msg[100];
int numdata;
char inSerial[30];
int i=0;
long lasttime;

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(4800))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
  
  //GPRS attach, put in order APN, username and password.
  //If no needed auth let them blank.
  if (gsm.attachGPRS("internet.wind", "", ""))
    Serial.println("status=ATTACHED");
  else Serial.println("status=ERROR");
  delay(5000);
  
  //Read IP address.
  int i=0;
  while(i<20){
    gsm.SimpleRead();
    i++;
  }
  delay(5000);
  gsm.write((const uint8_t*)"AT+CIFSR\r", 10);
  gsm.read(msg, 200);
  Serial.println(msg);
  delay(5000); 
  
  //Tweet
  //inet.tweet("*********************key************", "An Arduino at #cpes15");

  //TCP Server. Start the socket connection
  //as server on the assigned port.
  Serial.println(msg);
  delay(5000);
  if (gsm.connectTCPServer(80))
    Serial.println("status=TCPSERVERWAIT");
  else Serial.println("ERROR in Server");
  lasttime=millis();
};


void loop(){
  //serialhwread();
  //serialswread();
  //Check if there is an active connection.
  if (gsm.connectedClient()){
    //Read and print the last message received.
    gsm.read(msg, 200);
    Serial.println(msg);
  }
};

/*
void loop() 
{
  serialhwread();
  serialswread();
};
*/

void serialhwread(){
  i=0;
  if (Serial.available() > 0){            
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    
    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWrite(inSerial);
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWrite("AT+CSQ");
    }
    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }
    
    else{
      Serial.println(inSerial);
      gsm.SimpleWrite(inSerial);
    }    
    
    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}
