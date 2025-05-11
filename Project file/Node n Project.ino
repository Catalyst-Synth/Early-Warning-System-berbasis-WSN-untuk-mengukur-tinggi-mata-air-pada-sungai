// N2 com 9

#include <SPI.h>
#include <RF24.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

RF24 radio(4, 15);  // CE pin 9, CSN pin 10
char buffer[10];
// Shared address for all nodes
const byte broadcastAddress[6] = "BROAD";

// Unique identifier for each node (set uniquely for each)
const char nodeID[] = "N3"; // Set to "Node1", "Node2", or "Node3" for each node

// Unique transmit delay for each node to prevent collisions
unsigned int transmitDelay;  // Set differently for each node

long duration;
int distance;

const int trigPin = 2;
const int echoPin = 12;
const float PanjangTiang = 30.0;
const float DalamSungai = 5.5;



void setup() {
  Serial.begin(9600);
  
  // Initialize the radio
  lcd.init();                      // initialize the lcd 
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
//  radio.begin();
//  radio.setPALevel(RF24_PA_HIGH);
//  radio.setDataRate(RF24_1MBPS);
//
//  // Set up the shared address
//  radio.openReadingPipe(1, broadcastAddress);  
//  radio.openWritingPipe(broadcastAddress);     
//
//  // Start in listening mode
//  radio.startListening();
//
//  // Define unique delay based on node (adjust for each node manually)
//  transmitDelay = 1000;  // Node-specific delay in ms (1000 for Node1, 1200 for Node2, 1400 for Node3)
}

void loop() {
  // ----------------- Read Sensor -----------------
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034 / 2;
  int TMA = (DalamSungai - (distance - PanjangTiang))- 0.5;
  dtostrf(TMA, 6, 1, buffer);
   

  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("N3:");
  Serial.print("Distance: ");
  Serial.println(distance);
  lcd.print(TMA);
   lcd.print("cm");


 // ----------------- NRF -----------------
  radio.stopListening();  // Enter transmit mode
  
  char message[32];
  snprintf(message, sizeof(message), "%s:%d",nodeID,TMA);

  // Send message and check if it was successful
  bool success = radio.write(&message, sizeof(message));
  if (success) {
    Serial.print("Sent: ");
    Serial.println(message);
  } else {
    Serial.println("Failed to send message.");
  }

  delay(transmitDelay + random(0, 200));  // Unique delay + small random delay

  // ----------------- Listen for Data -----------------
  radio.startListening();  // Enter receive mode

  // Listen for 3 seconds
  unsigned long startTime = millis();
  while (millis() - startTime < 3000) { 
    if (radio.available()) {
      char receivedData[32] = "";
      radio.read(&receivedData, sizeof(receivedData));
      
      
      Serial.print("Received: ");
      Serial.println(receivedData);

      if (strstr(receivedData, "N1") != NULL) {
      Serial.println("Message is from Node1");
      lcd.setCursor(8, 0);  // Display on the second line of the LCD
      lcd.print(receivedData);
      lcd.print("cm");
    }
    // Check if the message is from Node3
    else if (strstr(receivedData, "N2") != NULL) {
      Serial.println("Message is from Node3");
      lcd.setCursor(0, 1);  // Display on the second line of the LCD
      lcd.print(receivedData);
    }
    // Otherwise, assume it's an unknown node or invalid message
    else {
      Serial.println("Unknown sender");
      lcd.setCursor(0, 1);  // Display on the second line of the LCD
      lcd.print("Unknown: ");
      lcd.print(receivedData);
    }
    }
  }
    delay(500);
  lcd.clear();


  
} 
