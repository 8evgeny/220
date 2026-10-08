#include "gst_pipeline_device_factory.hpp"

using namespace std;
using namespace devices;

std::string devices::gst_pipeline_device::readSettings(const std::string &pathToSettings,
                                                       const std::string &ini_section_name,
                                                       bool &success)
{
    success = true;
    cout << "Begin readSettings in section [" << ini_section_name << "]\n";
    INIReader reader(pathToSettings);
    if (reader.ParseError() < 0)
    {
        cout << "ini reader parse error!\n";
        return nullptr;
    }  // END if (reader.ParseError() < 0)

    //    namespace  ini = toolbox_utils::ini_reader_tool;

    cout << "dbg:: ini_section_name = " << ini_section_name << endl;


    ///SETTINGS.src
    std::string pipeline = "", src = "", bin = "", sink = "", sink2 = "";

    src = reader.Get(ini_section_name, "src", "oops");
    if(src == "oops") {cout << "Not found \"src\" in [" << ini_section_name << "]" << endl; success = 0;}
    else cout << "src = " << src << endl;

    bin = reader.Get(ini_section_name, "bin", "oops");
    if(bin == "oops") {cout << "Not found \"bin\" in [" << ini_section_name << "]" << endl; success = 0;}
    else cout << "bin = " << bin << endl;

    sink = reader.Get(ini_section_name, "sink", "oops");
    if(sink == "oops") {cout << "Not found \"sink\" in [" << ini_section_name << "]" << endl; success = 0;}
    else cout << "sink = " << sink << endl;

//    sink2 = reader.Get(ini_section_name, "sink2", "oops");
//    if(sink2 == "oops") {cout << "Not found \"sink2\" in [" << ini_section_name << "]" << endl; success = 0;}
//    else cout << "sink2 = " << sink2 << endl;

    pipeline = src + " ! " +  bin + " ! " + sink ;

    return pipeline;
} // -- END readSettings


std::shared_ptr<GST_PIPELINE_device> devices::gst_pipeline_device::create(const std::string &config_path, bool & ok, const std::string &ini_section_name, const std::string & location)
{
    bool success_rd_ini = false;
    std::string pipeline = readSettings(config_path, ini_section_name, success_rd_ini);
    cout << "location: " << location << endl;
    if(location != "location=")
    {
        cout << "pipeline = " << pipeline << endl;
        cout << "Update pipeline! New rtsp location = " << location << endl;

        int start_location_id;
        int end_location_id;
        start_location_id = pipeline.find("location=");

        for(int i = start_location_id; ; i++)
        {
            if(pipeline[start_location_id] == ' ' ) {break;}
            pipeline.erase(start_location_id, 1);
        } // END for(int i = start_location_id; ; i++)

        cout << "new location = " << location << endl;
        pipeline.insert(start_location_id, location);
        cout << "new pipeline = " << pipeline << endl;

    } // END if location != "location"
    if(!success_rd_ini)
    {
        ok = false;
        return nullptr;
    }  // END if(!success_rd_ini)
    else
    {
        ok = true;
    }
    shared_ptr<devices::GST_PIPELINE_device> gst_pipeline_device = make_shared<devices::GST_PIPELINE_device>(pipeline);
    return gst_pipeline_device;
} // -- END create
