#ifndef Sample_h
#define Sample_h

#include <stdint.h>

class Sample {
public:

    // sample name
    char name[7];
    // volume
    uint8_t vol;
    // Samplerate
    uint16_t sr;
    // Sample start and end positions
    uint16_t sp = 0;
    uint16_t ep = 1023; // In the main code I scale this to the sample size

// MEMBER FUNCTIONS
    Sample(char _name[7]); // constructor
    char* getName();
    void setName(char _name[7]);
};

#endif // Sample_h
