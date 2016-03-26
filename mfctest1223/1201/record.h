#ifndef RECORD_H
#define RECORD_H
#include <QThread>
#include <QDir>
#include "mfc_interface.h"
#include <QtDebug>
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

#include <asm/types.h>          /* for videodev2.h */
#include <QMessageBox>

#include <linux/videodev2.h>

#include "x264.h"

#define CAMERA_RECORD_NAME  "/dev/video3"

class PUBFILE
{
public:
    FILE *fp_nv12;
    FILE *fp_strm;
    FILE *fp_strm_x264;
    char *filename2;        // = "test.h264\0";
    char *filename1;        // = "test.nv12\0" ,
};

class record : public PUBFILE , public QThread
{
public:
    record();
    ~record();
    bool recordflag;
    QDir re_qdir;
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
    void InitMmap();
    void process_image(void *p, int size);
    int YUV422To420(unsigned char *yuv422, unsigned char *yuv420, int width, int height);
    int YUV422To420x264(unsigned char *yuv422, unsigned char *yuv420, int width, int height);

public:
    //@1225
    //获取图片，放回缓冲区
    int get_frame(void **, size_t*);
    int unget_frame();
    //以下两个函数用于YUV格式转换成RGB格式
     int convert_yuv_to_rgb_pixel(int y, int u, int v);
     int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
     void display_error(QString err);

private:
    void run();
    struct buffer
    {
        void * start;
        size_t length;
    };
    bool Valid;
    //int nCount;
    int fd;
    int Width;
    int Height;
    const char *device;
    struct buffer *buffers;
    unsigned int n_buffers;
    int fbuf_flat;
    int index;
};


class MFC:public PUBFILE
{
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
    void *hOpen;
    int width,height;
    unsigned char header[100];
    int headerSize;
    SSBSIP_MFC_ENC_H264_PARAM *param;
    SSBSIP_MFC_CODEC_TYPE codec_type;
    SSBSIP_MFC_ENC_INPUT_INFO input_info;
    SSBSIP_MFC_ENC_OUTPUT_INFO output_info;

};

//class x264Encoder: public PUBFILE
//{
//public:
//    int width;
//    int height;
//    x264_param_t param;
//    x264_picture_t pic;
//    x264_picture_t pic_out;
//    x264_t *h;
//    int i_frame;
//    int i_frame_size;
//    x264_nal_t *nal;
//    int i_nal;
//public:
//    x264Encoder();

//public:
//    int x264EncoderInit(void);
//    int x264EncoderExec(int width, int height);

//};

#endif // RECORD_H
