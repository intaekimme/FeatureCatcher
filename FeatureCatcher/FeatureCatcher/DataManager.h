#pragma once
#include <mysql.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <future>

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

public:
	DataManager();
	int process();
	int saveLog();
	int openLogFile(std::string logFileName);
};