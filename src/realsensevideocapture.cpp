#include "realsensevideocapture.h"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/opencv.hpp"

RealsenseVideoCapture::RealsenseVideoCapture()
    :device(nullptr), width(0), height(0), fps(30)
{
    ctx.reset(new rs::context());
}

RealsenseVideoCapture::~RealsenseVideoCapture()
{
    std::cout<<"closing camera... "<<std::endl;
    device->stop();
}

void RealsenseVideoCapture::stop()
{
    if (device)
    {
        device->stop();
    }
}

bool RealsenseVideoCapture::open(int device_number)
{
    try {
        std::cout<<"opening camera... "<<device_number<<std::endl;
        if (ctx->get_device_count() == 0)
        {
            std::cerr << "error: cant find devices" << std::endl;
            return false;
        }

        device = ctx->get_device(device_number);

        //device->enable_stream(rs::stream::depth, this->width, this->height, rs::format::z16, 60);
        device->enable_stream(rs::stream::color, this->width, this->height, rs::format::rgb8, fps);
        //device->enable_stream(rs::stream::infrared, this->width, this->height, rs::format::y8, 60);
        device->start();
    }
    catch(const rs::error & e) {
        std::cerr<<"Failed to open camera: "<<e.what()<<std::endl;
        return false;
    }
    catch(const std::exception & e) {
        std::cerr<<"Failed to open camera: "<<e.what()<<std::endl;
        return false;
    }

    return true;
}

double RealsenseVideoCapture::getProperty(int propIdx)
{
    if (propIdx == CV_CAP_PROP_FRAME_WIDTH)
    {
        return width;
    }
    else if (propIdx == CV_CAP_PROP_FRAME_HEIGHT)
    {
        return height;
    }
    else if (propIdx == CV_CAP_PROP_FPS)
    {
        return fps;
    }
    return 0;
}

bool RealsenseVideoCapture::setProperty(int propIdx, double propVal)
{
    if (propIdx == CV_CAP_PROP_FRAME_WIDTH)
    {
        this->width = propVal;
    }
    else if (propIdx == CV_CAP_PROP_FRAME_HEIGHT)
    {
        this->height = propVal;
    }
    else if (propIdx == CV_CAP_PROP_FPS)
    {
        this->fps = propVal;
    }

    return true;
}

bool RealsenseVideoCapture::grabFrame()
{
    try {
        device->wait_for_frames();
    }
    catch(...) {
        std::cerr << "error : failed to grab frame" << std::endl;
        return false;
    }

    return true;
}

unsigned char *RealsenseVideoCapture::retrieveFrame()
{
    return (uchar *)device->get_frame_data(rs::stream::color);
}

bool RealsenseVideoCapture::isOpened() const
{
    if (!device)
        return false;

    return device->is_streaming();
}
