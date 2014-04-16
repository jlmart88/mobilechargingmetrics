/*
Testing for RRD functionality and timeserver syncing
 	 
 */
 
#include <RRDSD.h>
#include <Time.h>
#include <SD.h>
#include <MemoryFree.h>

File myFile;
File myFile1;
File myFile2;

RRDSD archiveRRD;//
RRDSD archiveRRD2;
RRDSD myRRD;//

char dataBuffer[3];
char timestampBuffer[19]; 

int average(int intArray[], int length)
{
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += intArray[i];
    }
    return total/length;
}

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print(F("Initializing SD card..."));
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));

  archiveRRD2 = RRDSD("testArc2.txt",4,12,22,2);
  archiveRRD = RRDSD("testArch.txt",2,4,22,2,&archiveRRD2,&average);
  myRRD = RRDSD("testRRD.txt",1,2,22,2,&archiveRRD,&average);
  
  // Remove these lines to not clear the file on reset
  myRRD.resetFile();
  archiveRRD.resetFile();
  archiveRRD2.resetFile();

}

void loop()
{
  getTimestamp(&timestampBuffer[0]);
  Serial.println(timestampBuffer);
  //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
    boolean archive = myRRD.log("50",&timestampBuffer[0], &dataBuffer[0], &timestampBuffer[0]);
  if (archive){
    //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
    //Serial.print(F("databuffer: ")); Serial.println(dataBuffer);
    //Serial.print(F("timestampbuffer: ")); Serial.println(timestampBuffer);
    archive = archiveRRD.log(&dataBuffer[0],&timestampBuffer[0], &dataBuffer[0], &timestampBuffer[0]);
    Serial.print(F("freeMemory()="));Serial.println(freeMemory());
     if (archive){
   archiveRRD2.log(&dataBuffer[0],&timestampBuffer[0]); 
  }
  }

  
  Serial.print(F("freeMemory()="));Serial.println(freeMemory());
   
   myFile2 = SD.open("testArc2.txt");
  if(myFile2){
    while (myFile2.available()) {
  	Serial.write(myFile2.read());
    }
    myFile2.close();
  }
  else Serial.println("failed");
  
   myFile1 = SD.open("testArch.txt");
  if(myFile1){
    while (myFile1.available()) {
  	Serial.write(myFile1.read());
    }
    myFile1.close();
  }
  else Serial.println("failed");
  
  myFile = SD.open("testRRD.txt");
  if(myFile){
    while (myFile.available()) {
  	Serial.write(myFile.read());
    }
    myFile.close();
  }
  else Serial.println("failed");
  
  // delay for readability in Serial
  delay(3000);
}

void getTimestamp(char *timestamp){
 time_t t = now(); 
 sprintf(timestamp, "%d", hour(t));
 if (hour(t)<10){
   timestamp[1]=timestamp[0];
   timestamp[0]='0';
 }
 sprintf(timestamp+3, "%d", minute(t));
 if (minute(t)<10){
   timestamp[4]=timestamp[3];
   timestamp[3]='0';
 }
 sprintf(timestamp+6, "%d", second(t));
 if (second(t)<10){
   timestamp[7]=timestamp[6];
   timestamp[6]='0';
 }
  sprintf(timestamp+9, "%d", day(t));
 if (day(t)<10){
   timestamp[10]=timestamp[9];
   timestamp[9]='0';
 }
  sprintf(timestamp+12, "%d", month(t));
 if (month(t)<10){
   timestamp[13]=timestamp[12];
   timestamp[12]='0';
 }
  sprintf(timestamp+15, "%d", year(t));
  
 timestamp[2]=':';
 timestamp[5]=':';
 timestamp[8]=',';
 timestamp[11]='/';
 timestamp[14]='/'; 
}


