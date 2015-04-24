# IoT-Kit
Kit for hSenid Mobile Telco Application Platform's IOT API

## Installation Guide

### Prerequisites
1. Arduino Board with GSM Sheild and SIM

2. Create an USSD Application in [hSenid Mobile TAP Platform](http://devspace.hsenidmobile.com/wp-login.php?action=register)

3. Configure your USSD Application's MO Receiver URL to http://api.hsenidmobile.com:9009/ussd-notifications

4. Install [Arduino IDE](http://www.arduino.cc/en/main/software) in your computer


### Step 1.
Go to the [IoT-Kit repository](https://github.com/hsenid-mobile/iot-kit) and download the project zip.

### Step 2.
Unzip the downloaded iot-kit-master.zip.

### Step 3. 
Open the configure.h file and add the # code related to your application.

For example, if your applications USSD code is #771*576#

```c
char application_code[] = "#771*576#";

```

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

Now your arduino is ready to run an application.

Please follow the [developer-guide](https://github.com/hsenid-mobile/iot-kit/blob/master/developer-guide.md) to develop your application.

