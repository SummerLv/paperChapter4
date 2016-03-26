#ifndef VOICEPLAY_H
#define VOICEPLAY_H

#include <QThread>
#include "dialog.h"
#include <alsa/asoundlib.h>
#include <QDebug>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <data.h>
class VoicePlay : public QThread
{
    Q_OBJECT
public:
    VoicePlay(QObject *parent = 0);
    ~VoicePlay();

        snd_pcm_t *playback_handle;             //PCM设备句柄
       FILE *fp;

public slots:
    void playVoice();
private:
    void run();
    void setPcm();
    GlobalData *globaldata;
};

#endif // VOICEPLAY_H
