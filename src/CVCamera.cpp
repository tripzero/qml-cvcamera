/*
 * Copyright (C) 2014 EPFL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

/**
 * @file cvcamera.cpp
 * @brief Implementation of the QML wrapper for OpenCV camera access
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2014-09-22
 */

#include"CVCamera.h"
#include "realsensevideocapture.h"

CVCamera::CVCamera(QQuickItem* parent) :
    QQuickItem(parent), m_fps(30)
{
    //Build the list of available devices
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for(const auto& cameraInfo : cameras){
        QString device;
        device += cameraInfo.deviceName();
        device += " - ";
        device += cameraInfo.description();
        deviceList << device;
    }
    Q_EMIT deviceListChanged();

    size = QSize(640, 480);
    connect(this, &QQuickItem::parentChanged, this, &CVCamera::changeParent);

    camera = new RealsenseVideoCapture();
}

CVCamera::~CVCamera()
{
    if(thread)
        thread->stop();
    delete thread;
    delete camera;
    //Camera release is automatic when cv::VideoCapture is destroyed
}

void CVCamera::changeParent(QQuickItem* parent)
{
    //FIXME: we probably need to disconnect the previous parent
    //TODO: probably a good idea to stop the camera (and restart it if we are auto-starting in this context)
}

int CVCamera::getDevice() const
{
    return device;
}

int CVCamera::getFps() const
{
    return m_fps;
}

void CVCamera::setFps(int fps)
{
    if (m_fps != fps) {
        m_fps = fps;
        update();
        Q_EMIT fpsChanged();
    }
}

void CVCamera::setDevice(int device)
{
    if(device >= 0 && this->device != device){
        this->device = device;
        update();
    }
}

QSize CVCamera::getSize() const
{
    return size;
}

void CVCamera::setSize(QSize size)
{
    if(this->size.width() != size.width() || this->size.height() != size.height()){
        this->size = size;
        update();
        Q_EMIT sizeChanged();
    }
}

QVariant CVCamera::getCvImage()
{
    if(!exportCvImage){
        exportCvImage = true;
        update();
    }
    QVariant container(QVariant::UserType);
    container.setValue(cvImage);
    return container;
}

QStringList CVCamera::getDeviceList() const
{
    return deviceList;
}

void CVCamera::allocateCvImage()
{
    cvImage.release();
    delete[] cvImageBuf;
#ifdef ANDROID
    cvImageBuf = new unsigned char[size.width()*size.height()*3/2];
    cvImage = cv::Mat(size.height()*3/2,size.width(),CV_8UC1,cvImageBuf);
#else
    cvImageBuf = new unsigned char[size.width()*size.height()*3];
    cvImage = cv::Mat(size.height(),size.width(),CV_8UC3,cvImageBuf);
#endif
}

void CVCamera::allocateVideoFrame()
{
#ifdef ANDROID
    videoFrame = new QVideoFrame(size.width()*size.height()*3/2,size,size.width(),VIDEO_OUTPUT_FORMAT);
#else
    videoFrame = new QVideoFrame(size.width()*size.height()*4, size, size.width()*4, VIDEO_OUTPUT_FORMAT);
#endif
}

void CVCamera::update()
{
    DPRINT("Opening camera %d, width: %d, height: %d", device, size.width(), size.height());

    //Destroy old thread, camera accessor and buffers
    delete thread;

    if (camera->isOpened()) {
        camera->stop();
    }

    if(videoFrame && videoFrame->isMapped())
        videoFrame->unmap();
    delete videoFrame;
    videoFrame = NULL;
    delete[] cvImageBuf;
    cvImageBuf = NULL;

    //Create new buffers, camera accessor and thread
    if(exportCvImage)
        allocateCvImage();
    if(videoSurface)
        allocateVideoFrame();

    thread = new CameraThread(camera,videoFrame,cvImageBuf, size.width(), size.height());
    connect(thread,SIGNAL(imageReady()), this, SLOT(imageReceived()));

    //Open newly created device
    try{
        camera->setProperty(CV_CAP_PROP_FRAME_WIDTH, size.width());
        camera->setProperty(CV_CAP_PROP_FRAME_HEIGHT, size.height());
        camera->setProperty(CV_CAP_PROP_FPS, m_fps);

        if(camera->open(device)){
            DPRINT("setting width and height properties on camera");
            if(videoSurface){
                if(videoSurface->isActive())
                    videoSurface->stop();
                if(!videoSurface->start(QVideoSurfaceFormat(size, VIDEO_OUTPUT_FORMAT)))
                    DPRINT("Could not start QAbstractVideoSurface, error: %d", videoSurface->error());
            }
            thread->start();
            DPRINT("Opened camera %d",device);
        }
        else
            DPRINT("Could not open camera %d",device);
    }
    catch(int e){
        DPRINT("Exception %d",e);
    }
}

void CVCamera::imageReceived()
{
    //Update VideoOutput
    if(videoSurface)
        if(!videoSurface->present(*videoFrame))
            DPRINT("Could not present QVideoFrame to QAbstractVideoSurface, error: %d",videoSurface->error());

    //Update exported CV image
    if(exportCvImage)
        Q_EMIT cvImageChanged();
}

QAbstractVideoSurface* CVCamera::getVideoSurface() const
{
    return videoSurface;
}

void CVCamera::setVideoSurface(QAbstractVideoSurface* surface)
{
    if(videoSurface != surface){
        videoSurface = surface;
        update();
    }
}

