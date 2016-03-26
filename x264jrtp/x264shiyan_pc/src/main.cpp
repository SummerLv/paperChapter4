#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

#include "/usr/local/jthread/include/jthread/jmutex.h"

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

typedef unsigned char uint8_t;

using namespace std;

int         frame_num;
const char *filename    =   "test.yuv";
size_t      yuv_size_2  =   640*480*3/2;

RTPSession  sess;
int         status;


//extern FILE *tmpFP264;

FILE *tmpFP264;

int main(int argc, char ** argv)
{
        
        uint16_t    portbase,destport;
        uint32_t    destip;  
        std:string  ipstr;  //get the destiny ip from the std input stream
        



        printf("Please Enter:./test   framenum(500)\n");
        if(argc!=2)
        {
                printf("Please Enter:./test   framenum\n");
                exit(EXIT_FAILURE);
        }

  

        frame_num   =   atoi(argv[1]);    //convert a string to an integer
        if(!frame_num)
        {
            printf("frame_num should be over 0 frame\n");
            exit(EXIT_FAILURE);
        }



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
        // @ 1204 change the ip to the LAN IP
        //ipstr = "202.115.11.128";
        ipstr = "192.168.5.155";
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


        // printf("capture video is over\n");       
        // printf("encode x264 video is over\n");
        // printf("Wait to jrtpsend....\n");




        return 0;
}
