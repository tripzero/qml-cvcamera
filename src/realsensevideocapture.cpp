#include "realsensevideocapture.h"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/opencv.hpp"

RealsenseVideoCapture::RealsenseVideoCapture()
    :device(nullptr), width(0), height(0)
{
    ctx.reset(new rs::context());

    /*ptModule.reset(rs::person_tracking::person_tracking_video_module_factory::create_person_tracking_video_module("."));

    rs::core::intrinsics color_intrin = rs::utils::convert_intrinsics(device->get_stream_intrinsics(rs::stream::color));
    rs::core::intrinsics depth_intrin = rs::utils::convert_intrinsics(device->get_stream_intrinsics(rs::stream::depth));
    rs::core::extrinsics extrinsics = rs::utils::convert_extrinsics(device->get_extrinsics(rs::stream::depth, rs::stream::color));
    actualModuleConfig = rs::core::projection_interface::create_instance(&color_intrin, &depth_intrin, &extrinsics);
    */
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
    std::cout<<"opening camera... "<<device_number<<std::endl;
    if (ctx->get_device_count() == 0)
    {
        std::cerr << "error : cant find devices" << std::endl;
        return false;
    }

    device = ctx->get_device(device_number);

    device->enable_stream(rs::stream::depth, this->width, this->height, rs::format::z16, 60);
    device->enable_stream(rs::stream::color, this->width, this->height, rs::format::rgb8, 60);
    device->enable_stream(rs::stream::infrared, this->width, this->height, rs::format::y8, 60);
    device->start();

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

    return true;
}

bool RealsenseVideoCapture::grabFrame()
{
    device->wait_for_frames();
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
