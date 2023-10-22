#include "widget.h"
#include "ui_widget.h"
#include "vlc/vlc.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

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
        if (!m_pMediaPlayer)
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
}

Widget::~Widget()
{
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

        //释放media
        libvlc_media_release(m_pMedia);
        m_pMedia = nullptr;

        //播放视频
        libvlc_media_player_play(m_pMediaPlayer);
    }
    else
    {
        QMessageBox::information(this, u8"提示", u8"libvlc_media_new_path failed");
        exit(EXIT_FAILURE);
    }
}

