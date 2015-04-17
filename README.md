# iot-kit
Kit for HSenid Mobile Telco Application Platform's IOT API

## Installation Guide

### Step 1.
Go to the github repository (https://github.com/hsenid-mobile/iot-kit) and download the project zip.

### Step 2.
Unzip the downloaded iot-kit-master.zip.

### Step 3. 
Open the configure.h file and add the # code related to your application.
eg:   char application_code[] = "*123#";

Save the changes

### Step 4. 
Open the library_iot.ino file using the Arduino IDE

### Step 5.
Connect your arduino board to the computer

### Step 6.
Make sure you have selected the correct board and port in the IDE
(to check this go to the tools/board and tools/port tabs in the IDE)

### Step 7.
Load the code to the arduino board using the IDE.

### Step 8.
Disconnect the arduino from the computer, connect the sensors and actuators and the gsm shield, and finally connect the power supply.

Now your arduino will automatically create a connection with the application and retrive commands.