#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <time.h>

#define PROCESS_PER_FRAME 1

std::vector<std::vector<float>> globalReIdVec;
int frameForProcess = PROCESS_PER_FRAME;
std::array<std::string, 8> attributesString{ "is_male", "has_bag", "has_backpack", "has_hat", "has_longsleeves", "has_longpants", "has_longhair", "has_coat_jacket" };
int frameCounting = 0;
template <typename T>
float cosineSimilarity(const std::vector<T>& vecA, const std::vector<T>& vecB) {
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

uint findMatchingPerson(const std::vector<float>& newReIdVec) {
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

int main(int, char**)
{
    cv::Mat frame;
    cv::VideoCapture cap;

    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;        // 0 = open default camera
    int apiID = cv::CAP_ANY; // 0 = autodetect default API
    // open selected camera using selected API
    // cap.open(deviceID, apiID);

    uchar ntblue, ntgreen, ntred, nbblue, nbgreen, nbred;
    //ntblue = 0; ntgreen = 0; ntred = 0; nbblue = 0; nbgreen = 0; nbred = 0;


    cap.open("video_test5.mp4");
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    //what is video fps that get VideoCapture Class 
    double video_fps = cap.get(cv::CAP_PROP_FPS);


    std::string persondetectorConfigFile = "person-detection-0102.xml";
    std::string personDetectorFile = "person-detection-0102.bin";

    std::string personIdentifierConfigFile = "person-reidentification-retail-0270.xml";
    std::string personIdentifierFile = "person-reidentification-retail-0270.bin";

    std::string faceDetectorConfigFile = "deploy.prototxt";
    std::string faceDetectorFile = "res10_300x300_ssd_iter_140000_fp16.caffemodel";

    std::string ageGenderRecognizerConfigFile = "age-gender-recognition-retail-0013.xml";
    std::string ageGenderRecognizerFile = "age-gender-recognition-retail-0013.bin";

    std::string attributesRecognizerConfigFile = "person-attributes-recognition-crossroad-0230.xml";
    std::string attributesRecognizerFile = "person-attributes-recognition-crossroad-0230.bin";

    // Load the network
    std::cout << "Loading the person detector...\n";
    cv::dnn::Net personDetector = cv::dnn::readNet(personDetectorFile, persondetectorConfigFile);
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
    cv::dnn::Net personIdentifier = cv::dnn::readNet(personIdentifierFile, personIdentifierConfigFile);
    if (personDetector.empty())
    {
        std::cerr << "Couldn't load the person detector!\n";
        return -2;
    }
    else
    {
        std::cout << "Done loading the person detector!\n\n";
    }

    std::cout << "Loading the face detector...\n";
    cv::dnn::Net faceDetector = cv::dnn::readNet(faceDetectorFile, faceDetectorConfigFile);
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
    cv::dnn::Net ageGenderRecognizer = cv::dnn::readNet(ageGenderRecognizerFile, ageGenderRecognizerConfigFile);
    if (faceDetector.empty())
    {
        std::cerr << "Couldn't load the face age-gender recogniger!\n";
        return -2;
    }
    else
    {
        std::cout << "Done loading the face age-gender detector!\n\n";
    }

    std::cout << "Loading the person attributes recognizer...\n";
    cv::dnn::Net attributesRecognizer = cv::dnn::readNet(attributesRecognizerFile, attributesRecognizerConfigFile);
    if (attributesRecognizer.empty())
    {
        std::cerr << "Couldn't load the person attributes recognizer!\n";
        return -2;
    }
    else
    {
        std::cout << "Done loading the person attributes recognizer!\n\n";
    }

    //test print video fps and total frame
    std::cout << "present video fps is : " << video_fps << std::endl;
    std::cout << " Total Frame count : " << cvRound(cap.get(cv::CAP_PROP_FRAME_COUNT)) << std::endl;
    std::cout << "Frame per second : " << cvRound(cap.get(cv::CAP_PROP_FPS)) << std::endl;
    std::cout << "Frame width : " << cvRound(cap.get(cv::CAP_PROP_FRAME_WIDTH)) << std::endl;
    std::cout << "Frame height : " << cvRound(cap.get(cv::CAP_PROP_FRAME_HEIGHT)) << std::endl;

    // open videolog text file
    std::ofstream videologFile;
    std::string inputFileStr;
    videologFile.open("videolog.txt");
    if (videologFile.is_open() == false) {
        std::cerr << "Couldn't open the videolog text file!\n";
        return -2;
    }
    else {
        std::cout << "Done opening the videolog text file\n\n";
    }


    std::vector<std::string> peopleId;
    std::vector<cv::Rect> peopleRect;
    std::vector<cv::Rect> faceRect;
    std::vector<std::pair<std::string, std::string>> ageGender;
    std::vector<std::vector<bool>> peopleAttributes;
    std::vector<std::pair<cv::Point, cv::Point>> peopleColor_point;
    std::vector<std::pair<cv::Vec3b, cv::Vec3b>> peopleColor_vec3b;

    while (true)
    {
        frameCounting++;
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);

        //print video fps, total video frame count and present milli second in video
        cv::putText(frame,
            std::to_string(cvRound(cap.get(cv::CAP_PROP_FPS))),
            cv::Point(15, 50),
            cv::FONT_HERSHEY_COMPLEX,
            0.5,
            cv::Scalar(0, 0, 0),
            1.3
        );

        cv::putText(frame,
            std::to_string(cvRound(cap.get(cv::CAP_PROP_FRAME_COUNT))),
            cv::Point(45, 50),
            cv::FONT_HERSHEY_COMPLEX,
            0.5,
            cv::Scalar(0, 0, 0),
            1.3
        );

        cv::putText(frame,
            std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_FRAMES))),
            cv::Point(105, 50),
            cv::FONT_HERSHEY_COMPLEX,
            0.5,
            cv::Scalar(0, 0, 0),
            1.3
        );

        cv::putText(frame,
            std::to_string(cvRound(cap.get(cv::CAP_PROP_FRAME_COUNT))),
            cv::Point(45, 50),
            cv::FONT_HERSHEY_COMPLEX,
            0.5,
            cv::Scalar(0, 0, 0),
            1.3
        );

        cv::putText(frame,
            std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC))),
            cv::Point(165, 50),
            cv::FONT_HERSHEY_COMPLEX,
            0.5,
            cv::Scalar(0, 0, 0),
            1.3
        );


        if (frame.empty())
        {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        if (--frameForProcess == 0) {
            // 몇 프레임마다 처리할 것인지 결정
            frameForProcess = PROCESS_PER_FRAME;

            // 각 벡터들 초기화. 전에 저장되어 있던 값들 날림.
            peopleId.clear();
            peopleRect.clear();
            faceRect.clear();
            ageGender.clear();
            peopleAttributes.clear();
            peopleColor_point.clear();
            peopleColor_vec3b.clear();


            cv::Mat peopleDetectorInputBlob = cv::dnn::blobFromImage(frame, 0.58, cv::Size(512, 512), cv::Scalar(103.53, 116.28, 123.675));
            personDetector.setInput(peopleDetectorInputBlob, "image");
            cv::Mat peopleDetectorOutput = personDetector.forward("detection_out"); // output은 [1, 1, 200, 7]로 나옴
            cv::Mat peopleDetectorMat(peopleDetectorOutput.size[2], peopleDetectorOutput.size[3], CV_32F, peopleDetectorOutput.ptr<float>()); // 그래서 [200, 7]로 바꿈. 여기서 200은 사람, 7은 속성들

            // for문에서 감지한 각 사람마다 처리
            for (int i = 0; i < peopleDetectorMat.rows; i++)
            {
                std::string personId = "";
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
                    peopleRect.push_back(person); // 사람 사각형 저장. rect는 좌표 정보만 저장함(이미지 정보는 안 저장)



                    /*---------------------------------------------얼굴 감지---------------------------------------------*/
                    cv::Mat personFrame = frame(person); // 사람 이미지 따로 복사
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
                            int fx1 = static_cast<int>(faceDetectorMat.at<float>(i, 3) * personFrame.cols + px1);
                            int fy1 = static_cast<int>(faceDetectorMat.at<float>(i, 4) * personFrame.rows + py1);
                            int fx2 = static_cast<int>(faceDetectorMat.at<float>(i, 5) * personFrame.cols + px1);
                            int fy2 = static_cast<int>(faceDetectorMat.at<float>(i, 6) * personFrame.rows + py1); // input이 personFrame이었기 때문에 정확한 전체 좌표 위해서 이와 같은 연산.

                            face = cv::Rect(cv::Point(fx1, fy1), cv::Point(fx2, fy2));
                        }
                        faceRect.push_back(face);
                        break; // must detect only one face per one person
                    }
                    /*---------------------------------------------------------------------------------------------------*/



                    /*----------------------------------감지한 얼굴로부터 age, gender 감지-------------------------------*/
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
                    /*---------------------------------------------------------------------------------------------------*/



                    /*----------------------------감지한 사람으로부터 속성값, 상하의 색깔 감지---------------------------*/
                    cv::Mat attributesRecognizerInputBlob = cv::dnn::blobFromImage(personFrame, 0.5, cv::Size(160, 80), cv::Scalar(103.53, 116.28, 123.675));
                    attributesRecognizer.setInput(attributesRecognizerInputBlob, "0");
                    std::vector<cv::Mat> attributesRecognizerOutputs;
                    // 453 : [1, 8, 1, 1] across eight attributes. [is_male, has_bag, has_backpack, has_hat, has_longsleeves, has_longpants, has_longhair, has_coat_jacket]. Value > 0.5 means that an attribute is present.
                    // 456 : [1, 2, 1, 1]. It is location of point with top color.
                    // 459 : [1, 2, 1, 1]. It is location of point with bottom color.
                    std::vector<cv::String> attributesRecognizerOutputLayers{ "453", "456", "459" }; // output layer가 3개임.
                    attributesRecognizer.forward(attributesRecognizerOutputs, attributesRecognizerOutputLayers);
                    auto personAttributesPtr = attributesRecognizerOutputs[0].ptr<float>();
                    std::vector<float> personAttributesValues(personAttributesPtr, personAttributesPtr + attributesString.size()); // 실수값 8개 나옴.

                    std::vector<bool> personAttributes; // 0.5 기준으로 true, false 판단해 bool 값으로 저장
                    for (int j = 0; j < attributesString.size(); j++)
                        personAttributes.push_back(personAttributesValues[j] > 0.3f);
                    int topx = static_cast<int>(*attributesRecognizerOutputs[1].ptr<float>() * personFrame.cols + px1);
                    int topy = static_cast<int>(*(attributesRecognizerOutputs[1].ptr<float>() + 1) * personFrame.rows + py1);
                    int botx = static_cast<int>(*attributesRecognizerOutputs[2].ptr<float>() * personFrame.cols + px1);
                    int boty = static_cast<int>(*(attributesRecognizerOutputs[2].ptr<float>() + 1) * personFrame.rows + py1);
                    cv::Point2f topColor(topx, topy);
                    cv::Point2f bottomColor(botx, boty);

                    cv::Vec3b top_color = frame.at<cv::Vec3b>(cv::Point(topx, topy)); // color picking
                    cv::Vec3b bottom_color = frame.at<cv::Vec3b>(cv::Point(botx, boty)); // color picking

                    std::cout << "start detecting person's dress color" << std::endl;
                    std::cout << "top : " << topx << ", " << topy << std::endl;
                    std::cout << "bottom : " << botx << ", " << boty << std::endl;
                    std::cout << "complete detecting person's dress color" << std::endl;

                    peopleAttributes.push_back(personAttributes);
                    peopleColor_point.push_back(std::make_pair(topColor, bottomColor));
                    peopleColor_vec3b.push_back(std::make_pair(top_color, bottom_color));

                   

                    /*-----------------------------------------각 사람의 ID 감지-----------------------------------------*/
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
                    /*---------------------------------------------------------------------------------------------------*/

                    std::cout << "start saving img" << std::endl;
                    // saving image
                    //cv::Rect roi(cv::Point2f(x1, y1), cv::Point2f(x2, y2));
                    cv::Rect roi_all(cv::Point(0, 0), cv::Point(640, 360));
                    cv::Mat3b crop_all = frame(roi_all);
                    cv::String srcImg_all = "img/" + std::to_string(cvRound(cap.get(cv::CAP_PROP_FPS)))
                        + "_"
                        + std::to_string(cvRound(cap.get(cv::CAP_PROP_FRAME_COUNT)))
                        + "_"
                        + std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC)))
                        + ".bmp";
                    cv::imwrite(srcImg_all, crop_all);

                    cv::Mat3b crop = frame(person);
                    cv::String srcImg_person = "img/person_" + personId + "_" + std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC))) + ".bmp";
                    cv::String srcImg_face = "";
                    imwrite(srcImg_person, crop);
                    if (!face.empty()) {
                        cv::Mat3b crop = frame(face);
                        srcImg_face = "img/face_" + personId + "_" + std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC))) + ".bmp";
                        imwrite(srcImg_face, crop);
                    }
                    std::cout << "complete saving img" << std::endl;

                    std::cout << "start marking log" << std::endl;
                    // marking log. 
                    int age_int = static_cast<int>(age_float);
                    //inputFileStr = frameCount + "," + personId + "," + age_int + "," + gender + "\n";                    
                    inputFileStr.append(std::to_string(frameCounting));     // frame
                    inputFileStr.push_back(',');
                    // ID ( person-reidentification-retail-0270 )
                    inputFileStr.append(personId);      // id
                    // Face detect ( age-gender-recognition-retail-0013 )
                    inputFileStr.push_back(',');
                    if (age_int == 0)
                        inputFileStr.append("0");       // age(undefined)
                    else
                        inputFileStr.append(std::to_string(age_int));       // age
                    inputFileStr.push_back(',');
                    if (gender.empty())
                        inputFileStr.append("'undefined'");   // gender (undefined)
                    else {
                        std::string strr = "'" + gender + "'";
                        inputFileStr.append(strr);        // gender
                    }
                    // Person Attributes ( person-attributes-recognition-crossroad-0230 )
                    inputFileStr.push_back(',');
                    //if((face.x == 0 && face.y == 0) && (face.width == 0 && face.height == 0))
                    inputFileStr.append(std::to_string(personAttributes[0]));        // is_male
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[1]));       // has_bag
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[2]));       // has_backpack
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[3]));       // has_hat
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[4]));       // has_longsleeves
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[5]));       // has_longpants
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[6]));       // has_longhair
                    inputFileStr.push_back(',');
                    inputFileStr.append(std::to_string(personAttributes[7]));       // has_coat_jacket
                    inputFileStr.push_back(',');
                    //img
                    std::string img_person_src = "'C:/coding/person_detection_test/person_detection_test/img/person_" + personId + "_" + std::to_string(frameCounting) + ".bmp" + "'";
                    inputFileStr.append(img_person_src);       // img_person
                    inputFileStr.push_back(',');
                    std::string img_face_src = "'C:/coding/person_detection_test/person_detection_test/img/face_" + personId + "_" + std::to_string(frameCounting) + ".bmp" + "'";
                    inputFileStr.append(img_face_src);       // img_face
                    inputFileStr.push_back('\n');

                    videologFile.write(inputFileStr.c_str(), inputFileStr.size());      // write a log with a line(one frame) in videolog

                    std::cout << "complete marking log" << std::endl;
                }
            }
        }

        std::cout << "start showing proc" << std::endl;
        for (int i = 0; i < peopleRect.size(); i++) {
            if (!peopleId[i].empty()) {
                cv::putText(frame,
                    peopleId[i],
                    cv::Point2f(static_cast<float>(peopleRect[i].x), static_cast<float>(peopleRect[i].y + 30)),
                    cv::FONT_HERSHEY_COMPLEX,
                    0.5,
                    cv::Scalar(0, 0, 255), 1.3);
            }
            cv::rectangle(frame, peopleRect[i], cv::Scalar(0, 0, 255), 1.5);
            //-------------------------------색상 들어가는 곳-------------------------------//
            std::cout << "marking start" << std::endl;
            std::cout << i << " top coordinate: " << peopleColor_point[i].first.x << ", " << peopleColor_point[i].first.y << std::endl;
            std::cout << i << " bottom coordinate: " << peopleColor_point[i].second.x << ", " << peopleColor_point[i].second.y << std::endl;
            
            //-------------------------------색상 들어가는 곳-------------------------------//
            std::cout << "start color" << std::endl;
            ntblue = frame.at<cv::Vec3b>(peopleColor_point[i].first.x, peopleColor_point[i].first.y)[0];
            ntgreen = frame.at<cv::Vec3b>(peopleColor_point[i].first.x, peopleColor_point[i].first.y)[1];
            ntred = frame.at<cv::Vec3b>(peopleColor_point[i].first.x, peopleColor_point[i].first.y)[2];

            nbblue = frame.at<cv::Vec3b>(peopleColor_point[i].second.x, peopleColor_point[i].second.y)[0];
            nbgreen = frame.at<cv::Vec3b>(peopleColor_point[i].second.x, peopleColor_point[i].second.y)[1];
            nbred = frame.at<cv::Vec3b>(peopleColor_point[i].second.x, peopleColor_point[i].second.y)[2];
            /*---------------------------------------------------------------------------------------------------*/
            std::cout << "complete" << std::endl;
            

            std::cout << "=====================Comparison value======================" << std::endl;
            std::cout << "top | my : " << cv::Scalar(ntblue, ntgreen, ntred) << ", vec3b : " << cv::Scalar(peopleColor_vec3b[i].first) <<std::endl;
            std::cout << "bot | my : " << cv::Scalar(nbblue, nbgreen, nbred) << ", vec3b : " << cv::Scalar(peopleColor_vec3b[i].second) << std::endl;
            std::cout << "===========================================================" << std::endl;
            
            
            cv::circle(frame, peopleColor_point[i].first, 0, cv::Scalar(peopleColor_vec3b[i].first), 5); // top color
            cv::circle(frame, peopleColor_point[i].second, 0, cv::Scalar(peopleColor_vec3b[i].second), 5); // bottom color
            std::cout << "marking complete" << std::endl;
                                                                                                
            //-----------------------------------------------------------------------------//
            if (faceRect[i].x != 0 && faceRect[i].y != 0) {
                if (ageGender[i].second == "female")
                    cv::rectangle(frame, faceRect[i], cv::Scalar(127, 0, 255), 1.5);
                else
                    cv::rectangle(frame, faceRect[i], cv::Scalar(255, 84, 0), 1.5);
                cv::putText(frame,
                    ageGender[i].first,
                    cv::Point2f(static_cast<float>(faceRect[i].x), static_cast<float>(faceRect[i].y + faceRect[i].height + 30)),
                    cv::FONT_HERSHEY_COMPLEX,
                    0.5,
                    cv::Scalar(0, 255, 0), 1.3);
            }
        }

        // show live and wait for a key with timeout long enough to show images
        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }

    // close videolog text file
    videologFile.close();

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}


//using opencv lib to get total video frame and time for millisecond
//fps : 30
//total frame : 6828
//git branch develop master