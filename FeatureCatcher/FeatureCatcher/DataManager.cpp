// DataManager.cpp
#include "DataManager.h"

DataManager::DataManager()
{

}

DataManager::~DataManager() {
    //delete DataManager;
}

int savePersonImg(cv::Mat frame, std::string personId, int frameCounting, cv::Rect person, cv::Rect face)
{
    /*--------------------------------------Saving Image--------------------------------------*/
                        //cv::Rect roi(cv::Point2f(x1, y1), cv::Point2f(x2, y2));
    cv::Mat3b crop = frame(person);
    cv::String srcImg_person = "img/person_" + personId + "_" + std::to_string(frameCounting) + ".bmp";
    cv::String srcImg_face = "";
    imwrite(srcImg_person, crop);
    if (!face.empty()) {
        cv::Mat3b crop = frame(face);
        srcImg_face = "img/face_" + personId + "_" + std::to_string(frameCounting) + ".bmp";
        imwrite(srcImg_face, crop);
    }
}

int DataManager::saveLog(int personId)
{
    /*-----------------------------------------Marking a Log in a text file-----------------------------------------*/
                        // ID ( person-reidentification-retail-0270 )
    inputFileStr.append(personId);                                  // id
    inputFileStr.push_back(',');
    inputFileStr.append(std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_FRAMES))));   // frame
    inputFileStr.push_back(',');
    inputFileStr.append(std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC))));     // millisec
    // Face detect ( age-gender-recognition-retail-0013 )
    inputFileStr.push_back(',');
    int age_int = static_cast<int>(age_float);
    if (age_int == 0)
        inputFileStr.append("0");                                   // age(undefined)
    else
        inputFileStr.append(std::to_string(age_int));               // age
    inputFileStr.push_back(',');
    if (gender.empty())
        inputFileStr.append("'undefined'");                         // gender (undefined)
    else {
        std::string strr = "'" + gender + "'";
        inputFileStr.append(strr);                                  // gender
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
    std::string img_person_src = "'C:/UIW/person_detection_test/person_detection_test/img/person_" + personId + "_" + std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC))) + ".bmp" + "'";
    inputFileStr.append(img_person_src);                            // img_person
    inputFileStr.push_back(',');
    std::string img_face_src = "'C:/UIW/person_detection_test/person_detection_test/img/face_" + personId + "_" + std::to_string(cvRound(cap.get(cv::CAP_PROP_POS_MSEC))) + ".bmp" + "'";
    inputFileStr.append(img_face_src);                              // img_face
    inputFileStr.push_back(',');
    inputFileStr.append(std::to_string(clothe_top_color[0] << 16 | clothe_top_color[1] << 8 | clothe_top_color[2]));            // top_color
    inputFileStr.push_back(',');
    inputFileStr.append(std::to_string(clothe_bottom_color[0] << 16 | clothe_bottom_color[1] << 8 | clothe_bottom_color[2]));   // bottom_color
    inputFileStr.push_back('\n');
    videologFile.write(inputFileStr.c_str(), inputFileStr.size());      // write a log with a line(one frame) in videolog.txt
}

int DataManager::openLogFile(std::string logFileName)
{
    // open videolog text file
    videologFile.open(logFileName);
    if (videologFile.is_open() == false) {
        std::cerr << "Couldn't open the videolog text file!\n";
        return -2;
    }
    else {
        std::cout << "Done opening the videolog text file\n\n";
    }
}