#include "widget.h"
#include "ui_widget.h"
#include "vlc/vlc.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include <QKeyEvent>
#include <QMouseEvent>
#include <windows.h>
#include <QFile>

/*
 * 回调函数
 *
 *
 *
 *
 * */

BOOL CALLBACK EnumVLC(HWND hwnd,LPARAM lParam)
{
    TCHAR szTitle[1024];
    int nLen= GetWindowTextW(hwnd,szTitle,1024);
    if(nLen>0)
    {
        EnableWindow(hwnd,FALSE);
        KillTimer(hwnd,1);
    }
    return TRUE;
}
void  CALLBACK TimeProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
    HWND hwnd2=FindWindowEx(nullptr,nullptr,nullptr,L"vlcMain");
    if(hwnd2)
    {
        EnumChildWindows(hwnd2,EnumVLC,NULL);
    }
}


void Widget::setTimeSliderPos(int value)
{
    ui->sliderTimeLen->setValue(value);
}

void Widget::setSliderSoundPos(int val)
{
    ui->SliderVideoSound->setValue(val);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),m_pVlc(new cVlcKits())
{
    ui->setupUi(this);
    InitQSS();
    this->setWindowTitle("vlcMain");
    int res=m_pVlc->initVLC();
    switch (res) {
    case -1:
        exit(EXIT_FAILURE);
    case -2:
        exit(EXIT_FAILURE);
    }
    ui->labTime->setText("");
    //信号与槽建立
    //slider拖动，调用onTimeSliderMoved函数
    connect(ui->sliderTimeLen,&QSlider::sliderMoved,this,&Widget::onTimeSliderMoved);
    connect(ui->SliderVideoSound,&QSlider::sliderMoved,this,&Widget::onSliderSoundMoved);

    //m_pVlc中的对象会发出信号 sig_setLabTime、由该槽函数解析
    connect(m_pVlc.get(),&cVlcKits::sig_setLabTime,[=](const QString & text){
        ui->labTime->setText(text);
    });
   //m_pVlc中的对象会发出sig_setTimeSliderPos
    connect(m_pVlc.get(),&cVlcKits::sig_setTimeSliderPos,[=](const int& val){
        ui->sliderTimeLen->setValue(val);
    });
    connect(m_pVlc.get(),&cVlcKits::sig_setSliderSound,[=](const int &val){
        ui->SliderVideoSound->setValue(val);
    });
}

Widget::~Widget()
{

    delete ui;
}


void Widget::on_btnOpen_clicked()
{
    QStringList fileNameList = QFileDialog::getOpenFileNames(this, u8"请选择视频文件",
                                                    "D:/testvideo",
                                                    u8"视频文件(*.mp4 *.flv);;所有文件(*.*);;");

    if (fileNameList.isEmpty())
        return;
        void* hwnd=(void*)ui->vadio_Widget->winId();
        if( m_pVlc->play(fileNameList,hwnd)==-1)
        {
            QMessageBox::information(this,"提示","播放失败");
            exit(EXIT_FAILURE);
        }

    SetTimer(nullptr,1,300,TimeProc);

}


void Widget::on_btnOn_clicked()
{
    m_pVlc->playing();
}


void Widget::on_btnPause_clicked()
{
    m_pVlc->pause();
}


void Widget::on_btnStiop_clicked()
{
    m_pVlc->stop();
}

void Widget::onTimeSliderMoved(int position)
{
    libvlc_media_player_set_position(m_pVlc->media_player(),position/100.0);
}

void Widget::onSliderSoundMoved(int pos)
{
    libvlc_audio_set_volume(m_pVlc->media_player(),pos);
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(this->isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {
        if(this->isFullScreen())
        {
            //正常显示
            this->showNormal();
        }
    }
}
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(libvlc_media_player_get_state(m_pVlc->media_player())==libvlc_state_t::libvlc_Paused)
        {
            //切换为播放态
            libvlc_media_player_play(m_pVlc->media_player());
        }
        else if(libvlc_media_player_get_state(m_pVlc->media_player())==libvlc_state_t::libvlc_Playing)
        {
            libvlc_media_player_pause(m_pVlc->media_player());
        }
    }
}


void Widget::InitQSS()
{
    QFile fileName=QFile(":/myqss.qss");
    fileName.open(QFile::ReadOnly);
    QString qss;
    qss = fileName.readAll();
    setStyleSheet(qss);
    fileName.close();
}
