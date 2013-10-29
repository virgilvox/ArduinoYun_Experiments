/*
HeatSync Labs
www.heatsynclabs.org

Hacked together by 
Matt Shepard && Moheeb Zara (www.vlrev.com)

10/28/2013

********
Arduino Yun code to allow streaming of data from the Yun hardware to a pc listening for OSC protocol data on a udp port. 
Sends Osc channels carrying arrays of data over UDP by running shell commands
on the Arduino Yun using Bridge, Process, and FileIO. 
 
**USAGE**READ THIS**

NOTE: Requires netcat on the Linux side of the YUN. 
To install via ssh;
Use Yun's ip and port 22. 
User: root  |  Password: arduino
http://arduino.cc/en/Tutorial/YunPackageManager <- See for help on installing packages. 

OscSend(CHANNEL ID, ARRAY SIZE, ARRAY)

Located at bottom of program:
p.runShellCommand("netcat -u -c YOUR_TARGET_IP PORT < /tmp/sensordata");
Replace YOUR_TARGET_IP and PORT with your target computer's IP and UDP listening port.

 */

#include <Process.h>
#include <Bridge.h>
#include <FileIO.h>

byte buf[4];
int valuesA[4]; // array to store values to include with each OSC channel. Any size will do.
int valuesB[3];
void setup() {
  // Initialize Bridge
  Bridge.begin();
  
  pinMode(13,OUTPUT); // we'll use this to indicate status
  
  // Initialize Serial
   FileSystem.begin();
 
}

void loop() {
  // Read in analog data to valuesA array for channel /1
    valuesA[0] = analogRead(A0); 
    valuesA[1] = analogRead(A1);
    valuesA[2] = analogRead(A2);
    valuesA[3] = analogRead(A3);
    // OscSend(CHANNEL ID, ARRAY SIZE, ARRAY)
    OscSend(1, 4, valuesA);
    // OscSend will send over udp /1 valuesA[0] ...valuesA[n] 
    
    valuesB[0] = analogRead(A4);
    valuesB[1] = analogRead(A5);
    valuesB[2] = analogRead(A2);
    OscSend(2, 3, valuesB);
    OscSend(3, 3, valuesB);
    OscSend(4, 3, valuesB);
    OscSend(5, 3, valuesB);
    OscSend(6, 3, valuesB);
    OscSend(7, 3, valuesB);
    OscSend(8, 3, valuesB);
    OscSend(9, 3, valuesB);
    OscSend(10, 3, valuesB);
    OscSend(11, 3, valuesB);
    OscSend(12, 3, valuesB);
    delay(100);

}

void int2buf(int x) {
  // Don't fuss with this unless you know what you're doing!
  buf[0] = (x >> 24) & 0xFF;
  buf[1] = (x >> 16) & 0xFF;
  buf[2] = (x >> 8) & 0xFF;
  buf[3] = x & 0xFF;
  
}

void OscSend(int osc, int count, int* values) {
  // clear sensor data file before rewriting
  FileSystem.remove("/tmp/sensordata");
  //Open a file for writing on the Linux side of the YUN (you can rename this)
  File script = FileSystem.open("/tmp/sensordata", FILE_WRITE);
  //Begin writing array values to file in Open Sound Control protocol encoding. 
  script.print("/");
  script.print(osc);
  if(osc < 10){
    script.write('\0');
  }
  script.write('\0');
  script.print(",");
  for(int ix=0;ix<count;ix++){
    script.print("i");
  }
  for(int iy=0;iy<4-(count+1)%4;iy++){
    script.write('\0');
  }
  for(int iz=0;iz<count;iz++){
    int2buf(values[iz]);
    script.write(buf, 4);
  }
  script.close();
  Process p;	// create a process p for running shell commands through. 	
  //Replace YOUR_TARGET_IP and PORT with your target computer's IP and UDP listening port.
  p.runShellCommand("netcat -u -c 172.22.111.34 7400 < /tmp/sensordata");	
}



