/****************************************************************************
CAN Write Demo for the SparkFun CAN Bus Shield. 
Written by Stephen McCoy. 
Original tutorial available here: http://www.instructables.com/id/CAN-Bus-Sniffing-and-Broadcasting-with-Arduino
Used with permission 2016. License CC By SA. 
Distributed as-is; no warranty is given.
*************************************************************************/
//COM 10
#include <mcp_can.h>
#include <SPI.h>
#include <Servo.h> 

Servo myservo;  // create servo object to control a servo 

#define ID_BRAKE_CMD      0x060
#define ID_BRAKE_REPORT   0x061

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
int Linear_pos, ros_cmd;
MCP_CAN CAN0(10);                               // Set CS to pin 10
unsigned char Brake_report[8] = {0, 0, 0, 0, 0x45, 0, 0, 0};

//********************************Setup Loop*********************************//

void setup() {
  Serial.begin(115200);
  CAN0.begin(CAN_500KBPS);                       // init can bus : baudrate = 500k 
  Serial.println("MCP2515 Library Receive Example...");
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.writeMicroseconds(1000);
}

//********************************Main Loop*********************************//

void loop() 
{
  // send data:  id = 0x0, standrad flame, data len = 8, stmp: data buf
  CAN0.sendMsgBuf(ID_BRAKE_REPORT, 0, 8, Brake_report);  
  delay(10);                       // send data per 100ms
 
  //READ
 CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
      if(rxId != ID_BRAKE_REPORT)
      {
          Serial.print("ID: ");
          Serial.print(rxId, HEX);
          Serial.print("  Data: ");
          for(int i = 0; i<len; i++)                // Print each byte of the data
          {
            if(rxBuf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
            {
              Serial.print("0");
            }
            Serial.print(rxBuf[i], HEX);
            Serial.print(" ");
          }
          Serial.println();
      }

      //READ
 CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
    //  if(rxId != ID_BRAKE_REPORT)
  //    {
          Serial.print("ID: ");
          Serial.print(rxId, HEX);
          Serial.print("  Data: ");
          for(int i = 0; i<len; i++)                // Print each byte of the data
          {
            if(rxBuf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
            {
              Serial.print("0");
            }
            Serial.print(rxBuf[i], HEX);
            Serial.print(" ");
          }
          Serial.println();
   //   }
 
 //READ
 CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
      if(rxId == ID_BRAKE_CMD)
      {
          Serial.print("ID: ");
          Serial.print(rxId, HEX);
          Serial.print("  Data: ");
          for(int i = 0; i<len; i++)                // Print each byte of the data
          {
            if(rxBuf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
            {
              Serial.print("0");
            }
            Serial.print(rxBuf[i], HEX);
            Serial.print(" ");
          }
          Serial.println();

          ros_cmd = (rxBuf[1] << 8) + rxBuf[0];   
          Serial.print(" ros_cmd"); 
          Serial.println(ros_cmd);
          Linear_pos = map(ros_cmd, 0, 32767, 1000, 2000);
          Serial.print(" Linear_pos"); 
          Serial.println(Linear_pos);          
          myservo.writeMicroseconds(Linear_pos);                  // sets the servo position according to the scaled value 
          delay(2); 
      }
}