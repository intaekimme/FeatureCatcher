#pragma once
#include "VideoInspector.h"
#include "DataManager.h"

class FeatureCatcher {

public:
	FeatureCatcher();
	~FeatureCatcher();

private:
	cv::Mat frame;
	cv::VideoCapture cap;
	short frameForProcess;
	short processInterval; //∞Ì¡§

	VideoInspector* m_videoInspector;


public:
	FeatureCatcher(std::string videoName, short processInterval);
	int process();

};