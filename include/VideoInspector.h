#ifndef VIDEOINSPECTOR_H
#define VIDEOINSPECTOR_H

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "ThreadPool.h"
#include "DataManager.h"

class VideoInspector {
public:
    VideoInspector(int thread_num);
    ~VideoInspector();

    int loadModels();
    int process(cv::Mat& frame,   DataManager& dataManager,
                std::string frameCount, std::string millisec,
                std::string yourWebServerPath, int& framePerSaving);
    void visualize(cv::Mat& frame);

    void setFramePerSaving(int frame_per_saving);

private:
    void clearVars();

    void beforeProcess();
    void processOnePerson(cv::Mat& frame, int idx);

    int detectPeople(cv::Mat& frame);
    int detectFace(cv::Mat& personFrame, cv::Rect person, int idx);
    int recognizeAgeGender(cv::Mat& frame, cv::Rect face, int idx);

    int findColor(cv::Mat& personFrame, cv::Rect person, int idx);
    int identifyPeople(cv::Mat& personframe, int idx);

    std::vector<std::vector<float>> globalReIdVec;

    template <typename T>
    float cosineSimilarity(const std::vector<T>& vecA, const std::vector<T>& vecB);
    uint findMatchingPerson(const std::vector<float>& newReIdVec);

    std::unique_ptr<cv::dnn::Net> personDetector;
    std::vector<std::unique_ptr<cv::dnn::Net>> personIdentifier;
    std::vector<std::unique_ptr<cv::dnn::Net>> faceDetector;
    std::vector<std::unique_ptr<cv::dnn::Net>> attributesRecognizer;
    std::vector<std::unique_ptr<cv::dnn::Net>> ageGenderRecognizer;

    int framePerSaving;

    int threadNum;
    std::unique_ptr<ThreadPool::ThreadPool> pool;

public:
    std::vector<cv::Rect> people;
    std::vector<cv::Rect> faces;
    std::vector<std::pair<std::string, std::string>> ageGender;
    std::vector<std::string> peopleId;
    std::vector<std::pair<cv::Point, cv::Point>> clothesPoint;
    std::vector<std::pair<cv::Vec3b, cv::Vec3b>> clothesColor;

    std::mutex mInit;
    std::mutex mFace;
    std::mutex mAgeGender;
    std::mutex mId;
    std::mutex mGlobalId;
    std::mutex mPoint;
    std::mutex mColor;
};

#endif