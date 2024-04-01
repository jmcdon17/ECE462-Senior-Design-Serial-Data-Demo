#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ctime>  // Include ctime library

#define CSV_BUFFER_SIZE 1024
#define LOG_LIMIT 50

BLECharacteristic *pCharacteristic;  // Declare BLE characteristic globally

int waterLevel = 3;  // Declare and initialize waterLevel

// Function to generate random float within a specified range
float randomFloat(float min, float max) {
  return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Function to generate random integer within a specified range
int randomInt(int min, int max) {
  return rand() % (max - min + 1) + min;
}

// Function to update water level
int updateWaterLevel(int currentLevel) {
  if (currentLevel == 0) return 3;
  else return currentLevel - 1;
}

// Callback class for handling BLE server events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Device disconnected");
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize Bluetooth
  BLEDevice::init("ESP32Test");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BLEUUID((uint16_t)0x180F));
  pCharacteristic = pService->createCharacteristic(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ);
  pCharacteristic->setValue("Bluetooth connection successful");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // Set up callbacks
  pServer->setCallbacks(new MyServerCallbacks());
}

void loop() {
  // Generate random data
  char csvBuffer[CSV_BUFFER_SIZE];

  time_t now = time(0);  // Get current time
  struct tm *timeinfo = localtime(&now);
  char buffer[80];

  strftime(buffer, 80, "%d/%m/%Y %H:%M", timeinfo);

  sprintf(csvBuffer, "%s,%0.2f,%0.2f,%d,%0.2f", buffer, randomFloat(20.0, 30.0), randomFloat(40.0, 60.0), updateWaterLevel(waterLevel), randomFloat(0.0, 100.0));

  // Print to Serial
  Serial.println(csvBuffer);

  // Update water level
  waterLevel = updateWaterLevel(waterLevel);

  // Send over Bluetooth
  pCharacteristic->setValue(csvBuffer);
  pCharacteristic->notify();

  delay(1000);  // Add delay in ms
}
