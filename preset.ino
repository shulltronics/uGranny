// A preset is an ordered collection of 6 samples and their parameters.
// There can be 100 presets stored in files named "Pxx.TXT" on the SD card.

// grab the last preset name from EEPROM and put it in the global
// presetName variable:
void grabLastPreset() {
    for (uint8_t add = 0; add < 8; add++) {
        presetName[add] = EEPROM.read(add);
    }
}

// update the sample objects for the preset:
void restorePreset(char* name) {
    if (!file.open(root, name)) {
        error("prst");
    } else {
        file.close();
    }
}
