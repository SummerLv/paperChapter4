#include "voiceplay.h"
#define baojing_youjinxing 0
#define baojing_youpengzhuang 1
#define baojing_youzhangai 2
#define baojing_zuojinxing 3
#define baojing_zuopengzhuang 4
#define baojing_zuozhangai 5

#define yujing_youjinxing 6
#define yujing_youpengzhuang 7
#define yujing_youzhangai 8
#define yujing_zuojinxing 9
#define yujing_zuopengzhuang 10
#define yujing_zuozhangai 11
#define zhiwenduibichenggong 12


struct WAV_HEADER
{
    char rld[4];        //RIFF标志符号
    int rLen;
    char wld[4];    //格式类型
    char fld[4];   //fmt

    int fLen;           //sizeof(wav format matex)

    short wFormatTag;                   //编码格式
    short wChannels;                    //声道数
    int nSamplesPersec;             //采样频率
    int nAvgBitsPerSample;        //WAVE文件采样大小
    short wBlockAlign;              //块对齐
    short wBitsPerSample;       //WAV文件采样大小

    char dld[4];                        //data
    int wSampleLength;          //音频数据的大小
} wav_header;


//const char* voiceFile[] = {"/voice/baojing_youjinxing.wav","/voice/baojing_zuopengzhuang.wav","/voice/baojing_youzhangai.wav",
//                               "/voice/baojing_zuojinxing.wav","/voice/baojing_zuopengzhuang.wav","/voice/baojing_zuozhangai.wav",
//                               "/voice/yujing_youjinxing.wav","/voice/yujing_zuopengzhuang.wav","/voice/yujing_youzhangai.wav",
//                               "/voice/yujing_zuojinxing.wav","/voice/yujing_zuopengzhuang.wav","/voice/yujing_zuozhangai.wav"};
VoicePlay::VoicePlay(QObject *parent)
{
    globaldata = GlobalData::getGlobalData();
   // qDebug() <<"enter Voice";

    playback_handle = NULL;             //PCM设备句柄
   fp = NULL;

}

VoicePlay::~VoicePlay()
{
    snd_pcm_close(playback_handle);
    fclose(fp);
}

//声音播放槽函数
void VoicePlay::playVoice()
{
    QString command = tr("madplay /Cest.mp3");
    char *S;
    S = command.toAscii().data();
    system(S);
}
/**********************************
 *函数名：setPcm()
 *函数功能：打开PCM并初始化
 *
 ************************************/
//void VoicePlay::setPcm()
void VoicePlay::run()
{
   const  char *voiceFile[] = {"/voice/baojing_youjinxing.wav","/voice/baojing_youpengzhuang.wav","/voice/baojing_youzhangai.wav",
                                   "/voice/baojing_zuojinxing.wav","/voice/baojing_zuopengzhuang.wav","/voice/baojing_zuozhangai.wav",
                                   "/voice/yujing_youjinxing.wav","/voice/yujing_youpengzhuang.wav","/voice/yujing_youzhangai.wav",
                                   "/voice/yujing_zuojinxing.wav","/voice/yujing_zuopengzhuang.wav","/voice/yujing_zuozhangai.wav",
                                    "/voice/zhiwenduibichenggong.wav"};
    quint8 alrm_voice_index =0;
    quint8 warn_voice_index = 0;
    quint8 no_voice_flag = 0;
    int err=0;

//    snd_pcm_t *playback_handle;             //PCM设备句柄
//    FILE *fp;

    snd_pcm_hw_params_t *hw_params;       //硬件信息和PCM流配置
    snd_pcm_uframes_t frames;
    snd_pcm_uframes_t periodsize;
    int dir = 0;
    int size =72;
    int nread=0;

    char buffer[72];
    //buffer = (char *)malloc(size);
    /*打开音频文件*/
    while(1)
    {
        fp = NULL;
        no_voice_flag =0;
        if(globaldata->fingerPage[0] < 0x0f)
            fp = fopen(voiceFile[zhiwenduibichenggong],"rb");
        if(globaldata->ultimate_warn_flag > 1) {
            switch(alrm_voice_index)
            {
            case 0:                                                     //塔机碰撞报警
                if(globaldata->craneList.at(0).leftCollision >1) {
                 //   qDebug()<<"leftCollision";                   //88888888888888888888
                    fp = fopen(voiceFile[baojing_zuopengzhuang],"rb");
                    break;
                }else if(globaldata->craneList.at(0).rightCollision >1) {
                    fp = fopen(voiceFile[baojing_youpengzhuang],"rb");
                    break;
                }else{
                    no_voice_flag = 1;
                }
                alrm_voice_index++;
                break;
            case 1:                                                     //保护区报警
                if(globaldata->left_forbid_warn_flag >1) {
                    fp = fopen(voiceFile[baojing_zuojinxing],"rb");
                    break;
                }else if (globaldata->right_forbid_warn_flag >1) {
                    fp = fopen(voiceFile[baojing_youjinxing],"rb");
                    break;
                }else if(globaldata->left_barrier_warn_flag >1) {
                    fp = fopen(voiceFile[baojing_zuozhangai],"rb");
                    break;
                }else if (globaldata->right_barrier_warn_flag >1) {
                    fp = fopen(voiceFile[baojing_youzhangai],"rb");
                    break;
                }else{
                    no_voice_flag = 1;
                }
                alrm_voice_index++;
                break;
            case 2:                                                 //限位报警
                //没有超重的音频文件/风速/变幅
                if(globaldata->craneList.at(0).overweight_limit_warn_flag >1) {
                    //fp = fopen(voiceFile[],"rb");
                    break;
                }
                alrm_voice_index++;
                break;
            default:
                break;
            }
        }
        //无报警再预警
       else if(globaldata->ultimate_warn_flag >0) {
            switch (warn_voice_index) {
            case 0:                                                                 //塔机碰撞预警
                if (globaldata->craneList.at(0).leftCollision >0) {
                    fp = fopen(voiceFile[yujing_zuopengzhuang],"rb");
                    break;
                }else if(globaldata->craneList.at(0).rightCollision >0) {
                    fp = fopen(voiceFile[yujing_youpengzhuang],"rb");
                    break;
                }else{
                    no_voice_flag =1;
                }
                warn_voice_index++;
                break;
            case 1:                                                                 //保护区预警
                if (globaldata->left_barrier_warn_flag >0) {
                    fp = fopen(voiceFile[yujing_zuozhangai],"rb");
                    break;
                }else if(globaldata->right_barrier_warn_flag >0) {
                    fp = fopen(voiceFile[yujing_youzhangai],"rb");
                    break;
                }else if(globaldata->left_forbid_warn_flag > 0) {
                    fp = fopen(voiceFile[yujing_zuojinxing],"rb");
                    break;
                }else if(globaldata->right_forbid_warn_flag >0) {
                    fp = fopen(voiceFile[yujing_youjinxing],"rb");
                    break;
                }else{
                    no_voice_flag =1;
                }
                warn_voice_index++;
                break;
            default:
                break;
            }
        }else{
            no_voice_flag =1;
        }
        if(alrm_voice_index >=3)
            alrm_voice_index = 0;
        if(warn_voice_index >=2)
            warn_voice_index = 0;

    if (fp == NULL) {
        //perror("open music file failed");
    }else if(no_voice_flag == 0){
        nread = fread(&wav_header,1,sizeof(wav_header),fp);
        /*
        printf("nread = %d\n",nread);
        qDebug() << "RIFF标志"<<wav_header.rld;
        qDebug() << "文件大小rLen:"<<wav_header.rLen;
        qDebug() << "声道数:"<<wav_header.wChannels;
        qDebug() << "采样频率:"<<wav_header.nSamplesPersec;
        qDebug() << "采样位数:"<<wav_header.wBitsPerSample;
        qDebug() << "wSampleLen:"<<wav_header.wSampleLength;
*/
        /*打开PCM，最后一个参数为0意味着标准配置*/
        if ((err = snd_pcm_open(&playback_handle,"default",SND_PCM_STREAM_PLAYBACK,0) < 0 )) {
            fprintf(stderr,"cannot open audio device %s (%s)\n","default",snd_strerror(err));
            exit(1);
        }
        /*分配snd_pcm_hw_params_t结构体*/
        if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
            fprintf(stderr,"cannot allocate hardware parameter structure (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*初始化hw_params*/
        if ((err = snd_pcm_hw_params_any(playback_handle,hw_params)) < 0) {
            fprintf(stderr,"cannot initialize hardware parameter structure (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*初始化访问权限*/
        if ((err = snd_pcm_hw_params_set_access(playback_handle,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED)) < 0 ) {
            fprintf(stderr,"cannot set access type (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*初始化采样格式*/
        if ((err = snd_pcm_hw_params_set_format(playback_handle,hw_params,SND_PCM_FORMAT_S16_LE)) < 0) {
            fprintf(stderr,"cannot set sample format (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的*/
        unsigned int val = wav_header.nSamplesPersec;
        //qDebug() << "val" << val;
        if ((err = snd_pcm_hw_params_set_rate_near(playback_handle,hw_params,&val,0)) < 0) {
            fprintf(stderr,"cannot set sample rate (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*设置通道数量*/
        if ((err = snd_pcm_hw_params_set_channels(playback_handle,hw_params,1)) < 0) {
            fprintf(stderr,"cannot set channels count (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*set period size to 32 frames*/
        frames = 32;

        periodsize = frames * 2;
        //periodsize = frames;
        if ((err = snd_pcm_hw_params_set_buffer_size_near(playback_handle,hw_params,&periodsize)) < 0) {
            //fprintf(stderr,"cannot set buffsize  (%s)\n",frames*2,snd_strerror(err));
            exit(1);
        }
        periodsize /= 2;
        if ((err = snd_pcm_hw_params_set_period_size_near(playback_handle,hw_params,&periodsize,0)) < 0) {
            //fprintf(stderr,"cannot set period size  (%s)\n",periodsize,snd_strerror(err));
            exit(1);
        }
        /*设置hw_params*/
        if ((err = snd_pcm_hw_params(playback_handle,hw_params)) < 0 ) {
            fprintf(stderr,"cannot set params (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*释放分配的snd_pcm_hw_params_t结构体*/
        //snd_pcm_hw_params_free(hw_params);
        /*完成硬件参数设置，使设备准备好*/
        if ((err = snd_pcm_prepare(playback_handle)) < 0 ) {
            fprintf(stderr,"cannot prepare audio interface for user   (%s)\n",snd_strerror(err));
            exit(1);
        }
        /*Use a buffer large enough to hold one period*/
        snd_pcm_hw_params_get_period_size(hw_params,&frames,&dir);
       // size = frames * 2; /*2bytes/sample ,2 channels*/
        //qDebug() << "frames" << frames;
        size = frames * wav_header.wBlockAlign;

       // int buf[128];
       // QString command = tr("/Cest.mp3");
        //char *S;
       // S = command.toAscii().data();

        //fseek(fp,58,SEEK_SET);
        //char *buffer;
       // buffer = (char *)malloc(size);
        //fprintf(stderr,"size = %d\n",size);
        int ret=0;
        memset(buffer , 0 , 72);
        while(1)
        {
            if (0 == (ret = fread(buffer,1,size,fp)) ) {
               // fprintf(stderr,"end of file on input\n");
                break;
            }else if(ret != size) {

            }
            //snd_pcm_prepare(playback_handle);
            /*写音频数据到PCM设备*/
            while ((ret = snd_pcm_writei(playback_handle,buffer,frames)) < 0)
            {
                usleep(2000);
                if (ret == -EPIPE) {
                   // fprintf(stderr,"underrun occurred\n");
                    snd_pcm_prepare(playback_handle);
                }else if(ret < 0){
                    fprintf(stderr,"error from writei:%s\n",snd_strerror(ret));
                }
            }
        }
        snd_pcm_close(playback_handle);
        fclose(fp);
       // free(buffer);
    }
    }
    msleep(1000);
}

/*
void VoicePlay::run()
{
    setPcm();
    //playVoice();
}
*/
