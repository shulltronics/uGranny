#define NOTE_ON_WORD 0x09
#define NOTE_OFF_WORD 0x08
#define CC_WORD 0x0B

void initMidi() {
    Serial.begin(31250);
}

void receiveREX() {
    if (Serial.available() > 2) {
        uint8_t no = Serial.read();
        uint8_t nn = Serial.read();
        uint8_t nv = Serial.read();
        if (no >> 4 == NOTE_ON_WORD) playSample(nn % 6);
    }
}
