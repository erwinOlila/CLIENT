/*
The program for client devices in server/client architecture of field layer
devices.

This program applies to 2LI, 1AC and 2AC in which the configuration can be made
in the MyCommonFun.h header file.

The clients constantly listens to the server for any incoming and fetching of
message. When a message is avaliable the client.available fires TRUE and the
message is read per byte. In return, the load status is sent in the same
connection by client.print
*/

#include <ESP8266WiFi.h>
#include <MyCommonFun.h>

#define IND 2 // Pin assignement for indeicator GPIO02

char messageFromAP[] = "0000"; // Array that handles load control message
unsigned long timeOut = 0;

int starter = 0;

WiFiClient client; // Instance for client

void setup() {

  Serial.begin(115200);
  pinMode(IND, OUTPUT);
  pinMode(LOADCON, OUTPUT);
  pinMode(LOADSTA, INPUT);
  digitalWrite(IND, LOW); // Turn ON LED if before connection is established

  WiFi.mode(WIFI_STA); // Station mode
  conBuffer(ssid, pass, IND);

  Serial.println("");
  Serial.println("Connected: ");
}

void loop() {
  char messageToAP[] = "0000"; // Array that handles the message for the server
  int c = 0;

  // If connection is interrupted, reconnect
  if (WiFi.status() != WL_CONNECTED) {
    conBuffer(ssid, pass, IND);
  }

  // Insert the load type at the beggining of message
  strcpy(messageToAP, loadType);
  if(digitalRead(LOADSTA) == 1) {
    strcat(messageToAP, "1");
  }
  else {
    strcat(messageToAP, "0");
  }
/**************************Continued on the next page**************************/
  // If the load is assigned for air conditioning units, add this function.
  // This funcition modifies messageToAP to indicate whether to turn on or off
  // 1LI or 2LI
  #ifndef LIGHTS
    int illuminance = analogRead(A0);
    if (illuminance >= 100) {
      strcat(messageToAP, dark);
    }
    else {
      strcat(messageToAP, light);
    }
    Serial.println(messageToAP);
  #endif

  client.stop();
  client.connect(server, PORT);
  Serial.println(messageToAP);
  client.print(messageToAP);
  while(client.connected()){

    // IF readable data is available
    if (client.available()) {
      Serial.println("reading...");
      while(client.available() > 0) {
        char* m = messageFromAP;
        *(m + c) = client.read(); // Read data per byte

        // Check only the part of the message assigned
        if (c == LOADNUM) {
          if (*(m + c) == '1') {
            digitalWrite(LOADCON, HIGH);
          }
          else {
            digitalWrite(LOADCON, LOW);
          }
        }
        c++;
      }
      Serial.println(messageFromAP);
    }
  }

  digitalWrite(IND, HIGH); // Turn off LED when connection is finished

  delay(100);
}
/*************************************End**************************************/
