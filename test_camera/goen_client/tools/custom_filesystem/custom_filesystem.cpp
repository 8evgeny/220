#include "custom_filesystem.hpp"

#ifndef USE_FILESYSTEM
#include <dirent.h>
#include <fstream>
#endif
#include <algorithm>

using namespace custom_filesystem;

#ifndef USE_FILESYSTEM
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
#endif

#ifdef USE_FILESYSTEM
bool custom_filesystem::dir_content(const std::string& fold,
                                    std::vector<std::string>& contentList,
                                    int typ)
{
    contentList.reserve(100);
    for (auto const& dir_entry : std::filesystem::directory_iterator{fold})
    {
        contentList.push_back(dir_entry.path().filename());
    }
    contentList.shrink_to_fit();
    if(!contentList.empty())
    {
        std::sort(contentList.begin(), contentList.end());
        return true;
    }
    return false;
}
#endif

#ifdef USE_FILESYSTEM
void custom_filesystem::cleanDirectory(const std::string &dir,
                                       int maxNumberOfFolder)
{
    if(std::filesystem::exists(dir))
    {
        std::vector<std::string> contentList;
        dir_content(dir, contentList);
        if(contentList.size() > maxNumberOfFolder)
        {
            for(size_t i = 0; i < contentList.size() - maxNumberOfFolder; i++)
            {
                std::filesystem::remove_all(contentList.at(i));
            }
        }
    }
}
#else
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
#endif

#ifdef USE_FILESYSTEM
bool custom_filesystem::getFileList(const std::string& path,
                                    std::vector<std::string>& fileList)
{
    fileList.reserve(100);
    for (auto const& dir_entry : std::filesystem::directory_iterator{path})
    {
        fileList.push_back(dir_entry.path());
    }
    fileList.shrink_to_fit();
    if(!fileList.empty())
    {
        std::sort(fileList.begin(), fileList.end());
        return true;
    }
    return false;
}
#else
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
#endif
