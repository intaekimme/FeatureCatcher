#pragma once
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <time.h>

#include "DataManager.h"

class VideoInspector {
public:
	VideoInspector();
	~VideoInspector();

	int loadModels();
	int process(cv::Mat& frame, DataManager& dataManager, std::string frameCount, std::string millisec, std::string yourWebServerPath, int& framePerSaving);
	
private:
	void clearVars();

	int detectPeople(cv::Mat& frame4);
	int detectFace(cv::Mat& personFrame, cv::Rect person);
	int recognizeAgeGender(cv::Mat& frame, cv::Rect face);

	int findColor(cv::Mat& personFrame, cv::Rect person);
	int identifyPeople(cv::Mat& personframe);	

	std::vector<std::vector<float>> globalReIdVec;

	template <typename T>
	float cosineSimilarity(const std::vector<T>& vecA, const std::vector<T>& vecB);
	uint findMatchingPerson(const std::vector<float>& newReIdVec);

	cv::dnn::Net personDetector;
	cv::dnn::Net personIdentifier;
	cv::dnn::Net faceDetector;
	cv::dnn::Net attributesRecognizer;
	cv::dnn::Net ageGenderRecognizer;

	DataManager* m_dataManager;
	int framePerSaving;

public:
	void setFramePerSaving(int frame_per_saving);

	std::vector<cv::Rect> people;
	std::vector<cv::Rect> faces;
	std::vector<std::pair<std::string, std::string>> ageGender;
	std::vector<std::string> peopleId;
	std::vector<std::pair<cv::Point, cv::Point>> clothesPoint;
	std::vector<std::pair<cv::Vec3b, cv::Vec3b>> clothesColor;	
	
};