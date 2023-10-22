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
    libvlc_event_manager_t*m_pEvent_manager=nullptr;
    qint64 m_durations=0;
public:
    libvlc_media_player_t* media_player();
    void setTimeSliderPos(int value);
    void setSliderSoundPos(int val);
    int getDurations();
    void setLabtime(const QString & text);
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btnOpen_clicked();
    void on_btnOn_clicked();

    void on_btnPause_clicked();

    void on_btnStiop_clicked();

    void onTimeSliderMoved(int position);

    void onSliderSoundMoved(int pos);
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
