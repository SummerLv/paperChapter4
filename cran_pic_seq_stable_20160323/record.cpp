#include "record.h"
const int width_fd = 320;
const int height_fd = 240;
int fsize = 0;
int frames = 200;
unsigned char *fbuf = NULL;
static QString ipaddr1 = "202.115.11.128";//中心平台地址
//static QString ipaddr1 = "192.168.5.166";//中心平台地址
static int port1 = 10150;


static void errno_exit(const char * s) {
    fprintf(stderr, "%s error %d, %s/n", s, errno, strerror(errno));

    exit(EXIT_FAILURE);
}

PUBFILE *PUBFILE::m_PubFile = NULL;

PUBFILE::~PUBFILE(){
    delete fp_nv12;
    delete fp_strm;
    delete filename1;
    delete filename2;
}

record::record(QObject *parent) :
    QThread(parent),fd(-1){
    //connect_server();   //@1113 don't use 3g
    fsize = width_fd * height_fd *2 / 32 *24;
    //frames = 200000;
    frameNum = 0;
    fbuf_flat = 0;
    Width = width_fd;
    Height = height_fd;
    globalPubFile = PUBFILE::GetPubFileObj();

   // encode = new MFC();

    if(re_qdir.exists("/sdcard") == true){
       globalPubFile->filename1 = "/sdcard/test.nv12\0";
    }
    else{
       globalPubFile->filename1 = "/test.nv12\0";
    }

//    fbuf = (unsigned char*)malloc(frames * fsize);
//    if(NULL == fbuf){
//        printf("calloc failed");
//    }
    img_tcpsocket = new QTcpSocket;
    timer = new QTimer;

    recordflag = false;
    stopThread = false;
    img_tcpsocket->connectToHost(QHostAddress(ipaddr1), port1);
    connect(img_tcpsocket , SIGNAL(connected()) , this , SLOT(setflag()));
    //connect(timer,SIGNAL(timeout()) , this , SLOT(connect_server()));
    //OpenFile();
    OpenDevice();
    StartCapturing();
    qDebug()<<"in line 61";
}

record::~record(){
   // StopRecord();
    CloseDevice();
}

void record::connect_server(){
    qDebug()<<"connecting";
        QString command = tr("pppd call wcdma");
        char *S;
        S = command.toAscii().data();
        system(S);
        qDebug()<< "Connect Success";
}

void record::setflag()
{
    qDebug()<<" setflag()  recordflag = true";
    recordflag = true;
}

void record::run(){
//    timer->start(5000);
//    //while ( (stopThread == false) && (frames-- > 0)){
//    /**8月19号增加->**/
//    while ( (stopThread == false)){
   // recordflag = true;
    qDebug()<< "in the run funtion   ,in line 90";
    while(1)
    {
        /*  这里这一句不能删除
        */
            qDebug()<<"the recordflag is     "<< recordflag;
        if(recordflag == true)
            {
                    qDebug()<<"----> [202.115.11.128:10150] connected   ||  start record-----> ";
                    StartRecord();
                    break;
            }
    }
  //  }
    //encode->closeMFC();
    /**<-8月19号增加**/

//    qDebug()<<"fbuf_flat"<<fbuf_flat;
//    fwrite(fbuf, fsize * fbuf_flat, 1, fp_nv12);
//    free(fbuf);
//    qDebug()<<"record ok";
}

bool record::OpenFile(){
    fp_nv12 = fopen("test1.nv12", "wa+");
    if( fp_nv12 == NULL)
        return false;
    return true;
}

void record::OpenDevice(){
    //nCount = 200;
   device = CAMERA_RECORD_NAME;
    struct stat st;
    qDebug()<<"in the function openDevice()";
    //打开设备
    if (-1 == stat(device, &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s/n", device, errno,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device/n", device);
        exit(EXIT_FAILURE);
    }

    fd = open(device, O_RDWR /* required */| O_NONBLOCK, 0);

    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s/n", device, errno,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    //查询并设置设备功能
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

    struct v4l2_cropcap cropcap;  //设置摄像头的捕捉能力，包括捕捉方框的左上角坐标和宽高(bound)，默认捕捉方框左上角坐标和宽高（defect）
    struct v4l2_crop crop;  //设置采集窗口参数，包括其类型 大小等
    memset(&cropcap , 0 , sizeof(cropcap));

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  //表示支持图像获取

    if ( ioctl(fd, VIDIOC_CROPCAP, &cropcap) < 0) {
         fprintf(stderr, "VIDIOC_CROPCAP can't settled\n");
    }
    else{
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        crop.c.left = cropcap.defrect.left; /* reset to default */
//        crop.c.top = cropcap.defrect.top;
//        crop.c.width = 640;
//        crop.c.height = 480;
        crop.c =cropcap.defrect;
         if ( ioctl(fd, VIDIOC_S_CROP, &crop) < 0){
           fprintf(stderr, "VIDIOC_S_CROP ERROR\n");
         }
    }
//    printf("cropcap.defrect = (%d , %d , %d , %d)\n", cropcap.defrect.left,
//            cropcap.defrect.top ,  cropcap.defrect.width ,  cropcap.defrect.height);
   // struct v4l2_format fmt;
    memset(&fmt, 0, sizeof fmt);
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = Width;
    fmt.fmt.pix.height      = Height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
   // fmt.fmt.pix.sizeimage = (fmt.fmt.pix.width * fmt.fmt.pix.height * 16) / 8;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (::ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {//设置驱动的频捕获格式
        Valid = false;
        fprintf(stderr, "VIDIOC_S_FMT failed\n");
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

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s/n", device);
        exit(EXIT_FAILURE);
    }

    buffers = (struct buffer *)calloc(req.count, sizeof(struct buffer ));  //为多个数据申请内存空间

    if (!buffers) {
        fprintf(stderr, "Out of memory/n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
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
    qDebug()<<"in the function startcapturing";
    unsigned int i;
    enum v4l2_buf_type type;
        for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            memset( &buf , 0 , sizeof(buf) );

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (  ioctl(fd, VIDIOC_QBUF, &buf) < 0)//将拍摄的图像数据放入缓存队列
                errno_exit("VIDIOC_QBUF");
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if ( ioctl(fd, VIDIOC_STREAMON, &type) < 0 )
            errno_exit("VIDIOC_STREAMON");
        fprintf(stderr, "Start Record StartStream OK!\n");
}

void record::StartRecord(){
    while(1){
       // printf("frames:%d",frames);
        //qDebug()<< "in the startrecord() function , printing this line";
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            /* Timeout. */
            tv.tv_sec = 3;
            tv.tv_usec = 0;

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
    qDebug()<<"record over";
}

bool record::readframe(){
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if ( ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            errno_exit("VIDIOC_DQBUF");
        }
    assert(buf.index < n_buffers);

   // qDebug()<<"buf.length"<<(int)buf.length;
    gloindex = buf.index;
    process_image(buffers[buf.index].start, buffers[buf.index].length);

    if ( ioctl(fd, VIDIOC_QBUF, &buf) < 0)
        errno_exit("VIDIOC_QBUF");
    msleep(40);
    return true;
}

void record::process_image(void *p, int size) {
    //2015.4.1
/*
    int outLength = size / 32 * 24;
    char *out = (char*)malloc(outLength);
    void *data = p;
    YUV422To420((char*)data , out , Width , Height);
    fwrite(out, outLength , 1 , fp_nv12);
*/
    //2015.0401

     int outLength = size;
     unsigned char *out = (unsigned char*)malloc(outLength);
    outLength = compress_yuyv_to_jpeg(out,outLength,0,fmt.fmt.pix.pixelformat);
    //QByteArray byte;
           // byte = QByteArray((const char*)out,outLength);
          // qDebug()<<"convert  compress_yuyv_to_jpeg    "<<outLength;
          // qDebug()<<"picData.size()  is    "<<picData.size();
            if(picData.size() == 0){
                    QDataStream outbuffer(&picData , QIODevice::WriteOnly);
                  //  QBuffer picBuffer;
                   // picBuffer =  QBuffer((const char*)out,outLength) ;
                    outbuffer << (qint32)outLength;
                    picData.append((const char*)out,outLength);
                 //   outbuffer.device()->seek(0);
                  //  outbuffer << (qint32)outLength;
                    //qDebug()<<"byte"<<(qint32)picData[0];
              //      qDebug()<<"send frame "<<frames++<<", time:"<<QDateTime::currentDateTime();

                    img_tcpsocket->write(picData,picData.size());  //发送数据
                    img_tcpsocket->flush();

                    QDateTime current_date_time = QDateTime::currentDateTime();
                    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
                    ++frameNum;
                    qDebug()<<"[frameNum] : "<<frameNum<<"[frameSize]:"<<outLength<<"  [time] : "<<current_date;
                 //   qDebug()<<"emit got the frame , wait to display on the 210";
                    emit getFrame();     //触发显示（用信号和槽的机制触发）
            }

    /*
    QByteArray byte;//2015年3月10日
    byte = QByteArray(out);
   // qDebug()<<"byte"<<byte.size();
    qint64 siz = 0;
    siz = img_tcpsocket->write(byte,byte.size());
    qDebug()<<"size"<<siz;
    img_tcpsocket->flush();
   // memcpy( fbuf+fbuf_flat*outLength , out , outLength );
   // ++fbuf_flat;
    */
    free(out);
}

int record::compress_yuyv_to_jpeg(unsigned char *buffer, int size, int quality,uint32_t format)
{
    //TODO: figure out how to convert quality to simple_jpeg_encoder quality
  static int written;
    /*char fourcc[5]={0,0,0,0,0};
    memmove(fourcc,&format,4);
    printf("Compressing %s\n",fourcc);*/
    switch(format)
    {
        case V4L2_PIX_FMT_YUYV:{
            unsigned char* data = (unsigned char*) buffers[gloindex].start;
            int length = buffers[gloindex].length;
            written=s_encode_image(data,buffer,1024,FORMAT_CbCr422,Width,Height,length);
    }
            break;

        case V4L2_PIX_FMT_YVU420://WARNING: this is not properly implemented yet
        case V4L2_PIX_FMT_Y41P: //don't know how to handle
        case V4L2_PIX_FMT_GREY:
        default:
          //  written=s_encode_image((unsigned char*) buffers[gloindex].start,buffer,1024,FORMAT_CbCr400,Width,Height,buffers[gloindex].length);
            break;
    };
  return (written);
}



int record::YUV422To420(char *yuv422, char *yuv420, int width, int height)
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

void record::StopRecord(){
    //fclose( globalPubFile->fp_nv12);
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
    fclose(fp_nv12);

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
//globalPubFile = PUBFILE::GetPubFileObj();
}

SSBSIP_MFC_ERROR_CODE MFC::init_mfc(){
    //filename1 = "test.nv12\0";
    //2015年3月10日
    // globalPubFile->filename2 = "/sdcard/test.h264\0";
 //    printf("init mfc 1");
SSBSIP_MFC_ERROR_CODE ret;
ret = MFC_RET_FAIL;
     fp_nv12= fopen("test1.nv12" , "rb");//2015年3月10日
    if( fp_nv12 == NULL){
        qDebug()<<"Open test.nv12 failed!";
    }
//    FILE *fp_nv121 = fopen("test1.nv12","wb");
//    char* yuv2nv12_y =(char*) malloc(640*480);
//    if(yuv2nv12_y == NULL)
//        return ret;
//    char* yuv2nv12_u_v = ( char *)malloc(640*240);
//    if(yuv2nv12_u_v == NULL)
//        return ret;
//    char* yuv2nv12_u_v_temp = (char *)malloc(640*240);
//    if(yuv2nv12_u_v_temp == NULL)
//        return ret;
//    int readsize,i = 0;
//    for( ;i<127;i++)
//    {
//        readsize = fread(yuv2nv12_y,1,width*height,fp_nv12);
//        printf("readsize y: %d\n",readsize);
//        if(readsize == 0)
//            break;
//        else
//        {
//            readsize = fwrite(yuv2nv12_y,1,width*height,fp_nv121);
//            printf("writesize y: %d\n",readsize);
//        }
//        readsize = fread(yuv2nv12_u_v,1,width*height/2,fp_nv12);
//        printf("readsize uv: %d\n",readsize);
//        if(readsize == 0)
//            break;
//        else
//        {
//            int j,k;
//            for(j = 0;j<width*height/4;j++)
//            yuv2nv12_u_v_temp[j*2] = yuv2nv12_u_v[j];
//            k = 0;
//            for(j = width*height/4 ;j< width*height/2;j++)
//            {
//                yuv2nv12_u_v_temp[k*2 + 1] = yuv2nv12_u_v[j];
//                ++k;
//            }
//            fwrite(yuv2nv12_u_v_temp,1,width*height/2,fp_nv121);
//            printf("writesize uv: %d\n",readsize);
//        }
//    }
//            fclose(fp_nv12);
//            fclose(fp_nv121);
//            free(yuv2nv12_y);
//             free(yuv2nv12_u_v);
//              free(yuv2nv12_u_v_temp);
    //  fp_nv12= fopen("test1.nv12" , "rb");
    fp_strm = fopen( "test.h264" , "wb");
     printf("init mfc 2");

    unsigned int buf_type = NO_CACHE;
    width = width_fd;
    height = height_fd;
    param = (SSBSIP_MFC_ENC_H264_PARAM *)malloc(sizeof(SSBSIP_MFC_ENC_H264_PARAM));
    memset ( param , 0 , sizeof(SSBSIP_MFC_ENC_H264_PARAM) );

    param->codecType = H264_ENC;
    param->SourceWidth = 320;
    param->SourceHeight = 240;
    param->IDRPeriod = 100;
    param->SliceMode = 0; // 0,1,2,4
    param->RandomIntraMBRefresh = 0;
    param->EnableFRMRateControl = 1; // this must be 1 otherwise init error
    param->Bitrate = 2048000;
    param->FrameQp = 20; //<=51, the bigger the lower quality
    param->FrameQp_P = param->FrameQp;
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
    param->FrameQp_B = param->FrameQp;
    param->FrameRate = 25400; // real frame rate = FrameRate/1000 (refer to S5PV210 datasheet Section 6.3.4.2.2)
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
    param->EnableMBRateControl = 1;
    param->DarkDisable = 0;
    param->SmoothDisable = 0;
    param->StaticDisable = 0;
    param->ActivityDisable = 0;

    /*******6.05参数更改**********/
//        param->codecType = H264_ENC;
//        param->SourceWidth = 640;
//        param->SourceHeight = 480;
//        param->IDRPeriod = 100;
//        param->SliceMode = 0; // 0,1,2,4
//        param->RandomIntraMBRefresh = 0;
//        param->EnableFRMRateControl = 1; // this must be 1 otherwise init error
//        param->Bitrate = 2048000;
//        param->FrameQp = 40; //<=51, the bigger the lower quality
//        param->FrameQp_P = param->FrameQp+1;
//        param->QSCodeMin = 10; // <=51
//        param->QSCodeMax = 51; // <=51
//        param->CBRPeriodRf = 120;
//        param->PadControlOn = 0;
//        param->LumaPadVal = 0; // <=255
//        param->CbPadVal = 0; //<=255
//        param->CrPadVal = 0; //<=255
//        param->FrameMap = 0; // encoding input mode (0=linear, 1=tiled)

//        param->ProfileIDC = 1; // 0=main,1=high,2=baseline
//        param->LevelIDC = 40; // level 4.0
//        param->FrameQp_B = param->FrameQp+3;
//        param->FrameRate = 30; // real frame rate = FrameRate/1000 (refer to S5PV210 datasheet Section 6.3.4.2.2)
//        param->SliceArgument = 0;
//        param->NumberBFrames = 0; //<=2
//        param->NumberReferenceFrames = 2; // <=2
//        param->NumberRefForPframes = 2; // <=2
//        param->LoopFilterDisable = 1; // 0=enable, 1=disable
//        param->LoopFilterAlphaC0Offset = 0; // <=6
//        param->LoopFilterBetaOffset = 0; // <=6
//        param->SymbolMode = 1; // 0=CAVLC, 1=CABAC
//        param->PictureInterlace = 0; // Picture AFF 0=frame coding, 1=field coding, 2=adaptive
//        param->Transform8x8Mode = 1; // 0=only 4x4 transform, 1=allow 8x8 trans, 2=only 8x8 trans
//        param->EnableMBRateControl = 1;
//        param->DarkDisable = 0;
//        param->SmoothDisable = 0;
//        param->StaticDisable = 0;
//        param->ActivityDisable = 0;

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

    ret = SsbSipMfcEncGetOutBuf( hOpen , &output_info );//获取输出缓存
    if(output_info.headerSize <= 0){
        printf(" headerSize encoding failed\n ");
        ret = MFC_RET_FAIL;
        goto out;
    }

    headerSize = output_info.headerSize;
  //  qDebug()<<"header size:"<<headerSize;
    memcpy( header , output_info.StrmVirAddr , headerSize);//获取视频文件头

    //可选，将文件头写入文件
    fwrite(output_info.StrmVirAddr , 1 , headerSize ,  fp_strm);


    memset(&input_info,0,sizeof(input_info));
    if (SsbSipMfcEncGetInBuf(hOpen , &input_info) != MFC_RET_OK){
        printf("SsbSipMfcEncGetIn failed\n");
        ret = MFC_RET_FAIL;
        goto out;
    }


    printf( "MFC init success:: Yphy(0x%08x) Cphy(0x%08x)\n" , input_info.YPhyAddr ,
            input_info.CPhyAddr);
    return ret;
out:
  //  fprintf(stderr, "run here");
    SsbSipMfcEncClose(hOpen);
    return ret;
}

int MFC::getHeader(unsigned char **p){
    *p = header;
    return headerSize;
}

bool MFC::Encode(){
    int readsize;
    int frame = 0;
    while((fread( input_info.YVirAddr , 1 , width * height , fp_nv12 ) == width * height) &&
          ( fread( input_info.CVirAddr , 1 , (width * height) >>1 ,  fp_nv12) ==  (width * height)/2)){
//printf(" run run1\n ");
   // readsize = fread( input_info.YVirAddr , 1 , width * height , (FILE*)temData );
  //readsize = fread( input_info.YVirAddr , 1 , width * height , fp_nv12 );
//printf(" run run2\n ");
   // if( readsize == 0){
     //   printf("Y readsize == 0\n");
    //    break;
   // }
 //   readsize = fread( input_info.CVirAddr , 1 , (width * height) >>1 ,  (FILE*)temData);
  //  readsize = fread( input_info.CVirAddr , 1 , (width * height) >>1 ,  fp_nv12);
   // if( readsize == 0){
    //    printf("CbCr readsize == 0\n");
   //     break;
  //  }
    /*convert yv12 to nv12
     */
//SsbSipMfcEncSetInBuf(
//printf(" run run3\n ");
    if( SsbSipMfcEncExe(hOpen) != MFC_RET_OK ){
        printf("Encoding Failed\n");
        return false;
    }
    memset(&output_info,0,sizeof(output_info));
    SsbSipMfcEncGetOutBuf(hOpen , &output_info);
    if( output_info.StrmVirAddr == NULL){
        printf("SsbSipMfcEncGetOutBuf failed\n");
        return false;
    }
    readsize = output_info.dataSize;
    fwrite(output_info.StrmVirAddr , 1 , readsize , fp_strm);
  //  printf("frame:%d\n" , frame++);
    }
     printf(" encoding success\n ");
    return true;
}

void MFC::closeMFC(){
    SsbSipMfcEncClose(hOpen);
    free(param);
    fclose( fp_nv12);
    fclose( fp_strm);
 //   QString command = "rm /sdcard/test.nv12";
    char *S;
  //  S = command.toAscii().data();
 //   system(S);
}
