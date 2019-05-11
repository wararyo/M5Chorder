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