#ifndef _CHORD_H_
#define _CHORD_H_

#include <stdint.h>
#include <vector>
#include <functional>
#include "MidiNote.h"

class Chord {
public:
    //Root
    const uint8_t C      = 0;
    const uint8_t CSharp = 1;
    const uint8_t D      = 2;
    const uint8_t DSharp = 3;
    const uint8_t E      = 4;
    const uint8_t F      = 5;
    const uint8_t FSharp = 6;
    const uint8_t G      = 7;
    const uint8_t GSharp = 8;
    const uint8_t A      = 9;
    const uint8_t ASharp = 10;
    const uint8_t B      = 11;

    //Third, Fifth, and Tension
    const uint16_t Major   = 0b00000000;
    const uint16_t Minor   = 0b00000001;
    const uint16_t Dimish  = 0b00000010;
    const uint16_t Sus4    = 0b00000100;
    const uint16_t Sus2    = 0b00001000;
    const uint16_t Aug     = 0b00010000;

    const uint16_t FifthFlat       = 0b00000001 << 8;
    const uint16_t Ninth           = 0b00000010 << 8;
    const uint16_t NinthSharp      = 0b00000100 << 8;
    const uint16_t Eleventh        = 0b00001000 << 8;
    const uint16_t EleventhSharp   = 0b00010000 << 8;
    const uint16_t Thirteenth      = 0b00100000 << 8;
    const uint16_t ThirteenthSharp = 0b01000000 << 8;
    const uint16_t ThirteenthFlat  = 0b10000000 << 8;

    uint8_t root;
    uint16_t option;

    Chord();
    Chord(uint8_t root, uint8_t option);
    
    std::vector<MidiNote*> toMidiNotes();
    String toString();
};

#endif