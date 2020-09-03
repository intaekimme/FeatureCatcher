#pragma once
#include <mysql.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <future>
#include <Windows.h>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


class DataManager {

public:
	DataManager();
	~DataManager();

private:
	MYSQL* conn, mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	int query_state;

	

	std::ofstream videologFile;
	std::string inputFileStr;
	std::string logFileName;

	std::string server;
	unsigned int port;
	std::string user;
	std::string pw;
	std::string database;
	std::string table;

public:
	int process();
	int saveLog(std::string personId, std::string frameCount, std::string millisec, std::string age,
		std::string gender, std::string yourWebServerPath, cv::Vec3b clothe_top_color, cv::Vec3b clothe_bottom_color);
	int openLogFile(std::string logFileName);
	int savePersonImg(cv::Mat frame, std::string personId, std::string millisec, cv::Rect person, std::string yourWebServerPath);
	
	//DB
	void setInfo_DB(std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table);
	void send_insert_to_db(std::string table, std::vector<std::string> columns, std::vector<std::string> values);
	void readFile(std::string txtfile, std::string table, std::vector<std::string> columns, int columnSize);
	int saveLog2DB(std::string logFileName, std::string server, unsigned int port, std::string user, std::string pw, std::string database, std::string table);

};