#include "inetGSM.h"


int InetGSM::httpGET(const char* server, int port, const char* path, char* result, int resultlength)
{
  int length_write;
  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';

  /*
  Status = ATTACHED.
  if(gsm.getStatus()!=GSM::ATTACHED)
    return 0;
  */
    
  if(!gsm.connectTCP(server, port))
    return 0;
  
  gsm.write((const uint8_t*)"GET ", 4);
  gsm.write((const uint8_t*)path, strlen(path));
  gsm.write((const uint8_t*)" HTTP/1.0\nHost: ", 16);
  gsm.write((const uint8_t*)server, strlen(server));
  gsm.write((const uint8_t*)"\n",1);
  gsm.write((const uint8_t*)"User-Agent: Arduino",20);
  gsm.write((const uint8_t*)"\n\n",2);

  gsm.write((const uint8_t*)end_c,2);

  switch(gsm.WaitResp(10000, 100, "SEND")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }


  int res= gsm.read(result, resultlength);

  //gsm.disconnectTCP();
  
  //int res=1;
  return res;
}

int InetGSM::httpPOST(const char* server, int port, const char* path, const char* parameters, char* result, int resultlength)
{
  char itoaBuffer[8];
  int num_char;
  
  if (!gsm.connectTCP(server, port))
    return 0;

  
  strcpy(_buffer,"POST ");
  strcat(_buffer,path);
  strcat(_buffer," HTTP/1.0\nHost: ");
  strcat(_buffer,server);
  //  strcat(_buffer,"\n\rUser-Agent: Mozilla/4.0\n\rContent-Length: ");
  strcat(_buffer,"\nContent-Length: ");
  itoa(strlen(parameters),itoaBuffer,10);  
  strcat(_buffer,itoaBuffer);
  strcat(_buffer,"\n\n");
  strcat(_buffer,parameters);
  strcat(_buffer,"\n\n");
  
  
  
  gsm.write((const uint8_t*)_buffer, strlen(_buffer));


//  int res= gsm.read(result, resultlength);

  gsm.disconnectTCP();
  //return res;
   return 1;
}

int InetGSM::tweet(const char* token, const char* msg)
{
  //    gsm.httpPOST("arduino-tweet.appspot.com",80,"/update", "token=15514242-eWYAlMwjRQfrhnZxQiOfDXUpaYwjbSvMl1Nm5Qyg&status=Spam", buffer, 200);
  char shortbuf[200];
  strcpy(shortbuf,"token=");
  strcat(shortbuf,token);
  strcat(shortbuf,"&status=");
  strcat(shortbuf, msg);
  httpPOST("arduino-tweet.appspot.com",80,"/update",shortbuf, shortbuf, BUFFERSIZE);
}

int InetGSM::openmail(char* server, char* loginbase64, char* passbase64, char* from, char* to, char* subj)
{
	  if (!gsm.connectTCP(server, 25))
    	return 0;
    
    delay(1000);
    gsm.read(_buffer, BUFFERSIZE);    
    gsm.write("HELO\n");
    delay(500);
    gsm.read(_buffer, BUFFERSIZE);
    gsm.write("AUTH LOGIN\n");
    delay(500);
    gsm.read(_buffer, BUFFERSIZE);
    gsm.write(loginbase64);gsm.write("\n");
    delay(500);
    gsm.read(_buffer, BUFFERSIZE);
    gsm.write(passbase64);gsm.write("\n");
    delay(500);
    gsm.read(_buffer, BUFFERSIZE);
    gsm.write("MAIL FROM: ");gsm.write(from);gsm.write("\n");
    delay(500);
    gsm.read(_buffer, BUFFERSIZE);
    gsm.write("RCPT TO: ");gsm.write(to);gsm.write("\n");
    delay(500);
    gsm.read(_buffer, BUFFERSIZE);
    gsm.write("Subject: ");gsm.write(subj);gsm.write("\n\n");
    return 1;
}
int InetGSM::closemail()
{
	gsm.write("\n.\n");
	gsm.disconnectTCP();
	return 1;
}
 
