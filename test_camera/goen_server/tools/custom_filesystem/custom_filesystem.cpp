#include "custom_filesystem.hpp"

#include <dirent.h>
#include <fstream>
#include <algorithm>

using namespace custom_filesystem;

bool custom_filesystem::FileIsExist(const std::string& filePath)
{
    bool isExist = false;
    std::ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
} // -- END FileIsExist

bool custom_filesystem::dir_content(const std::string& fold,
                                    std::vector<std::string>& contentList,
                                    int typ)
{
    DIR *dir;
    if((dir = opendir(fold.c_str())) != NULL)
    {
        struct dirent *ent;
        while((ent = readdir(dir)) != NULL)
        {
            std::string fname = std::string(ent->d_name);
            // -- typ = 4 (folder), typ = 8 (file).
            if(ent->d_type == typ && fname != "." && fname != "..")
            {
                contentList.emplace_back(fname);
            }
        } // -- END while ((ent = readdir (dir)) != NULL)
        closedir(dir);
        std::sort(contentList.begin(), contentList.end());
        return 1;
    } // -- END if((dir = opendir(way)) != NULL)
    return 0;
} // -- END dir_content

void custom_filesystem::cleanDirectory(const std::string &dir,
                                       int maxNumberOfFolder)
{
    std::vector<std::string> contentList;
    bool dir_ok = dir_content(dir, contentList, 8);
    if(dir_ok && contentList.size() > maxNumberOfFolder)
    {
        for(size_t i = 0; i < contentList.size() - maxNumberOfFolder; i++)
        {
            std::string s1 = "rm " + dir + "/" + contentList[i];
            int report_clean_dir = system(s1.c_str());
        } // -- END for(size_t i = 0; i < contentList.size() - maxNumberOfFolder; i++)
    } // -- END if(dir_ok && contentList.size() > maxNumberOfFolder)
} // -- END cleanLogDirectory

bool custom_filesystem::getFileList(const std::string& path,
                                    std::vector<std::string>& fileList)
{
    if(dir_content(path, fileList, 8))
    {
        for(auto &file : fileList)
        {
            file = path + "/" + file;
        }
        return true;
    }
    return false;
} // -- END getFileList
