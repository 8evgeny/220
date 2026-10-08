#ifndef CUSTOM_FILESYSTEM_H
#define CUSTOM_FILESYSTEM_H

#ifdef USE_FILESYSTEM
#include <filesystem>
#endif
#include <string>
#include <vector>

namespace custom_filesystem
{
bool FileIsExist(const std::string& filePath);
bool dir_content(const std::string& fold,
                 std::vector<std::string>& contentList,
                 int typ = 8);
void cleanDirectory(const std::string &dir, int maxNumberOfFolder = 0);
bool getFileList(const std::string& path, std::vector<std::string>& fileList);
}

#endif // CUSTOM_FILESYSTEM_H
