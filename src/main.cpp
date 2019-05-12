#include <M5Stack.h>
#include <M5TreeView.h>
#include "M5StackUpdater.h"
#include "BLEMidi.h"
#include "Chord.h"
#include "Scale.h"

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"
#define DEVICE_NAME "M5Chord"

std::vector<uint8_t> playingNotes;
Scale scale = Scale(0);

M5TreeView tv;
typedef std::vector<MenuItem*> vmi;

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

  //終了処理
  switch(currentScene) {

  }

  //開始処理
  switch(scene) {
    case Scene::Connection:
      M5.Lcd.setCursor(0, 48);
      M5.Lcd.setTextSize(4);
      M5.Lcd.println("BLE MIDI\n");
      M5.Lcd.setTextSize(1);
      M5.Lcd.println("Advertising...");
      M5.Lcd.println("Press the button A to power off.");
    break;
    case Scene::Play:
      M5.Lcd.setCursor(0,0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println(scale.key + scale.currentScale->name);
    break;
    case Scene::Function:
      tv.begin();
    break;
  }
  currentScene = scene;
}

void sendNotes(bool isNoteOn, std::vector<uint8_t> notes, int vel) {
  if(isNoteOn) {
    for(uint8_t n : notes) {
      Midi.sendNote(0x90, n, vel);
    }
    playingNotes.insert(playingNotes.end(),notes.begin(),notes.end());
  }
  else {
    for(uint8_t n : playingNotes) {
      Midi.sendNote(0x80, n, 0);
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

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Midi.isConnected = true;
      changeScene(Scene::Play);
    };

    void onDisconnect(BLEServer* pServer) {
      Midi.isConnected = false;
      changeScene(Scene::Connection);
    }
};

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
  }
};

void callBackExitFunction(MenuItem* sender)
{
  changeScene(Scene::Play);
}

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

  Midi.begin(DEVICE_NAME, MIDI_SERVICE_UUID, MIDI_CHARACTERISTIC_UUID,
    new ServerCallbacks(), new CharacteristicCallbacks());

  //FunctionMenu
  tv.setItems(vmi{
    new MenuItem("ahoge"),
    new MenuItem("ahoge"),
    new MenuItem("exit", callBackExitFunction)
  });
  M5ButtonDrawer::width = 106;
  tv.clientRect.x = 2;
  tv.clientRect.y = 16;
  tv.clientRect.w = 196;
  tv.clientRect.h = 200;
  tv.itemWidth = 176;
  tv.useFACES       = true;
  tv.useCardKB      = true;
  tv.useJoyStick    = true;
  tv.usePLUSEncoder = true;
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
      if(M5.BtnC.pressedFor(1000)) {changeScene(Scene::Function); break;}
      if(M5.BtnA.wasPressed())  playChord(CM7);
      if(M5.BtnA.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      if(M5.BtnB.wasPressed())  playChord(FM7);
      if(M5.BtnB.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      if(M5.BtnC.wasPressed())  playChord(G7);
      if(M5.BtnC.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
    break;
    case Scene::Function:
      tv.update();
    break;
  }
}