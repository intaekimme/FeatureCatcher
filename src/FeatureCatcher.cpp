// FeatureCatcher.cpp
#include "FeatureCatcher.h"

//video name, processInterval, yourWebServerPath, MySQL server, MySQL port, MySQL user, MySQL pw, MySQL database name, MySQL table name, framePerSaving, visualizing data on video
FeatureCatcher::FeatureCatcher(std::string videoName, short processInterval,
                               std::string yourWebServerPath,
                               std::string server, unsigned int port,
                               std::string user, std::string pw,
                               std::string database, std::string table,
                               int framePerSaving, bool visualizing) {
    // open video file.
    cap.open(videoName);
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
    }

    // VideoInspector Object 생성.
    m_videoInspector = new VideoInspector(4);
    m_videoInspector->loadModels();

    // DataManager Object 생성.
    m_dataManager = new DataManager();

    // !!set table name!! //
    setInfo_DB(server, port, user, pw, database, table);
    FeatureCatcher::framePerSaving = framePerSaving;

    // 몇 프레임 당 프로세스를 처리할 것인지 설정.
    this->processInterval = processInterval;
    frameForProcess = 1;

    // 그림 표시
    this->visualizing = visualizing;

    // Web Server Path 설정.
    FeatureCatcher::yourWebServerPath = yourWebServerPath;
}

FeatureCatcher::~FeatureCatcher() {
    delete m_videoInspector;
    // delete m_dataManager;
}

int FeatureCatcher::process() {
    // open log file.
    // m_dataManager->openLogFile(logFileName);
    m_videoInspector->setFramePerSaving(framePerSaving);

    while (true) {
        cap.read(frame);
        if (frame.empty()) {
            break;
        }

        if (--frameForProcess == 0) {
            // 몇 프레임마다 처리할 것인지 결정
            frameForProcess = processInterval;
            millisec = std::to_string(
                cvRound(cap.get(cv::CAP_PROP_POS_MSEC)));  // set millisec.
            frameCount = std::to_string(
                cvRound(cap.get(cv::CAP_PROP_POS_FRAMES)));  // set frame count.
            m_videoInspector->process(frame, *m_dataManager,
                                      frameCount, millisec, yourWebServerPath,
                                      framePerSaving);
        }

        if (this->visualizing) m_videoInspector->visualize(frame);
        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0) break;
    }

    // save log to DB
    // m_dataManager->saveLog2DB(FeatureCatcher::logFileName, server, port,
    // user,
    //                           pw, database, table);

    //close DB
    m_dataManager->CloseDB();

    return 0;
}

void FeatureCatcher::setInfo_DB(std::string server, unsigned int port,
                                std::string user, std::string pw,
                                std::string database, std::string table) {
    FeatureCatcher::server = server;
    FeatureCatcher::port = port;
    FeatureCatcher::user = user;
    FeatureCatcher::pw = pw;
    FeatureCatcher::database = database;
    FeatureCatcher::table = table;

    // set DB Info
    m_dataManager->setInfo_DB(server, port, user, pw, database, table);

    // connect to DB(MySQL)
    m_dataManager->ConnectDB(server, port, user, pw, database);

    // create a table according to the parameter 'table'
    m_dataManager->createTable(table);
}