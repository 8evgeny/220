#include "autofocusmodel.h"
#include <fstream>
#include <sstream>
#include <cmath>

AutofocusModel::AutofocusModel(const std::string & file_name):
    file_name(file_name)
{
    load(file_name);
}
AutofocusModel::~AutofocusModel()
{
    temp_table.clear();
    zoom_table.clear();
    focus_table.clear();
}

int AutofocusModel::get_focus(const double & temp, const int & zoom)
{
    if(temp_table.size() == 0 || zoom_table.size() == 0 || focus_table.size() == 0) return -1;
    size_t temp_idx = temp_table.size() - 1;
    size_t zoom_idx = zoom_table.size() - 1;
    for(size_t i = 0; i < temp_table.size() - 1; ++i)
    {
        if(temp < temp_table[i])
        {
            temp_idx = 0;
            break;
        }
        else if(temp >= temp_table[i] && temp < temp_table[i + 1])
        {
            temp_idx = i;
            break;
        }
    }
    for(size_t i = 0; i < zoom_table.size() - 1; ++i)
    {
        if(zoom < zoom_table[i])
        {
            zoom_idx = 0;
            break;
        }
        else if(zoom >= zoom_table[i] && zoom < zoom_table[i + 1])
        {
            zoom_idx = i;
            break;
        }
    }
    double focus0 = -1;
    double focus1 = -1;
    double focus = -1;

    size_t zoom_idx_next = zoom_idx == (zoom_table.size()-1) ? zoom_idx : zoom_idx+1;
    size_t temp_idx_next = temp_idx == (temp_table.size()-1) ? temp_idx : temp_idx+1;

    focus0 = focus_table[zoom_idx][temp_idx] + double(zoom - zoom_table[zoom_idx]) * double(focus_table[zoom_idx_next][temp_idx] - focus_table[zoom_idx][temp_idx]) / double(zoom_table[zoom_idx_next] - zoom_table[zoom_idx]);
    focus1 = focus_table[zoom_idx][temp_idx_next] + double(zoom - zoom_table[zoom_idx]) * double(focus_table[zoom_idx_next][temp_idx_next] - focus_table[zoom_idx][temp_idx_next]) / double(zoom_table[zoom_idx_next] - zoom_table[zoom_idx]);
    focus = focus0 + double(temp - temp_table[temp_idx]) * double(focus1 - focus0) / double(temp_table[temp_idx_next] - temp_table[temp_idx]);
    return int(round(focus));
}

void AutofocusModel::load(const std::string & file_name)
{
    temp_table.clear();
    zoom_table.clear();
    focus_table.clear();
    std::ifstream config;
    config.open(file_name, std::ios::in);
    if(config.is_open())
    {
        std::string line;
        // First line - temperature
        if(getline(config, line))
        {
            std::stringstream ss(line);
            double temp;
            while(ss >> temp)
            {
                temp_table.push_back(temp);
            }
            // Next - table with zoom-focus
            while(getline(config, line))
            {
                std::stringstream ss(line);
                // First column - zoom
                int zoom;
                if(ss >> zoom)
                {
                    zoom_table.push_back(zoom);
                    // Next columns - focuses
                    int focus;
                    focus_table.push_back(std::vector<int>());
                    while(ss >> focus)
                    {
                        focus_table.back().push_back(focus);
                    }
                }
            }
        }
    }
}
void AutofocusModel::save(const std::string & file_name)
{
    std::ofstream config;
    config.open(file_name, std::ios::out);
    if(config.is_open())
    {
        // First line - temperature
        std::stringstream ss;
        for(size_t i = 0; i < temp_table.size(); ++i)
        {
            ss << temp_table[i] << "\t";
        }
        config << ss.str() << std::endl;
        // Next - table with zoom-focus
        for(size_t i = 0; i < zoom_table.size(); ++i)
        {
            std::stringstream ss;
            // First column - zoom
            ss << zoom_table[i] << "\t";
            // Next columns - focuses
            for(size_t j = 0; j < focus_table[i].size(); ++j)
            {
                ss << focus_table[i][j] << "\t";
            }
            config << ss.str() << std::endl;
        }
    }
}
