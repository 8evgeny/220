#ifndef AUTOFOCUSMODEL_H
#define AUTOFOCUSMODEL_H

#include <string>
#include <map>
#include <vector>

//#define CORSAIR_SET_1
#define CORSAIR_SET_2
//#define CORSAIR_SET_3

class AutofocusModel
{
public:
    AutofocusModel(const std::string & file_name);
    ~AutofocusModel();

    int get_focus(const double & temp, const int & zoom);

private:
    void load(const std::string & file_name);
    void save(const std::string & file_name);

private:
    std::string file_name;
    std::vector<double> temp_table;
    std::vector<int> zoom_table;
    std::vector<std::vector<int>> focus_table;
};

#endif // AUTOFOCUSMODEL_H
