#IoT Kit Developer Guide


### Prerequisites

1. Setup IoT-Kit as mentioned in [README](https://github.com/hsenid-mobile/iot-kit/README.md)

2. Install [Node.js](https://nodejs.org/download)

3. Install following Node.js packages
   
   [ws](https://www.npmjs.com/package/ws)
   [wscat](https://www.npmjs.com/package/wscat)
   

### IoT API

IoT API can be used by telco applications to send 'programmable commands' to the IoT devices(IoT-Kit installed arduinos) 
and receive notifications when the commands executes, through a WebSocket connection.

IoT API's programmable commands are defined in [Bitlash](http://bitlash.net/) format.


### Application Connection URL

IoT Application can open a WebSocket connection to following URL 

ws://api.hsenidmobile.com:9008/iot/connect?token=<your application's token>

Application will be authenticated by the token given in the URL (You can get the token from devspace application detail email).
 
### Message Types

IoT application can program/instruct the connected arduinos to control actuators/read sensors 
by sending command messages and receive their output via notification messages.

Following Message Types are defined in the API

Boot Message Format
-------------------
This message is sent by the device to application, when device starts.
Purpose of this message is to identify the device and it's name. 

{ "id"        : "123",
  "type"      : "boot",
  "content"   : "Coffee Machine 001",
  "device"    : "tel:A#3B4wccSY6TuBsCHVWYI/wrqbrTr2W4FT4wto8C2c2fOrZ/WTjU1w3ND5asyKmWgLEQS",
  "timestamp" : 1429697258 }
  
parameter descriptions

'id'       : tracking id of the message
'type'     : boot type
'content'  : name of the device
'device'   : mobile number /masked mobile number of the device
'timestamp': message received time at IoT API platform
 

please note that, you have to use the received 'device' value to send a command back.


Command Message Format
----------------------
This message is sent by application to device, when application wants to load a programmable command

For example: 

Assume that, the device is connected to sensor on analog pin0 and
we want to get a notification when pin0 value goes to < 200

Such a program can be written in Bitlash like following

"while 1 {if(a0 < 200){print 'level=' a0;}; snooze(1000); };"

Here print function is used to collect the output and send back as notification message.
If you don't need notifications, you can skip using the print function.


To load this program to our "Coffee Machine 001", we have to send command message like following

{ "id" : "123456",
  "type" : "command",
  "content" : "while 1 {if(a0 < 200){print 'level=' a0;}; snooze(1000); };",
  "device" : "tel:A#3B4wccSY6TuBsCHVWYI/wrqbrTr2W4FT4wto8C2c2fOrZ/WTjU1w3ND5asyKmWgLEQS" }

For more details, Please check [Bitlash Commands](https://github.com/billroy/bitlash/wiki/commands)

'id'       : tracking id of the message
'type'     : command type
'content'  : Bitlash program
'device'   : mobile number /masked mobile number of the device

Status Message Format
----------------------
This message is sent by device to application, when the command is loaded successfully in the device

{ "id": "123456",
  "type": "status",
  "content": "S1000",
  "device" : "tel:A#3B4wccSY6TuBsCHVWYI/wrqbrTr2W4FT4wto8C2c2fOrZ/WTjU1w3ND5asyKmWgLEQS"
  "timestamp" : 1429697258 }

'id'       : id of the command
'type'     : status type
'content'  : status code of the command
'device'   : mobile number /masked mobile number of the device
'timestamp': message received time at IoT API platform


Notification Message Format
---------------------------
This message is sent by device to application, when the program print it's output

{ "id": "123456",
  "type": "notification",
  "content": "level=160",
  "device" : "tel:A#3B4wccSY6TuBsCHVWYI/wrqbrTr2W4FT4wto8C2c2fOrZ/WTjU1w3ND5asyKmWgLEQS"
  "timestamp" : 1429697301 }

'id'       : tracking id of the message
'type'     : notification type
'content'  : output of the command
'device'   : mobile number /masked mobile number of the device
'timestamp': message received time at IoT API platform



### Sample Node.js Code

Please check the [IoT Sample Application](https://github.com/hsenid-mobile/iot-app)



 
 
 
 
 
 
 
 
 
 