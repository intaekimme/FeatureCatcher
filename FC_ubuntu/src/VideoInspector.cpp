// VideoInspector.cpp
#include "VideoInspector.h"

#include "models.h"

VideoInspector::VideoInspector() {}

VideoInspector::~VideoInspector() {}

// Loading OpenVINO Models
int VideoInspector::loadModels() {
    // Load the network
    std::cout << "Loading the person detector...\n";
    personDetector =
        cv::dnn::readNet(personDetectorFile, persondetectorConfigFile);
    if (personDetector.empty()) {
        std::cerr << "Couldn't load the person detector!\n";
        return -2;
    } else {
        std::cout << "Done loading the person detector!\n\n";
    }

    std::cout << "Loading the person re-identifier...\n";
    personIdentifier =
        cv::dnn::readNet(personIdentifierFile, personIdentifierConfigFile);
    if (personIdentifier.empty()) {
        std::cerr << "Couldn't load the person detector!\n";
        return -2;
    } else {
        std::cout << "Done loading the person detector!\n\n";
    }

    std::cout << "Loading the face detector...\n";
    faceDetector = cv::dnn::readNet(faceDetectorFile, faceDetectorConfigFile);
    if (faceDetector.empty()) {
        std::cerr << "Couldn't load the face detector!\n";
        return -2;
    } else {
        std::cout << "Done loading the face detector!\n\n";
    }

    std::cout << "Loading the face age-gender recogniger...\n";
    ageGenderRecognizer = cv::dnn::readNet(ageGenderRecognizerFile,
                                           ageGenderRecognizerConfigFile);
    if (ageGenderRecognizer.empty()) {
        std::cerr << "Couldn't load the face age-gender recogniger!\n";
        return -2;
    } else {
        std::cout << "Done loading the face age-gender detector!\n\n";
    }

    std::cout << "Loading the person attributes recognizer...\n";
    attributesRecognizer = cv::dnn::readNet(attributesRecognizerFile,
                                            attributesRecognizerConfigFile);
    if (attributesRecognizer.empty()) {
        std::cerr << "Couldn't load the person attributes recognizer!\n";
        return -2;
    } else {
        std::cout << "Done loading the person attributes recognizer!\n\n";
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

int VideoInspector::process(cv::Mat& frame,  // DataManager& dataManager,
                            std::string frameCount, std::string millisec,
                            std::string yourWebServerPath,
                            int& framePerSaving) {
    clearVars();
    detectPeople(frame);

    for (int i = 0; i < people.size(); i++)  // frame �� people �� ��ŭ �ݺ�.
    {
        cv::Mat personFrame = frame(people[i]);  // ��� �̹��� ���� ����

        detectFace(personFrame, people[i]);   // set faces
        recognizeAgeGender(frame, faces[i]);  // set ageGender(pair)
        findColor(personFrame,
                  people[i]);  // set topColor, bottomColor, clothesColor(pair)
        identifyPeople(personFrame);  // set peopleId

        // DB ���� �Լ� ȣ��
        if (--framePerSaving == 0) {
            framePerSaving = this->framePerSaving;
            /*
            // image ����
            dataManager.savePersonImg(frame, peopleId[i], millisec, people[i],
                                      yourWebServerPath);
            // log ����
            dataManager.saveLog(peopleId[i], frameCount, millisec,
                                ageGender[i].first, ageGender[i].second,
                                yourWebServerPath, clothesColor[i].first,
                                clothesColor[i].second);
            */
        }
    }
    return 0;
}

int VideoInspector::detectPeople(cv::Mat& frame) {
    cv::Mat peopleDetectorInputBlob = cv::dnn::blobFromImage(
        frame, 0.58, cv::Size(512, 512), cv::Scalar(103.53, 116.28, 123.675));
    personDetector.setInput(peopleDetectorInputBlob, "image");
    cv::Mat peopleDetectorOutput = personDetector.forward(
        "detection_out");  // output�� [1, 1, 200, 7]�� ����
    cv::Mat peopleDetectorMat(
        peopleDetectorOutput.size[2], peopleDetectorOutput.size[3], CV_32F,
        peopleDetectorOutput.ptr<float>());  // �׷��� [200, 7]�� �ٲ�.
                                             // ���⼭ 200�� ���, 7�� �Ӽ���

    // for������ ������ �� ������� ó��
    for (int i = 0; i < peopleDetectorMat.rows; i++) {
        // std::string personId = "";
        cv::Rect face = cv::Rect();
        float confidence = peopleDetectorMat.at<float>(
            i, 2);  // (i, 2)�� �ŷڵ�. (i, 0)�̶� (i, 1)�� �ʿ� x
        if (confidence > 0.7f) {
            // cout << "confidence : " << confidence << endl << endl;
            int px1 = static_cast<int>(peopleDetectorMat.at<float>(i, 3) *
                                       frame.cols);
            int py1 = static_cast<int>(peopleDetectorMat.at<float>(i, 4) *
                                       frame.rows);
            int px2 = static_cast<int>(peopleDetectorMat.at<float>(i, 5) *
                                       frame.cols);
            int py2 = static_cast<int>(peopleDetectorMat.at<float>(i, 6) *
                                       frame.rows);  // (i, 3) ~ (i, 6)�� ��ǥ��

            cv::Rect person(cv::Point(px1, py1), cv::Point(px2, py2));
            if (0 > person.x || 0 > person.width ||
                person.x + person.width > frame.cols || 0 > person.y ||
                0 > person.height || person.y + person.height > frame.rows)
                continue;
            people.push_back(person);  // ��� �簢�� ����. rect�� ��ǥ ������
                                       // ������(�̹��� ������ �� ����)
        }
    }
    return 0;
}

int VideoInspector::detectFace(cv::Mat& personFrame, cv::Rect person) {
    cv::Rect face = cv::Rect();  // �ʱⰪ --> face.x : 0, face.y : 0

    cv::Mat faceDetectorInputBlob =
        cv::dnn::blobFromImage(personFrame, 1.0, cv::Size(300, 300),
                               cv::Scalar(103.53, 116.28, 123.675));
    faceDetector.setInput(faceDetectorInputBlob, "data");
    cv::Mat faceDetectorOutput = faceDetector.forward(
        "detection_out");  // ����� [1, 1, 200, 7]�� ������ ��� ������ ���
    cv::Mat faceDetectorMat(faceDetectorOutput.size[2],
                            faceDetectorOutput.size[3], CV_32F,
                            faceDetectorOutput.ptr<float>());
    // ������ �� �󱼸��� ó��
    for (int i = 0; i < faceDetectorMat.rows; i++) {
        float confidence = faceDetectorMat.at<float>(i, 2);
        if (confidence > 0.5f) {
            int fx1 = static_cast<int>(
                faceDetectorMat.at<float>(i, 3) * personFrame.cols + person.x);
            int fy1 = static_cast<int>(
                faceDetectorMat.at<float>(i, 4) * personFrame.rows + person.y);
            int fx2 = static_cast<int>(
                faceDetectorMat.at<float>(i, 5) * personFrame.cols + person.x);
            int fy2 = static_cast<int>(
                faceDetectorMat.at<float>(i, 6) * personFrame.rows +
                person.y);  // input�� personFrame�̾��� ������ ��Ȯ�� ��ü ��ǥ
                            // ���ؼ� �̿� ���� ����.

            face = cv::Rect(
                cv::Point(fx1, fy1),
                cv::Point(fx2, fy2));  // face �����Ǹ� ����� �� �� �־���
        }
        break;  // must detect only one face per one person
    }
    faces.push_back(face);
    return 0;
}

int VideoInspector::recognizeAgeGender(cv::Mat& frame, cv::Rect face) {
    std::string age = "";
    std::string gender = "";
    float age_float = 0;
    if (face.x != 0 && face.y != 0) {
        cv::Mat faceFrame = frame(face);  // �� �̹��� ���� ����
        cv::Mat ageGenderRecognizerInputBlob =
            cv::dnn::blobFromImage(faceFrame, 1.0, cv::Size(62, 62),
                                   cv::Scalar(103.53, 116.28, 123.675));
        ageGenderRecognizer.setInput(ageGenderRecognizerInputBlob, "data");
        std::vector<cv::Mat> ageGenderRecognizerOutputs;
        std::vector<cv::String> ageGenderOutputLayers{"age_conv3", "prob"};
        ageGenderRecognizer.forward(
            ageGenderRecognizerOutputs,
            ageGenderOutputLayers);  // output layer�� 1�� �̻��϶��� �̿� ���� ��
                                     // ���� ����.

        age = std::to_string(
            *ageGenderRecognizerOutputs[0].ptr<float>() *
            100);  // [1, 1, 1, 1]. ���ο��� age���� 100���� ���� ���� �� �ִ�.
        age_float = *ageGenderRecognizerOutputs[0].ptr<float>() * 100;
        float pFemale =
            *ageGenderRecognizerOutputs[1]
                 .ptr<float>();  // [1, 2, 1, 1]. ��, 2���� �Ǽ� ���� �ȿ� ����Ǿ�
                                 // �ִµ�, �� ���� ���ϸ� 1�� �Ǵ� softMax���̴�. �׳�
                                 // ����� �ϸ� ������ Ȯ�� ����. ���� �ε�������
                                 // ������ Ȯ�� �� �ִ�.
        gender = (pFemale > 0.5f) ? "female" : "male";
    }
    ageGender.push_back(std::make_pair(age, gender));
    return 0;
}

int VideoInspector::findColor(cv::Mat& personFrame, cv::Rect person) {
    cv::Mat attributesRecognizerInputBlob =
        cv::dnn::blobFromImage(personFrame, 0.5, cv::Size(160, 80),
                               cv::Scalar(103.53, 116.28, 123.675));
    attributesRecognizer.setInput(attributesRecognizerInputBlob, "0");
    std::vector<cv::Mat> attributesRecognizerOutputs;
    // 453 : [1, 8, 1, 1] across eight attributes. [is_male, has_bag,
    // has_backpack, has_hat, has_longsleeves, has_longpants, has_longhair,
    // has_coat_jacket]. Value > 0.5 means that an attribute is present. 456 :
    // [1, 2, 1, 1]. It is location of point with top color. 459 : [1, 2, 1, 1].
    // It is location of point with bottom color.
    std::vector<cv::String> attributesRecognizerOutputLayers{
        "456",
        "459"};  // output layer�� 3����. ������ top color, bottom color�� ���
    attributesRecognizer.forward(attributesRecognizerOutputs,
                                 attributesRecognizerOutputLayers);

    int topx = static_cast<int>(*attributesRecognizerOutputs[0].ptr<float>() *
                                    personFrame.cols +
                                person.x);
    int topy = static_cast<int>(
        *(attributesRecognizerOutputs[0].ptr<float>() + 1) * personFrame.rows +
        person.y);
    int botx = static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() *
                                    personFrame.cols +
                                person.x);
    int boty = static_cast<int>(
        *(attributesRecognizerOutputs[1].ptr<float>() + 1) * personFrame.rows +
        person.y);
    cv::Point top(topx, topy);
    cv::Point bottom(botx, boty);
    clothesPoint.push_back(std::make_pair(top, bottom));

    // color picker
    cv::Vec3b topColor = personFrame.at<cv::Vec3b>(cv::Point(
        static_cast<int>(*attributesRecognizerOutputs[0].ptr<float>() *
                         personFrame.cols),
        static_cast<int>(*(attributesRecognizerOutputs[0].ptr<float>() + 1) *
                         personFrame.rows)));  // color picking
    cv::Vec3b bottomColor = personFrame.at<cv::Vec3b>(cv::Point(
        static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() *
                         personFrame.cols),
        static_cast<int>(*(attributesRecognizerOutputs[1].ptr<float>() + 1) *
                         personFrame.rows)));  // color picking
    clothesColor.push_back(std::make_pair(topColor, bottomColor));
    return 0;
}

int VideoInspector::identifyPeople(cv::Mat& personFrame) {
    std::string personId = "";

    cv::Mat personIdentifierInputBlob =
        cv::dnn::blobFromImage(personFrame, 0.58, cv::Size(128, 256),
                               cv::Scalar(103.53, 116.28, 123.675));
    personIdentifier.setInput(personIdentifierInputBlob, "data");
    cv::Mat personIdentifierOutput = personIdentifier.forward("reid_embedding");
    auto numOfChannels = personIdentifierOutput.cols;  // 256
    auto outputValues = personIdentifierOutput.ptr<float>();
    std::vector<float> reIdVector =
        std::vector<float>(outputValues, outputValues + numOfChannels);

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
    auto size = globalReIdVec.size();

    /* assigned REID is index of the matched vector from the globalReIdVec */
    for (size_t i = 0; i < size; i++) {
        float cosSim = cosineSimilarity(newReIdVec, globalReIdVec[i]);
        // std::cout << "cosineSimilarity: " << cosSim << std::endl;

        if (cosSim > 0.7f) {
            /* We substitute previous person's vector by a new one
             * characterising last person's position */
            globalReIdVec[i] = newReIdVec;
            return i;
        }
    }
    globalReIdVec.push_back(newReIdVec);
    return size;
}

void VideoInspector::visualize(cv::Mat& frame) {
    for (int i = 0; i < people.size(); i++) {
        if (!peopleId[i].empty()) {
            cv::putText(frame, peopleId[i],
                        cv::Point2f(static_cast<float>(people[i].x),
                                    static_cast<float>(people[i].y + 30)),
                        cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 0, 255),
                        1.3);
        }
        cv::rectangle(frame, people[i], cv::Scalar(0, 0, 255), 1.5);
        cv::circle(frame, clothesPoint[i].first, 0,
                   cv::Scalar(clothesColor[i].first), 20);  // top color
        cv::circle(frame, clothesPoint[i].second, 0,
                   cv::Scalar(clothesColor[i].second), 20);  // bottom color
        if (faces[i].x != 0 && faces[i].y != 0) {
            if (ageGender[i].second == "female")
                cv::rectangle(frame, faces[i], cv::Scalar(127, 0, 255), 1.5);
            else
                cv::rectangle(frame, faces[i], cv::Scalar(255, 84, 0), 1.5);
            cv::putText(
                frame, ageGender[i].first,
                cv::Point2f(
                    static_cast<float>(faces[i].x),
                    static_cast<float>(faces[i].y + faces[i].height + 30)),
                cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 255, 0), 1.3);
        }
    }
}