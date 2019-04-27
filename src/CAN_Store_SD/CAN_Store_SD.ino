//librarys
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <SD.h>

//defines
#define LED0 7
#define LED1 8
#define SD_PIN 9
#define DATA_SIZE 100

//global value
File dataFile;
int fileCount;
int dataCount;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);

  //init MCP2515
  while(1){
    if(Canbus.init(CANSPEED_500)){  //Initialise MCP2515 CAN controller at the specified speed
      digitalWrite(LED0, HIGH);
      delay(1000);
      break;
    }else{
      delay(1000);
    }
  }

  //init SD
  while(1){
    if(SD.begin(SD_PIN)){
      digitalWrite(LED1, HIGH);
      delay(1000);
      break;
    }else{
      delay(1000);
    }
  }
  dataCount = 0;
  fileCount = 0;
}

void loop() {
  tCAN message;
  if (mcp2515_check_message()) {
    if (mcp2515_get_message(&message)){
      digitalWrite(LED1, !digitalRead(LED1));
      
      //open new file
      if(dataCount == 0){
        dataFile = SD.open("file" + String(fileCount) + ".txt", FILE_WRITE);
        fileCount++;
        digitalWrite(LED0, !digitalRead(LED0));
      }
      dataFile.print("ID: ");
      dataFile.print(message.id,HEX);
      dataFile.print(", Length: ");
      dataFile.print(message.header.length,DEC);
      dataFile.print(", Data: ");
      for(int i=0;i<message.header.length;i++) { 
        dataFile.print(message.data[i],HEX);
        dataFile.print(" ");
      }
      dataFile.println("");
      dataCount++;

      // close file
      if(dataCount > DATA_SIZE){
        dataFile.close();
        dataCount = 0;
      }
    }
  } 
}
