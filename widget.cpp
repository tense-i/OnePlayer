#include "widget.h"
#include "ui_widget.h"
#include "vlc/vlc.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <iostream>

void vlc_callback( const struct libvlc_event_t *p_event, void *p_data )
{
    Widget * pWidget=static_cast<Widget*> (p_data);
    if(pWidget)
    {
        switch (p_event->type) {
        case libvlc_MediaPlayerPositionChanged:
        {
            float pos= libvlc_media_player_get_position(pWidget->media_player());
            pWidget->setTimeSliderPos(pos*100);

            //这个函数不会自动补全
            int curSecs=libvlc_media_player_get_time(pWidget->media_player())/1000;
            std::cout<<curSecs<<std::endl;
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
            pWidget->setLabtime(text);

        }
        break;
        case libvlc_MediaPlayerAudioVolume:
        {
           int val= libvlc_audio_get_volume(pWidget->media_player());
            pWidget->setSliderSoundPos(val);
        }
        break;
        }
    }
}

libvlc_media_player_t *Widget::media_player()
{
    return m_pMediaPlayer;

}

void Widget::setTimeSliderPos(int value)
{
    ui->sliderTimeLen->setValue(value);
}

void Widget::setSliderSoundPos(int val)
{
    ui->SliderVideoSound->setValue(val);
}

int Widget::getDurations()
{
    return m_durations;
}

void Widget::setLabtime(const QString &text)
{
    ui->labTime->setText(text);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // vlc初始化
    m_pInstance = libvlc_new(0, nullptr);

    if (m_pInstance)
    {
        m_pMediaPlayer = libvlc_media_player_new(m_pInstance);
        if (m_pMediaPlayer)
        {
            m_pEvent_manager = libvlc_media_player_event_manager(m_pMediaPlayer);

            libvlc_event_attach(m_pEvent_manager, libvlc_MediaPlayerPositionChanged, vlc_callback, this);

            libvlc_event_attach(m_pEvent_manager, libvlc_MediaPlayerAudioVolume, vlc_callback, this);

        }
        else
        {
            libvlc_release(m_pInstance);
            QMessageBox::information(this, u8"提示", u8"libvlc_media_player_new failed");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        QMessageBox::information(this, u8"提示", u8"libvlc_new failed");
        exit(EXIT_FAILURE);
    }

    ui->labTime->setText("");
    connect(ui->sliderTimeLen,&QSlider::sliderMoved,this,&Widget::onTimeSliderMoved);
    connect(ui->SliderVideoSound,&QSlider::sliderMoved,this,&Widget::onSliderSoundMoved);

}

Widget::~Widget()
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
    delete ui;
}


void Widget::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, u8"请选择视频文件",
                                                    "D:/testvideo",
                                                    u8"视频文件(*.mp4 *.flv);;所有文件(*.*);;");

    if (fileName.isEmpty())
        return;

    fileName = QDir::toNativeSeparators(fileName);  // 这一句很重要，决定m_pMedia是否创建成功




    //设置路径
    m_pMedia = libvlc_media_new_path(m_pInstance, fileName.toStdString().c_str());

    if (m_pMedia)
    {
        //解析media
        libvlc_media_parse(m_pMedia);

        //设置media
        libvlc_media_player_set_media(m_pMediaPlayer, m_pMedia);

        //设置播放的窗口句柄
        libvlc_media_player_set_hwnd(m_pMediaPlayer, (void*)(ui->vadio_Widget->winId()));

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
        QMessageBox::information(this, u8"提示", u8"libvlc_media_new_path failed");
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
        exit(EXIT_FAILURE);
    }
}


void Widget::on_btnOn_clicked()
{
    //若为暂停态
    if(libvlc_media_player_get_state(m_pMediaPlayer)==libvlc_state_t::libvlc_Paused)
    {
        //切换为播放态
        libvlc_media_player_play(m_pMediaPlayer);
    }
}


void Widget::on_btnPause_clicked()
{
    if(libvlc_media_player_get_state(m_pMediaPlayer)==libvlc_state_t::libvlc_Playing)
    {
        libvlc_media_player_pause(m_pMediaPlayer);
    }
}


void Widget::on_btnStiop_clicked()
{
    if((libvlc_media_player_get_state(m_pMediaPlayer)==(libvlc_state_t::libvlc_Playing))
        ||(libvlc_media_player_get_state(m_pMediaPlayer)==(libvlc_state_t::libvlc_Paused)))
    {
        libvlc_media_player_stop(m_pMediaPlayer);
    }
}

void Widget::onTimeSliderMoved(int position)
{
    libvlc_media_player_set_position(m_pMediaPlayer,position/100.0);
}

void Widget::onSliderSoundMoved(int pos)
{
    libvlc_audio_set_volume(m_pMediaPlayer,pos);
}



