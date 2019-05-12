#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <M5TreeView.h>
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

//画面
enum Scene : uint8_t {
  Connection,
  Play,
  Function,
  length
};

Scene currentScene = Scene::length; //初回changeSceneにて正しく描画するためConnection以外を指定

void changeScene(Scene scene) {
  if(currentScene == scene) return;
  M5.Lcd.clear();
  switch(scene) {
    case Scene::Connection:
      M5.Lcd.setCursor(0, 48);
      M5.Lcd.setTextSize(4);
      M5.Lcd.println("BLE MIDI\n");
      M5.Lcd.setTextSize(1);
      M5.Lcd.println("Press the button A to power off.");
    break;
    case Scene::Play:
      M5.Lcd.setCursor(0,0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println(scale.key + scale.currentScale->name);
    break;
    case Scene::Function:

    break;
  }
  currentScene = scene;
}

uint8_t midiPacket[] = {
  0x80,  // header
  0x80,  // timestamp, not implemented
  0x00,  // status
  0x3c,  // 0x3c == 60 == middle c
  0x00   // velocity
};

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
      isConnected = true;
      changeScene(Scene::Play);
    };

    void onDisconnect(BLEServer* pServer) {
      isConnected = false;
      changeScene(Scene::Connection);
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
  }
};

void setup() {
  M5.begin();
  Wire.begin();

  //SD Updater
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  changeScene(Scene::Connection);

  Serial.begin(115200);

  //TODO: BLEMidi.begin();
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
  switch(currentScene) {
    case Scene::Connection:
      M5.update();
      if(M5.BtnA.wasPressed()) M5.Power.deepSleep();
    break;
    case Scene::Play:
      M5.update();
      if(M5.BtnA.wasPressed())  playChord(CM7);
      if(M5.BtnA.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      if(M5.BtnB.wasPressed())  playChord(FM7);
      if(M5.BtnB.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      if(M5.BtnC.wasPressed())  playChord(G7);
      if(M5.BtnC.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
    break;
    case Scene::Function:

    break;
  }
}