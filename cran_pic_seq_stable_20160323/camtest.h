#ifndef CAMTEST_H
#define CAMTEST_H
#include <iostream>
#include <fcntl.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <sys/poll.h>
#include "videodev2_samsung.h"
//#include "cv.h"
#include <QImage>
#include <QWidget>
#include <QtDebug>

//using namespace cv;
#define CAMERA_DEV_NAME   "/dev/video0"
class TError {
public:
    TError(const char *msg) {
        this->msg = msg;
    }
    TError(const TError bitand e) {
        msg = e.msg;
    }
    void Output() {
        std::cerr << msg << std::endl;
    }
    virtual ~TError() {}
protected:
    TError bitand operator=(const TError bitand);
private:
    const char *msg;
};

// Linear memory based image
class TRect {
public:
    unsigned char *Addr;
     int Width, Height, LineLen;
    TRect():  Addr(0), Size(0), Width(0), Height(0), LineLen(0), BPP(32) {
    }
    virtual ~TRect() {
    }
    bool DrawRect(const TRect & SrcRect, int x, int y) const;

    bool DrawRect(const TRect & rect) const { // default is Center
        return DrawRect(rect, (Width - rect.Width) / 2, (Height - rect.Height) / 2);
    }

    bool Clear() const;

protected:
    TRect(const TRect bitand);
    TRect bitand operator=( const TRect bitand);

protected:
    int Size;
    unsigned BPP;
};

class TFrameBuffer: public TRect {
public:
    TFrameBuffer();

    virtual ~TFrameBuffer();
protected:
    TFrameBuffer(const TFrameBuffer bitand);
    TFrameBuffer bitand operator=( const TFrameBuffer bitand);
private:
    int fd;
};

class TVideo : public TRect {
public:
    QImage image;
    int fd;
    TVideo();
    bool IsValid() const { return Valid; }
    bool WaitPic();
    bool FetchPicture();
    int CloseDevice();
    virtual ~TVideo();

protected:
    TVideo(const TVideo &);
    TVideo & operator=(const TVideo &);
    void OpenDevice();

    void StartStream();
    void StopStream();

    bool Valid;
    struct pollfd  m_events_c;
    static const int CAPTURE_BUFFER_NUMBER = 1;
    struct { void * data; size_t len; } captureBuffer[CAPTURE_BUFFER_NUMBER];
//    struct buffer {
//        void * start;
//        size_t length;
//    };
    unsigned char *fdata;
     int flen;
};


#endif // CAMTEST_H
