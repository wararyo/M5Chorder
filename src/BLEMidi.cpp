#include <BLEMidi.h>

void BLEMidi::begin(std::string deviceName,
    std::string midiServiceUUID,
    std::string midiCharacteristicUUID,
    BLEServerCallbacks *serverCallback,
    BLECharacteristicCallbacks *characteristicCallback) {

    this->deviceName = deviceName;
    this->midiServiceUUID = midiServiceUUID;
    this->midiCharacteristicUUID = midiCharacteristicUUID;

    BLEDevice::init(deviceName);

    //Start Server
    server = BLEDevice::createServer();
    server->setCallbacks(serverCallback);
    BLEService *service = server->createService(BLEUUID(midiServiceUUID));
    characteristic = service->createCharacteristic(
    BLEUUID(midiCharacteristicUUID),
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE_NR
    );
    characteristic->setCallbacks(characteristicCallback);
    service->start();

    //Start Advertising
    BLEAdvertisementData advertisementData = BLEAdvertisementData();
    advertisementData.setFlags(0x04);
    advertisementData.setCompleteServices(BLEUUID(midiServiceUUID));
    advertisementData.setName(deviceName);
    advertising = server->getAdvertising();
    advertising->setAdvertisementData(advertisementData);
    advertising->start();
}

void BLEMidi::sendNote(uint8_t status, uint8_t noteNo, uint8_t vel) {
    uint8_t midiPacket[] = {
        0x80,  // header
        0x80,  // timestamp, not implemented
        0x00,  // status
        0x3c,  // 0x3c == 60 == middle c
        0x00   // velocity
    };
    midiPacket[2] = status; // note on/off, channel 0
    midiPacket[3] = noteNo;
    midiPacket[4] = vel;
    characteristic->setValue(midiPacket, 5); // packet, length in bytes)
    characteristic->notify();
}

BLEMidi Midi;