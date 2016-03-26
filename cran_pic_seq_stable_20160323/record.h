#ifndef RECORD_H
#define RECORD_H
#include <QThread>
#include <QTcpSocket>
#include<QHostAddress>
#include <QTimer>
//#include <QBuffer>
#include <QDir>
#include "mfc_interface.h"
#include <QtDebug>
#include <QDateTime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <QDateTime>
#include <QString>
#include <QDebug>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
extern "C"
{
#include "simplified_jpeg_encoder.h"
}

#define CAMERA_RECORD_NAME "/dev/video3"
class MFC;
class PUBFILE{
public:
    static PUBFILE *GetPubFileObj()
    {
        if(m_PubFile == NULL)
            m_PubFile = new PUBFILE();
        return m_PubFile;
    }
    FILE *fp_nv12;
    FILE *fp_strm;
    char *filename2;// = "test.h264\0";
    char *filename1;// = "test.nv12\0" ,
    virtual ~PUBFILE();
private:
    PUBFILE(){}
    static PUBFILE *m_PubFile;
};

class record :public QThread
{
    Q_OBJECT
public:
    record(QObject *parent = 0);
    ~record();
    bool recordflag;
    QDir re_qdir;
    QByteArray picData;
    bool stopThread;
   // bool Image2File();
    bool readframe();
    void StartCapturing();
    bool OpenFile();
    void StartRecord();
    void StopRecord();
    void CloseDevice();
protected:
    void OpenDevice();
    void connect_server();
    void InitMmap();
    void process_image(void *p, int size);
     int compress_yuyv_to_jpeg(unsigned char *buffer, int size, int quality, uint32_t format);
    int YUV422To420(char *yuv422,char *yuv420, int width, int height);
private:
    // add for print the frame num
    int frameNum;

    void run();
    MFC *encode;
    int gloindex;
    int glosize;
    struct buffer {
        void * start;
        size_t length;
    };
    struct v4l2_format fmt;
    bool Valid;
    //int nCount;
    int fd;
    int Width;
    int Height;
    const char *device;
    struct buffer *buffers;
    QTcpSocket *img_tcpsocket;
    unsigned int n_buffers;
    int fbuf_flat;
    QTimer *timer;
    PUBFILE *globalPubFile;
    FILE *fp_nv12;
signals:
    void getFrame();
private slots:
    void setflag();
};


class MFC{
public:
    friend class UVideo;
public:
    MFC();
    SSBSIP_MFC_ERROR_CODE init_mfc();
    //ss
    bool Encode();
    void closeMFC();
    int getHeader(unsigned char **p);
private:
    PUBFILE *globalPubFile;
     FILE *fp_nv12,*fp_strm;
    void *hOpen;
    int width,height;
    unsigned char header[100];
    int headerSize;
    SSBSIP_MFC_ENC_H264_PARAM *param;
    SSBSIP_MFC_CODEC_TYPE codec_type;
    SSBSIP_MFC_ENC_INPUT_INFO input_info;
    SSBSIP_MFC_ENC_OUTPUT_INFO output_info;

};

#endif // RECORD_H
