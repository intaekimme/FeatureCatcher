#include <iostream>
#include "FeatureCatcher.h"

int main() {
	FeatureCatcher FC("video_test2.mp4", 1, "videoLog.txt", "C:/Bitnami/wampstack-7.4.8-0/apache2/htdocs/0903_2",
		"localhost", 3306, "root", "1111", "video_detector", "table_videoanalyzing", 10, true);
	//FC.setInfo_DB("localhost", 3306, "root", "1111", "video_detector", "table_videoanalyzing");
	FC.process();
}