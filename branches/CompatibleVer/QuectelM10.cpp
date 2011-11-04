#include "QuectelM10.h"  
#include "Streaming.h"

#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

#define RESETPIN 7

QuectelM10 gsm;

QuectelM10::QuectelM10(){};

QuectelM10::~QuectelM10(){};
  
int QuectelM10::restart(char* pin)
{
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH);
  delay(10000);
  digitalWrite(RESETPIN, LOW);
  delay(1000);

  return configandwait(pin);
}

int QuectelM10::start(char* pin)
{

  _tf.setTimeout(_TCP_CONNECTION_TOUT_);

  //_cell.flush();
// Just for old style software restart();  
//  _cell << "AT+CFUN=1" <<  _BYTE(cr) << endl; //Comprobar
//   if (!_tf.find("OK")) 
//   {
//     setStatus(IDLE);
//     return 0;
//   }

  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH);
  delay(10000);
  digitalWrite(RESETPIN, LOW);
  delay(1000);

  return configandwait(pin);
}

int QuectelM10::configandwait(char* pin)
{
  int connCode;
  _tf.setTimeout(_GSM_CONNECTION_TOUT_);

  if(pin) setPIN(pin); //syv

  // Try 10 times to register in the network. Note this can take some time!
  for(int i=0; i<10; i++)
  {  	
    //Ask for register status to GPRS network.
    _cell << "AT+CGREG?" <<  _BYTE(cr) << endl; 

    //Se espera la unsolicited response de registered to network.
    while (_tf.find("+CGREG: 0,"))  // CHANGE!!!!
	{
		connCode=_tf.getValue();
		if((connCode==1)||(connCode==5))
		{
		  setStatus(READY);
		  
		_cell << "AT+CMGF=1" <<  _BYTE(cr) << endl; //SMS text mode.
		delay(200);
		  // Buah, we should take this to readCall()
		_cell << "AT+CLIP=1" <<  _BYTE(cr) << endl; //SMS text mode.
		delay(200);
		//_cell << "AT+QIDEACT" <<  _BYTE(cr) << endl; //To make sure not pending connection.
		//delay(1000);
	  
		  return 1;
		}
	}
  }
  return 0;
}

int QuectelM10::shutdown()
{
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, HIGH);
  delay(800);
  digitalWrite(RESETPIN, LOW);
  delay(1000);

  _tf.setTimeout(_TCP_CONNECTION_TOUT_);
  //_cell.flush();
  _cell << "AT+CFUN=4" <<  _BYTE(cr) << endl; //Comprobar
   if (_tf.find("OK")) 
   {
     setStatus(IDLE);
     return 1;
   }
   // After shutdown the modem may accept commands giving no answer.
   // We'll play safe
   delay(1000);
   return 0;
}     
  
int QuectelM10::sendSMS(const char* to, const char* msg)
{

  //Status = READY or ATTACHED.
  /*
  if((getStatus() != READY)&&(getStatus() != ATTACHED))
    return 0;
  */    

  _tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to send a SMS. Destination telephone number 
  _cell << "AT+CMGS=\"" << to << "\"" <<  _BYTE(cr) << endl; // Establecemos el destinatario

  //Expect for ">" character.
  
  switch(WaitResp(5000, 50, ">")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  //SMS text.
  _cell << msg << _BYTE(ctrlz) << _BYTE(cr) << "\"\r"; 
   if (!_tf.find("OK")) 
	return 0;
  return 1;
}

int QuectelM10::attachGPRS(char* domain, char* dom1, char* dom2)
{

   delay(5000);
   
  _tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.
 
//  _cell.flush();

  _cell << "AT+CSTT=\"";
  _cell << domain;
  _cell << "\",\"";
  _cell << dom1;
  _cell << "\"\r";
  
  switch(WaitResp(5000, 50, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  _cell << "AT+CIICR";
  _cell << "\r";
  switch(WaitResp(5000, 50, "OK")){
	case RX_TMOUT_ERR: 
		return 0; 
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

/*

  //Expect "OK". 
  if(_tf.find("OK"))
  {
    setStatus(ATTACHED); 
    delay(1000);
    return 1;
  }
  else
  {
    //setStatus(ATTACHED); 
    //delay(1000);
    //return 1;

    // In this case we dont know the modem mental position
    setStatus(ERROR);
    return 0;   
  }*/
 _cell << "AT+CIFSR\r";
 setStatus(ATTACHED);
 delay(3000);
 return 1;
}

int QuectelM10::dettachGPRS()
{
  if (getStatus()==IDLE) return 0;
   
  _tf.setTimeout(_GSM_CONNECTION_TOUT_);

  //_cell.flush();

  //GPRS dettachment.
  _cell << "AT+CGATT=0" <<  _BYTE(cr) << endl;
  
  if(!_tf.find("OK")) 
  {
    setStatus(ERROR);
    return 0;
  }
  delay(500);
  
  // Commented in initial trial code!!
  //Stop IP stack.
  //_cell << "AT+WIPCFG=0" <<  _BYTE(cr) << endl;
  //	if(!_tf.find("OK")) return 0;
  //Close GPRS bearer.
  //_cell << "AT+WIPBR=0,6" <<  _BYTE(cr) << endl;

  setStatus(READY);
  return 1;
}

int QuectelM10::connectTCP(const char* server, int port)
{
  _tf.setTimeout(_TCP_CONNECTION_TOUT_);

  //Status = ATTACHED.
  //if (getStatus()!=ATTACHED)
    //return 0;

  //_cell.flush();
  
  //Visit the remote TCP server.
  _cell << "AT+CIPSTART=\"TCP\",\"" << server << "\"," << port <<  "\r";
  
  switch(WaitResp(1000, 200, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  switch(WaitResp(15000, 200, "CONNECT")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }


  _cell << "AT+CIPSEND\r";
  switch(WaitResp(15000, 200, ">")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  Serial.println("CONNESSO");
  return 1;
}

int QuectelM10::disconnectTCP()
{
  //Status = TCPCONNECTEDCLIENT or TCPCONNECTEDSERVER.
  /*
  if ((getStatus()!=TCPCONNECTEDCLIENT)&&(getStatus()!=TCPCONNECTEDSERVER))
     return 0;
  */
  _tf.setTimeout(_GSM_CONNECTION_TOUT_);


  //_cell.flush();

  //Switch to AT mode.
  //_cell << "+++" << endl;
  
  //delay(200);
  
  //Close TCP client and deact.
  _cell << "AT+CIPCLOSE\r";

  //If remote server close connection AT+QICLOSE generate ERROR
  /*if(_tf.find("OK"))
  {
    if(getStatus()==TCPCONNECTEDCLIENT)
      setStatus(ATTACHED);
    else
      setStatus(TCPSERVERWAIT);
    return 1;
  }
  setStatus(ERROR);
  
  return 0;    */
  if(getStatus()==TCPCONNECTEDCLIENT)
      	setStatus(ATTACHED);
   else
        setStatus(TCPSERVERWAIT);   
    return 1;
}

int QuectelM10::connectTCPServer(int port)
{
/*
  if (getStatus()!=ATTACHED)
     return 0;
*/
  _tf.setTimeout(_GSM_CONNECTION_TOUT_);

  //_cell.flush();

  // Set port
  _cell << "AT+CIPSERVER=1," << port << "\r";
/*
  switch(WaitResp(5000, 50, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  switch(WaitResp(5000, 50, "SERVER")){ //Try SERVER OK
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
*/
  //delay(200);  

  return 1;

}

boolean QuectelM10::connectedClient()
{
  if (getStatus()!=TCPSERVERWAIT)
     return 0;
   _cell << "AT+CIPSTATUS" << "\r";
  // Alternative: AT+QISTAT, although it may be necessary to call an AT 
  // command every second,which is not wise
  _tf.setTimeout(1);
  if(_tf.find("CONNECT")) 
  {
    setStatus(TCPCONNECTEDSERVER);
    return true;
  }
  else
    return false;
 }

int QuectelM10::write(const uint8_t* buffer, size_t sz)
{
/*
   if((getStatus() != TCPCONNECTEDSERVER)&&(getStatus() != TCPCONNECTEDCLIENT))
    return 0;
    
   if(sz>1460)
     return 0;
*/
  _tf.setTimeout(_GSM_DATA_TOUT_);

//  _cell.flush();
    
  for(int i=0;i<sz;i++)
    _cell << _BYTE(buffer[i]);
  
  //Not response for a write.
  /*if(_tf.find("OK"))
    return sz;
  else
    return 0;*/
    
  return sz;  
}


int QuectelM10::read(char* result, int resultlength)
{
  // Or maybe do it with AT+QIRD

  int charget;
  _tf.setTimeout(3);
  // Not well. This way we read whatever comes in one second. If a CLOSED 
  // comes, we have spent a lot of time
    charget=_tf.getString("","",result, resultlength);

  /*if(strtok(result, "CLOSED")) // whatever chain the Q10 returns...
  {
    // TODO: use strtok to delete from the chain everything from CLOSED
    if(getStatus()==TCPCONNECTEDCLIENT)
      setStatus(ATTACHED);
    else
      setStatus(TCPSERVERWAIT);
  }  */
  
  return charget;
}

 int QuectelM10::readCellData(int &mcc, int &mnc, long &lac, long &cellid)
{
  if (getStatus()==IDLE)
    return 0;
    
   _tf.setTimeout(_GSM_DATA_TOUT_);
   //_cell.flush();
  _cell << "AT+QENG=1,0" << endl; 
  _cell << "AT+QENG?" << endl; 
  if(!_tf.find("+QENG:"))
    return 0;

  mcc=_tf.getValue(); // The first one is 0
  mcc=_tf.getValue();
  mnc=_tf.getValue();
  lac=_tf.getValue();
  cellid=_tf.getValue();
  _tf.find("OK");
  _cell << "AT+QENG=1,0" << endl; 
  _tf.find("OK");
  return 1;
}


boolean QuectelM10::readSMS(char* msg, int msglength, char* number, int nlength)
{
  long index;
  /*
  if (getStatus()==IDLE)
    return false;
  */
  _tf.setTimeout(_GSM_DATA_TOUT_);
  //_cell.flush();
  _cell << "AT+CMGL=\"REC UNREAD\",1" << endl;
  if(_tf.find("+CMGL: "))
  {
    index=_tf.getValue();
    _tf.getString("\"+", "\"", number, nlength);
    _tf.getString("\n", "\nOK", msg, msglength);
    _cell << "AT+CMGD=" << index << endl;
    _tf.find("OK");
    return true;
  };
  return false;
};


boolean QuectelM10::readCall(char* number, int nlength)
{
  int index;

  if (getStatus()==IDLE)
    return false;
  
  _tf.setTimeout(_GSM_DATA_TOUT_);

  if(_tf.find("+CLIP: \""))
  {
    _tf.getString("", "\"", number, nlength);
    _cell << "ATH" << endl;
    delay(1000);
    //_cell.flush();
    return true;
  };
  return false;
};

boolean QuectelM10::call(char* number, unsigned int milliseconds)
{ 
  if (getStatus()==IDLE)
    return false;
  
  _tf.setTimeout(_GSM_DATA_TOUT_);

  _cell << "ATD" << number << ";" << endl;
  delay(milliseconds);
  _cell << "ATH" << endl;

  return true;
 
}

int QuectelM10::setPIN(char *pin)
{
  //Status = READY or ATTACHED.
  if((getStatus() != IDLE))
    return 2;
      
  _tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to set PIN.
  _cell << "AT+CPIN=" << pin <<  _BYTE(cr) << endl; // Establecemos el pin

  //Expect "OK".
  if(!_tf.find("OK"))
    return 0;
  else  
    return 1;
}

int QuectelM10::write(uint8_t c)
{
  if ((getStatus() == TCPCONNECTEDCLIENT) ||(getStatus() == TCPCONNECTEDSERVER) )
    return write(&c, 1);
  else
    return 0;
}

int QuectelM10::write(const char* str)
{
  if ((getStatus() == TCPCONNECTEDCLIENT) ||(getStatus() == TCPCONNECTEDSERVER) )
      return write((const uint8_t*)str, strlen(str));
  else
      return 0;
}

int QuectelM10::changeNSIPmode(char mode) ///SYVV
{
    _tf.setTimeout(_TCP_CONNECTION_TOUT_);
    
    //if (getStatus()!=ATTACHED)
    //    return 0;

    //_cell.flush();

    _cell << "AT+QIDNSIP=" << mode <<  _BYTE(cr) << endl;

    if(!_tf.find("OK")) return 0;
    
    return 1;
}

int QuectelM10::getCCI(char *cci)
{
  //Status must be READY
  if((getStatus() != READY))
    return 2;
      
  _tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to get CCID.
  _cell << "AT+QCCID" << _BYTE(cr) << endl; // Establecemos el pin
  
  //Read response from modem
  _tf.getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);
  
  //Expect "OK".
  if(!_tf.find("OK"))
    return 0;
  else  
    return 1;
}
  
int QuectelM10::getIMEI(char *imei)
{
      
  _tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

  //_cell.flush();

  //AT command to get IMEI.
  _cell << "AT+GSN" << _BYTE(cr) << endl; 
  
  //Read response from modem
  _tf.getString("AT+GSN\r\r\r\n","\r\n",imei, 15);
  
  //Expect "OK".
  if(!_tf.find("OK"))
    return 0;
  else  
    return 1;
}

uint8_t QuectelM10::read()
{
  return _cell.read();
}

void QuectelM10::SimpleRead()
{
	char datain;
	if(_cell.available()>0){
		datain=_cell.read();
		if(datain>0){
			Serial.print(datain, BYTE);
		}
	}
}

void QuectelM10::SimpleWrite(char *comm)
{
	_cell.println(comm);
}
