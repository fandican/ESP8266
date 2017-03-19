#include <ESP8266WiFi.h>
#include "Gsender.h"

// digital pin D6 has a door sensor attached to it. Give it a name:
int pushButton = D3;
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

#pragma region Globals
const char* ssid = "WIFINAME";                           // WIFI network name
const char* password = "WIFIpassword";                  // WIFI network password
uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 10000;             // If not connected wait time to try again
#pragma endregion Globals

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 3)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 2) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT_PULLUP); //**** CHANGED
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  if (buttonState != lastButtonState) {
    
    // print out the state of the button:
    if(buttonState==1){
      digitalWrite(D4, HIGH);    // turn the LED off by making the voltage LOW
      Serial.println("WINDOW/DOOR IS OPENED");  
      delay(200);  

      // Email send
        Serial.begin(115200);
        connection_state = WiFiConnect();
        if(!connection_state)  // if not connected to WIFI
            Awaits();          // constantly trying to connect
  
        Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
        String subject = "Window/Door Opened";

        if(gsender->Subject(subject)->Send("Gmail@gmail.com", "Window or Door opened!!")) {
            Serial.println("Message send.");
        } else {
            Serial.print("Error sending message: ");
            Serial.println(gsender->getError());
        }
        
    }else{
      digitalWrite(D4, LOW);   // turn the LED on (HIGH is the voltage level)
      Serial.println("WINDOW/DOOR IS CLOSED");  

    }
    
  }
   //for next time through the loop
  lastButtonState = buttonState;
  
  delay(10);        // delay in between reads for stability
}


