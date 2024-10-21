//Controller:

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16x2 LCD (I2C address could vary)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16 columns, 2 rows

int blinds_opennes = 0;
int target_light = 0 ;


// Wi-Fi credentials
const char* ssid = "IoT-AP";
const char* password = "MySecurePassword";

// UDP settings
WiFiUDP udp;
const char* udpAddress = "192.168.199.103";  // IP address of the Actuator (receiver)
const int udpPort = 3333;                     // Port to send and receive data

// Define Potentiometer pin
#define POT_PIN 34  // Analog pin connected to potentiometer

// Define the I2C pins for the ESP32 WROOM module
#define I2C_SDA 21  // SDA pin (data line)
#define I2C_SCL 22  // SCL pin (clock line)

char incomingPacket[255];  // Buffer for incoming packets

void setup() {  
  // Start the serial communication
  Serial.begin(115200);

 // Initialize I2C communication with custom SDA and SCL pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the LCD with the correct number of columns and rows 
  lcd.begin();  // 16 columns and 2 rows
  lcd.backlight();   // Turn on the LCD backlight

  // Initialize the LCD
  //lcd.init();
  // Turn on the backlight
  //lcd.backlight();


  // Display a welcome message  

  // Prepare the display for variable data
//  lcd.setCursor(0, 1); // Set the cursor to the first column, second row
//  lcd.print("Connecting to WiFi");  

  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  

  // Wait until the ESP32 connects to the Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Print the IP address of the controller
  Serial.print("Controller IP Address: ");
  Serial.println(WiFi.localIP());


  lcd.setCursor(0, 0);  // Set cursor to the first row, first column
  lcd.print("Target: ");
  lcd.setCursor(10, 0);  // Set cursor to the first row, 8th column
  lcd.print(target_light);

  lcd.setCursor(0, 1);  // Set cursor to the first row, first column
  lcd.print("BL % ");
  lcd.setCursor(6, 1);  // Set cursor to the first row, 8th column
  lcd.print(blinds_opennes);




  // Start the UDP connection
  udp.begin(udpPort);
}

void loop() {
  // Read the value from the potentiometer
  int potValue = analogRead(POT_PIN); 

  // Check if there's any UDP packet available from the actuator (LDR value and Servo Position)
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read the incoming packet (LDR value and Servo Position from actuator)
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;  // Null-terminate the string
      String receivedMessage = String(incomingPacket);

      // Print the received message for debugging
      Serial.print("Received message: ");
      Serial.println(receivedMessage); // Add this line to see the incoming message

      // Check if the received message is an LDR value
      if (receivedMessage.startsWith("LDR:")) {
        int ldrValue = receivedMessage.substring(4).toInt();  // Extract the LDR value
        Serial.print("LDR Value (from Actuator): ");
        Serial.println(ldrValue);
        target_light= map(ldrValue, 4000, 600, 0, 100);

        lcd.clear();       // Clear the display

        //lcd.setCursor(0, 0);  // Set cursor to the first row, first column
        //lcd.print("Target: ");
        //lcd.setCursor(10, 0);  // Set cursor to the first row, 8th column
        //lcd.print(target_light);

        //lcd.setCursor(0, 0);
        //lcd.print("Target: " +String(target_light));        
      }
      // Check if the received message is a Servo Position
      else if (receivedMessage.startsWith("SERVO:")) {
        int servoPosition = receivedMessage.substring(6).toInt();  // Extract the Servo position
        Serial.print("Servo Position (from Actuator): ");
        Serial.println(servoPosition);

        lcd.clear();
        blinds_opennes = map(servoPosition, 160, 10, 0, 100);

        //lcd.setCursor(0, 1);  // Set cursor to the first row, first column
        //lcd.print("BL % ");
        //lcd.setCursor(6, 1);  // Set cursor to the first row, 8th column
        //lcd.print(blinds_opennes);


//        lcd.setCursor(0, 1);
//        lcd.print("Blinds Per.: " +String(blinds_opennes));
      }
    }
  }
 // Send the potentiometer value to the actuator every 2 seconds
  String message = "POT:" + String(potValue);  // Prefix with "POT:" to differentiate
  udp.beginPacket(udpAddress, udpPort);        // Prepare to send packet
  udp.print(message);                           // Send the potentiometer value
  udp.endPacket();                              // Send the packet

  // Print the potentiometer value locally on the controller
  Serial.print("Potentiometer Value (Controller): ");
  Serial.println(potValue);
  delay(1000);  // Delay for 2 seconds before repeating
}
