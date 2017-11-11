#ifndef REALSENSEVIDEOCAPTURE_H
#define REALSENSEVIDEOCAPTURE_H

#include "abstractvideocapture.h"
#include <librealsense/rs.hpp>
#include <librealsense/rscore.hpp>
#include <memory>

class RealsenseVideoCapture : public AbstractVideoCapture
{
public:
    RealsenseVideoCapture();
    virtual ~RealsenseVideoCapture();

    bool open(int device);
    double getProperty(int propIdx);
    bool setProperty(int propIdx, double propVal);
    bool grabFrame();
    unsigned char* retrieveFrame();
    bool isOpened() const;
    void stop();

private:
    std::unique_ptr<rs::context> ctx;
    rs::device *device;
    uint width;
    uint height;
    uint fps;
    //std::unique_ptr<rs::person_tracking::person_tracking_video_module_interface> ptModule;
};

#endif // REALSENSEVIDEOCAPTURE_H
