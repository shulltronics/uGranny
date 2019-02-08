// A preset is an ordered collection of 6 samples and their parameters.
// Because I couldn't get the SdFat library working (using another 512 byte buffer...
//   ran out of space), presets are stored in EEPROM.

// the address in EEPROM where sample presets begin
#define PRESET_STORAGE_START 2
// address 0 currently for last open preset
#define LAST_PRESET_ADDR 1

// TODO: make this section accurate
/* EEPROM architecture is like this, where p = 0 - 9
    | bytes 0 - 7 ---> Last opened preset name
    | bytes (n*preset+1) - (n*preset+1 + sizeof(Sample)) ---> preset p parameters
*/

// grab the last preset name from EEPROM and put it in the global
// presetName variable:
uint8_t grabLastPreset() {
    return EEPROM.read(LAST_PRESET_ADDR);
}

// update the currently open preset in EEPROM
void updateLastPreset(uint8_t ps) {
    EEPROM.update(LAST_PRESET_ADDR, ps);
}

// update the sample objects for the preset:
void savePreset(uint8_t preset) {
    #ifdef COMS
    putstring_nl("saving preset..");
    #endif
    char* save = "save";
    save[3] = String(preset, INT).charAt(0);
    show4while(save);
    // starting address for EEPROM put
    uint16_t add = (preset*6*sizeof(Sample)) + PRESET_STORAGE_START;
    for (uint8_t s = 0; s < NUMBER_OF_BIG_BUTTONS; s++) {
        //uint8_t pb[] = samples[s].dumpParams();
        EEPROM.put(add, samples[s]);
        add += sizeof(Sample);
    }
}

// restore the preset
void restorePreset(uint8_t preset) {
    hw.freezeAllKnobs();
    #ifdef COMS
    putstring_nl("restoring preset..");
    #endif
    char* prst = "pr  ";
    prst[3] = String(preset, INT).charAt(0);
    show4while(prst);
    uint16_t add = (preset*6*sizeof(Sample)) + PRESET_STORAGE_START;
    for (uint8_t s = 0; s < NUMBER_OF_BIG_BUTTONS; s++) {
        EEPROM.get(add, samples[s]);
        add += sizeof(Sample);
    }
}
