#include <iostream>
#include <unistd.h>
#include "FeatureCatcher.h"

std::string parse_last(std::string str, std::string delimiter)
{
    int pos = 0;
    std::string token;    
    while((pos = str.find(delimiter)) != std::string::npos){
        
        token = str.substr(0, pos);
        str.erase(0, pos + delimiter.length());
    }
    return str;
}

std::string parse_first(std::string str, std::string delimiter)
{
    int pos = 0;
    std::string token;        
    pos = str.find(delimiter);
    return str.substr(0, pos);
}

std::string get_table_name(std::string path)
{
    std::string delimiter("/");
    std::string delimiter2(".");
    std::string firstParseStr = parse_last(path, delimiter);
    return parse_first(firstParseStr, delimiter2);
}

int main()
{

    std::string table;
    std::string videoPath;
    std::string db_server;
    std::string img_saving_path;
    std::string db_user;
    std::string db_database = "featurecatcher";
    unsigned int db_port;
    short processInterval;
    int framePerSaving;

    std::cout << "server: ";
    std::cin >> db_server;
    std::cout << "port: ";
    std::cin >> db_port;
    std::cout << "user: ";
    std::cin >> db_user;
    std::cout << "detected person image saving path: ";
    std::cin >> img_saving_path;
    std::cout << "processing interval: ";
    std::cin >> processInterval;
    std::cout << "process per saving: ";
    std::cin >> framePerSaving;

    DataManager *dm = new DataManager();
    dm->setInfo_DB(db_server, db_port, db_user, "", db_database, table);
    dm->ConnectDB(db_server, db_port, db_user, "", db_database);

    while (1)
    {
        usleep(2000000); // sleep
        

        std::vector<std::string> id_and_path = dm->select_video_list();

        if (id_and_path[0] != "false")  
        {
            // get table name
            std::string table_name = "";
            table_name.append(get_table_name(id_and_path[1]));
            dm->setInfo_table_DB(table_name);

            std::cout << id_and_path[1] << "\n" << processInterval << "\n" << img_saving_path << "\n" <<
            db_server << "\n" << db_port << "\n" << db_user << "\n" <<
            db_database << "\n" << table_name << "\n" << framePerSaving << "\n" <<
            id_and_path[0] << "\n";            

            FeatureCatcher FC(id_and_path[1], processInterval,
                              img_saving_path,
                              db_server, db_port, db_user, "", db_database,
                              table_name, framePerSaving, true, *dm);
            
            FC.process();
            dm->update_is_processed(id_and_path[0]);

            std::cout << table_name << "\n";
        }
    }

    dm->CloseDB();
}