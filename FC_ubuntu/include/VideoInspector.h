#ifndef VIDEOINSPECTOR_H
#define VIDEOINSPECTOR_H

#include <stdio.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

class VideoInspector {
public:
    VideoInspector();
    ~VideoInspector();

    int loadModels();
    int process(cv::Mat& frame,  // DataManager& dataManager,
                std::string frameCount, std::string millisec,
                std::string yourWebServerPath, int& framePerSaving);
    void visualize(cv::Mat& frame);

    void setFramePerSaving(int frame_per_saving);

private:
    void clearVars();

    int detectPeople(cv::Mat& frame);
    int detectFace(cv::Mat& personFrame, cv::Rect person);
    int recognizeAgeGender(cv::Mat& frame, cv::Rect face);

    int findColor(cv::Mat& personFrame, cv::Rect person);
    int identifyPeople(cv::Mat& personframe);

    std::vector<std::vector<float>> globalReIdVec;

    template <typename T>
    float cosineSimilarity(const std::vector<T>& vecA,
                           const std::vector<T>& vecB);
    uint findMatchingPerson(const std::vector<float>& newReIdVec);

    cv::dnn::Net personDetector;
    cv::dnn::Net personIdentifier;
    cv::dnn::Net faceDetector;
    cv::dnn::Net attributesRecognizer;
    cv::dnn::Net ageGenderRecognizer;

    int framePerSaving;

public:
    std::vector<cv::Rect> people;
    std::vector<cv::Rect> faces;
    std::vector<std::pair<std::string, std::string>> ageGender;
    std::vector<std::string> peopleId;
    std::vector<std::pair<cv::Point, cv::Point>> clothesPoint;
    std::vector<std::pair<cv::Vec3b, cv::Vec3b>> clothesColor;
};

#endif