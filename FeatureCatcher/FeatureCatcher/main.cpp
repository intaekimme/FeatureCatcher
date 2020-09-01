#include <iostream>
#include "FeatureCatcher.h"

int main() {
	FeatureCatcher FC("video_test.mp4", 1);
	FC.process();
}