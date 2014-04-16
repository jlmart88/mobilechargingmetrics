/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0xBA, 0x72 };
IPAddress ip(18,247,1,85);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

EthernetUDP Udp;

IPAddress timeServer(206,246,122,250 ); // time.nist.gov NTP server

unsigned int localPort = 2390;      // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  Udp.begin(localPort);
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  //client.println("Refresh: 10");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<script src=\"https://raw.github.com/nnnick/Chart.js/master/Chart.js\"></script>");
          client.println("<canvas id=\"myChart\" width=\"400\" height=\"400\"></canvas>");
          client.println("<script>");
          client.println("var data = {labels :[\"January\",\"February\",\"March\",\"April\",\"May\",\"June\",\"July\"],datasets : [{fillColor : \"rgba(220,220,220,0.5)\",strokeColor : \"rgba(220,220,220,1)\",pointColor : \"rgba(220,220,220,1)\",pointStrokeColor : \"#fff\",data : [65,59,90,81,56,55,40]},{fillColor : \"rgba(151,187,205,0.5)\",strokeColor : \"rgba(151,187,205,1)\",pointColor : \"rgba(151,187,205,1)\",pointStrokeColor : \"#fff\",data : [28,48,40,19,96,27,100]}]}");
          client.println("var ctx = document.getElementById(\"myChart\").getContext(\"2d\");var myNewChart = new Chart(ctx).Line(data,{animation:false});");
          client.println("</script><br>");

          
          
          
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


