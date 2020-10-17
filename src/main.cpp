#include <iostream>

#include "FeatureCatcher.h"

int main() {
    FeatureCatcher FC("resource/video_test5.mp4", 1,
                      "temp",
                      "localhost", 3306, "root", "", "video_detector",
                      "testtwo", 10, true);

    FC.process();
}