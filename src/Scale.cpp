#include "Scale.h"
#include "Chord.h"

std::vector<std::shared_ptr<ScaleBase>> Scale::availableScales;

Scale::Scale(uint8_t key) : key(key) {
    currentScale = getAvailableScales()[0].get();
}

Scale::Scale() : key(0) {}

String Scale::toString() {
    return Chord::rootStrings[key] + " " + currentScale->name();
}

Chord Scale::degreeToChord(uint8_t degree, uint8_t offset, Chord base) {
    return currentScale->degreeToChord(key,degree,offset,base);
}

std::vector<std::shared_ptr<ScaleBase>> Scale::getAvailableScales() {
    if(availableScales.empty()) {
        availableScales.push_back(std::make_shared<MajorScale>());
        availableScales.push_back(std::make_shared<MinorScale>());
    }
    return availableScales;
}

Chord MajorScale::degreeToChord(uint8_t key, uint8_t degree, uint8_t offset, Chord base) {
    base.root = key + pitch[degree];
    base.root += offset;

    //0から12の範囲内に収める
    while(base.root < 0)  base.root += 12;
    while(base.root >= 12)  base.root -= 12;

    return base;
}

Chord MinorScale::degreeToChord(uint8_t key, uint8_t degree, uint8_t offset, Chord base) {
    base.root = key + pitch[degree];
    base.root += offset;

    //0から12の範囲内に収める
    while(base.root < 0)  base.root += 12;
    while(base.root >= 12)  base.root -= 12;

    return base;
}