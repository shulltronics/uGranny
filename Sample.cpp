#include "Sample.h"
#include <string.h>

// constructor
Sample::Sample(char _name[7]) {
    strcpy(name, _name);
    sr = 22050;
}

char* Sample::getName() {
    return name;
}

void Sample::setName(char _name[7]) {
    strcpy(name, _name);
}
