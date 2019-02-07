#define NOTE_ON_WORD 0x09
#define NOTE_OFF_WORD 0x08
#define CC_WORD 0x0B

void initMidi() {
    Serial.begin(31250);
}

void scanMidiMode() {
    // flip the midi mode state when the page button is pressed
    if (hw.justPressed(PAGE)) midiMode = !midiMode;
}

void receiveREX() {
    if (Serial.available() >= 63) Serial.flush();
    if (Serial.available() > 2) {
        uint8_t no = Serial.read();
        uint8_t nn = Serial.read();
        uint8_t nv = Serial.read();
        bool noteOff = (no >> 4 == NOTE_OFF_WORD) || 
                       (nn >> 4 == NOTE_OFF_WORD) ||
                       (nv >> 4 == NOTE_OFF_WORD);
        if (no >> 4 == NOTE_ON_WORD) playSample(nn % 6);
    }
}
