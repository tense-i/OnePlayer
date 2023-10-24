#ifndef CVLCKITS_H
#define CVLCKITS_H

#include <QObject>
#include "vlc/vlc.h"
#include <vector>


class cVlcKits:public QObject
{
    Q_OBJECT
private:
    libvlc_instance_t * m_pInstance=nullptr;

    libvlc_media_player_t* m_pMediaPlayer = nullptr;
    libvlc_media_t* m_pMedia = nullptr;
    libvlc_event_manager_t*m_pEvent_manager=nullptr;
    std::vector<libvlc_time_t> m_timeVector;
    libvlc_media_list_player_t*m_pMediaPlayerList;
    libvlc_media_list_t*m_pMediaList;
    int curIndex=0;
    libvlc_time_t curDuration=0;
public:
    libvlc_time_t getCurDuration()  const;
    void setCurDuration(libvlc_time_t val);
    int addCurindex();
    int getCurIndex()const;
    std::vector<libvlc_time_t> getTimeVector()const;
    libvlc_media_player_t* media_player()const;
    void setSliderSoundPos(int val);
    void setLabTime(const QString & text);
public:
    int initVLC();
    void setTimeSliderPos(int val);

    int play(const QStringList& fileNames,void* hwndd);
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
