//
// Created by dan on 02.01.2020.
//

#include "LogoRecognizer.h"


int main(int, char *[]) {
    // load logo image
    LogoRecognizer citi_recognizer = LogoRecognizer("../data/citi12.jpg");

    citi_recognizer.run(true);

    return 0;
}
