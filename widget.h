#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "vlc/vlc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
private:
    libvlc_instance_t * m_pInstance=nullptr;

    libvlc_media_player_t* m_pMediaPlayer = nullptr;
    libvlc_media_t* m_pMedia = nullptr;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btnOpen_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
