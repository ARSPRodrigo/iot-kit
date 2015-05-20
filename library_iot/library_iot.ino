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
int max_notification_attempts = 3; 
int notification_retry_count = 0;
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
      send_notification();

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
    if (is_registered == 0) {
      String request = "Boot:DEVICE_1234 ";
      Serial.println(request);
      
      write_message(request);
      String response = read_message();
      
      Serial.println(response);
      is_registered = 1;
    } else {
      
      String request = "Poll ";
      Serial.println(request);
      
      write_message(request);
      
      String response = read_message();
      Serial.println(response);
      
      if (response == "Ok") {
        Serial.println("No pending command");
        delay(1000);
      } else {
        Serial.println("New command received");
        extract_and_execute_instructions_new(response);
      }  
    }  
  }

  if (!client.connected()) {
    Serial.println("disconnecting.");
    client.stop();
    is_registered = 0;
  }

}

String read_message(){
   String data = "";
  
    while (!client.available()){delay(1000);}
    
    uint8_t len = client.read();
    
    char read_buf[len];
    
    int i =0;
    Serial.println("reading the available response..");
    
    while (i < len) {      
       data.concat((char)client.read());
       i+=1;
    }
    
   return data;
}

void write_message(String message){
  uint8_t len = message.length();
  
  char charBuf[len];
  
  message.toCharArray(charBuf, len);
  
  uint8_t frame_length = len +1;

  uint8_t write_buf[frame_length];
  
  write_buf[0] = (uint8_t)len;
  
  for (int j = 0; j < len; j++) {
    write_buf[j+1] = charBuf[j];
  }
     
  client.write(write_buf, frame_length);
  client.flush();
}


void send_notification(){
  String notification = "Ntfy:";
  notification.concat(ussd_notification_data);
  Serial.println(notification);
   
  write_message(notification);//
  String response = read_message();      
  Serial.println(response);
  if (response == "Ok"){
    ussd_notification_data = "";  
    notification_retry_count = 0;  
  }
  else if (response != "Ok" & notification_retry_count< max_notification_attempts){
    delay(100);
    send_notification();
    notification_retry_count +=1;
  }
}

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
    
    String cmd =  getValue(instruction, ':', 1);
    cmd.toCharArray(charBuf,200);
    
    char* bitlash_command = charBuf;
    doCommand(bitlash_command);   
 
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

