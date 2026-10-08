#include "tmml_cl.hpp"

using namespace std;
using namespace cv;
using namespace cl;

tmml_cl::tmml_cl(bool& ok, float& min_max_Val)
{
    cout << "START Constructor tmml_cl\n";
    initDevice(ok);
    if(!ok){cout << "error initDevice!!!\n"; return;}
    loadAndBuildProgram(ok);
    if(!ok){cout << "error loadAndBuildProgram!!!\n"; return;}
    img_work_buff = Buffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(unsigned char) * WORK_AREA);
    img_temp_buff = Buffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(unsigned char) * TEMPLATE_AREA);
    img_result_buff = Buffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(float) * RESULT_AREA);
    maxVal_int_buff = Buffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(int));
    max_pix_buff = Buffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(Pix));
    int clError1 = 0;
    tmml_cl_kernel1 = Kernel(program, KERNEL_tmml_cl_NAME1, &clError1);
    if(clError1 != 0)
    {
        cout << "clError1=" << clError1 << "; KERNEL_tmml_cl_NAME1=" << KERNEL_tmml_cl_NAME1 << " =============== !!!" << endl;
        ok = false;
        return;
    } // END if(clError1 != 0)
    tmml_cl_kernel1.setArg(0, img_work_buff);
    tmml_cl_kernel1.setArg(1, img_temp_buff);
    tmml_cl_kernel1.setArg(2, img_result_buff);
    tmml_cl_kernel1.setArg(3, maxVal_int_buff);
    tmml_cl_kernel1.setArg(4, max_pix_buff);
    cout << "Constructor tmml_cl ok=" << ok << endl;
} // END tmml_cl

void tmml_cl::CL_FREE()
{
    clReleaseKernel(tmml_cl_kernel1());
    clReleaseKernel(tmml_cl_kernel2());
    clReleaseCommandQueue(qu());
    clReleaseContext(context());
    clReleaseDevice(default_device());
    clReleaseProgram(program());
    all_devices.clear();
    all_platforms.clear();
    kernel_source = "";
    clReleaseMemObject(img_work_buff());
    clReleaseMemObject(img_temp_buff());
    clReleaseMemObject(img_result_buff());
    clReleaseMemObject(max_pix_buff());
    clReleaseMemObject(maxVal_int_buff());
} // END CL_FREE

tmml_cl::~tmml_cl()
{
    CL_FREE();
    cout << "Destructor tmml_cl\n";
} // END ~tmml_cl

void tmml_cl::work_tmml(const Mat & img_work, const Mat & img_temp, Pix & max_pix)
{
    qu.enqueueWriteBuffer(maxVal_int_buff, CL_TRUE, 0, sizeof(int), &maxVal_int);
    qu.enqueueWriteBuffer(img_work_buff, CL_TRUE, 0, sizeof(unsigned char) * WORK_AREA, &img_work.data[0]);
    qu.enqueueWriteBuffer(img_temp_buff, CL_TRUE, 0, sizeof(unsigned char) * TEMPLATE_AREA, &img_temp.data[0]);
    qu.enqueueNDRangeKernel(tmml_cl_kernel1, NullRange, NDRange(RESULT_AREA), NullRange);
    qu.finish();
    qu.enqueueReadBuffer(max_pix_buff, CL_TRUE, 0, sizeof(Pix), &max_pix);
#if defined(find_diff_result)
    qu.enqueueReadBuffer(img_result_buff, CL_TRUE, 0, sizeof(float) * RESULT_AREA, &img_result.data[0]);
#endif // END #if defined(find_diff_result)
} // END work_tmml

string tmml_cl::loadKernelFile(const string & program)
{
    ifstream f(program);
    return string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
} // END loadKernelFile

void tmml_cl::initDevice(bool & init_OK)
{
    Platform::get(&all_platforms);
    if(!all_platforms.size())
    {
        cout << "No platforms found. Check OpenCL installation!\n";
        init_OK = false;
    } // END if(!all_platforms.size())
    default_platform=Platform(all_platforms[0]);
    cout << "Using CL platform: \t" << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(!all_devices.size())
    {
        cout << "No devices found. Check OpenCL installation!\n";
        init_OK = false;
        return;
    } // END if(!all_devices.size())
    default_device = Device(all_devices[0]);
    cout << "Using CL device: \t" << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
    context = Context(default_device);
    qu = CommandQueue(context, default_device);
    init_OK = true;
} // END initDevice

void tmml_cl::loadAndBuildProgram(bool& init_OK)
{
    Program::Sources sources;
    string kernel_source; // = loadKernelFile(KERNEL_FILE);
    get_cl_string(kernel_source);

//    cout << "=========================== BEGIN kernel_source ===============================\n";
//    cout << kernel_source << endl;
//    cout << "========================== END kernel_source ================================\n";

    pair<const char*, ::size_t> src(kernel_source.c_str(), kernel_source.length());
    sources.push_back(src);
    program = Program(context, sources);
    vector<Device> devices;
    devices.push_back(default_device);
    if(program.build(devices) != CL_SUCCESS)
    {
        cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
        init_OK = false;
        return;
    } // END if(program.build(devices) != CL_SUCCESS)
    init_OK = true;
} // END loadAndBuildProgram
