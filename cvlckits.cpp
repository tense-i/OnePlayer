#include "cvlckits.h"
#include <QMessageBox>

libvlc_media_player_t *cVlcKits::media_player()
{
    return m_pMediaPlayer;

}

libvlc_time_t cVlcKits::getDurations()
{
    return m_durations;
}

void cVlcKits::setSliderSoundPos(int val)
{
    emit sig_setSliderSound(val);
}

void cVlcKits::setLabTime(const QString &text)
{
    emit sig_setLabTime(text);
}

//全局函数、不是类的成员函数
void vlc_callback( const struct libvlc_event_t *p_event, void *p_data )
{
    cVlcKits * pWidget=static_cast<cVlcKits*> (p_data);
    if(pWidget)
    {
        //根据事件类型
        switch (p_event->type) {
            //视频位置变化
        case libvlc_MediaPlayerPositionChanged:
        {
            float pos= libvlc_media_player_get_position(pWidget->media_player());
            pWidget->setTimeSliderPos(pos*100);

            //这个函数不会自动补全
            int curSecs=libvlc_media_player_get_time(pWidget->media_player())/1000;
            //显示经历时间
            int curH=curSecs/3600;
            int curMinute=(curSecs-curH*3600)/60;
            int cursec=curSecs-curH*3600-curMinute*60;
            char buf[64]="";
            sprintf(buf,"%02d:%02d:%02d",curH,curMinute,cursec);
            QString str(buf);

            //显示总时间
            int totalSecs=pWidget->getDurations();
            int _h=totalSecs/3600;
            int _minute=(totalSecs-_h*3600)/60;
            int _sec=totalSecs-_h*3600-_minute*60;
            char buf2[64]="";
            sprintf(buf2,"%02d:%02d:%02d",_h,_minute,_sec);
            QString str2(buf2);
            QString text=str+"/"+str2;

            //因为vlc_callback函数是全局函数、不能访问ui
            pWidget->setLabTime(text);
        }
        break;
            //视频声音大小变化
        case libvlc_MediaPlayerAudioVolume:
        {
            int val= libvlc_audio_get_volume(pWidget->media_player());
            pWidget->setSliderSoundPos(val);
        }
        break;
        }
    }
}


//初始化VLC

int cVlcKits::initVLC()
{
    // vlc初始化
    m_pInstance = libvlc_new(0, nullptr);

    if (m_pInstance)
    {
        //用Instance初始化一个播放器
        m_pMediaPlayer = libvlc_media_player_new(m_pInstance);
        //创建成功
        if (m_pMediaPlayer)
        {
            //初始化事件管理器
            m_pEvent_manager = libvlc_media_player_event_manager(m_pMediaPlayer);
            //事件管理器管理事件
            //位置变化事件
            libvlc_event_attach(m_pEvent_manager, libvlc_MediaPlayerPositionChanged, vlc_callback, this);
            //音量大小变化事件
            libvlc_event_attach(m_pEvent_manager, libvlc_MediaPlayerAudioVolume, vlc_callback, this);

        }
        else
        {
            //初始化失败
            libvlc_release(m_pInstance);
            return -2;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

void cVlcKits::setTimeSliderPos(int val)
{
    emit sig_setTimeSliderPos(val);
}


int cVlcKits::play(const QString &fileName, void *hwndd)
{

    //设置路径
    m_pMedia = libvlc_media_new_path(m_pInstance, fileName.toStdString().c_str());

    if (m_pMedia)
    {
        //解析media
        libvlc_media_parse(m_pMedia);

        //设置media
        libvlc_media_player_set_media(m_pMediaPlayer, m_pMedia);

        //设置播放的窗口句柄
        libvlc_media_player_set_hwnd(m_pMediaPlayer, hwndd);

        //不能再外边调用该函数、再if外面vlc还未初始化、肯定崩溃
        //获取视频时长、毫秒级
        m_durations=libvlc_media_get_duration(m_pMedia)/1000;
        //释放media
        libvlc_media_release(m_pMedia);
        m_pMedia = nullptr;

        //播放视频
        libvlc_media_player_play(m_pMediaPlayer);
    }
    else
    {
        if(m_pMediaPlayer)
        {
            libvlc_media_player_release(m_pMediaPlayer);
            m_pMediaPlayer=nullptr;
        }
        if(m_pInstance)
        {
            libvlc_release(m_pInstance);
            m_pInstance=NULL;
        }
        return -1;
    }
    return 0;
}

void cVlcKits::playing()
{
    //若为暂停态
    if(libvlc_media_player_get_state(m_pMediaPlayer)==libvlc_state_t::libvlc_Paused)
    {
        //切换为播放态
        libvlc_media_player_play(m_pMediaPlayer);
    }
}

void cVlcKits::pause()
{
    if(libvlc_media_player_get_state(m_pMediaPlayer)==libvlc_state_t::libvlc_Playing)
    {
        libvlc_media_player_pause(m_pMediaPlayer);
    }
}

void cVlcKits::stop()
{
    if((libvlc_media_player_get_state(m_pMediaPlayer)==(libvlc_state_t::libvlc_Playing))
        ||(libvlc_media_player_get_state(m_pMediaPlayer)==(libvlc_state_t::libvlc_Paused)))
    {
        libvlc_media_player_stop(m_pMediaPlayer);
    }
}

cVlcKits::cVlcKits()
{
    initVLC();
}

cVlcKits::~cVlcKits()
{
    //释放资源
    if(m_pMediaPlayer)
    {
        libvlc_media_player_release(m_pMediaPlayer);
        m_pMediaPlayer=nullptr;
    }
    if (m_pInstance)
    {
        libvlc_release(m_pInstance);
        m_pInstance = nullptr;
    }
}
