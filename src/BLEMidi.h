#ifndef _BLEMIDI_H_
#define _BLEMIDI_H_

#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

class BLEMidi {
public:
    BLEServer *server;
    BLEAdvertising *advertising;
    BLECharacteristic *characteristic;

    std::string deviceName;
    std::string midiServiceUUID;
    std::string midiCharacteristicUUID;

    bool isConnected = false;

    void begin(std::string deviceName,
        std::string midiServiceUUID,
        std::string midiCharacteristicUUID,
        BLEServerCallbacks *serverCallback,
        BLECharacteristicCallbacks *chatacteristicCallback);

    void sendNote(uint8_t status, uint8_t noteNo, uint8_t vel);
};

extern BLEMidi Midi;

#endif