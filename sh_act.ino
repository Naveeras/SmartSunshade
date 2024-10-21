//Actuator

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "IoT-AP";
const char* password = "MySecurePassword";

// UDP settings
WiFiUDP udp;
const char* udpAddress = "192.168.199.144";  // IP address of the Controller (receiver)
const int udpPort = 3333;                   // Port to send and receive data

// Define the LDR pin and Servo pin
#define LDR_PIN 34    // Analog pin connected to LDR
#define SERVO_PIN 5   // Define the pin for Servo

// Create a Servo object
Servo myServo;

char incomingPacket[255];  // Buffer for incoming packets

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Initialize the LDR pin as input
  pinMode(LDR_PIN, INPUT);
  myServo.attach(SERVO_PIN);  // Attach the Servo to the defined pin

  // Connect to Wi-Fi network
  Serial.print("Connecting to WiFi network ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait until the ESP32 connects to the Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nESP32 is Connected to Wi-Fi");

  // Print the IP address of the actuator
  Serial.print("Actuator IP Address: ");
  Serial.println(WiFi.localIP());

  // Start listening on the UDP port
  udp.begin(udpPort);
}

void loop() {
  // Read the analog value from the LDR
  int ldrValue = analogRead(LDR_PIN);  

  // Check if there's any UDP packet available from the controller (Potentiometer value)
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read the incoming packet (Potentiometer value from controller)
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;  // Null-terminate the string
      String receivedMessage = String(incomingPacket);
      
      // Check if the received message is a Potentiometer value
      if (receivedMessage.startsWith("POT:")) {
        int potValue = receivedMessage.substring(4).toInt();  // Extract the Potentiometer value
        Serial.print("Potentiometer Value (from Controller): ");
        Serial.println(potValue);

        // Determine servo position based on LDR and Potentiometer values
        if (potValue >= 3500) {
          myServo.write(0); // Turn off the servo
          Serial.println("BLinds Off, Night Senerio");
        } 
        
        else {

            if (potValue >= 0 && potValue <= 1200) {
              Serial.println("System is in Slow response state");
              if (ldrValue >= 0 && ldrValue <= 1000) {
                myServo.write(160);  
                Serial.println("Servo turned to 160 degrees.");
            } else if (ldrValue >= 1001 && ldrValue <= 3000) {
                myServo.write(140);   
                Serial.println("Servo turned to 140 degrees.");
            } else if (ldrValue >= 3001 && ldrValue <= 5000) {
                myServo.write(120);   
                Serial.println("Servo turned to 120 degrees.");
            } else if (ldrValue >= 5001 && ldrValue <= 6000) {
                myServo.write(100);   
                Serial.println("Servo turned to 100 degrees.");
            } else if (ldrValue >= 6001 && ldrValue <= 8200) {
                myServo.write(80);   
                Serial.println("Servo turned to 80 degrees.");
            } else {
                Serial.println("LDR value out of range for servo control.");
            }
            }


          else if (potValue >= 1201 && potValue <= 2400) {
              Serial.println("System is in Medium Response State");
              if (ldrValue >= 0 && ldrValue <= 1000) {
                myServo.write(160);  
                Serial.println("Servo turned to 160 degrees.");
            } else if (ldrValue >= 1001 && ldrValue <= 3000) {
                myServo.write(130);   
                Serial.println("Servo turned to 130 degrees.");
            } else if (ldrValue >= 3001 && ldrValue <= 5000) {
                myServo.write(100);   
                Serial.println("Servo turned to 100 degrees.");
            } else if (ldrValue >= 5001 && ldrValue <= 6000) {
                myServo.write(70);   
                Serial.println("Servo turned to 70 degrees.");
            } else if (ldrValue >= 6001 && ldrValue <= 8200) {
                myServo.write(40);   
                Serial.println("Servo turned to 40 degrees.");
            } else {
                Serial.println("LDR value out of range for servo control.");
            }
            }


          else if (potValue >= 2401 && potValue <= 3499) {
            Serial.println("System is in Fast response state");
            if (ldrValue >= 0 && ldrValue <= 1000) {
              myServo.write(160);  
              Serial.println("Servo turned to 160 degrees.");
          } else if (ldrValue >= 1001 && ldrValue <= 3000) {
              myServo.write(120);   
              Serial.println("Servo turned to 120 degrees.");
          } else if (ldrValue >= 3001 && ldrValue <= 5000) {
              myServo.write(80);   
              Serial.println("Servo turned to 80 degrees.");
          } else if (ldrValue >= 5001 && ldrValue <= 6000) {
              myServo.write(40);   
              Serial.println("Servo turned to 40 degrees.");
          } else if (ldrValue >= 6001 && ldrValue <= 8200) {
              myServo.write(10);  
              Serial.println("Servo turned to 10 degrees.");
          } else {
              Serial.println("LDR value out of range for servo control.");
          }
        }
      }
    }
  }
  }
 // Send the LDR value and Servo position to the controller every 2 seconds
  String message = "LDR:" + String(ldrValue) + " SERVO:" + String(myServo.read()); // Include SERVO position
  udp.beginPacket(udpAddress, udpPort);        // Prepare to send packet
  udp.print(message);                           // Send the LDR and Servo position values
  udp.endPacket();                              // Send the packet

  // Print the LDR value locally on the actuator
  Serial.print("LDR Value (Actuator): ");
  Serial.println(ldrValue);

  // Print current state
  Serial.print("Servo Position: ");
  Serial.println(myServo.read());
  
  delay(1000);  // Delay for 2 seconds before repeating
}
