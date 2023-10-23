#ifndef CVLCKITS_H
#define CVLCKITS_H

#include <QObject>
#include "vlc/vlc.h"

class cVlcKits:public QObject
{
    Q_OBJECT
private:
    libvlc_instance_t * m_pInstance=nullptr;

    libvlc_media_player_t* m_pMediaPlayer = nullptr;
    libvlc_media_t* m_pMedia = nullptr;
    libvlc_event_manager_t*m_pEvent_manager=nullptr;
    libvlc_time_t m_durations=0;
public:
    libvlc_media_player_t* media_player();
    libvlc_time_t getDurations();
    void setSliderSoundPos(int val);
    void setLabTime(const QString & text);
public:
    int initVLC();
    void setTimeSliderPos(int val);
    int play(const QString& fileName,void* hwndd);
    void playing();
    void pause();
    void stop();
signals:
    void sig_setTimeSliderPos(int val);
    void sig_setSliderSound(int val);
    void sig_setLabTime(const QString &text);
public:
    cVlcKits();
    ~cVlcKits();
};

#endif // CVLCKITS_H
