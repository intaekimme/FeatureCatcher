#ifndef MODELS_H
#define MODELS_H

std::string persondetectorConfigFile = "models/person-detection-0102.xml";
std::string personDetectorFile = "models/person-detection-0102.bin";

std::string personIdentifierConfigFile = "models/person-reidentification-retail-0270.xml";
std::string personIdentifierFile = "models/person-reidentification-retail-0270.bin";

std::string faceDetectorConfigFile = "models/deploy.prototxt";
std::string faceDetectorFile = "models/res10_300x300_ssd_iter_140000_fp16.caffemodel";

std::string ageGenderRecognizerConfigFile = "models/age-gender-recognition-retail-0013.xml";
std::string ageGenderRecognizerFile = "models/age-gender-recognition-retail-0013.bin";

std::string attributesRecognizerConfigFile = "models/person-attributes-recognition-crossroad-0230.xml";
std::string attributesRecognizerFile = "models/person-attributes-recognition-crossroad-0230.bin";

#endif