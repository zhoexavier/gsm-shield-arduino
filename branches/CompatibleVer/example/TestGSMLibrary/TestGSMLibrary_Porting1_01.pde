#include "QuectelM10.h"
#include <NewSoftSerial.h>
#include "inetGSM.h"


InetGSM inet;
char msg[150];
int numdata;
char inSerial[40];
int i=0;


void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration.
  if (gsm.begin(4800))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
  
};

void loop() 
{
  serialhwread();
  serialswread();
};

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
    
    if(!strcmp(inSerial,"TEST")){
      Serial.println("TESTING");
    }
    
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
