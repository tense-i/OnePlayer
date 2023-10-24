#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "vlc/vlc.h"
#include "cvlckits.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

private:
    void InitQSS();
public:

    void setTimeSliderPos(int value);
    void setSliderSoundPos(int val);
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
    std::unique_ptr<cVlcKits> m_pVlc;

    // QWidget interface
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};




#endif // WIDGET_H
