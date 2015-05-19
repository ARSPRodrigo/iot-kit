/// gprs based configurations ///
// libraries
#include <GSM.h>
#include <bitlash.h>
#include "configure.h"
// PIN Number
#define PINNUMBER ""
// APN Settings
#define GPRS_APN       "dialogbb"
#define GPRS_LOGIN     ""    
#define GPRS_PASSWORD  ""

GPRS gprs;
GSM gsmAccess; 
GSMClient client;

char server[] = "api.hsenidmobile.com";
int port = 9009;
/// end of gprs based /// 


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #define serialPrintByte(b) Serial.write(b)
#else
  #define serialPrintByte(b) Serial.print(b,BYTE)
#endif

char* ussd_notification_data;
char* command;
char* register_request;

char* saved = "saved";
char* notify_request = "NtfyL";
char* command_request = "Cmd";
char* stop_process = "{stop 0;}";

int loop_delay;
int max_resp_len = 175;	                                       
int exist_new_command = 1;
int notification_sent = 0;
int poll_timer = periodic_check_timer;
int is_registered = 0;

String received_command = "";
String return_data="";


void set_bitlash_command(char* received_command){
   command = received_command ;
}

void run_bitlash_command(){ 
    doCommand(command);
}

void serialHandler(byte b) {
  if (b == 13 ){
    if (return_data.equals("saved")){
      return_data = "";       
    }
    else {
      char charBuf[200];                              
      return_data.toCharArray(charBuf,200);
      ussd_notification_data = charBuf;
      Serial.println("Preparing to send the notification");
      //send_notification();

      return_data = "";
    }   
  }
  else {
    if (b == 10){
      //
    }
    else{
      return_data.concat((char)b);       
    }  
  }
}

void setup() {  
  initBitlash(9600);  
  addBitlashFunction("run_bitlash_command", (bitlash_function) run_bitlash_command);
  setOutputHandler(&serialHandler);
  Serial.println("Booting the Device");
  setupIfGPRSNotReady();

}

void loop() {
  runBitlash();
  if (!client.connected()) {
    if (!connectHttp()) {
      setupIfGPRSNotReady();
      return;
    }  
  }

  if (client.connected()) {
    uint8_t buffer[]= "Boot:DEVICE_1234";
    uint8_t write_buf[17];
    write_buf[0] = 16;
    for (int j = 0; j < sizeof(buffer); j++) {
       write_buf[j+1] = buffer[j];
     }
     
    client.write(write_buf);
    client.flush();
    while (!client.available()){delay(1000);}
    int length = client.read();
    uint8_t read_buf[length];
    int i =0;
    Serial.println("reading the available response..");
    
    while (i< length)
    {      
       read_buf[i] = client.read();
       i+=1;
    }
    for (int i = 0; i < sizeof(read_buf); i++) {
       Serial.print((char)read_buf[i]);
    }
    Serial.println("");
  }

  if (!client.connected()) {
    Serial.println("disconnecting.");
    client.stop();
  }

}


uint8_t read_write()

void setupIfGPRSNotReady(){
  Serial.println("Setting up GPRS");
  // connection state
  boolean notConnected = true;

  while(notConnected) {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY)) {
      notConnected = false;
      Serial.println("GPRS_READY");
     } else {
      Serial.println("GSM/GPRS NOT READY");
      delay(1000);
    }
  }
}



int connectHttp() {
  Serial.println("Connecting to IoT Gateway...");
  int rs = client.connect(server, port);  
  if (rs){
    Serial.println("connected");
  } else{
    Serial.println("connection failed");
  }
  return rs;
}  






void extract_and_execute_instructions_new(String instruction){
  if (instruction.substring(0,8) == "function") {
    //String temp1 = "function iot_function {";        
    //temp1.concat(instruction);                      // eg: "function iot_function {pinMode(13, 1); dw(13,1);  if(a0< 200){print \"level=\"a0;}; snooze(2000); }"
    //temp1.concat("}");
    char charBuf[200];                              
    instruction.toCharArray(charBuf,200);
    char* bitlash_command = charBuf;
  
    set_bitlash_command(stop_process);
    set_bitlash_command(bitlash_command);
    run_bitlash_command();
    bitlash_command = "run iot_function;";
    set_bitlash_command(bitlash_command);
    run_bitlash_command();
    Serial.println("Bitlash command running.."); 
  }
  else { 
    Serial.println("Command is executing.."); 
    char charBuf[200];                              
    instruction.toCharArray(charBuf,200);
    char* bitlash_command = charBuf;
    doCommand(bitlash_command);   
 
} 
}


void execute_command(){  
  //received_command = "CmdL:while 1 {if(!d13){print \"light is toggling\";}; snooze(100);};";  
    received_command = "Cmd:function iot_function {pinMode(13, 1); dw(13,1);  if(a0< 200){print \"level=\"a0;}; snooze(2000); }";  
  // received_command = "CmdL:print d13;"; 
  //perform the replacements:
  String temp_string = received_command;
  temp_string.replace("'", "\"");
  received_command = temp_string;
  
  String extracted_command =getValue(received_command,':',0);   
  if (extracted_command=="Cmd") {
    Serial.println("Command ready: " + getValue(received_command,':',1));  
    extract_and_execute_instructions_new(getValue(received_command,':',1));     
  } 
}




String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

