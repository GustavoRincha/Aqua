#include <WiFi.h>
#include <Servo.h>

static const int servoPin =13;

Servo s; 
int pos; 

const char* ssid = "Claudio";
const char* password = "422812ee";

WiFiServer server(80);

String header;

String output26State = "close";

unsigned long currentTime = millis();

unsigned long previousTime = 0; 

const long timeoutTime = 2000;

void setup() {

  s.attach(servoPin);
  s.write(0);
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   

  if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          
    String currentLine = "";               
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {           
        char c = client.read();           
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                   
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            

            if (header.indexOf("GET /open") >= 0) {
                for(pos = 0; pos < 90; pos++)
                  {
                    s.write(pos);
                  delay(15);
                  }
              Serial.println("ABERTURA ABERTA");
              output26State = "open";
            } else if (header.indexOf("GET /close") >= 0) {
              for(pos = 90; pos >= 0; pos--)
                  {
                    s.write(pos);
                    delay(15);
                  }
              Serial.println("ABERTURA FECHADA");
              output26State = "close";
            } 
            

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            

            client.println("<body><h1>Alimentador de Paixes</h1>");

            client.println("<p>ALIMENTADOR - State " + output26State + "</p>");
     
            if (output26State=="close") {
              client.println("<p><a href=\"/open\"><button class=\"button\">OPEN</button></a></p>");
            } else {
              client.println("<p><a href=\"/close\"><button class=\"button button2\">CLOSE</button></a></p>");
            } 

            client.println("</body></html>");
            

            client.println();

            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }

    header = "";

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}