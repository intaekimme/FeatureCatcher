// VideoInspector.cpp
#include "VideoInspector.h"
#include "models.h"

VideoInspector::VideoInspector() {

}

VideoInspector::~VideoInspector() {

}

// Loading OpenVINO Models
int VideoInspector::loadModels()
{
	// Load the network
	std::cout << "Loading the person detector...\n";
	personDetector = cv::dnn::readNet(personDetectorFile, persondetectorConfigFile);
	if (personDetector.empty())
	{
		std::cerr << "Couldn't load the person detector!\n";
		return -2;
	}
	else
	{
		std::cout << "Done loading the person detector!\n\n";
	}

	std::cout << "Loading the person re-identifier...\n";
	personIdentifier = cv::dnn::readNet(personIdentifierFile, personIdentifierConfigFile);
	if (personIdentifier.empty())
	{
		std::cerr << "Couldn't load the person detector!\n";
		return -2;
	}
	else
	{
		std::cout << "Done loading the person detector!\n\n";
	}

	std::cout << "Loading the face detector...\n";
	faceDetector = cv::dnn::readNet(faceDetectorFile, faceDetectorConfigFile);
	if (faceDetector.empty())
	{
		std::cerr << "Couldn't load the face detector!\n";
		return -2;
	}
	else
	{
		std::cout << "Done loading the face detector!\n\n";
	}

	std::cout << "Loading the face age-gender recogniger...\n";
	ageGenderRecognizer = cv::dnn::readNet(ageGenderRecognizerFile, ageGenderRecognizerConfigFile);
	if (ageGenderRecognizer.empty())
	{
		std::cerr << "Couldn't load the face age-gender recogniger!\n";
		return -2;
	}
	else
	{
		std::cout << "Done loading the face age-gender detector!\n\n";
	}

	std::cout << "Loading the person attributes recognizer...\n";
	attributesRecognizer = cv::dnn::readNet(attributesRecognizerFile, attributesRecognizerConfigFile);
	if (attributesRecognizer.empty())
	{
		std::cerr << "Couldn't load the person attributes recognizer!\n";
		return -2;
	}
	else
	{
		std::cout << "Done loading the person attributes recognizer!\n\n";
	}
}

void VideoInspector::clearVars() {
	people.clear();
	faces.clear();
	ageGender.clear();
	peopleId.clear();
	clothesPoint.clear();
	clothesColor.clear();
}

void VideoInspector::setFramePerSaving(int frame_per_saving)
{
	VideoInspector::framePerSaving = frame_per_saving;
}

int VideoInspector::process(cv::Mat& frame, DataManager& dataManager, std::string frameCount, std::string millisec, std::string yourWebServerPath, int &framePerSaving) {
	clearVars();
	detectPeople(frame);
	
	for (int i = 0; i < people.size(); i++)		// frame 당 people 수 만큼 반복.
	{
		cv::Mat personFrame = frame(people[i]); // 사람 이미지 따로 복사

		detectFace(frame, people[i]);			// set faces
		recognizeAgeGender(frame, faces[i]);	// set ageGender(pair)
		findColor(personFrame, people[i]);		// set topColor, bottomColor, clothesColor(pair)
		identifyPeople(personFrame);			// set peopleId

		//DB 관련 함수 호출
		if (--framePerSaving == 0) {
			framePerSaving = VideoInspector::framePerSaving;
			//image 저장
			dataManager.savePersonImg(frame, peopleId[i], millisec, people[i], yourWebServerPath);
			//log 저장
			dataManager.saveLog(peopleId[i], frameCount, millisec, ageGender[i].first, ageGender[i].second, yourWebServerPath, clothesColor[i].first, clothesColor[i].second);
		}	
	}
	return 0;
}

int VideoInspector::detectPeople(cv::Mat& frame) {
	cv::Mat peopleDetectorInputBlob = cv::dnn::blobFromImage(frame, 0.58, cv::Size(512, 512), cv::Scalar(103.53, 116.28, 123.675));
	personDetector.setInput(peopleDetectorInputBlob, "image");
	cv::Mat peopleDetectorOutput = personDetector.forward("detection_out"); // output은 [1, 1, 200, 7]로 나옴
	cv::Mat peopleDetectorMat(peopleDetectorOutput.size[2], peopleDetectorOutput.size[3], CV_32F, peopleDetectorOutput.ptr<float>()); // 그래서 [200, 7]로 바꿈. 여기서 200은 사람, 7은 속성들

	// for문에서 감지한 각 사람마다 처리
	for (int i = 0; i < peopleDetectorMat.rows; i++)
	{
		//std::string personId = "";
		cv::Rect face = cv::Rect();
		float confidence = peopleDetectorMat.at<float>(i, 2); // (i, 2)는 신뢰도. (i, 0)이랑 (i, 1)은 필요 x
		if (confidence > 0.7f)
		{
			//cout << "confidence : " << confidence << endl << endl;
			int px1 = static_cast<int>(peopleDetectorMat.at<float>(i, 3) * frame.cols);
			int py1 = static_cast<int>(peopleDetectorMat.at<float>(i, 4) * frame.rows);
			int px2 = static_cast<int>(peopleDetectorMat.at<float>(i, 5) * frame.cols);
			int py2 = static_cast<int>(peopleDetectorMat.at<float>(i, 6) * frame.rows); // (i, 3) ~ (i, 6)은 좌표들

			cv::Rect person(cv::Point(px1, py1), cv::Point(px2, py2));
			if (0 > person.x || 0 > person.width || person.x + person.width > frame.cols || 0 > person.y || 0 > person.height || person.y + person.height > frame.rows)
				continue;
			people.push_back(person); // 사람 사각형 저장. rect는 좌표 정보만 저장함(이미지 정보는 안 저장)
		}
	}
	return 0;
}

int VideoInspector::detectFace(cv::Mat& personFrame, cv::Rect person) {
	cv::Rect face = cv::Rect(); // 초기값 --> face.x : 0, face.y : 0

	cv::Mat faceDetectorInputBlob = cv::dnn::blobFromImage(personFrame, 1.0, cv::Size(300, 300), cv::Scalar(103.53, 116.28, 123.675));
	faceDetector.setInput(faceDetectorInputBlob, "data");
	cv::Mat faceDetectorOutput = faceDetector.forward("detection_out"); // 결과는 [1, 1, 200, 7]로 나오며 사람 감지와 비슷
	cv::Mat faceDetectorMat(faceDetectorOutput.size[2], faceDetectorOutput.size[3], CV_32F, faceDetectorOutput.ptr<float>());
	// 감지한 각 얼굴마다 처리
	for (int i = 0; i < faceDetectorMat.rows; i++)
	{
		float confidence = faceDetectorMat.at<float>(i, 2);
		if (confidence > 0.5f)
		{
			int fx1 = static_cast<int>(faceDetectorMat.at<float>(i, 3) * personFrame.cols + person.x);
			int fy1 = static_cast<int>(faceDetectorMat.at<float>(i, 4) * personFrame.rows + person.y);
			int fx2 = static_cast<int>(faceDetectorMat.at<float>(i, 5) * personFrame.cols + person.x);
			int fy2 = static_cast<int>(faceDetectorMat.at<float>(i, 6) * personFrame.rows + person.y); // input이 personFrame이었기 때문에 정확한 전체 좌표 위해서 이와 같은 연산.

			face = cv::Rect(cv::Point(fx1, fy1), cv::Point(fx2, fy2)); // face 감지되면 제대로 된 값 넣어줌
		}
		break; // must detect only one face per one person
	}
	faces.push_back(face);
	return 0;
}

int VideoInspector::recognizeAgeGender(cv::Mat& frame, cv::Rect face) {
	std::string age = "";
	std::string gender = "";
	float age_float = 0;
	if (face.x != 0 && face.y != 0)
	{
		cv::Mat faceFrame = frame(face); // 얼굴 이미지 따로 복사
		cv::Mat ageGenderRecognizerInputBlob = cv::dnn::blobFromImage(faceFrame, 1.0, cv::Size(62, 62), cv::Scalar(103.53, 116.28, 123.675));
		ageGenderRecognizer.setInput(ageGenderRecognizerInputBlob, "data");
		std::vector<cv::Mat> ageGenderRecognizerOutputs;
		std::vector<cv::String> ageGenderOutputLayers{ "age_conv3", "prob" };
		ageGenderRecognizer.forward(ageGenderRecognizerOutputs, ageGenderOutputLayers); // output layer가 1개 이상일때는 이와 같이 할 수도 있음.

		age = std::to_string(*ageGenderRecognizerOutputs[0].ptr<float>() * 100); // [1, 1, 1, 1]. 내부에는 age값을 100으로 나눈 값이 들어가 있다.
		age_float = *ageGenderRecognizerOutputs[0].ptr<float>() * 100;
		float pFemale = *ageGenderRecognizerOutputs[1].ptr<float>(); // [1, 2, 1, 1]. 즉, 2개의 실수 값이 안에 저장되어 있는데, 두 값을 더하면 1이 되는 softMax값이다. 그냥 저대로 하면 여자일 확률 나옴. 다음 인덱스에는 남자일 확률 들어가 있다.
		gender = (pFemale > 0.5f) ? "female" : "male";
	}
	ageGender.push_back(std::make_pair(age, gender));
	return 0;
}

int VideoInspector::findColor(cv::Mat& personFrame, cv::Rect person) {
	cv::Mat attributesRecognizerInputBlob = cv::dnn::blobFromImage(personFrame, 0.5, cv::Size(160, 80), cv::Scalar(103.53, 116.28, 123.675));
	attributesRecognizer.setInput(attributesRecognizerInputBlob, "0");
	std::vector<cv::Mat> attributesRecognizerOutputs;
	// 453 : [1, 8, 1, 1] across eight attributes. [is_male, has_bag, has_backpack, has_hat, has_longsleeves, has_longpants, has_longhair, has_coat_jacket]. Value > 0.5 means that an attribute is present.
	// 456 : [1, 2, 1, 1]. It is location of point with top color.
	// 459 : [1, 2, 1, 1]. It is location of point with bottom color.
	std::vector<cv::String> attributesRecognizerOutputLayers{ "456", "459" }; // output layer가 3개임. 하지만 top color, bottom color만 사용
	attributesRecognizer.forward(attributesRecognizerOutputs, attributesRecognizerOutputLayers);

	int topx = static_cast<int>(*attributesRecognizerOutputs[0].ptr<float>() * personFrame.cols + person.x);
	int topy = static_cast<int>(*(attributesRecognizerOutputs[0].ptr<float>() + 1) * personFrame.rows + person.y);
	int botx = static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() * personFrame.cols + person.x);
	int boty = static_cast<int>(*(attributesRecognizerOutputs[1].ptr<float>() + 1) * personFrame.rows + person.y);
	cv::Point top(topx, topy);
	cv::Point bottom(botx, boty);
	clothesPoint.push_back(std::make_pair(top, bottom));

	//color picker
	cv::Vec3b topColor = personFrame.at<cv::Vec3b>(cv::Point(static_cast<int>(*attributesRecognizerOutputs[0].ptr<float>() * personFrame.cols),
		static_cast<int>(*(attributesRecognizerOutputs[0].ptr<float>() + 1) * personFrame.rows))); // color picking
	cv::Vec3b bottomColor = personFrame.at<cv::Vec3b>(cv::Point(static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() * personFrame.cols),
		static_cast<int>(*(attributesRecognizerOutputs[1].ptr<float>() + 1) * personFrame.rows))); // color picking
	clothesColor.push_back(std::make_pair(topColor, bottomColor));
	return 0;
}

int VideoInspector::identifyPeople(cv::Mat& personFrame) {
	std::string personId = "";

	cv::Mat personIdentifierInputBlob = cv::dnn::blobFromImage(personFrame, 0.58, cv::Size(128, 256), cv::Scalar(103.53, 116.28, 123.675));
	personIdentifier.setInput(personIdentifierInputBlob, "data");
	cv::Mat personIdentifierOutput = personIdentifier.forward("reid_embedding");
	auto numOfChannels = personIdentifierOutput.cols; //256
	auto outputValues = personIdentifierOutput.ptr<float>();
	std::vector<float> reIdVector = std::vector<float>(outputValues, outputValues + numOfChannels);

	/* Check cosine similarity with all previously detected persons.
	   If it's new person it is added to the global Reid vector and
	   new global ID is assigned to the person. Otherwise, ID of
	   matched person is assigned to it. */
	auto foundId = findMatchingPerson(reIdVector);
	personId = std::to_string(foundId);
	peopleId.push_back(personId);
	return 0;
}

template <typename T>
float VideoInspector::cosineSimilarity(const std::vector<T>& vecA, const std::vector<T>& vecB) {
	if (vecA.size() != vecB.size()) {
		throw std::logic_error("cosine similarity can't be called for the vectors of different lengths: "
			"vecA size = " + std::to_string(vecA.size()) +
			"vecB size = " + std::to_string(vecB.size()));
	}

	T mul, denomA, denomB, A, B;
	mul = denomA = denomB = A = B = 0;
	for (size_t i = 0; i < vecA.size(); ++i) {
		A = vecA[i];
		B = vecB[i];
		mul += A * B;
		denomA += A * A;
		denomB += B * B;
	}
	if (denomA == 0 || denomB == 0) {
		throw std::logic_error("cosine similarity is not defined whenever one or both "
			"input vectors are zero-vectors.");
	}
	return mul / (sqrt(denomA) * sqrt(denomB));
}

uint VideoInspector::findMatchingPerson(const std::vector<float>& newReIdVec) {
	auto size = globalReIdVec.size();

	/* assigned REID is index of the matched vector from the globalReIdVec */
	for (size_t i = 0; i < size; i++) {
		float cosSim = cosineSimilarity(newReIdVec, globalReIdVec[i]);
		//std::cout << "cosineSimilarity: " << cosSim << std::endl;

		if (cosSim > 0.7f) {
			/* We substitute previous person's vector by a new one characterising
			 * last person's position */
			globalReIdVec[i] = newReIdVec;
			return i;
		}
	}
	globalReIdVec.push_back(newReIdVec);
	return size;
}