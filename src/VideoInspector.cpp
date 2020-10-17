// VideoInspector.cpp
#include "VideoInspector.h"

#include <string>
#include <thread>

#include "models.h"

VideoInspector::VideoInspector(int threadNum = 5) {
    if (threadNum <= 0) {
        std::cerr << "thread_num must be positive integer" << std::endl;
        return;
    } else
        this->threadNum = threadNum;

    pool = std::make_unique<ThreadPool::ThreadPool>(threadNum);
    personDetector = nullptr;
}

VideoInspector::~VideoInspector() {}
// Loading OpenVINO Models
int VideoInspector::loadModels() {
    personDetector = std::make_unique<cv::dnn::Net>(cv::dnn::readNet(personDetectorFile, persondetectorConfigFile, "dldt"));
    if (personDetector->empty()) {
        std::cerr << "Couldn't load the person detector!" << std::endl;
        return -2;
    } else
        std::cout << "Done loading the person detector!" << std::endl;

    // multi-threading을 위해 각 Net instance threadNum만큼 생성
    for (int i = 0; i < this->threadNum; i++) {
        personIdentifier.push_back(std::make_unique<cv::dnn::Net>(cv::dnn::readNet(personIdentifierFile, personIdentifierConfigFile, "dldt")));
        if (personIdentifier[i]->empty()) {
            std::cerr << "Couldn't load the person Identifier[" << i << "]!" << std::endl;
            return -2;
        } else
            std::cout << "Done loading the person Identifier[" << i << "]!" << std::endl;

        faceDetector.push_back(std::make_unique<cv::dnn::Net>(cv::dnn::readNet(faceDetectorFile, faceDetectorConfigFile, "caffe")));
        if (faceDetector[i]->empty()) {
            std::cerr << "Couldn't load the face detector[" << i << "]!" << std::endl;
            return -2;
        } else
            std::cout << "Done loading the face detector[" << i << "]!" << std::endl;

        ageGenderRecognizer.push_back(std::make_unique<cv::dnn::Net>(cv::dnn::readNet(ageGenderRecognizerFile, ageGenderRecognizerConfigFile, "dldt")));
        if (ageGenderRecognizer[i]->empty()) {
            std::cerr << "Couldn't load the face age-gender recognizer[" << i << "]!" << std::endl;
            return -2;
        } else
            std::cout << "Done loading the face age-gender recognizer[" << i << "]!" << std::endl;

        attributesRecognizer.push_back(std::make_unique<cv::dnn::Net>(cv::dnn::readNet(attributesRecognizerFile, attributesRecognizerConfigFile, "dldt")));
        if (attributesRecognizer[i]->empty()) {
            std::cerr << "Couldn't load the person attributes recognizer[" << i << "]!" << std::endl;
            return -2;
        } else
            std::cout << "Done loading the person attributes recognizer[" << i << "]!" << std::endl;
    }

    return 0;
}

void VideoInspector::clearVars() {
    people.clear();
    faces.clear();
    ageGender.clear();
    peopleId.clear();
    clothesPoint.clear();
    clothesColor.clear();
}

void VideoInspector::setFramePerSaving(int frame_per_saving) {
    this->framePerSaving = frame_per_saving;
}

void VideoInspector::beforeProcess() {
    std::lock_guard<std::mutex> initLock(mInit);

    faces.push_back(cv::Rect());
    ageGender.push_back(std::make_pair("", ""));
    clothesPoint.push_back(std::make_pair(cv::Point(), cv::Point()));
    clothesColor.push_back(std::make_pair(cv::Vec3b(), cv::Vec3b()));
    peopleId.push_back("");
}

void VideoInspector::processOnePerson(cv::Mat& frame, int idx) {
    // 일단 default value 밀어넣은 후에 해당 index에 process 결과로 바꿔줌.
    beforeProcess();

    cv::Mat personFrame = frame(this->people[idx]);
    detectFace(personFrame, this->people[idx], idx);   // set faces
    recognizeAgeGender(frame, this->faces[idx], idx);  // set ageGender(pair)
    findColor(personFrame, this->people[idx], idx);    // set topColor, bottomColor, clothesColor(pair)
    identifyPeople(personFrame, idx);                  // set peopleId
}

int VideoInspector::process(cv::Mat& frame,   DataManager& dataManager,
                            std::string frameCount, std::string millisec,
                            std::string yourWebServerPath,
                            int& framePerSaving) {
    clearVars();
    detectPeople(frame);

    std::vector<std::future<void>> futures;
    for (int i = 0; i < this->people.size(); i++) {
        futures.emplace_back(pool->EnqueueJob([&, i] { processOnePerson(frame, i); }));
        // pool->EnqueueJob([&] { processOnePerson(frame, i); });
        // DB ���� �Լ� ȣ��

    }
    for (auto& f : futures) {
        try {
            f.get();
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    if (--framePerSaving == 0) {
        framePerSaving = this->framePerSaving;
        for (int i = 0; i < people.size(); i++) {
            dataManager.savePersonImg(frame, peopleId[i], millisec, people[i],
                                      yourWebServerPath);
            // log ����
            // table name input!!!
            dataManager.log_to_DB(dataManager.getTableName(), peopleId[i], frameCount, millisec,
                                ageGender[i].first, ageGender[i].second,
                                yourWebServerPath, clothesColor[i].first,
                                clothesColor[i].second);
        }
    }
    return 0;
}

int VideoInspector::detectPeople(cv::Mat& frame) {
    cv::Mat peopleDetectorInputBlob = cv::dnn::blobFromImage(frame, 0.58, cv::Size(512, 512), cv::Scalar(103.53, 116.28, 123.675));
    personDetector->setInput(peopleDetectorInputBlob, "image");
    cv::Mat peopleDetectorOutput = personDetector->forward("detection_out");  // output은 [1, 1, 200, 7] matrix

    // [200, 7]의 matrix로 변환
    cv::Mat peopleDetectorMat(peopleDetectorOutput.size[2], peopleDetectorOutput.size[3], CV_32F, peopleDetectorOutput.ptr<float>());

    // 감지한 각 사람 별로 for문 처리
    for (int i = 0; i < peopleDetectorMat.rows; i++) {
        float confidence = peopleDetectorMat.at<float>(i, 2);
        if (confidence > 0.7f) {
            int px1 = static_cast<int>(peopleDetectorMat.at<float>(i, 3) * frame.cols);
            int py1 = static_cast<int>(peopleDetectorMat.at<float>(i, 4) * frame.rows);
            int px2 = static_cast<int>(peopleDetectorMat.at<float>(i, 5) * frame.cols);
            int py2 = static_cast<int>(peopleDetectorMat.at<float>(i, 6) * frame.rows);

            cv::Rect person(cv::Point(px1, py1), cv::Point(px2, py2));
            if (0 > person.x || 0 > person.width || person.x + person.width > frame.cols ||
                0 > person.y || 0 > person.height || person.y + person.height > frame.rows)
                continue;
            this->people.push_back(person);
        }
    }
    return 0;
}

int VideoInspector::detectFace(cv::Mat& personFrame, cv::Rect person, int idx) {
    cv::Rect face = cv::Rect();  // �ʱⰪ --> face.x : 0, face.y : 0

    cv::Mat faceDetectorInputBlob =
        cv::dnn::blobFromImage(personFrame, 1.0, cv::Size(300, 300), cv::Scalar(103.53, 116.28, 123.675));

    int netIdx = (idx == 0) ? 0 : (idx % this->threadNum);
    faceDetector[netIdx]->setInput(faceDetectorInputBlob, "data");
    cv::Mat faceDetectorOutput = faceDetector[netIdx]->forward("detection_out");
    cv::Mat faceDetectorMat(faceDetectorOutput.size[2], faceDetectorOutput.size[3], CV_32F, faceDetectorOutput.ptr<float>());

    for (int i = 0; i < faceDetectorMat.rows; i++) {
        float confidence = faceDetectorMat.at<float>(i, 2);
        if (confidence > 0.5f) {
            int fx1 = static_cast<int>(faceDetectorMat.at<float>(i, 3) * personFrame.cols + person.x);
            int fy1 = static_cast<int>(faceDetectorMat.at<float>(i, 4) * personFrame.rows + person.y);
            int fx2 = static_cast<int>(faceDetectorMat.at<float>(i, 5) * personFrame.cols + person.x);
            int fy2 = static_cast<int>(faceDetectorMat.at<float>(i, 6) * personFrame.rows + person.y);

            face = cv::Rect(cv::Point(fx1, fy1), cv::Point(fx2, fy2));
        }
        break;  // must detect only one face per one person
    }
    std::lock_guard<std::mutex> faceLock(mFace);
    this->faces.at(idx) = face;
    return 0;
}

int VideoInspector::recognizeAgeGender(cv::Mat& frame, cv::Rect face, int idx) {
    std::string age = "";
    std::string gender = "";
    float ageFloat = 0;
    if (face.x != 0 && face.y != 0) {
        cv::Mat faceFrame = frame(face);
        cv::Mat ageGenderRecognizerInputBlob =
            cv::dnn::blobFromImage(faceFrame, 1.0, cv::Size(62, 62), cv::Scalar(103.53, 116.28, 123.675));

        int netIdx = (idx == 0) ? 0 : (idx % this->threadNum);
        ageGenderRecognizer[netIdx]->setInput(ageGenderRecognizerInputBlob, "data");
        std::vector<cv::Mat> ageGenderRecognizerOutputs;
        std::vector<cv::String> ageGenderOutputLayers{"age_conv3", "prob"};
        ageGenderRecognizer[netIdx]->forward(ageGenderRecognizerOutputs, ageGenderOutputLayers);

        age = std::to_string(*ageGenderRecognizerOutputs[0].ptr<float>() * 100);
        ageFloat = *ageGenderRecognizerOutputs[0].ptr<float>() * 100;
        float pFemale = *ageGenderRecognizerOutputs[1].ptr<float>();

        gender = (pFemale > 0.5f) ? "female" : "male";
    }

    std::lock_guard<std::mutex> ageGenderLock(mAgeGender);
    this->ageGender.at(idx) = std::make_pair(age, gender);
    return 0;
}

int VideoInspector::findColor(cv::Mat& personFrame, cv::Rect person, int idx) {
    cv::Mat attributesRecognizerInputBlob =
        cv::dnn::blobFromImage(personFrame, 0.5, cv::Size(160, 80), cv::Scalar(103.53, 116.28, 123.675));

    int netIdx = (idx == 0) ? 0 : (idx % this->threadNum);
    attributesRecognizer[netIdx]->setInput(attributesRecognizerInputBlob, "0");
    std::vector<cv::Mat> attributesRecognizerOutputs;
    // 453 : [1, 8, 1, 1] across eight attributes. [is_male, has_bag, has_backpack, has_hat, has_longsleeves, has_longpants, has_longhair, has_coat_jacket].
    //       Value > 0.5 means that an attribute is present.
    // 456 : [1, 2, 1, 1]. It is location of point with top color.
    // 459 : [1, 2, 1, 1]. It is location of point with bottom color.
    std::vector<cv::String> attributesRecognizerOutputLayers{"456", "459"};
    attributesRecognizer[netIdx]->forward(attributesRecognizerOutputs, attributesRecognizerOutputLayers);

    int topx = static_cast<int>(*attributesRecognizerOutputs[0].ptr<float>() * personFrame.cols + person.x);
    int topy = static_cast<int>(*(attributesRecognizerOutputs[0].ptr<float>() + 1) * personFrame.rows + person.y);
    int botx = static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() * personFrame.cols + person.x);
    int boty = static_cast<int>(*(attributesRecognizerOutputs[1].ptr<float>() + 1) * personFrame.rows + person.y);
    cv::Point top(topx, topy);
    cv::Point bottom(botx, boty);

    {
        std::lock_guard<std::mutex> pointLock(mPoint);
        this->clothesPoint.at(idx) = std::make_pair(top, bottom);
    }

    // color picker
    cv::Vec3b topColor = personFrame.at<cv::Vec3b>(cv::Point(static_cast<int>(*attributesRecognizerOutputs[0].ptr<float>() * personFrame.cols),
                                                             static_cast<int>(*(attributesRecognizerOutputs[0].ptr<float>() + 1) * personFrame.rows)));
    cv::Vec3b bottomColor = personFrame.at<cv::Vec3b>(cv::Point(static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() * personFrame.cols),
                                                                static_cast<int>(*(attributesRecognizerOutputs[1].ptr<float>() + 1) * personFrame.rows)));

    std::lock_guard<std::mutex> colorLock(mColor);
    this->clothesColor.at(idx) = std::make_pair(topColor, bottomColor);
    return 0;
}

int VideoInspector::identifyPeople(cv::Mat& personFrame, int idx) {
    std::string personId = "";

    cv::Mat personIdentifierInputBlob =
        cv::dnn::blobFromImage(personFrame, 0.58, cv::Size(128, 256), cv::Scalar(103.53, 116.28, 123.675));

    int netIdx = (idx == 0) ? 0 : (idx % this->threadNum);
    personIdentifier[netIdx]->setInput(personIdentifierInputBlob, "data");
    cv::Mat personIdentifierOutput = personIdentifier[netIdx]->forward("reid_embedding");

    auto numOfChannels = personIdentifierOutput.cols;  // 256
    auto outputValues = personIdentifierOutput.ptr<float>();
    std::vector<float> reIdVector = std::vector<float>(outputValues, outputValues + numOfChannels);

    // Check cosine similarity with all previously detected persons.
    // If it's new person it is added to the global Reid vector and new global ID is assigned to the person.
    // Otherwise, ID of matched person is assigned to it.
    auto foundId = findMatchingPerson(reIdVector);
    personId = std::to_string(foundId);

    std::lock_guard<std::mutex> idLock(mId);
    this->peopleId.at(idx) = personId;
    return 0;
}

template <typename T>
float VideoInspector::cosineSimilarity(const std::vector<T>& vecA,
                                       const std::vector<T>& vecB) {
    if (vecA.size() != vecB.size()) {
        throw std::logic_error(
            "cosine similarity can't be called for the vectors of different "
            "lengths: "
            "vecA size = " +
            std::to_string(vecA.size()) +
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
        throw std::logic_error(
            "cosine similarity is not defined whenever one or both "
            "input vectors are zero-vectors.");
    }
    return mul / (sqrt(denomA) * sqrt(denomB));
}

uint VideoInspector::findMatchingPerson(const std::vector<float>& newReIdVec) {
    std::lock_guard<std::mutex> gIdLock(mGlobalId);
    auto size = globalReIdVec.size();

    // assigned REID is index of the matched vector from the globalReIdVec
    for (size_t i = 0; i < size; i++) {
        float cosSim = cosineSimilarity(newReIdVec, globalReIdVec[i]);
        // std::cout << "cosineSimilarity: " << cosSim << std::endl;

        if (cosSim > 0.7f) {
            // Substitute previous person's vector by a new one
            // characterising last person's position.
            globalReIdVec[i] = newReIdVec;
            return i;
        }
    }
    globalReIdVec.push_back(newReIdVec);
    return size;
}

void VideoInspector::visualize(cv::Mat& frame) {
    for (int i = 0; i < people.size(); i++) {
        // draw people Id
        if (!peopleId[i].empty()) {
            cv::putText(frame, peopleId[i],
                        cv::Point2f(static_cast<float>(people[i].x), static_cast<float>(people[i].y + 30)),
                        cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.3);
        }

        // draw people rect
        cv::rectangle(frame, people[i], cv::Scalar(0, 0, 255), 1.5);

        // draw clothes point & color
        if (clothesPoint[i].first != cv::Point(0, 0) && clothesPoint[i].second != cv::Point(0, 0)) {
            cv::circle(frame, clothesPoint[i].first, 0, cv::Scalar(clothesColor[i].first), 20);    // top color
            cv::circle(frame, clothesPoint[i].second, 0, cv::Scalar(clothesColor[i].second), 20);  // bottom color
        }

        // draw face rect(red : female, blue : male) and age
        if (faces[i].x != 0 && faces[i].y != 0) {
            if (ageGender[i].second == "female")
                cv::rectangle(frame, faces[i], cv::Scalar(127, 0, 255), 1.5);
            else
                cv::rectangle(frame, faces[i], cv::Scalar(255, 84, 0), 1.5);

            cv::putText(frame, ageGender[i].first,
                        cv::Point2f(static_cast<float>(faces[i].x), static_cast<float>(faces[i].y + faces[i].height + 30)),
                        cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 255, 0), 1.3);
        }
    }
}