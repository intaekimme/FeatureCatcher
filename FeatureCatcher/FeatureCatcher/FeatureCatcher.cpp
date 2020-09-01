// FeatureCatcher.cpp 
#include "FeatureCatcher.h"



FeatureCatcher::FeatureCatcher(std::string videoName, short processInterval)
{
    cap.open(videoName);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }

    m_videoInspector = new VideoInspector();
    m_videoInspector->loadModels();

    this->processInterval = processInterval;
    frameForProcess = 1;
}

FeatureCatcher::~FeatureCatcher() {
    delete m_videoInspector;
}

int FeatureCatcher::process() {



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
            m_videoInspector->process(frame);
        }

        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }
    
    //
}
