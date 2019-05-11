#include "Scale.h"
#include "Chord.h"

std::vector<ScaleBase> Scale::availableScales = {MajorScale(), MinorScale()};

Scale::Scale() {
    currentScale = &availableScales[0];
}

String Scale::toString() {
    return key + currentScale->name;
}

Chord Scale::degreeToChord(uint8_t degree, uint8_t offset, Chord base) {
    return currentScale->degreeToChord(key,degree,offset,base);
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