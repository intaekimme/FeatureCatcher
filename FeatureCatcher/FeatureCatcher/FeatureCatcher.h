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
	std::string logFileName;
	std::string frameCount;
	std::string millisec;
	std::string yourWebServerPath;

	VideoInspector* m_videoInspector;
	DataManager* m_dataManager;

	// DB info
	std::string server;
	unsigned int port;
	std::string user;
	std::string pw;
	std::string database;
	std::string table;

	int framePerSaving;

public:
	FeatureCatcher(std::string videoName, short processInterval, std::string logFileName, std::string yourWebServerPath, 
		std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table, int framePerSaving);
	int process();
	void setInfo_DB(std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table);
	

};