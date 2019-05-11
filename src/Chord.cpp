#include <Chord.h>

Chord::Chord()
: Chord(C,0) {}

Chord::Chord(uint8_t root, uint16_t option)
: root(root) , option(option) {}

String Chord::toString() {
    String str = "";
    str += rootStrings[root];
    for(auto item : optionStrings) {
        if(option & item.first) {
            str += item.second;
        }
    }
    return str;
}

std::vector<uint8_t> Chord::toMidiNoteNumbers(uint8_t centerNoteNo, uint8_t region) {
    maxNoteNo = centerNoteNo + region/2;
    minNoteNo = centerNoteNo - region/2;
    uint8_t baseNoteNo = root;
    while(baseNoteNo < minNoteNo) baseNoteNo += 12;

    std::vector<uint8_t> notes = std::vector<uint8_t>();

    addMidiNote(&notes, baseNoteNo);

    //Third
    if(option & Sus4) addMidiNote(&notes, baseNoteNo + 6);
    else if(option & Minor) addMidiNote(&notes, baseNoteNo + 3);
    else addMidiNote(&notes, baseNoteNo + 4); //Major

    //Fifth
    if(option & FifthFlat) addMidiNote(&notes, baseNoteNo + 6);
    else addMidiNote(&notes, baseNoteNo + 7);

    //Seventh
    if(option & Seventh) addMidiNote(&notes, baseNoteNo + 10);
    if(option & MajorSeventh) addMidiNote(&notes, baseNoteNo + 11);

    return notes;
}

void Chord::addMidiNote(std::vector<uint8_t>* notes,uint8_t noteNo) {
    while(noteNo > maxNoteNo) noteNo -= 12;
    while(noteNo < minNoteNo) noteNo += 12;
    notes->push_back(noteNo);
}