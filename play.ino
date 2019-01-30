// ---------- Welcome! To ugranny custom ---------- //
// ---------- Code by Carsten Thue-Bludworth ------ //

// comment this out to turn off all Serial communication
//#define COMS
// comment this out to turn on/off free sram per second
//#define RAM_COMS

#include "mg2HW.h"
#include "Sample.h"
#include <WaveHC.h>
#ifdef COMS
#include <WaveUtil.h>
#endif
// strcpy used when changing sample names
#include <string.h>


#define MIDI_CHANNEL 1

// Object to control the UI
mg2HW hw;

// Object to control the SD card, WAV files
SdReader card;
FatVolume vol;
FatReader root;
FatReader file;

// Sample objects .. correspond to each of the big buttons
Sample S1("A0.WAV");
Sample S2("A1.WAV");
Sample S3("A2.WAV");
Sample S4("A3.WAV");
Sample S5("A4.WAV");
Sample S6("A5.WAV");
// TODO: Make sample for each button selectable!
Sample samples[NUMBER_OF_BIG_BUTTONS] = {
    S1,
    S2,
    S3,
    S4,
    S5,
    S6
};

// Object to control the player
WaveHC wave;

void setup() {
    
    // see midi.ino
    initMidi();

    #ifdef COMS
    Serial.begin(9600);
    putstring_nl("Welcome to uGranny custom...");
    putstring("S1 name: ");
    Serial.println(S1.getName());
    #endif
    // Init UI hardware
    hw.initialize();
    hw.displayText("test");
    // SD Card Initialization
    if (!card.init()) {
        error("card");
    }
    // enable optimized read (from WaveHC/example/daphc.ino)
    card.partialBlockRead(true);
    // FAT volume initialization
    // assumes only one partition
    if (!vol.init(card, 1)) {
        error("vol");
    }

    #ifdef COMS
    putstring("Found FAT");
    Serial.print(vol.fatType(), DEC);
    putstring_nl(" volume! :)");
    #endif

    // Root directory initialization
    if (!root.openRoot(vol)) {
        error("root");
    }
    #ifdef COMS
    putstring_nl("Successfully opened root!");
    #endif
}

// variable to hold the most recently played sample;
uint8_t cur_sample = 0;
// variable to hold the loop state
bool lup = false;

uint32_t t1 = millis();
void loop() {
    // Handle midi messages
    receiveREX();
    // turn off the leds if no sample is playing
    if (!wave.isplaying) {
        resetBBLeds();
        hw.displayText("    ");
    }
    // read the loop button
    if (hw.justPressed(HOLD)) {
        lup = !lup;
        hw.setLed(HOLD, lup);
        #ifdef COMS
        Serial.println(lup);
        #endif
    }

    hw.update();

    // code block to run once per second
    uint32_t nowtime = millis();
    if (nowtime - t1 > 1000) {
        #ifdef COMS
        #ifdef RAM_COMS
        Serial.println(FreeRam());
        #endif
        #endif
        // code here
        t1 = nowtime;
    }

    receiveREX();
    scanBigButtons();
    adjustSampleRate();
    adjustStartEndPos();
    receiveREX();

    // if you hold the button down and press up / down
    // select the next sample!
    if (hw.buttonState(bigButton[cur_sample])) {
        char cur_name[7];
        strcpy(cur_name, samples[cur_sample].getName());
        if (hw.justPressed(UP)) {
            if (wave.isplaying) wave.stop();
            incSampleName(cur_name);            
            while (!file.open(root, cur_name)) {
                incSampleName(cur_name);
                hw.displayText("srch");
                hw.updateDisplay();
            }
            samples[cur_sample].setName(cur_name);
            playSample(cur_sample);
        } else if (hw.justPressed(DOWN)) {
            if (wave.isplaying) wave.stop();
            decSampleName(cur_name);            
            while (!file.open(root, cur_name)) {
                decSampleName(cur_name);
                hw.displayText("srch");
                hw.updateDisplay();
            }
            samples[cur_sample].setName(cur_name);
            playSample(cur_sample);
            #ifdef COMS
            Serial.println(cur_name);
            #endif
        }
    }

    receiveREX();
    // stop the sample if it's past the current sample end point.
    // TODO: does this belong somewhere else?
    if (wave.isplaying) {
        uint32_t true_ep = map(samples[cur_sample].ep, 0, 1023, 0, wave.getSize());
        if (wave.getCurPosition() >= true_ep) {
            wave.stop();
            if (lup) playSample(cur_sample);
        }
    }

}

// increments the sample name according to the typography
// uses Arduino built ins isDigit(), isAlpha(), and isUpperCase();
void incSampleName(char n[7]) {
    n[1]++;
    if (isDigit(n[1])) {
        return;
    } else {
        n[1] = '0';
    }
    n[0]++;
    if (isAlpha(n[0]) && isUpperCase(n[0])) {
        return;
    } else {
        n[0] = 'A';
        n[1] = '0';
    }
}

// decrements the sample name according to the typography
void decSampleName(char n[7]) {
    n[1]--;
    if (isDigit(n[1])) {
        return;
    } else {
        n[1] = '9';
    }
    n[0]--;
    if (isAlpha(n[0]) && isUpperCase(n[0])) {
        return;
    } else {
        n[0] = 'Z';
        n[1] = '9';
    }
}
// reads second and third knobs to update start and end positions of sample.
void adjustStartEndPos() {
    uint16_t _sp, _ep, spdiff, epdiff;
    _sp = hw.knobValue(1)^1023;
    _ep = hw.knobValue(2)^1023;
    spdiff = abs(samples[cur_sample].sp - _sp);
    epdiff = abs(samples[cur_sample].ep - _ep);
    if (spdiff < 10) {
        hw.unfreezeKnob(1);
        hw.setLed(KNOB_LED_2, HIGH);
    }
    if (!hw.knobFreezed(1) && spdiff > 10) {
        samples[cur_sample].sp = _sp;
    }
    if (epdiff < 10) {
        hw.unfreezeKnob(2);
        hw.setLed(KNOB_LED_3, HIGH);
    }
    if (!hw.knobFreezed(2) && epdiff > 10) {
        samples[cur_sample].ep = _ep;
    }
}

void adjustSampleRate() {
    // TODO: put this in a 'knob2SampleRate()' function?
    uint16_t _sr = hw.knobValue(0);
    _sr = ((_sr ^ 1023) << 5) + 1000;
    uint16_t diff = abs(samples[cur_sample].sr - _sr);
    // If the knob is withing a certain threshold of the current
    // sample rate, then activate it! The knob is frozen again
    // when a NEW sample is played.
    // TODO: put this threshold in some #define statement
    if (diff < 60) {
        hw.unfreezeKnob(0);
        hw.setLed(KNOB_LED_1, HIGH);
    }
    if (!hw.knobFreezed(0) && diff > 50) {
        #ifdef COMS
        putstring_nl("new sr");
        #endif
        samples[cur_sample].sr = _sr;
        if (wave.isplaying) {
            wave.setSampleRate(_sr);
        }
    }
}

// Scan the big buttons for presses and do the stuff
void scanBigButtons() {
    for (uint8_t i = 0; i < NUMBER_OF_BIG_BUTTONS; i++) {
        if(hw.justPressed(bigButton[i])) {
            playSample(i);
            // update cur_sample
            cur_sample = i;
        }
    }
}

// play a sample with proper rate and effects
// input s is the sample number to play (corresponds to big buttons)
void playSample(uint8_t s) {
    #ifdef COMS
    putstring_nl("playing sample ");
    Serial.println(s);
    #endif
    // if a sample is currently playing, stop it
    if (wave.isplaying) {
        wave.stop();
        resetBBLeds();
    }
    // try to open a file by name
    if (!file.open(root, samples[s].getName())) {
        error("file");
    }
    // now try to play it!
    if (!wave.create(file)) {
        return;
    }

    // scale the start position to the sample size
    uint32_t sp = map(samples[s].sp, 0, 1023, 0, wave.getSize());
    wave.seek(sp);

    // begin playback
    wave.play();
    // indicate playback
    hw.setLed(bigButton[s], HIGH);

    // if a new sample is selected, freeze all knobs
    if (s != cur_sample) {
        resetKnobs();
    }

    // set the sample rate
    #ifdef COMS
    putstring("setting sr...");
    Serial.println(samples[s].sr);
    #endif
    wave.setSampleRate(samples[s].sr);

    showSampleName(samples[s]);
}

void resetKnobs() {
    hw.freezeAllKnobs();
    for (uint8_t i = 0; i < NUMBER_OF_KNOBS; i++) {
        hw.setLed(knobLed[i], LOW);
    }
}

void resetBBLeds() {
    for (uint8_t i = 0; i < NUMBER_OF_BIG_BUTTONS; i++) {
        hw.setLed(bigButton[i], LOW);    
    }
}

// Display str and hang
void error(char* str) {
    hw.displayText(str);
    while(1) {
        hw.updateDisplay();
    }
}

// Put the sample name on the display
void showSampleName(Sample s) {
    char sn[] = "s   ";
    sn[2] = s.getName()[0];
    sn[3] = s.getName()[1];
    hw.displayText(sn);
}
