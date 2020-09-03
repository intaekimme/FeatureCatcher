// DataManager.cpp
#include "DataManager.h"

DataManager::DataManager()
{
    //setInfo_DB(server, port, user, pw, database, table);
}

DataManager::~DataManager() {
    //delete DataManager;
}

// Save person image to 'img' file.
int DataManager::savePersonImg(cv::Mat frame, std::string personId, std::string millisec, cv::Rect person, std::string yourWebServerPath)
{
    
        cv::Mat3b crop = frame(person);
        //cv::String srcImg_person = yourWebServerPath + "/img_person/person_" + personId + "_" + millisec + ".bmp";
        cv::String srcImg_person = "";
        srcImg_person.append(yourWebServerPath);
        srcImg_person.append("/person_img/person_");
        
        srcImg_person.append(personId);
        srcImg_person.append("_");
        srcImg_person.append(millisec);
        srcImg_person.append(".bmp");
        cv::String srcImg_face = "";
        cv::imwrite(srcImg_person, crop);
        
        /* if (!face.empty()) {
             cv::Mat3b crop = frame(face);
             srcImg_face = "img/face_" + personId + "_" + std::to_string(millisec) + ".bmp";
             imwrite(srcImg_face, crop);
         }*/
    

    return 0;
}

int DataManager::saveLog(std::string personId, std::string frameCount, std::string millisec, std::string age, 
    std::string gender, std::string yourWebServerPath, cv::Vec3b clothe_top_color, cv::Vec3b clothe_bottom_color)
{
    // ID ( person-reidentification-retail-0270 )
    inputFileStr.append(personId);                                  // id
    inputFileStr.push_back(',');
    inputFileStr.append(frameCount);   // frame
    inputFileStr.push_back(',');
    inputFileStr.append(millisec);     // millisec
    // Face detect ( age-gender-recognition-retail-0013 )
    inputFileStr.push_back(',');
    if(age == "")
        inputFileStr.append("0");
    else
        inputFileStr.append(age);                                       // age    
    inputFileStr.push_back(',');
    if (gender.empty())
        inputFileStr.append("'undefined'");                         // gender (undefined)
    else {
        std::string strr = "'" + gender + "'";
        inputFileStr.append(strr);                                  // gender
    }
    // Person Attributes ( person-attributes-recognition-crossroad-0230 )
    inputFileStr.push_back(',');
    //img
    std::string img_person_src = "'" + yourWebServerPath + "/person_" + personId + "_" + millisec + ".bmp'";
    inputFileStr.append(img_person_src);                            // img_person
    inputFileStr.push_back(',');
    inputFileStr.append(std::to_string(clothe_top_color[0] << 16 | clothe_top_color[1] << 8 | clothe_top_color[2]));            // top_color
    inputFileStr.push_back(',');
    inputFileStr.append(std::to_string(clothe_bottom_color[0] << 16 | clothe_bottom_color[1] << 8 | clothe_bottom_color[2]));   // bottom_color
    inputFileStr.push_back('\n');
    videologFile.write(inputFileStr.c_str(), inputFileStr.size());      // write a log with a line(one frame) in videolog.txt

    return 0;
}

// open txt log file.
int DataManager::openLogFile(std::string logFileName)
{
    // open videolog text file
    videologFile.open(logFileName);
    DataManager::logFileName = logFileName;
    if (videologFile.is_open() == false) {
        std::cerr << "Couldn't open the videolog text file!\n";
        return -2;
    }
    else {
        std::cout << "Done opening the videolog text file\n\n";
    }
}


/// DB
void DataManager::setInfo_DB(std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table)
{
    DataManager::server = server;
    DataManager::port = port;
    DataManager::user = user;
    DataManager::pw = pw;
    DataManager::database = database;
    DataManager::table = table;
}

void DataManager::send_insert_to_db(std::string table, std::vector<std::string> columns, std::vector<std::string> values)
{
    //string msg = "INSERT INTO "+ table + " (frame, id, age, gender, is_male, has_bag, has_bagpack, has_hat, has_longsleeves, has_pants, has_longhair, has_coatjacket) VALUES('" + to_string(frame) + "', '" + to_string(id) + "', '" + to_string(bb_left) + "', '" + to_string(bb_top) + "', '" + to_string(bb_width) + "', '" + to_string(bb_height) + "')";

    std::string msg_insert = "INSERT INTO " + table + " (";
    for (int i = 0; i < columns.size() - 1; i++) {
        msg_insert.append(columns[i]);
        msg_insert.append(", ");
    }
    msg_insert.append(columns[columns.size() - 1]);
    msg_insert.append(") values(");
    for (int i = 0; i < values.size() - 1; i++) {
        msg_insert.append(values[i]);
        msg_insert.append(", ");
    }
    msg_insert.append(values[values.size() - 1]);
    msg_insert.append(")");

    std::cout << std::endl << msg_insert << std::endl;

    const char* query = msg_insert.c_str(); // query_state
    query_state = mysql_query(conn, query);
    if (query_state != 0) {
        std::cout << mysql_error(conn) << std::endl << std::endl;
        return;
    }
}

// reading text log file and send INSERT command with information of it.
void DataManager::readFile(std::string txtfile, std::string table, std::vector<std::string> columns, int columnSize)
{
    std::ifstream infile;
    infile.open(txtfile.c_str());
    if (infile.fail()) {
        std::cout << "ERROR. Could not open file!" << std::endl;
        return;
    }
    infile.seekg(0, infile.end);
    int len = infile.tellg();
    infile.seekg(0, infile.beg);
    std::vector<std::string> contents;

    // split and send "INSERT" command.
    while (infile) {
        contents.clear();
        std::string str = "'";
        infile >> str;
        //str.append("'");
        contents.push_back(str);
        send_insert_to_db(table, columns, contents);
    }
}

// save log to DB
int DataManager::saveLog2DB(std::string logFileName, std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table)
{
    // connect mysql
    std::cout << mysql_get_client_info();

    const char* query_state_query = "select * from table_videoanalyzing"; // query_state

    // db 연결 및 에러확인
    mysql_init(&mysql);
    conn = mysql_real_connect(&mysql, server.c_str(), user.c_str(), pw.c_str(), database.c_str(), port, 0, 0);
    if (conn == NULL) {
        std::cout << mysql_error(&mysql) << std::endl << std::endl;
        return 1;
    }

    // 비동기
    //future<void> check = std::async(launch::async, signal_cheking);

    // set colums
    std::vector<std::string> columns;
    columns.push_back("id");				//1: id
    columns.push_back("frame");				//2: frame
    columns.push_back("millisec");			//3: millisec
    columns.push_back("age");
    columns.push_back("gender");
    //columns.push_back("is_male");
    //columns.push_back("has_bag");
    //columns.push_back("has_backpack");
    //columns.push_back("has_hat");
    //columns.push_back("has_longsleeves");	//10: has_longsleeves
    //columns.push_back("has_longpants");
    //columns.push_back("has_longhair");
    //columns.push_back("has_coatjacket");
    columns.push_back("img_person");
    //columns.push_back("img_face");
    columns.push_back("top_color");
    columns.push_back("bottom_color");		//17: bottom_color

    // txtfile -> mysql
    readFile(logFileName, table, columns, 8);

    mysql_free_result(res);
    mysql_close(conn);
    std::cout << "saveLog2DB done" << std::endl;
}

