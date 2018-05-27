
#include <mcp_can.h>
#include <SPI.h>
#include <Servo.h> 

Servo myservo;  // create servo object to control a servo 

#define ID_BRAKE_CMD      0x060
#define ID_BRAKE_REPORT   0x061

#define pressed  1990         //Brake pedal fully pressed
#define released 1500         //Brake pedal released

const int Brake_Report = 3;
const int gearPin = 2;
const int EStopPin = 6;

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
int Linear_pos, ros_cmd;

int report;  
char gear;

MCP_CAN CAN0(10);                               // Set CS to pin 10
unsigned char Brake_report[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // testing

//********************************Setup Loop*********************************//

void setup() {
  Serial.begin(115200);
  CAN0.begin(CAN_500KBPS);                       // init can bus : baudrate = 500k 
  Serial.println("Start");
  pinMode(gearPin, OUTPUT);
  pinMode(EStopPin, INPUT);
  digitalWrite(gearPin,LOW);
  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  //myservo.writeMicroseconds(pressed);
  myservo.writeMicroseconds(released);
  
}

//********************************Main Loop*********************************//

void loop() 
{
    if(digitalRead(EStopPin) == 0)
    {
       myservo.writeMicroseconds(pressed);                  
        delay(2); 
    }
    
    // if there's any serial available, read it:
    if (Serial.available() > 0) {
      gear = Serial.read(); 
    }
    
    if (gear == 'D') {
     digitalWrite(gearPin,LOW);
      }
      else if(gear == 'R') {
     digitalWrite(gearPin,HIGH);
      }
      else{
      }
  
    report = pulseIn(Brake_Report,HIGH);    //Read current position of brake pedal
    Serial.print("report = ");              //report = 100 when pressed, =500 when released 
    Serial.println(report);
    report = map(report,100,500,0xffff,0);
    Brake_report[7]=0x01;
    Brake_report[5]=report;
    CAN0.sendMsgBuf(ID_BRAKE_REPORT, 0, 8, Brake_report); 

    //READ other messages
    CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      rxId = CAN0.getCanId();                    // Get message ID
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

   //READ brake command
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

          /*
          if(rxBuf[7]= 0) //Neutral
          {
            
          }
          else if(rxBuf[7]= 1)  //Forward
          {
             digitalWrite(gearPin,LOW);
          }
          else if(rxBuf[7]= 2)  //Reverse
          {
             digitalWrite(gearPin,HIGH);
          }*/
          
         if((rxBuf[3]& 0x01)==1)
          {
            ros_cmd = (rxBuf[1] << 8) + rxBuf[0];    //16 bit brake cmd value from ROS 
            Serial.print(" ros_cmd"); 
            Serial.println(ros_cmd);
           
            if(ros_cmd<10000) 
            {
              Linear_pos = released;
              
            }
            else if(ros_cmd>14000)
            {
              Linear_pos = pressed;
            }
            else
            {
              Linear_pos = map(ros_cmd, 10000, 14000, released, pressed); 
            }

            // Linear_pos = map(ros_cmd, 0, 32767, released, pressed);
            
            Serial.print(" Linear_pos"); 
            Serial.println(Linear_pos);          
            myservo.writeMicroseconds(Linear_pos);                  // sets the servo position according to the scaled value 
            delay(2);
            //delay(1000); 
          }
      }
}
