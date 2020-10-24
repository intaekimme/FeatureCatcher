// DataManager.cpp
#include "DataManager.h"


DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

// make directory to save person_img.
void DataManager::makeDirectory(std::string dirname, std::string tablename)
{
    std::string command = "";
    command.append("mkdir ");
    command.append(dirname);
    command.append("/");
    command.append(tablename);

    const char* cmd = command.c_str();
    system(cmd);                            // make dir "input dir path/table name"

    command.append("/");
    command.append("person_img");
    const char* cmd2 = command.c_str();
    system(cmd2);                            // make dir "input dir path/table name/person_img"
}

// Save person image to 'img' file.
int DataManager::savePersonImg(cv::Mat frame, std::string personId, std::string millisec, cv::Rect person, std::string yourWebServerPath)
{
    cv::Mat3b crop = frame(person);
    //cv::String srcImg_person = yourWebServerPath + "/img_person/person_" + personId + "_" + millisec + ".bmp";
    
    // make directory to save
    makeDirectory(yourWebServerPath, table);
    
    cv::String srcImg_person = "";
    srcImg_person.append(yourWebServerPath);
    srcImg_person.append("/");
    srcImg_person.append(table);
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

/// set MySQL DB info (server, port, user, password, database, table)
void DataManager::setInfo_DB(std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table)
{
    this->server = server;
    this->port = port;
    this->user = user;
    this->pw = pw;
    this->database = database;
    this->table = table;
}

void DataManager::setInfo_table_DB(std::string table)
{
    this->table = table;
}

// connect to MySQL
int DataManager::ConnectDB(std::string server, unsigned int port, std::string user, std::string pw, std::string database)
{

    std::cout << mysql_get_client_info();
    mysql_init(&mysql);
    conn = mysql_real_connect(&mysql, server.c_str(), user.c_str(), pw.c_str(), database.c_str(), port, 0, 0);
    if (conn == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl
                  << std::endl;
        return 1;
    }
    std::cout << "DB connected.\n";
    return 0;
}

// close MySQL connect.
void DataManager::CloseDB()
{
    mysql_free_result(res);
    mysql_close(conn);
    std::cout << "DB closed." << std::endl;
}

// send INSERT message to MySQL
void DataManager::log_to_DB(std::string table, std::string personId, std::string frameCount, std::string millisec, std::string age,
                            std::string gender, std::string yourWebServerPath, cv::Vec3b clothe_top_color, cv::Vec3b clothe_bottom_color)
{

    // table name
    std::string msg_insert = "INSERT INTO " + table + " (";
    // table colums
    msg_insert.append("person_id, frame, millisec, age, gender, img_person, top_color, bottom_color) ");
    // [values]
    msg_insert.append("VALUES (");
    // id (person ID number)
    if (personId == "\0")
        msg_insert.append("-1");
    else
        msg_insert.append(personId);
    msg_insert.append(", ");
    // frame (counting frames)
    msg_insert.append(frameCount);
    msg_insert.append(", ");
    // millisec (counting seconds)
    msg_insert.append(millisec);
    msg_insert.append(", ");
    // age
    if (age == "")
        msg_insert.append("0");
    else
        msg_insert.append(age);
    msg_insert.append(", ");
    // gender
    if (gender.empty())
        msg_insert.append("'undefined'");
    else
    {
        std::string strr = "'" + gender + "'";
        msg_insert.append(strr);
    }
    msg_insert.append(", ");
    // img_person ( path of img_person )
    std::string img_person_src = "'" + yourWebServerPath + "/person_img/person_" + personId + "_" + millisec + ".bmp'";
    msg_insert.append(img_person_src); // img_person
    msg_insert.push_back(',');
    // top_color ( rgb color value of top )
    msg_insert.append(std::to_string(clothe_top_color[0] << 16 | clothe_top_color[1] << 8 | clothe_top_color[2])); // top_color
    msg_insert.push_back(',');
    // bottom_color ( rgb color value of bottom )
    msg_insert.append(std::to_string(clothe_bottom_color[0] << 16 | clothe_bottom_color[1] << 8 | clothe_bottom_color[2])); // bottom_color
    msg_insert.append(")");

    std::cout << std::endl
              << msg_insert << std::endl; // print msg_insert string.

    const char *query = msg_insert.c_str(); // query_state

    query_state = mysql_query(conn, query);
    if (query_state != 0)
    {
        std::cout << mysql_error(conn) << std::endl
                  << std::endl;
        return;
    }
}

// create table on MySQL accoring to 'tableName'.
void DataManager::createTable(std::string tableName)
{

    std::string msg_create_table = "";

    msg_create_table.append("CREATE TABLE ");
    msg_create_table.append(tableName);
    msg_create_table.append(" (id INT auto_increment primary key, person_id INT not null, frame INT not null, millisec INT not null, age INT not null, gender VARCHAR(10) not null, img_person VARCHAR(100) not null, top_color INT not null, bottom_color INT not null) ");

    std::cout
        << std::endl
        << msg_create_table << std::endl; // print msg_insert string.

    const char *query = msg_create_table.c_str(); // query_state

    query_state = mysql_query(conn, query);
    if (query_state != 0)
    {
        std::cout << mysql_error(conn) << std::endl
                  << std::endl;
        return;
    }
}

std::vector<std::string> DataManager::select_video_list()
{
    // val - video_list
    std::string vl_id;
    std::string vl_video_name;
    std::string vl_is_processed;
    std::vector<std::string> id_and_path;

    int state = mysql_query(conn, "SELECT id, video_name, is_processed FROM video_list");
    if (state != 0)
    {
        std::cout << mysql_error(conn);
        //return 1;
    }

    /* must call mysql_store_result() before we can issue any * other query calls */
    res = mysql_store_result(conn);
    std::cout << "Rows: " << mysql_num_rows(res) << "\n";

    /* process each row in the result set */
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        vl_id = row[0];
        vl_video_name = row[1];
        vl_is_processed = row[2];

        //printf("id: %s, val: %s\n", (row[0] ? row[0] : "NULL"), (row[1] ? row[1] : "NULL"));
        std::cout << "id: " << vl_id << "video_name: " << vl_video_name << "is_processed: " << vl_is_processed << "\n";

        // if 'is_processed' is 0,
        if (vl_is_processed == "0")
        {
            id_and_path.push_back(vl_id);
            id_and_path.push_back(vl_video_name);
            // FC
            return id_and_path;

            // ------------> update
            std::cout << "make FeatureCatcher instance and call process function.\n";
            std::cout << "r is : " << vl_video_name << " and " << vl_id << "\n";
            std::cout << "then, UPDATE isprocessed col to 1\n";
        }
    }

    /* free the result set */
    mysql_free_result(res);

    // when 'is_processed' is not detected as '0'
    id_and_path.push_back("false");
    id_and_path.push_back("false");

    return id_and_path;
}

void DataManager::update_is_processed(std::string vl_id)
{
    std::string str_msg = "UPDATE video_list SET is_processed=1 WHERE id=";
    str_msg.append(vl_id);

    const char *msg = str_msg.c_str();
    std::cout << msg << std::endl;
    int query_state = mysql_query(conn, msg);
    if (query_state != 0)
    {
        std::cout << mysql_error(conn) << std::endl
                  << std::endl;
    }
}

std::string DataManager::getTableName()
{
    return this->table;
}

void DataManager::setTableName(std::string tableName)
{
    this->table = tableName;
}