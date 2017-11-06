#ifndef ABSTRACTVIDEOCAPTURE_H
#define ABSTRACTVIDEOCAPTURE_H

class AbstractVideoCapture {
public:
    virtual ~AbstractVideoCapture() {}
    virtual bool open(int device) = 0;
    virtual double getProperty(int propIdx) = 0;
    virtual bool setProperty(int propIdx, double propVal) = 0;
    virtual bool grabFrame() = 0;
    virtual unsigned char* retrieveFrame() = 0;
    virtual bool isOpened() const = 0;
    virtual void stop() = 0;
};

#endif // ABSTRACTVIDEOCAPTURE_H
