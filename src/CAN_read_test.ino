//include modules
#include <SPI.h>
#include <SD.h>
#include <mcp_can.h>

//define pins
#define LED0 7
#define LED1 8
#define SD_PIN 9
#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10

//define const value
#define DATA_SIZE 20 //data size per 1 file

//global value
File dataFile;
int fileCount;
int dataCount;

//CAN data
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

//init
void setup() {
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(SD_PIN, OUTPUT);

  //init SD
  while(1){
    if(SD.begin(SD_PIN)){
      digitalWrite(LED0, HIGH);
      delay(1500);
      break;
    }else{
      delay(1500);
    }
  }

  //init MCP2515
  while(1){
    if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK){
      digitalWrite(LED1, HIGH);
      //delay(1500);
      break;
    }else{
      delay(1500);
    }
  }
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT); 
  //delay(1500);
}

//main loop
void loop() {
  if(!digitalRead(CAN0_INT)){
    digitalWrite(LED0, LOW);
    
    //open new file
    if(dataCount == 0){
      dataFile = SD.open("file" + String(fileCount) + ".txt", FILE_WRITE);
      fileCount++;
    }

    //write data
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    dataFile.print("ID: ");
    dataFile.print(rxId, HEX);
    dataFile.print("  Data: ");
    for(int i = 0; i<len; i++) {
      if(rxBuf[i] < 0x10) {
        dataFile.print("0");
      }
      dataFile.print(rxBuf[i], HEX);
      dataFile.print(" ");
    }
    dataFile.println();
    dataCount++;

    // close file
    if(dataCount > DATA_SIZE){
      dataFile.close();
      dataCount = 0;
    }  
  }
}
