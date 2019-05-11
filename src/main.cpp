#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "M5StackUpdater.h"
#include "Chord.h"
#include "Scale.h"

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"
#define DEVICE_NAME "M5Chord"

BLEServer *pServer;
BLEAdvertising *pAdvertising;
BLECharacteristic *pCharacteristic;

bool isConnected = false;

std::vector<uint8_t> playingNotes;
Scale scale = Scale(0);

uint8_t midiPacket[] = {
  0x80,  // header
  0x80,  // timestamp, not implemented
  0x00,  // status
  0x3c,  // 0x3c == 60 == middle c
  0x00   // velocity
};

int8_t getBatteryLevel()
{
  Wire.beginTransmission(0x75);
  Wire.write(0x78);
  if (Wire.endTransmission(false) == 0
   && Wire.requestFrom(0x75, 1)) {
    switch (Wire.read() & 0xF0) {
    case 0xE0: return 25;
    case 0xC0: return 50;
    case 0x80: return 75;
    case 0x00: return 100;
    default: return 0;
    }
  }
  return -1;
}

void sendNote(bool isNoteOn, int noteNo, int vel) {
  midiPacket[2] = isNoteOn ? 0x90 : 0x80; // note on/off, channel 0
  midiPacket[3] = noteNo;
  midiPacket[4] = vel;
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
  pCharacteristic->notify();
}

void sendNotes(bool isNoteOn, std::vector<uint8_t> notes, int vel) {
  if(isNoteOn) {
    for(uint8_t n : notes) {
      sendNote(isNoteOn, n, vel);
    }
    playingNotes.insert(playingNotes.end(),notes.begin(),notes.end());
  }
  else {
    for(uint8_t n : playingNotes) {
      sendNote(false, n, 0);
    }
    playingNotes.clear();
  }
}

void playChord(Chord chord) {
  sendNotes(true,chord.toMidiNoteNumbers(64,16),120);
  M5.Lcd.setTextSize(4);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextSize(5);
  M5.Lcd.drawString(chord.toString(), 160, 120, 2);
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(10,0);
      M5.Lcd.printf("BLE MIDI Connected.");
      isConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(10,0);
      M5.Lcd.printf("BLE MIDI Disconnected.");
      isConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
  }
};

void setup() {
  M5.begin();
  Wire.begin();

  playingNotes = std::vector<uint8_t>();//仮

  //SD Updater
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  //バッテリー残量の表示
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Battery Level: ");
  M5.Lcd.print(getBatteryLevel());
  M5.Lcd.print("%\n\n");
  M5.Lcd.println("BLE MIDI");
  M5.Lcd.println("Press the button A\nto power off.");

  Serial.begin(115200);

  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));
  pCharacteristic = pService->createCharacteristic(
    BLEUUID(MIDI_CHARACTERISTIC_UUID),
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE_NR
  );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04);
  oAdvertisementData.setCompleteServices(BLEUUID(MIDI_SERVICE_UUID));
  oAdvertisementData.setName(DEVICE_NAME);
  pAdvertising = pServer->getAdvertising();
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->start();
}

Chord CM7 = scale.degreeToChord(0,0,Chord(Chord::C,Chord::MajorSeventh));
Chord FM7 = scale.degreeToChord(3,0,Chord(Chord::C,Chord::MajorSeventh));
Chord G7 =  scale.degreeToChord(4,0,Chord(Chord::C,Chord::Seventh));

void loop() {
  if(!isConnected && M5.BtnA.wasPressed()) M5.Power.deepSleep();
  if(M5.BtnA.wasPressed())  playChord(CM7);
  if(M5.BtnA.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
  if(M5.BtnB.wasPressed())  playChord(FM7);
  if(M5.BtnB.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
  if(M5.BtnC.wasPressed())  playChord(G7);
  if(M5.BtnC.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
  M5.update();
}