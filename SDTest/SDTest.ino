/*
Testing for RRD functionality and timeserver syncing
 	 
 */
 
#include <RRDSD.h>
#include <Time.h>
#include <SD.h>
//#include <MemoryFree.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

File myFile;
File myFile1;
File myFile2;

RRDSD archiveRRD;//
RRDSD archiveRRD2;
RRDSD myRRD;//

char dataBuffer[3];
char timestampBuffer[19]; 

 /* ******** Ethernet Card Settings ******** */
// Set this to your Ethernet Card Mac Address
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xBA, 0x72 };
 
/* ******** NTP Server Settings ******** */
/* us.pool.ntp.org NTP server
   (Set to your time server of choice) */
IPAddress timeServer(132, 163, 4, 101);
 
/* Set this to the offset (in seconds) to your local time
   This example is GMT - 6 */
const long timeZoneOffset = -18000L;  
 
/* Syncs to NTP server every 15 seconds for testing,
   set to 1 hour or more to be reasonable */
unsigned int ntpSyncTime = 60;        
 
 
/* ALTER THESE VARIABLES AT YOUR OWN RISK */
// local port to listen for UDP packets
unsigned int localPort = 8888;
// NTP time stamp is in the first 48 bytes of the message
const int NTP_PACKET_SIZE= 48;      
// Buffer to hold incoming and outgoing packets
byte packetBuffer[NTP_PACKET_SIZE];  
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;                    
// Keeps track of how long ago we updated the NTP server
unsigned int ntpLastUpdate = 0;    
// Check last time clock displayed (Not in Production)
time_t prevDisplay = 0;

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

  //Serial.print(F("Initializing SD card..."));
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    //Serial.println(F("initialization failed!"));
    return;
  }
  //Serial.println(F("initialization done."));

  archiveRRD2 = RRDSD("testArc2.txt",4,12,22,2);
  archiveRRD = RRDSD("testArch.txt",2,4,22,2,&archiveRRD2,&average);
  myRRD = RRDSD("testRRD.txt",1,2,22,2,&archiveRRD,&average);
  
  // Remove these lines to not clear the file on reset
  myRRD.resetFile();
  archiveRRD.resetFile();
  archiveRRD2.resetFile();
 /*     
 // Ethernet shield and NTP setup
 int i = 0;
 int DHCP = 0;
 DHCP = Ethernet.begin(mac);
 //Try to get dhcp settings 30 times before giving up
 /*
 while( DHCP == 0 && i < 30){
   delay(1000);
   DHCP = Ethernet.begin(mac);
   i++;
 }
 */
 /*
 if(!DHCP){
  Serial.println("DHCP FAILED");
   for(;;); //Infinite loop because DHCP Failed
 }
 Serial.println("DHCP Success");
 */
 //Try to get the date and time
/*
 i=0;
 while(!getTimeAndDate() && i<10) {
   i++;
 }
*/
}

void loop()
{
  /*
    // Update the time via NTP server as often as the time you set at the top
  if(now()-ntpLastUpdate > ntpSyncTime) {
    int trys=0;
    while(!getTimeAndDate() && trys<10){
      trys++;
    }
    if(trys<10){
      //Serial.println("ntp server update success");
    }
    else{
     // Serial.println("ntp server update failed");
    }
  }
 */
  // Display the time if it has changed by more than a second.
  //if( now() != prevDisplay){
    //prevDisplay = now();
    //clockDisplay();  
 // }
        
  getTimestamp(&timestampBuffer[0]);
  //Serial.println(timestampBuffer);
  //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
    boolean archive = myRRD.log("50",&timestampBuffer[0], &dataBuffer[0], &timestampBuffer[0]);
  if (archive){
    //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
    //Serial.print(F("databuffer: ")); Serial.println(dataBuffer);
    //Serial.print(F("timestampbuffer: ")); Serial.println(timestampBuffer);
    archive = archiveRRD.log(&dataBuffer[0],&timestampBuffer[0], &dataBuffer[0], &timestampBuffer[0]);
    //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
     if (archive){
       archiveRRD2.log(&dataBuffer[0],&timestampBuffer[0],&dataBuffer[0],&timestampBuffer[0]); 
     }
  }

  
  //Serial.print(F("freeMemory()="));Serial.println(freeMemory());
  
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
/*
// Do not alter this function, it is used by the system
int getTimeAndDate() {
   int flag=0;
   Udp.begin(localPort);
   sendNTPpacket(timeServer);
   delay(1000);
   if (Udp.parsePacket()){
     Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
     unsigned long highWord, lowWord, epoch;
     highWord = word(packetBuffer[40], packetBuffer[41]);
     lowWord = word(packetBuffer[42], packetBuffer[43]);  
     epoch = highWord << 16 | lowWord;
     epoch = epoch - 2208988800 + timeZoneOffset;
     flag=1;
     setTime(epoch);
     ntpLastUpdate = now();
   }
   return flag;
}
 
// Do not alter this function, it is used by the system
unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;                  
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket();
}
 
// Clock display of the time and date (Basic)
/*
void clockDisplay(){
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}
 
// Utility function for clock display: prints preceding colon and leading 0
void printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
*/

