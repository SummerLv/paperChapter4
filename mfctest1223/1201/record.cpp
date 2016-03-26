#include "record.h"
#include "x264.h"


const int width_fd = 640;
const int height_fd = 480;
int fsize = 0;
int frames = 0;
unsigned char *fbuf = NULL;
unsigned char  buff420sp[width_fd*height_fd*3/2];
unsigned char buffI420p[width_fd*height_fd*3/2];

#define CLEAR(x) memset(&(x), 0, sizeof(x))


static void errno_exit(const char * s)
{
    fprintf(stderr, "%s error %d, %s/n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}


record::record()
:fd(-1)
{
            fsize = width_fd * height_fd *2 / 32 *24;// w * h * 1.5  ===  YUV420 size
            frames = 200;//  @  1225
            fbuf_flat = 0;
            Width = width_fd;//640
            Height = height_fd;//480
            fd = -1;
            fp_nv12 = NULL;
            //是否有SDcard   如果存在则存储在sdcard 的文件中
            if(re_qdir.exists("/sdcard") == true)
            {
                filename1 = "/sdcard/test.nv12\0";
            }
            else
            {
                filename1 = "/test.nv12\0";
            }

            fbuf = (unsigned char*)malloc(frames * fsize);//申请了 200* 800 == 160 K
            if(NULL == fbuf)
            {
                printf("calloc failed");
            }

            recordflag = false;
            stopThread = false;

            OpenDevice();
            StartCapturing();
}

record::~record()
{
   // StopRecord();
    CloseDevice();
}

void record::run()
{
    while ( (stopThread == false) && (frames-- > 0))
    {
        StartRecord();
    }
    qDebug()<<"fbuf_flat"<<fbuf_flat;
    // 把 数组fbuf 写入 到 fp_nv12
    fwrite(fbuf, fsize * fbuf_flat, 1, fp_nv12);
    free(fbuf);
    qDebug()<<"record ok";
}

bool record::OpenFile()
{
    fp_nv12 = fopen(filename1, "wa+");
    if(fp_nv12 == NULL)
        return false;
    return true;
}

void record::OpenDevice()
{
    //nCount = 200;
   device = CAMERA_RECORD_NAME;

    fd = open(device, O_RDWR, 0);

    if (-1 == fd)
    {
        fprintf(stderr, "Cannot open '%s': %d, %s/n", device, errno,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    //查询并设置设备功能
    struct v4l2_capability cap;
    if( ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
    {//将设备功能导入到cap结构体中
        Valid = false;
        fprintf(stderr, "cannot query capability\n");
        return;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))       //视频捕获功能
    {
        Valid = false;
        fprintf(stderr, "not a video capture device\n");
        return;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))           //是否支持流操作，支持数据流控制
    {
        Valid = false;
        fprintf(stderr, "does not support streaming i/o\n");
        return;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++
    //查询设备所支持的视频数据格式   YUYV  MPEG * 2 format
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
  //  }
//    struct v4l2_format fmt1;
//    fmt1.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    ioctl(fd, VIDIOC_G_FMT, &fmt1);
//    qDebug()<<"w   "<<fmt1.fmt.pix.width;
//    qDebug()<<"h   "<<fmt1.fmt.pix.width;
//    qDebug()<<"111111111111";
                  //0121  yuyv 422        < 640 . 640 >  new usb
    //printf(“Current data format information:\n\t width:%d \n\t height:%d\n”,fmt.fmt.pix.width,fmt.fmt.pix.height);

    //+++++++++++++++++++++++++++++++++++++++++++++++++++



    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    memset(&cropcap , 0 , sizeof(cropcap));

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  //表示支持图像获取

    if ( ioctl(fd, VIDIOC_CROPCAP, &cropcap) < 0)
    {
         fprintf(stderr, "VIDIOC_CROPCAP can't settled\n");
    }
    else
    {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        crop.c.left = cropcap.defrect.left; /* reset to default */
//        crop.c.top = cropcap.defrect.top;
//        crop.c.width = 640;
//        crop.c.height = 480;
//        qDebug()<<cropcap.defrect.left<<"\t"<<cropcap.defrect.top<<"\t"<<cropcap.defrect.width<<"\t"<<cropcap.defrect.height;
       crop.c =cropcap.defrect;
         if ( ioctl(fd, VIDIOC_S_CROP, &crop) < 0){
           fprintf(stderr, "VIDIOC_S_CROP ERROR  record\n");
        }
    }
//    printf("cropcap.defrect = (%d , %d , %d , %d)\n", cropcap.defrect.left,
//            cropcap.defrect.top ,  cropcap.defrect.width ,  cropcap.defrect.height);
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof fmt);
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = Width;
    fmt.fmt.pix.height      = Height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
   // fmt.fmt.pix.sizeimage = (fmt.fmt.pix.width * fmt.fmt.pix.height * 16) / 8;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {//设置驱动的频捕获格式
        Valid = false;
        fprintf(stderr, "VIDIOC_S_FMT failed  __ record\n");
        return;
    }

    unsigned int min;
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min) //bytesperline 为每一行字节数
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)  //sizeimage表示图像所占存储空间大小
        fmt.fmt.pix.sizeimage = min;

    bool CouldSetFrameRate = false;

//    struct v4l2_streamparm StreamParam;
//    memset(&StreamParam, 0, sizeof StreamParam);
//    StreamParam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    StreamParam.parm.capture.timeperframe.denominator = 25;
//    StreamParam.parm.capture.timeperframe.numerator = 1;
//    if (ioctl(fd, VIDIOC_G_PARM, &StreamParam) < 0)  {//读取驱动的频捕获格式,成功表示可以设置帧率
//        fprintf(stderr, "could not set frame rate\n");
//    } else {
//        CouldSetFrameRate = StreamParam.parm.capture.capability & V4L2_CAP_TIMEPERFRAME;
//    }
   //进行采集图像缓冲区设置
    //V4L2申请的内存位于内核空间，应用程序需要使用内存映射方法将内核空间映射到用户空间来简介访问内核空间
    InitMmap();
    fprintf(stderr, "Open Record Device OK!\n");
}

 void record::InitMmap(){
    struct v4l2_requestbuffers req; //缓冲区结构体

    memset(&req , 0 , sizeof(req));

    req.count = 4;  //缓存数量，设置缓存区队列里保持多少张照片
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;  //方法为内存映射方法

    if ( ioctl(fd, VIDIOC_REQBUFS, &req) < 0 ) {  //请求帧缓冲区
            fprintf(stderr, "%s does not support "
                    "memory mapping/n", device);
            exit(EXIT_FAILURE);
    }
    //一般会要求申请的帧缓冲数量是 大于 2 故作下面的判断
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s/n", device);
        exit(EXIT_FAILURE);
    }
    //========================================================
    //为多个数据申请内存空间，在内存中，申请能够相应的空间，
    buffers = (struct buffer *)calloc(req.count, sizeof(struct buffer ));

    if (!buffers) {
        fprintf(stderr, "Out of memory/n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        struct v4l2_buffer buf;
        memset( &buf , 0 , sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (  ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0)
            errno_exit("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length,
                PROT_READ | PROT_WRITE /* required */,
                MAP_SHARED /* recommended */, fd, buf.m.offset);  //缓冲区起始存储位置

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

void record::StartCapturing(){
    unsigned int i;
    enum v4l2_buf_type type;
        for (i = 0; i < n_buffers; ++i)
        {
            struct v4l2_buffer buf;
            memset( &buf , 0 , sizeof(buf) );
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            //这里应该是把申请到的缓冲区加入到图像队列   ？？？？
            if (  ioctl(fd, VIDIOC_QBUF, &buf) < 0)//1223xx将拍摄的图像数据放入缓存队列
                errno_exit("VIDIOC_QBUF");
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if ( ioctl(fd, VIDIOC_STREAMON, &type) < 0 )  //开始捕捉视频图像
            errno_exit("VIDIOC_STREAMON");
        fprintf(stderr, "Start Record StartStream OK!\n");
}

void record::StartRecord(){
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            /* Timeout. */
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            //视频设备驱动采集并保存到视频数据到缓冲区
            //select返回之后，可以继续读取数据
            r = select(fd + 1, &fds, NULL, NULL, &tv);


            if (-1 == r) {
                if (EINTR == errno)
                    continue;
                errno_exit("select");
            }

            if (0 == r) {
                fprintf(stderr, "select timeout\n");
                exit(EXIT_FAILURE);
            }

            if (readframe())
                break;

            /* EAGAIN - continue select loop. */
        }
}

bool record::readframe(){
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    //从缓冲区输出队列中取出  一个视频帧
    if ( ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            errno_exit("VIDIOC_DQBUF");
        }
    assert(buf.index < n_buffers);

    //******************************@0421
    //@0421 change to transfer the fomate from 422---> 420 directly
   // qDebug()<<"buf.length"<<(int)buf.length;
    //process_image(buffers[buf.index].start, buffers[buf.index].length);
    //qDebug()<<"buffers[buf.index].length :\t"<<buffers[buf.index].length; //614400
    YUV422To420((unsigned char*)buffers[buf.index].start, buff420sp, width_fd, height_fd);
    YUV422To420x264((unsigned char*)buffers[buf.index].start, buffI420p, width_fd, height_fd);

    if ( ioctl(fd, VIDIOC_QBUF, &buf) < 0)
        errno_exit("VIDIOC_QBUF");
    return true;
}

void record::process_image(void *p, int size) {
    int outLength = size / 32 * 24;//YUV420  -- length = w * h *  1.5 ----- 460800
    unsigned char *out = (unsigned char*)malloc(outLength);
    //？1224这里是否因该给申请到的内存单元清零
    void *data = p;
    YUV422To420((unsigned char*)data , out , Width , Height);
    memcpy( fbuf+fbuf_flat*outLength , out , outLength );
    ++fbuf_flat;
    //fwrite(out, outLength , 1 , fp_nv12);
    free(out);
}

int record::YUV422To420x264(unsigned char *yuv422, unsigned char *yuv420, int width, int height)
{

       int ynum=width*height;
      // qDebug()<<"ynum:"<<ynum;
       int i,j,k=0;
       //得到Y分量
       for(i=0;i<ynum;i++){
               yuv420[i]=yuv422[i*2];
       }
      // qDebug()<<"outLength";
       //得到U分量
       for(i=0;i<height;i++){
               if((i%2)!=0)continue;//只取其中奇数行
               for(j=0;j<(width/2);j++){
                       if((4*j+1)>(2*width))break;
                       yuv420[ynum+k*2*width/4+j]=yuv422[i*2*width+4*j+1];
               }
               k++;
       }
       k=0;
       //得到V分量
       for(i=0;i<height;i++){
               if((i%2)==0)continue;
               for(j=0;j<(width/2);j++){
                       if((4*j+3)>(2*width))break;
                       yuv420[ynum+ynum/4+k*2*width/4+j]=yuv422[i*2*width+4*j+3];

               }
               k++;
       }

       return 1;
}
//@0421 ---------------------------------------------------------------
int record::YUV422To420(unsigned char *yuv422, unsigned  char *yuv420, int width, int height)
{

       int ynum=width*height;
      // qDebug()<<"ynum:"<<ynum;
       int i,j,k=0;
       //得到Y分量
       for(i=0;i<ynum;i++){
               yuv420[i]=yuv422[i*2];
       }
      // qDebug()<<"outLength";
       //得到U分量
       for(i=0;i<height;i++){
               if((i%2)!=0)continue;
               for(j=0;j<(width/2);j++){
                       if((4*j+1)>(2*width))break;
                       yuv420[ynum+j*2+k*width]=yuv422[i*2*width+4*j+1];
               }
               k++;
       }
       k=0;
       //得到V分量
       for(i=0;i<height;i++){
               if((i%2)==0)continue;
               for(j=0;j<(width/2);j++){
                       if((4*j+3)>(2*width))break;
                       yuv420[ynum+j*2+1+k*width]=yuv422[i*2*width+4*j+3];

               }
               k++;
       }
        //420p to 420sp
       return 1;
}

//-------------------------------------------------------------------------------------------------------------
int record::get_frame(void **frame_buf, size_t* len)
{
    v4l2_buffer queue_buf;
    CLEAR(queue_buf);

    queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queue_buf.memory = V4L2_MEMORY_MMAP;

    if(-1 == ioctl(fd, VIDIOC_DQBUF, &queue_buf))
    {
        switch(errno)
        {
        case EAGAIN:
//            perror("dqbuf");
            return -1;
        case EIO:
            return -1 ;
        default:
            emit display_error(tr("VIDIOC_DQBUF: %1").arg(QString(strerror(errno))));
            return -1;
        }
    }

    *frame_buf = buffers[queue_buf.index].start;
    *len = buffers[queue_buf.index].length;
    index = queue_buf.index;

    return 0;
}

int record::unget_frame()
{
    if(index != -1)
    {
        v4l2_buffer queue_buf;
        CLEAR(queue_buf);

        queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        queue_buf.memory = V4L2_MEMORY_MMAP;
        queue_buf.index = index;

        if(-1 == ioctl(fd, VIDIOC_QBUF, &queue_buf))
        {
            emit display_error(tr("VIDIOC_QBUF: %1").arg(QString(strerror(errno))));
            return -1;
        }
        return 0;
    }
    return -1;
}


int record::convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
 unsigned int in, out = 0;
 unsigned int pixel_16;
 unsigned char pixel_24[3];
 unsigned int pixel32;
 int y0, u, y1, v;
 for(in = 0; in < width * height * 2; in += 4)
    {
          pixel_16 =
           yuv[in + 3] << 24 |
           yuv[in + 2] << 16 |
           yuv[in + 1] <<  8 |
           yuv[in + 0];
          y0 = (pixel_16 & 0x000000ff);
          u  = (pixel_16 & 0x0000ff00) >>  8;
          y1 = (pixel_16 & 0x00ff0000) >> 16;
          v  = (pixel_16 & 0xff000000) >> 24;
          pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
          pixel_24[0] = (pixel32 & 0x000000ff);
          pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
          pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
          rgb[out++] = pixel_24[0];
          rgb[out++] = pixel_24[1];
          rgb[out++] = pixel_24[2];
          pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
          pixel_24[0] = (pixel32 & 0x000000ff);
          pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
          pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
          rgb[out++] = pixel_24[0];
          rgb[out++] = pixel_24[1];
          rgb[out++] = pixel_24[2];
    }
 return 0;
}

int record::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
     unsigned int pixel32 = 0;
     unsigned char *pixel = (unsigned char *)&pixel32;
     int r, g, b;
     r = y + (1.370705 * (v-128));
     g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
     b = y + (1.732446 * (u-128));
     if(r > 255) r = 255;
     if(g > 255) g = 255;
     if(b > 255) b = 255;
     if(r < 0) r = 0;
     if(g < 0) g = 0;
     if(b < 0) b = 0;
     pixel[0] = r * 220 / 256;
     pixel[1] = g * 220 / 256;
     pixel[2] = b * 220 / 256;
     return pixel32;
}



void record::StopRecord(){
    fclose(fp_nv12);
   // delete filename1;
}

void record::CloseDevice(){
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if ( ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
        errno_exit("VIDIOC_STREAMOFF");

    unsigned int i;
    for (i = 0; i < n_buffers; ++i){
        if (-1 == munmap(buffers[i].start, buffers[i].length))
            errno_exit("munmap");
    }

     free(buffers);
//qDebug()<<"run here";
     if (-1 == close(fd))
         errno_exit("close");
     fd = -1;
     //delete device;
     //delete filename1;
}

MFC::MFC(){
init_mfc();
}

SSBSIP_MFC_ERROR_CODE       MFC::init_mfc(){
    //filename1 = "test.nv12\0";


    filename2 = "mfctest.h264";
    fp_nv12 = fopen("/sdcard/test.nv12" , "rb");
    if(fp_nv12 == NULL)
    {
        qDebug()<<"Open test.nv12 failed!";
    }

    fp_strm = fopen(filename2 , "wb");


    SSBSIP_MFC_ERROR_CODE ret;
    unsigned int buf_type = NO_CACHE;
    width = width_fd;
    height = height_fd;
    param = (SSBSIP_MFC_ENC_H264_PARAM *)malloc(sizeof(SSBSIP_MFC_ENC_H264_PARAM));
    memset ( param , 0 , sizeof(SSBSIP_MFC_ENC_H264_PARAM) );

    param->codecType = H264_ENC;
    param->SourceWidth = 640;
    param->SourceHeight = 480;
    param->IDRPeriod = 100;
    param->SliceMode = 0; // 0,1,2,4
    param->RandomIntraMBRefresh = 0;
    param->EnableFRMRateControl = 1; // this must be 1 otherwise init error
    param->Bitrate = 2048000;
    param->FrameQp = 30; //<=51, the bigger the lower quality
    param->FrameQp_P = param->FrameQp+1;
    param->QSCodeMin = 10; // <=51
    param->QSCodeMax = 51; // <=51
    param->CBRPeriodRf = 120;
    param->PadControlOn = 0;
    param->LumaPadVal = 0; // <=255
    param->CbPadVal = 0; //<=255
    param->CrPadVal = 0; //<=255
    param->FrameMap = 0; // encoding input mode (0=linear, 1=tiled)

    param->ProfileIDC = 1; // 0=main,1=high,2=baseline
    param->LevelIDC = 40; // level 4.0
    param->FrameQp_B = param->FrameQp+3;
    param->FrameRate = 25000; // real frame rate = FrameRate/1000 (refer to S5PV210 datasheet Section 6.3.4.2.2)
    param->SliceArgument = 0;
    param->NumberBFrames = 0; //<=2
    param->NumberReferenceFrames = 2; // <=2
    param->NumberRefForPframes = 2; // <=2
    param->LoopFilterDisable = 1; // 0=enable, 1=disable
    param->LoopFilterAlphaC0Offset = 0; // <=6
    param->LoopFilterBetaOffset = 0; // <=6
    param->SymbolMode = 1; // 0=CAVLC, 1=CABAC
    param->PictureInterlace = 0; // Picture AFF 0=frame coding, 1=field coding, 2=adaptive
    param->Transform8x8Mode = 1; // 0=only 4x4 transform, 1=allow 8x8 trans, 2=only 8x8 trans
    param->EnableMBRateControl = 0;
    param->DarkDisable = 0;
    param->SmoothDisable = 0;
    param->StaticDisable = 0;
    param->ActivityDisable = 0;



    hOpen = SsbSipMfcEncOpen( &buf_type);
    if ( hOpen == NULL){
        printf("SsbSipMfcEncOpen failed\n");
        ret = MFC_RET_FAIL;
        return ret;
    }

    if (SsbSipMfcEncInit(hOpen , param) != MFC_RET_OK){
        printf("SsbSipMfcEncInit failed\n");
        ret = MFC_RET_FAIL;
        goto out;
    }

    if (SsbSipMfcEncGetInBuf(hOpen , &input_info) != MFC_RET_OK){
        printf("SsbSipMfcEncGetIn failed\n");
        ret = MFC_RET_FAIL;
        goto out;
    }

    ret = SsbSipMfcEncGetOutBuf( hOpen , &output_info );//获取输出缓存
    if(output_info.headerSize <= 0){
        printf(" headerSize encoding failed\n ");
        ret = MFC_RET_FAIL;
        goto out;
    }

    headerSize = output_info.headerSize;
//   qDebug()<<"header size:"<<headerSize;
//   qDebug()<<"header content:\t"<<(unsigned char *)output_info.StrmVirAddr;
    memcpy( header , output_info.StrmVirAddr , headerSize);//获取视频文件头

    //可选，将文件头写入文件
    fwrite(header , 1 , headerSize , fp_strm);

    printf( "MFC init success:: Yphy(0x%08x) Cphy(0x%08x)\n" , input_info.YPhyAddr ,
            input_info.CPhyAddr);
    return ret;
out:
  //  fprintf(stderr, "run here");
    SsbSipMfcEncClose(hOpen);
    return ret;
}

//------ unused function -----------------------------------
int MFC::getHeader(unsigned char **p){
    *p = header;
    return headerSize;
}
//----------------------------------------------------------------------
bool MFC::Encode(){
    int readsize;
    //int frame = 0;

        //-----@0421 get the UV package from the NV12 mode -----------------------------
//                readsize = fread( input_info.YVirAddr , 1 , width * height , fp_nv12 );
//                if( readsize == 0){
//                    printf("Y readsize == 0\n");
//                    break;
//                }
//                readsize == fread( input_info.CVirAddr , 1 , (width * height) >>1 , fp_nv12);
//                if( readsize == 0){
//                    printf("CbCr readsize == 0\n");
//                    break;
//                }
        //--------------------------------------------------------------------------------------------------------

                memcpy( input_info.YVirAddr, buff420sp, width_fd*height_fd );
                memcpy( input_info.CVirAddr, buff420sp + width_fd*height_fd, (width_fd*height_fd)>>1 );


            if( SsbSipMfcEncExe(hOpen) != MFC_RET_OK ){
                printf("Encoding Failed\n");
                return false;
            }
            SsbSipMfcEncGetOutBuf(hOpen , &output_info);
            if( output_info.StrmVirAddr == NULL){
                printf("SsbSipMfcEncGetOutBuf failed\n");
                return false;
            }
            readsize = output_info.dataSize;
           // qDebug()<<"output_info.dataSize\t"<<output_info.dataSize;
            fwrite(output_info.StrmVirAddr , 1 , readsize , fp_strm);
          //  printf("frame:%d\n" , frame++);


            return true;
}

void MFC::closeMFC(){
    SsbSipMfcEncClose(hOpen);
    free(param);
    //fclose(fp_nv12);
    fclose(fp_strm);
//    QString command = "rm /sdcard/test.nv12";
//    char *S;
//    S = command.toAscii().data();
    //system(S);
}

void record::display_error(QString err)
{
    //QMessageBox::warning(this,"error", err);
    qDebug()<<"Error\t"<<err;
}

//**************  x264 encoder   ********************
// x264Encoder:: x264Encoder()
// {

// }


//int x264Encoder::x264EncoderInit(void)
//{
//    fp_strm_x264 = fopen("/testx264.h264","wb");

//    width = width_fd ;
//    height = height_fd;
//    i_frame = 0;

//    /* Get default params for preset/tuning */
//    int retPreset = x264_param_default_preset( &param, "medium", NULL ) ;
//    if(retPreset < 0 )
//        return -1;

//    /* Configure non-default params */
//    param.i_csp = X264_CSP_I420;
//    param.i_width  = width;
//    param.i_height = height;
//    param.b_vfr_input = 0;
//    param.b_repeat_headers = 1;
//    param.b_annexb = 1;

//    /* Apply profile restrictions. */
//    int returnXprofile =  x264_param_apply_profile( &param, "high" );
//    if(  returnXprofile < 0 ) {
//        qDebug()<<" x264_param_apply_profile < 0   FAIL";
//        return -1;
//    }


//    if( x264_picture_alloc( &pic, param.i_csp, param.i_width, param.i_height ) < 0 ){
//        qDebug()<<"x264_picture_alloc < 0   FAIL";
//        return -1;
//     }

//    /* open the encoder*/
//    h = x264_encoder_open( &param );
//    if( !h ){
//        qDebug()<<"x264_encoder_open  FAIL";
//        x264_encoder_close( h );
//     }



//}

//int x264Encoder::x264EncoderExec(int width, int height)
//{
//    int luma_size = width * height;
//    int chroma_size = luma_size / 4;
//    /* Encode frames */
//    for( ;; i_frame++ )
//    {

//        //*********    memcpy     *******************
//        memcpy( pic.img.plane[0], buffI420p, luma_size );
//        memcpy( pic.img.plane[1], buffI420p + luma_size, chroma_size );
//        memcpy( pic.img.plane[2], buffI420p + (luma_size + luma_size /4 ), chroma_size );
//        //*********    memcpy     *******************

//        pic.i_pts = i_frame;
//        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, &pic, &pic_out );
//        if( i_frame_size < 0 )
//        {
//            x264_encoder_close( h );
//        }
//        else if( i_frame_size )
//        {
//            if( !fwrite( nal->p_payload, i_frame_size, 1, fp_strm_x264 ) )
//                 x264_encoder_close( h );
//        }
//    }
//    /* Flush delayed frames */
//    while( x264_encoder_delayed_frames( h ) )
//    {
//        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, NULL, &pic_out );
//        if( i_frame_size < 0 )
//        {
//            x264_encoder_close( h );
//        }
//        else if( i_frame_size )
//        {
//            if( !fwrite( nal->p_payload, i_frame_size, 1, fp_strm_x264) )
//               x264_encoder_close( h );
//        }
//    }

//    x264_encoder_close( h );
//    x264_picture_clean( &pic );
//    return 0;
//}

//int record::videoX264(  int widthPara , int heightPara )
//{
//    int width, height;
//    x264_param_t param;
//    x264_picture_t pic;
//    x264_picture_t pic_out;
//    x264_t *h;
//    int i_frame = 0;
//    int i_frame_size;
//    x264_nal_t *nal;
//    int i_nal;


//    width = widthPara;
//    height = heightPara;

//    /* Get default params for preset/tuning */
//    if( x264_param_default_preset( &param, "medium", NULL ) < 0 )
//        goto fail;

//    /* Configure non-default params */
//    param.i_csp = X264_CSP_I420;
//    param.i_width  = width;
//    param.i_height = height;
//    param.b_vfr_input = 0;
//    param.b_repeat_headers = 1;
//    param.b_annexb = 1;

//    /* Apply profile restrictions. */
//    if( x264_param_apply_profile( &param, "high" ) < 0 )
//        goto fail;

//    if( x264_picture_alloc( &pic, param.i_csp, param.i_width, param.i_height ) < 0 )
//        goto fail;
//#undef fail
//#define fail fail2

//    h = x264_encoder_open( &param );
//    if( !h )
//        goto fail;
//#undef fail
//#define fail fail3


//    //------------------------------------------------------------------------------------------------------------
//    int luma_size = width * height;
//    int chroma_size = luma_size / 4;
//    /* Encode frames */
//    for( ;; i_frame++ )
//    {
//        /* Read input frame */
//        if( fread( pic.img.plane[0], 1, luma_size, stdin ) != luma_size )
//            break;
//        if( fread( pic.img.plane[1], 1, chroma_size, stdin ) != chroma_size )
//            break;
//        if( fread( pic.img.plane[2], 1, chroma_size, stdin ) != chroma_size )
//            break;
//        //*********    memcpy     *******************

//        pic.i_pts = i_frame;
//        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, &pic, &pic_out );
//        if( i_frame_size < 0 )
//            goto fail;
//        else if( i_frame_size )
//        {
//            if( !fwrite( nal->p_payload, i_frame_size, 1, stdout ) )
//                goto fail;
//        }
//    }
//    /* Flush delayed frames */
//    while( x264_encoder_delayed_frames( h ) )
//    {
//        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, NULL, &pic_out );
//        if( i_frame_size < 0 )
//            goto fail;
//        else if( i_frame_size )
//        {
//            if( !fwrite( nal->p_payload, i_frame_size, 1, stdout ) )
//                goto fail;
//        }
//    }

//    x264_encoder_close( h );
//    x264_picture_clean( &pic );
//    return 0;

//#undef fail
//fail3:
//    x264_encoder_close( h );
//fail2:
//    x264_picture_clean( &pic );
//fail:
//    return -1;
//}


