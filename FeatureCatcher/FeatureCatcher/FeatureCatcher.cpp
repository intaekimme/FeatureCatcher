// FeatureCatcher.cpp 
#include "FeatureCatcher.h"



FeatureCatcher::FeatureCatcher(std::string videoName, short processInterval, std::string logFileName, std::string yourWebServerPath, 
    std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table, int framePerSaving)
{
    // open video file.
    cap.open(videoName);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }

    // VideoInspector Object 생성.
    m_videoInspector = new VideoInspector();
    m_videoInspector->loadModels();

    // DataManager Object 생성.
    m_dataManager = new DataManager();
    FeatureCatcher::logFileName = logFileName;
    setInfo_DB(server, port, user, pw, database, table);
    FeatureCatcher::framePerSaving = framePerSaving;

    // 몇 프레임 당 프로세스를 처리할 것인지 설정.
    this->processInterval = processInterval;
    frameForProcess = 1;

    // Web Server Path 설정.
    FeatureCatcher::yourWebServerPath = yourWebServerPath;
}

FeatureCatcher::~FeatureCatcher() {
    delete m_videoInspector;
    delete m_dataManager;
}

int FeatureCatcher::process() {

    // open log file.
    m_dataManager->openLogFile(logFileName);
    m_videoInspector->setFramePerSaving(framePerSaving);

    while (true)
    {
        cap.read(frame);
        if (frame.empty())
        {
            break;
        }

        if (--frameForProcess == 0) {
            // 몇 프레임마다 처리할 것인지 결정
            frameForProcess = processInterval;            
            millisec = std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC)));       //set millisec.
            frameCount = std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_FRAMES)));     // set frame count.
            m_videoInspector->process(frame, *m_dataManager, frameCount, millisec, yourWebServerPath, framePerSaving);
            
        }

        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }
    
    // save log to DB
    m_dataManager->saveLog2DB(FeatureCatcher::logFileName, server, port, user, pw, database, table);

    return 0;
}

void FeatureCatcher::setInfo_DB(std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table)
{
    FeatureCatcher::server = server;
    FeatureCatcher::port = port;
    FeatureCatcher::user = user;
    FeatureCatcher::pw = pw;
    FeatureCatcher::database = database;
    FeatureCatcher::table = table;

    m_dataManager->setInfo_DB(server, port, user, pw, database, table);
}