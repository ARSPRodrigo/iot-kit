/*******this library can be used to initiate, maintain and end a ussd session
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*******/

#include <GSM.h>
#include <GSM3ShieldV1DirectModemProvider.h>
#include "USSDAccess.h"

uint8_t bufferLen = 123; 		            	//Max length of the buffer on GSM module
uint8_t ussdLen = 182;  		           	//Max length of the ussd message
uint8_t length_Resp = 190;           			//Length of the char array used to store the response
uint8_t length_Response = 100;       		//Max length of the payload
uint8_t typeIndex = 9;               			//Index of +CUSD code giving the type of response
uint8_t length_Atcommand_type = 8;		// basic length of the ATcommand +CUSD
uint8_t if_connected = 3;				//if module is connected
uint8_t error = 1;					//error response for AT command
uint8_t success = 4;					//Success response for AT command
uint8_t length_extra_characters = 14;		//length of other characters required to build ATcommand

unsigned long comDelay = 1000;       		//delay for communicating with shield
unsigned long timeout = 3000;        		//Delay for reading a ussd reply
unsigned long connectDelay = 1000;   		//Delay for getting connected to a GSM network

char Atcommand_type[9] = "AT+CUSD="; 	//prototype ussd ATcommand
char enable_presentation = '1';			//enabling USSD presentation					
char cancel_session = '2';				//ending a USSD session

GSM gsmAccess;
GSM3ShieldV1DirectModemProvider modem;

/*** constructor***/

USSDAccess :: USSDAccess() {
}

/***connects the shield***/

void USSDAccess:: connectModule(){
  boolean notConnected = false;
  if (gsmAccess.getStatus() != GSM_READY)
  {
    notConnected = true;
  }

  while(notConnected)
  {
    Serial.println("Connecting to the network");
    if(gsmAccess.begin() == GSM_READY)
    {
      notConnected = false;
      Serial.println("Connected");
    }
    else
    {
      Serial.println("Not connected");
      delay(connectDelay);
    }
  }
}

/***calculates length of a string***/

uint8_t USSDAccess:: string_length(char* string) {
   uint8_t length = 0;
   uint8_t i = 0;

   char c = string[0];

   while (c != '\0')
  {
    i++;
    length++;
    c = string[i];
  }
 
  return length;
}


/***builds the ATcommand to be sent***/

void USSDAccess:: setATCommand(char* code, char type, char* dcs, char ATCommand[], int length_code, int length_dcs) {
  int i;

  for(i = 0; i < length_Atcommand_type; i++)
  {
    ATCommand[i] = Atcommand_type[i];
  }
  
  ATCommand[8] = type;
  ATCommand[9] = ',';
  ATCommand[10] = '"';

  for (i = 0; i < length_code; i++)
  {
    ATCommand[i + 11] = code[i]; 
  }

  ATCommand[11 + length_code] = '"';
  ATCommand[12 + length_code] = ',';

  for (i = 0; i < length_dcs; i++)
  {
    ATCommand[i + 13 + length_code] = dcs[i]; 
  }  

  ATCommand[13 + length_code + length_dcs] = '\0';
}


/***sends an ATcommand to the shield***/

int USSDAccess :: sendATCommand(char ATcommand[]) {
  String response;

  if(gsmAccess.getStatus() == if_connected)
  {
    response = modem.writeModemCommand(ATcommand,comDelay);
  }

  else
     Serial.println("GSM module not ready");

  if (response == "")
  {
    return (success);
  }

  else
  {
    return (error );
  }    
}


/***initialise an USSD session***/

int USSDAccess :: initSession(char* ussd_code) {
  char *dcs = "";

  int length_code = string_length(ussd_code);
  int length_dcs = string_length(dcs);

  uint8_t response;
	
  char ATCommand[length_code + length_dcs + length_extra_characters];

  connectModule(); 

  setATCommand(ussd_code, enable_presentation, dcs, ATCommand, length_code, length_dcs);
  response = sendATCommand(ATCommand);

  if (response == success)
    return (success);

  else 
    return (error );  
}

/***ends a ussd session with no message***/

int USSDAccess :: endSession() {
  char *dcs = "";
  char* code = "";

  int length_code = string_length(code);
  int length_dcs = string_length(dcs);
  
  char ATCommand[length_code + length_dcs + length_extra_characters];

  uint8_t response;
   
  setATCommand(code, cancel_session, dcs, ATCommand, length_code, length_dcs);
  response = sendATCommand(ATCommand);

  if (response == success)
    return (success);

  else 
    return (error );
}

/***ends a ussd session with a message***/

int USSDAccess :: endSession(char* end_response) {
  char *dcs = "";
  
  int length_end_response = string_length(end_response);
  int length_dcs = string_length(dcs);
  
  char ATCommand[length_end_response + length_dcs + length_extra_characters];

  uint8_t response;

  setATCommand(end_response, cancel_session, dcs, ATCommand, length_end_response, length_dcs);
  response = sendATCommand(ATCommand);

  if (response == success)
    return (success);

  else 
    return (error );  
}

/***responsds to a ussd request***/

int USSDAccess :: respond(char* message) {
  char *dcs = "";
  
  int length_message = string_length(message);
  int length_dcs = string_length(dcs);
  
  char ATCommand[length_message + length_dcs + length_extra_characters];

  uint8_t response; 

  setATCommand(message, enable_presentation, dcs, ATCommand, length_message, length_dcs);
  response = sendATCommand(ATCommand);

  if (response == success)
    return (success);

  else 
    return (error );
}

/***decodes the 7bit encoding***/

char USSDAccess :: decoder(int character) {
  char c = character;

  return c;
}

/***returns the response from the network***/

void USSDAccess :: getResponse(char resp[]) {
  char r;

  uint8_t i;

  unsigned long startTime;
  
    for(i = 0; i < 175; i++)
    resp[i] = '\0';
 
    
  i = 0;

  startTime = millis();

  Serial.println("Start reading response");

  while(millis() - startTime <= timeout)
  {

    while(modem.available())
    {
       r = decoder(modem.read());
       if(r != '\0')
       {
          if(r != '\n' && r != '\r')
  	  {
            resp[i] = r;
            i++;  
          }
          else
  	  {
            resp[i] = ' ';
            i++;  
          }
       }     
    }
  }
  
  resp;
}
