#pragma once

#include <iostream> //
#include <fstream>  //
#include <ctime>    //
#include <iomanip>  // for pit_time
#include <sstream>  // for stringstream
#include "INIReader.h"
#include <chrono>   //

class LogCmd
{
public:
    LogCmd(const std::string & config_path, const std::string & section, bool & ok)
    {
        ok = get_ini_params(config_path, section);
        if(!ok) {std::cout << "ERROR create LogCmd object!" << std::endl;}
    } // -- END LogCmd

    void start()
    {
        time_t now = time(0);
        std::cout << "LogCmd::start::START DATE and TIME: " << ctime(&now) << std::endl;
        std::string str = "START PROGRAMM AT " + (std::string)ctime(&now);
        std::string filename = (std::string)ctime(&now);
        filename.pop_back();
        std::string path_f = path + "log" + filename + ".csv";
        file = std::ofstream(path_f, std::ios::app);
        if(file.is_open())
        {
            std::cout << str << std::endl;
//            file << str << " \n";
        } // END if (file.is_open())
        else
        {
            std::cout << "File NOT open!!!" << std::endl;
        } // END if (!file.is_open())
    } // -- END start

    void quit()
    {
        time_t now = time(0);
        std::cout << "LogCmd::start::QUIT DATE and TIME: " << ctime(&now) << std::endl;
        std::string str = "QUIT PROGRAMM AT " + (string)ctime(&now);
        file << " " << str << " \n";
        file.close();
    } // END quit

    void log_(std::string & str, bool f_time = true)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm *tm_now = std::localtime(&now_time);
        std::stringstream time_s ;
//        time_s << std::put_time(tm_now, "%H.%M.%S") << '.' << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        time_s << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() * 0.001;
        if(f_time) {file <<  time_s.str() <<  " ";}
        file << str << " \n";
    } // -- END log

private:
    std::string config_path = "../config.ini";
    std::string path = "../log/";
    std::ofstream file; // ("example.txt", ios::app);
    bool get_ini_params(const std::string& config, const std::string & block_name)
    {
        std::cout << "BEGIN get_ini_params LogCmd" << std::endl;
        setlocale(LC_NUMERIC, "en_US.UTF-8");
        bool configFileExists = FileIsExist(config);
        if(!configFileExists)
        {std::cout << "Config file '" << config << "' not exist!" << std::endl; return 0;}

        INIReader reader(config);
        if(reader.ParseError() < 0){std::cout << "Can't load '" << config << "'\n"; return 0;}
        std::cout << "\n[" << block_name << "]:" << std::endl;
        path = reader.Get(block_name, "path", "oops");
        if(path == "oops"){std::cout << "\tpath not declared\n"; return 0;}
        std::cout << "\tpath = " << path << ";\n";

        std::cout << "END get_ini_params LogCmd\n" << std::endl;
        return 1;
    } // -- END get_ini_params

    bool FileIsExist(const std::string& filePath)
    {
        bool isExist = false;
        std::ifstream fin(filePath.c_str());
        if(fin.is_open()){isExist = true;}
        fin.close();
        return isExist;
    } // -- END FileIsExist

}; // END class LogCmd
