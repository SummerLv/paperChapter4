#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

//#include "/usr/local/jthread/include/jthread/jmutex.h"

#include "/home/summer/friendlyARM/jthread/include/jthread/jmutex.h"

#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"

#include "capture.h"
#include "encoder.h"
#include "yuv422_to_I420.h"
#include "jrtpsend.h"

#include "sys/time.h"

#define true 1
#define false 0

//extern "C"{
//#ifdef __cplusplus
//#define __STDC_CONSTANT_MACROS
//#ifdef _STDINT_H
//#undef _STDINT_H
//#endif
//#include <stdint.h>
//#endif

//}

using namespace std;
//char * dev_name ;
int frame_num;
//FILE *fp=NULL;
const char *filename="test.yuv";
//int src_width=640;
//int src_height=480;
//enum AVPixelFormat src_pix_fmt=AV_PIX_FMT_YUYV422;
//enum AVPixelFormat dst_pix_fmt=AV_PIX_FMT_YUV420P;
//const char *dst_size="640x480";
//const char *dst_filename="frame_yuyv420.yuv";

//int x264_width=640;
//int x264_height=480;
//int fps=25;
size_t yuv_size_2=640*480*3/2;
//unsigned char *yuv_buffer=NULL;

typedef unsigned char uint8_t;

// void checkerror(int rtperr)
// {
//         if (rtperr < 0)
//         {
//         std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
//                 exit(-1);
//         }
// }

RTPSession sess;
int status;

FILE *tmpFP264;


int main(int argc, char ** argv)
{
        
        uint16_t portbase,destport;
        uint32_t destip;  
        std:string ipstr;  //get the destiny ip from the std input stream
        
     //   NALU_t *n;


        printf("Please Enter:./test   framenum(500)\n");
        if(argc!=2)
        {
                printf("Please Enter:./test   framenum\n");
                exit(EXIT_FAILURE);
        }

        //dev_name=argv[1];

        frame_num=atoi(argv[1]);    //convert a string to an integer
        if(!frame_num)
                {
                        printf("frame_num should be over 0 frame\n");
                        exit(EXIT_FAILURE);
                }

        // @0506 here ,there is no need to test YUYV | YUV420 | tmpH264 , for Module Test        
        //openFiles();

//*****************************************************************************        

//****************** the capturing process *****************start*********
                printf("capture video begin\n");
        open_device();
        init_device();
        start_capturing();
        //fp = fopen(filename, "wa+"); //yuv422

        // ***********
        //alloc_image();
        //  ***********

        // ***********
        //open_x264file("test420.yuv");
        init_encoder();
        init_picture(yuv_size_2);
        //  ***********
        portbase = 33334;
        destport = 9000;
        //ipstr = "222.197.174.76";
        ipstr = "202.115.11.128";
        destip = inet_addr(ipstr.c_str()); 
        //destip = ntohl(destip); ///************** very important **************
        destip = htonl(destip);

//********************** get the rtp parameter from std input stream ****above ***
        // @2 Setting basic parameter ----------------------------------
        RTPUDPv4TransmissionParams transparams;
        transparams.SetPortbase(portbase);


        RTPSessionParams sessparams;
        sessparams.SetOwnTimestampUnit(1.0/90000.0);

        status=sess.Create(sessparams,&transparams);
        checkerror(status);

        RTPIPv4Address addr(destip,destport);

        status = sess.AddDestination(addr);
        checkerror(status);

        sess.SetDefaultPayloadType(96);
        sess.SetDefaultMark(false);
        sess.SetDefaultTimestampIncrement(90000.0/10.0);  // frame rate control 10 / 25

        RTPTime delay(0.040);//RTPTime delay(0.040)
        //RTPTime::Wait(delay);
        RTPTime starttime=RTPTime::CurrentTime();

//         //  @3.0    Preparation for sending  NALU package----------------------------------------------------
//         char sendbuf[1500];
//         char *nalu_payload;
//         int size=0;
//         unsigned int timestamp_increase=0,ts_current=0;
//         OpenBitstreamFile("test.h264");

//         n=AllocNALU(8000000);// alloc memory for the <struct NALU>  n->maxSize = 8_000_000 = 8 MB
//         int start=false;

//         //****************************************************************



//****************** the capturing process ********************end ******


//****************** x264 encode ******************************* start ***
        // open_yuvfile(filename);
        // alloc_image();
        // swscale_start();     // ******  yuv422 -> I420 *******
        // swscale_close();

        // open_x264file(dst_filename);
        // init_encoder();
        // init_picture(yuv_size);
        // encode_frame();      // **** encode x264 format ***
        // close_encoder();
//****************** x264 encode ******************************* end ***
//-----------------------------------------------------------
        //@1223--01
        struct timeval tpstart,tpend;
        float  timeuse;

        gettimeofday(&tpstart,NULL);        
///------ implement capture the pictures next line -----------------------------
        tmpFP264 = fopen("softEncodeH264.h264","wb");
        mainloop(frame_num);            // **** capture frame *****
        fclose(tmpFP264);
///-----------------------------------------------------------------------------
        gettimeofday(&tpend,NULL);
        timeuse=1000000 * (tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
        timeuse/=1000000;
        printf("timeuse total Used Time( Second ):%f\n",timeuse);       
        //@1223--02
        //-----------------------------------------------------------

        stop_capturing();
        uninit_device();
        close_device();


        close_encoder();




        return 0;
}
