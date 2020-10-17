#ifndef FEATURECATCHER_H
#define FEATURECATCHER_H

#include "VideoInspector.h"

class FeatureCatcher {
public:
    FeatureCatcher();
    ~FeatureCatcher();

private:
    cv::Mat frame;
    cv::VideoCapture cap;
    short frameForProcess;
    short processInterval;  //����
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
    bool visualizing;

public:
    //video name, processInterval, yourWebServerPath, MySQL server, MySQL port, MySQL user, MySQL pw, MySQL database name, MySQL table name, framePerSaving, visualizing data on video
    FeatureCatcher(std::string videoName, short processInterval, std::string yourWebServerPath,
                   std::string server, unsigned int port, std::string user,
                   std::string pw, std::string database, std::string table,
                   int framePerSaving, bool visualzing);
    int process();
    void setInfo_DB(std::string server, unsigned int port, std::string user,
                    std::string pw, std::string database, std::string table);
};

#endif