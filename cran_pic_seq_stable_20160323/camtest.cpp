#include "camtest.h"
const int width_fd = 640;
const int height_fd = 480;

static void decodeYUV420SP(unsigned int* rgbBuf, unsigned char* yuv420sp, int width, int height) {
    int frameSize = width * height;

    int i = 0, y = 0;
    int uvp = 0, u = 0, v = 0;
    int y1192 = 0, r = 0, g = 0, b = 0;
    unsigned int xrgb8888;
    int xrgb8888Index = 0;

    for (int j = 0, yp = 0; j < height; j++) {
        uvp = frameSize + (j >> 1) * width;
        u = 0;
        v = 0;
        for (i = 0; i < width; i++, yp++) {
            y = (0xff & ((int) yuv420sp[yp])) - 16;
            if (y < 0) y = 0;
            if ((i & 1) == 0) {
                v = (0xff & yuv420sp[uvp++]) - 128;
                u = (0xff & yuv420sp[uvp++]) - 128;
            }

            y1192 = 1192 * y;
            r = (y1192 + 1634 * v);
            g = (y1192 - 833 * v - 400 * u);
            b = (y1192 + 2066 * u);

            if (r < 0) r = 0; else if (r > 262143) r = 262143;
            if (g < 0) g = 0; else if (g > 262143) g = 262143;
            if (b < 0) b = 0; else if (b > 262143) b = 262143;


            r = (unsigned char)(r >> 10);
            g = (unsigned char)(g >> 10);
            b = (unsigned char)(b >> 10);

            xrgb8888 = (unsigned int)((r << 16) | (g << 8) | b);
            rgbBuf[xrgb8888Index++] = xrgb8888;
        }
    }

}

static void errno_exit(const char * s) {
    fprintf(stderr, "%s error %d, %s/n", s, errno, strerror(errno));

    exit(EXIT_FAILURE);
}

bool TRect::DrawRect(const TRect & SrcRect, int x, int y) const{
        if (BPP not_eq 32 or SrcRect.BPP not_eq 32) {
            // don't support that yet
            throw TError("does not support other than 32 BPP yet");
        }

        // clip
        int x0, y0, x1, y1;
        x0 = x;
        y0 = y;
        x1 = x0 + SrcRect.Width - 1;
        y1 = y0 + SrcRect.Height - 1;
        if (x0 < 0) {
            x0 = 0;
        }
        if (x0 > Width - 1) {
            return true;
        }
        if( x1 < 0) {
            return true;
        }
        if (x1 > Width - 1) {
            x1 = Width - 1;
        }
        if (y0 < 0) {
            y0 = 0;
        }
        if (y0 > Height - 1) {
            return true;
        }
        if (y1 < 0) {
            return true;
        }
        if (y1 > Height - 1) {
            y1 = Height -1;
        }

        //copy
        int copyLineLen = (x1 + 1 - x0) * BPP / 8;
        unsigned char *DstPtr = Addr + LineLen * y0 + x0 * BPP / 8;
        const unsigned char *SrcPtr = SrcRect.Addr + SrcRect.LineLen *(y0 - y) + (x0 - x) * SrcRect.BPP / 8;

        for (int i = y0; i <= y1; i++) {
            memcpy(DstPtr, SrcPtr, copyLineLen);
            DstPtr += LineLen;
            SrcPtr += SrcRect.LineLen;
        }


        return true;
    }

bool TRect::Clear() const{
        int i;
        unsigned char *ptr;
        for (i = 0, ptr = Addr; i < Height; i++, ptr += LineLen) {
            memset(ptr, 0, Width * BPP / 8);
        }
        return true;
    }

TFrameBuffer::TFrameBuffer()
    : TRect(), fd(-1) {
        Addr = (unsigned char *)MAP_FAILED;

        fd = open("/dev/fb0", O_RDWR);
        if (fd < 0) {
            throw TError("cannot open frame buffer");
        }

        struct fb_fix_screeninfo Fix;
        struct fb_var_screeninfo Var;
        if (ioctl(fd, FBIOGET_FSCREENINFO, bitand Fix) < 0 or ioctl(fd, FBIOGET_VSCREENINFO, bitand Var) < 0) {
            throw TError("cannot get frame buffer information");
        }

        BPP = Var.bits_per_pixel;
        if (BPP not_eq 32) {
            throw TError("support 32 BPP frame buffer only");
        }

            Width  = Var.xres;
            Height = Var.yres;
            LineLen = Fix.line_length;
            Size = LineLen * Height;

        int PageSize = getpagesize();
        Size = (Size + PageSize - 1) / PageSize * PageSize ;
            Addr = (unsigned char *)mmap(NULL, Size, PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
        if (Addr == (unsigned char *)MAP_FAILED) {
            throw TError("map frame buffer failed");
            return;
        }
        ::close(fd);
        fd = -1;

        Clear();
    }

TFrameBuffer::~TFrameBuffer(){
            ::munmap(Addr, Size);
            Addr = (unsigned char *)MAP_FAILED;
}

TVideo::TVideo()
    : TRect(), fd(-1) {
    Width = width_fd;
    Height = height_fd;
    BPP = 32;
    LineLen = Width * BPP / 8;
    Size = LineLen * Height;
    fd = -1;
    Valid = true;
    Addr = new unsigned char[Size];
    Clear();

    OpenDevice();
    StartStream();
}

TVideo::~TVideo(){
             //StopStream();
            qDebug()<<"fd "<<fd;
            CloseDevice();
            fd = -1;
            //fclose(fp_nv12);
           Valid = false;
           Addr = 0;
           delete[] Addr;

}

void TVideo::OpenDevice()
{
	// Open Device
	const char *device = CAMERA_DEV_NAME;

    //fp_nv12 = fopen(filename1, "wa+");

    fd = ::open(device, O_RDWR|O_NONBLOCK,0);
    //qDebug()<<"fd "<<fd;
	if (fd < 0) {
        Valid = false;
		fprintf(stderr, "cannot open device %s\n", device);
		return;
	}
    
	// Check capability
	struct v4l2_capability cap;
    if( ::ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {//将设备功能导入到cap结构体中
        Valid = false;
		fprintf(stderr, "cannot query capability\n");
		return;
	}

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {//视频捕获功能
        Valid = false;
		fprintf(stderr, "not a video capture device\n");
		return;
	}
    
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {//是否支持流操作，支持数据流控制
        Valid = false;
		fprintf(stderr, "does not support streaming i/o\n");
		return;
	}

    static struct v4l2_input input;
    input.index = 0; 
    if (ioctl(fd, VIDIOC_ENUMINPUT, &input) != 0) {//枚举设备的输入输出物理连接
        Valid = false;
        fprintf(stderr, "No matching index found\n");
        return;
    }
    if (!input.name) {
        Valid = false;
        fprintf(stderr, "No matching index found\n");
        return;
    }
    if (ioctl(fd, VIDIOC_S_INPUT, &input) < 0) {
        Valid = false;
        fprintf(stderr, "VIDIOC_S_INPUT failed\n");
        return;
    }

    //查询设备所支持的视频数据格式
//    struct v4l2_fmtdesc fmtde;
//    memset(&fmtde , 0 , sizeof(fmtde));
//    fmtde.index = 0;
//    int ret = -1;
//    fmtde.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    while((ret = ioctl(fd , VIDIOC_ENUM_FMT , &fmtde)) == 0){
//        fmtde.index++;
//        printf("{pixelformat = \"%c%c%c%c\" , description = \"%s\"}\n",
//               fmtde.pixelformat & 0XFF , (fmtde.pixelformat >> 8) & 0XFF,
//               (fmtde.pixelformat >> 16) &0xFF,(fmtde.pixelformat >>24)&0xFF,fmtde.description);
//    }

    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof fmt);
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = Width;
    fmt.fmt.pix.height      = Height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
    fmt.fmt.pix.sizeimage = (fmt.fmt.pix.width * fmt.fmt.pix.height * 16) / 8;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (::ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {//设置驱动的频捕获格式,如果出错则摄像头不支持这个格式
        Valid = false;
        fprintf(stderr, "VIDIOC_S_FMT failed\n");
        return;
    }
    
	bool CouldSetFrameRate = false;

	struct v4l2_streamparm StreamParam;
	memset(&StreamParam, 0, sizeof StreamParam);
	StreamParam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    StreamParam.parm.capture.timeperframe.denominator = 25;
//    StreamParam.parm.capture.timeperframe.numerator = 1;
    if (ioctl(fd, VIDIOC_G_PARM, &StreamParam) < 0)  {//读取驱动的频捕获格式
        fprintf(stderr, "could not set frame rate\n");
    } else {
        CouldSetFrameRate = StreamParam.parm.capture.capability & V4L2_CAP_TIMEPERFRAME;
    }

    // map the capture buffer...
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof req);
    req.count  = CAPTURE_BUFFER_NUMBER;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {//分配内存
        Valid = false;
    	fprintf(stderr, "request capture buffer failed\n");
    	return;
    }
    
    if (int(req.count) != CAPTURE_BUFFER_NUMBER) {
    	fprintf(stderr, "capture buffer number is wrong\n");
        Valid = false;
        return;
    }

    for (int i = 0; i < CAPTURE_BUFFER_NUMBER; i++) {
    	struct v4l2_buffer b;
    	memset(&b, 0, sizeof b);
    	b.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    	b.memory = V4L2_MEMORY_MMAP;
    	b.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &b) < 0) {//将VIDIOC_REQBUFS分配的数据缓存转换成物理地址
            Valid = false;
    		fprintf(stderr, "query capture buffer failed\n");
    		return;
    	}
        
        captureBuffer[i].data = mmap(0, b.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, b.m.offset);
    	captureBuffer[i].len = b.length;
        
    	if (captureBuffer[i].data == MAP_FAILED) {
            Valid = false;
    		fprintf(stderr, "unable to map capture buffer\n");
    		return;
        }
        fprintf(stderr, "ImageSize[%d] = %ld\n", i, b.length);
    }

    if (Valid) {
        fprintf(stderr, "Open Device OK!\n");
    }

    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_CAMERA_CHECK_DATALINE;
    ctrl.value = 0;
    if(ioctl(fd, VIDIOC_S_CTRL,&ctrl)) {
        fprintf(stderr, "VIDIOC_S_CTRL V4L2_CID_CAMERA_CHECK_DATALINE failed\n");
        Valid = false;
        return;
    }

    memset(&m_events_c, 0, sizeof(m_events_c));
    m_events_c.fd = fd;
    m_events_c.events = POLLIN | POLLERR;
    
	return;
}

int TVideo::CloseDevice(){
    //StopStream();
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_STREAMOFF, &type);

    for (int i = 0; i < CAPTURE_BUFFER_NUMBER; i++) {
        ::munmap(captureBuffer[i].data , captureBuffer[i].len);
    }
    ::munmap(Addr, Size);
    Addr = (unsigned char *)MAP_FAILED;

    if (fd != -1){
        close(fd);
        fprintf(stderr, "Close Device Success\n");
        return (TRUE);
    }
    else
    {
    fprintf(stderr, "Close Device Failed\n");
    return (FALSE);
    }
}

void TVideo::StartStream()
{
    for (int i = 0; i < CAPTURE_BUFFER_NUMBER; i++) {
        struct v4l2_buffer b;
        memset(&b, 0, sizeof b);
        b.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        b.memory = V4L2_MEMORY_MMAP;
        b.index = i;

        if (ioctl (fd, VIDIOC_QBUF, &b) < 0) {
            Valid = false;
            fprintf(stderr, "queue capture failed\n");
            return;
        }
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl (fd, VIDIOC_STREAMON, &type) < 0) {
        Valid = false;
        fprintf(stderr, "cannot start stream\n");
        return;
    }

    if (Valid) {
        fprintf(stderr, "StartStream OK!\n");
    }
}

void TVideo::StopStream()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl (fd, VIDIOC_STREAMOFF, &type) < 0) {
        Valid = false;
        fprintf(stderr, "cannot stop stream\n");
        return;
    }
}

bool TVideo::WaitPic()
{
    int ret = poll(&m_events_c,  1, 10000);
    if (ret > 0) {
        return true;
    }
    return false;
}

bool TVideo::FetchPicture()
{
	struct v4l2_buffer b;
	memset(&b, 0, sizeof b);
	b.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	b.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_DQBUF, &b) < 0) {
        Valid = false;
		fprintf(stderr, "cannot fetch picture(VIDIOC_DQBUF failed)\n");
		return false;
	}

    
    void *data_ = captureBuffer[b.index].data;
    unsigned int len = b.bytesused;
    unsigned int index = b.index;

    //unsigned char *fdata = (unsigned char*) data_;
    fdata = (unsigned char*) data_;
    flen =captureBuffer[b.index].len;
    //flen = b.length;
    decodeYUV420SP((unsigned int*)Addr, fdata, Width, Height);
   // if ( recordflag == true ){
   //     Image2File( fdata , flen);
  //  }
    //QImage image((const uchar*)Addr , Width , Height ,QImage::Format_RGB32);
    //this->image = image;
	if (ioctl (fd, VIDIOC_QBUF, &b) < 0) {
        Valid = false;
		fprintf(stderr, "cannot fetch picture(VIDIOC_QBUF failed)\n");
		return false;
	}
    
    return true;
}






//int main(int argc, char **argv)
//{
//	try {
//		TFrameBuffer FrameBuffer;
//		TVideo Video;
//        while(Video.IsValid()) {
//            if (Video.WaitPic()) {
//                if (Video.FetchPicture()) {
//                    FrameBuffer.DrawRect(Video);
//                }
//            }
//        }
//	} catch (TError bitand e) {
//		e.Output();
//		return 1;
//	}

//	return 0;
//}

