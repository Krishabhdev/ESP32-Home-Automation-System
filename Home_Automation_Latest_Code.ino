#include <DHT.h>
#include <TM1637Display.h>
#include <IRremote.hpp>
#include <BluetoothSerial.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BT_STATUS_LED    2   // LED indicator for Bluetooth connection
#define DHT_PIN          4  // DHT11 sensor connected to GPIO 27
#define DHT_TYPE         DHT11
#define TM1637_CLK       18  // TM1637 clock pin
#define TM1637_DIO       5  // TM1637 data pin
#define IR_RECEIVE_PIN   15  // IR receiver pin
#define RELAY_1_PIN     23   // Relay 1
#define RELAY_2_PIN     22   // Relay 2
#define RELAY_3_PIN     19  // Relay 3
#define RELAY_4_PIN     21  // Relay 4

// Segment encoding for "O", "N", and "F"
const uint8_t SEG_O = 0x3F;  // O
const uint8_t SEG_N = 0x37;  // N (Approximated)
const uint8_t SEG_f = 0x71;  // F

DHT dht(DHT_PIN, DHT_TYPE);
TM1637Display display(TM1637_CLK, TM1637_DIO);
BluetoothSerial SerialBT;

// Define custom symbols
uint8_t degreeSymbol = 0x63;  // "°"
uint8_t celsiusSymbol = 0x39; // "C"
uint8_t percentSymbol1 = 0x76; // "%"
uint8_t percentSymbol2 = 0x1C; // "%"

bool sensorActive = true; // Flag to control sensor activity
bool coolerStatus = false;
bool hasRun = false;

void readDHTAndDisplayTask(void *parameter) {
    while (1) {
        if(sensorActive){
            // Read temperature and humidity
            float temperature = dht.readTemperature();
            float humidity = dht.readHumidity();
        
          if (!isnan(temperature) && !isnan(humidity)) {
            int tempToDisplay = (int)temperature;
            int humidityToDisplay = (int)humidity;
            display.clear();
            display.showNumberDec(tempToDisplay, false, 2, 0); // Show "25"
            display.setSegments(&degreeSymbol, 1, 2); // "°" at position 2
            display.setSegments(&celsiusSymbol, 1, 3); // "C" at position 3
            delay(2000);
                if(sensorActive){
                       display.clear();  
                       display.showNumberDec(humidity, false, 2, 0); // Show "75"
                       display.setSegments(&percentSymbol1, 1, 2); // "H" at position 2
                       display.setSegments(&percentSymbol2, 1, 3); // "U" at position 3
                       }
                       delay(2000);
            // ✅ Print on Bluetooth Serial Monitor (Each on a new line)
            SerialBT.print("Temperature: ");
            SerialBT.print(temperature);
            SerialBT.print(" °C\n");   // ✅ Newline for Bluetooth Serial
            SerialBT.print("Humidity: ");
            SerialBT.print(humidity);
            SerialBT.println(" %\n");  // ✅ Ensure newline after humidity
            }

            // Check if temperature is less than 25°C then turn off Relay 1
            if(coolerStatus){
            // //to blink led when auto mode is on
            //      SerialBT.print("auto on print ");
            //      digitalWrite(BT_STATUS_LED, HIGH);
            //      delay(100);
            //      digitalWrite(BT_STATUS_LED, LOW);
            //      delay(100);
               if(temperature > 29){
                digitalWrite(RELAY_2_PIN, HIGH);
                }
                else if(temperature <= 25){
                digitalWrite(RELAY_2_PIN, LOW);
                } 
            } 
        }
        
        vTaskDelay(1 / portTICK_PERIOD_MS);  // Delay before reading again  
    }
}

void setup() {
  //------------------------------------------------------------------------//
    Serial.begin(115200);
    display.setBrightness(1);
    display.clear(); 
    // Set TM1637 display brightness
    uint8_t HELO[] = { 0x76, 0x79, 0x38, 0x3F };  // "H", "E", "L", "O"
    display.setSegments(HELO);
    delay(1000);
    SerialBT.begin("Krishabh_Home_Automation"); // Bluetooth name
    dht.begin(); // Initialize DHT sensor
    
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    
    pinMode(BT_STATUS_LED, OUTPUT);
    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);
    pinMode(RELAY_3_PIN, OUTPUT);
    pinMode(RELAY_4_PIN, OUTPUT);

    digitalWrite(RELAY_1_PIN, LOW);
    digitalWrite(RELAY_2_PIN, LOW);
    digitalWrite(RELAY_3_PIN, LOW);
    digitalWrite(RELAY_4_PIN, LOW);
    
    // Create FreeRTOS tasks on specific cores
    xTaskCreatePinnedToCore(readDHTAndDisplayTask, "DHT_Display_Task", 2048, NULL, 1, NULL, 1);  // Core 1
   
}

void loop() {
    //--------------------------------------------------------------------//
    //Turn on led when bluetooth is connected
    digitalWrite(BT_STATUS_LED, SerialBT.hasClient() ? HIGH : LOW);
     // IR Remote Control
        if (IrReceiver.decode()) {
            //Serial.println(IrReceiver.decodedIRData.command, HEX);
            handleIR(IrReceiver.decodedIRData.command);
            IrReceiver.resume();
        }
        
        // Bluetooth Control
        if (SerialBT.available()) {
            char command = SerialBT.read();
            handleBluetooth(command);
            // To turn ON IR on command 7
            irON(command);
            //to automate cooler
            autoCool(command);
        }
         
        //delay(200);  // Small delay for responsiveness
}

void handleIR(uint8_t command) {
    switch (command) {
        
        case 0x51: // IR button for Relay 1
        showDevice1();
        break;

        case 0x16: // IR button for Relay 2
        showDevice2();
        break;  

        case 0xF: // IR button for Relay 3
        showDevice3(); 
        break;

        // case 0xC: // IR button for Relay 4
        // showDevice4();
        // break;
        
        // case 0x12: resetESP32(); 
        // break;   // IR button for Reset Action
    }
}

void handleBluetooth(char command) {
    switch (command) {
        
        case '1':
        showDevice1();
        break;

        case '2':
        showDevice2();
        break;

        case '3':
        showDevice3(); 
        break;

        case '4': 
        showDevice4();
        break;

        case '5': 
        resetESP32();
        break;

        case '6': 
        sensorActive = !sensorActive;
        IrReceiver.stop();  // Disable IR receiver
        display.clear();
        // Show "IR OFF" on TM1637
        uint8_t IROFF[] = { 0x06, 0x50, 0x3F, 0x71 };  // "I", "R", "O", "FF"
        display.setSegments(IROFF);
        SerialBT.println("IR Receiver OFF");
        delay(1000);
        sensorActive = true;
        break;

    }
}

void toggleRelay(int relayPin) {
    digitalWrite(relayPin, !digitalRead(relayPin));
    if(digitalRead(relayPin)){
       digitalWrite(BT_STATUS_LED, HIGH);
       delay(300);
       digitalWrite(BT_STATUS_LED, LOW);
       sensorActive = !sensorActive;
       display.clear();
       // Show "ON" on TM1637
       uint8_t onDisplay[] = { SEG_O, SEG_N, 0x00, 0x00 };
       display.setSegments(onDisplay);
       SerialBT.println("ON");
       delay(1000);  
    }
    else{
       digitalWrite(BT_STATUS_LED, HIGH);
       delay(100);
       digitalWrite(BT_STATUS_LED, LOW);
       delay(100);
       digitalWrite(BT_STATUS_LED, HIGH);
       delay(100);
       digitalWrite(BT_STATUS_LED, LOW);
       sensorActive = !sensorActive;
       display.clear();
        // Show "OFF" on TM1637 
       uint8_t onDisplay[] = { SEG_O, SEG_f, SEG_f, 0x00 };
       display.setSegments(onDisplay);
       SerialBT.println("OFF");
       delay(1000);
    }
}

void resetESP32() {
    //To show bluetooth serial monitor
    SerialBT.println("Resetting ESP32...");
    display.clear();
    // Custom segment data for "RESt"
    uint8_t RESET[] = { 0x50, 0x79, 0x6D, 0x78 };  // "R", "E", "S", "T"
    display.setSegments(RESET);
    // Display a message to indicate reset
    delay(1000);
    // Reset the ESP32
    ESP.restart();  // This will restart the ESP32
}

void showDevice1(){
        toggleRelay(RELAY_1_PIN);
        display.clear();
        // Custom segment data for "TV" (with blank spaces at the end)
        uint8_t TV[] = { 0x78, 0x3E, 0x00, 0x00 };  // "T", "V", blank, blank
        display.setSegments(TV);
        SerialBT.println("TV");
        delay(1000);
        sensorActive = true;
}

void showDevice2(){
        toggleRelay(RELAY_2_PIN);
        display.clear();
        // Custom segment data for "COOL"
        uint8_t COOL[] = { 0x39, 0x3F, 0x3F, 0x38 };  // "C", "O", "O", "L"
        display.setSegments(COOL);
        SerialBT.println("Cooler");
        delay(1000);
        sensorActive = true;
}

void showDevice3(){
        toggleRelay(RELAY_3_PIN);
        display.clear();
        uint8_t MUSI[] = { 0x37, 0x3E, 0x6D, 0x06 };  // "M", "U", "S", "I"
        display.setSegments(MUSI);
        SerialBT.println("Music System");
        delay(1000);
        sensorActive = true;
}

void showDevice4(){
        toggleRelay(RELAY_4_PIN);
        display.clear();
        uint8_t LIGH[] = { 0x38, 0x06, 0x3D, 0x76 };  // "L", "I", "G", "H"
        display.setSegments(LIGH);
        SerialBT.println("Light");
        delay(1000);
        sensorActive = true;
}

void irON(char command){
  if(command == '7'){
              sensorActive = !sensorActive;
              IrReceiver.start();  // Turn ON IR Receiver
              digitalWrite(BT_STATUS_LED, HIGH);
              delay(500);
              digitalWrite(BT_STATUS_LED, LOW);
              display.clear();
              //Show "IR ON" on TM1637
              uint8_t IRON[] = { 0x06, 0x50, 0x3F, 0x54 };  // "I", "R", "O", "N"
              display.setSegments(IRON);
              SerialBT.println("IR Receiver ON");
              delay(1000);
              sensorActive = true;
            }
}

void autoCool(char command){
    if(command == '8'){
    coolerStatus = !coolerStatus;
    
            if(coolerStatus){
                sensorActive = !sensorActive;
                display.clear();
                // Segment bytes for "A", "T", "O", "N"
                uint8_t ATON[] = {
                0x77, // A
                0x78, // T
                0x3F, // O
                0x37  // N (approximated)
                };
                display.setSegments(ATON);
                SerialBT.println("Cooler Automation ON");
                delay(1000);
                sensorActive = true;
                }else{
                    sensorActive = !sensorActive;
                    display.clear();
                    // Segment byte codes for "A", "T", "O", "F"
                    uint8_t ATOF[] = {
                    0x77, // A
                    0x78, // T
                    0x3F, // O
                    0x71  // F
                    };
                    display.setSegments(ATOF);
                    SerialBT.println("Cooler Automation OFF");
                    delay(1000);
                    sensorActive = true;
                }
    }
}
