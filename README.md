## 介绍

使用VLC与QT实现的一款播放器软件



## 技术栈

1. Qt
2. C/C++
3. QSS

## 标题栏拖动窗口

```c++
void TitleBar::mousePressEvent(QMouseEvent* event)
{
    //
    if (ReleaseCapture())
    {
        //获得当前窗口指针
        QWidget* pWindow = this->window();
        //若是顶层窗口
        if (pWindow->isTopLevel())
        {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
}
```

`ReleaseCapture` 函数是一个Windows API函数，它用于释放先前由 `SetCapture` 函数设置的鼠标捕获。在这个上下文中，通过调用 `ReleaseCapture` 来释放鼠标的捕获。

在窗口拖动的情境中，通常会在鼠标按下事件（`mousePressEvent`）中调用 `SetCapture` 来捕获鼠标，以便能够在鼠标移动时继续接收鼠标事件，即使鼠标离开了原始的窗口区域。而在鼠标释放事件（`mouseReleaseEvent`）中，通常需要调用 `ReleaseCapture` 来释放鼠标的捕获，以便正常恢复对鼠标事件的处理。

在给定的代码片段中，调用 `ReleaseCapture` 的目的是确保在标题栏上的鼠标按下事件后，如果发生窗口拖动，就能够正常释放鼠标的捕获，以避免潜在的问题。这是一种保证鼠标捕获正确释放的一般性做法，以确保应用程序能够正常处理后续的鼠标事件。






这行代码使用了 `SendMessage` 函数，向窗口发送一个系统命令消息，目的是模拟鼠标拖动窗口的效果。下面是对该行代码的解释：

```c
SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
```

- `pWindow->winId()`：获取窗口的唯一标识符（HWND），这是一个 Windows 操作系统中用于标识窗口的句柄。
- `HWND(pWindow->winId())`：将窗口标识符转换为 `HWND` 类型，以便在 Windows API 函数中使用。
- `WM_SYSCOMMAND`：系统命令消息，用于执行系统级命令。
- `SC_MOVE + HTCAPTION`：这是要执行的系统命令的参数。`SC_MOVE` 表示移动窗口，`HTCAPTION` 表示在窗口标题栏上移动鼠标。通过组合它们，实现了拖动窗口的效果。
- `0`：额外的消息参数，这里没有使用。

## 重写EventFilter事件

在 Qt 中，`eventFilter` 是一种事件过滤器，用于拦截和处理对象的事件。通过重写 `eventFilter` 方法，您可以自定义如何处理特定类型的事件，而无需修改对象的源代码。这种机制非常有用，可以在不修改类层次结构的情况下，为对象添加额外的行为。

默认情况下menu打开时的位置在父组件的下侧

![image-20240119193448418](readme.assets/image-20240119193448418.png)

通过重写EventFilter事件、可达到预期效果

![image-20240119193806865](readme.assets/image-20240119193806865.png)



```cpp
bool BtnOpenFile::eventFilter(QObject *watched, QEvent *event)
{
    if (m_pBtnArrow) {
        // 检查事件类型是否为菜单显示事件，并且事件源是否是与 m_pBtnArrow 相关联的菜单
        if (event->type() == QEvent::Show && watched == m_pBtnArrow->menu()) {
            // 计算菜单的新位置
            int menuXPos = m_pBtnArrow->menu()->pos().x() - LEFT_BUTTON_WIDTH;   // 默认是显示在当前按钮的左下方
            int menuWidth = m_pBtnArrow->menu()->size().width();
            int buttonWidth = m_pBtnArrow->size().width();
            QPoint pos = QPoint(menuXPos, m_pBtnArrow->menu()->pos().y() + 2);

            // 移动菜单到新位置
            m_pBtnArrow->menu()->move(pos);
            return true;  // 事件已处理
        }
    }

    return false;  // 事件未处理
}

```



## QSlider实现点击效果

经典的slider没有点击效果

要实现点击效果可以重写eventfilter实现点击效果

```c++
// 确定控件操作的基本参数
        int cxctl = 0;  // 滑块宽度
        int cxwnd = 0;  // 滑槽长度
        int mxpos = 0;  // 鼠标按下的位
        // 计算结果，并设置新计算得到的 position 值
        int scpos =pSliderctrl->minimum() +
                    (int)((pSliderctrl->maximum() - pSliderctrl->minimum()) *
                           ((mxpos - cxctl / 2.0) / (cxwnd - cxctl)));

        if (pSliderctrl->sliderPosition() == scpos)
        {
            break;
        }

        pSliderctrl->setSliderPosition(scpos);

    }while(0);
//为什么要开do-while循环？

```

![image-20240119201938131](readme.assets/image-20240119201938131.png)



## 信号的下发和处理

Qt信号最好集中在一个类中处理

这时会让外层的信号向内层下发信号





对于sig_OpenFile信号

```c++
connect(m_pBtnOpenFile, &QPushButton::clicked, this, &BtnOpenFile::on_openFile);
```

```c++
void BtnOpenFile::on_openFile()
{
    *;
    //在BtnOpenFIle中继续将信号下发
    emit sig_openFile(filelist);
}
```

```c++
 connect(m_pBtnFileOpen,&BtnOpenFile::sig_openFile,this,&vIdeoWidgetTopWidget::sig_OpenFile);//vIdeoWidgetTopWidget
```

```c++
connect(m_pTopWidget, &vIdeoWidgetTopWidget::sig_OpenFile, this, &VideoWidget::sig_OpenFile);//VideoWidget
```

```c++
connect(m_pVideoWidget, &VideoWidget::sig_OpenFile, this, &Widget::onOpenFile);
```

经btn::cliced->BtnOpenFile::`on_openFile`-> emit sig_openFile(filelist)->vIdeoWidgetTopWidget::sig_OpenFile->VideoWidget::sig_OpenFile->Widget::`onOpenFile`



高亮为槽函数、其余为信号

`可知信号可在信号之间传递`

```cpp
connect(m_pBtnFileOpen,&BtnOpenFile::sig_openFile,this,&vIdeoWidgetTopWidget::sig_OpenFile);//vIdeoWidgetTopWidget
//两个信号用connnet建立连接
```

`最后到Widget才用槽函数OnOPenFile处理`



## VLC

`pro文件添加Libvlc`

```c
INCLUDEPATH +=.\vlc-3.0.9.2\sdk\include
LIBS +=$$PWD\vlc-3.0.9.2\sdk\lib\libvlc.lib
LIBS +=$$PWD\vlc-3.0.9.2\sdk\lib\libvlccore.lib
```

![image-20240122134725535](readme.assets/image-20240122134725535.png)



流程图中包含了3个结构体：
libvlc_instance_t：代表一个libVLC的`实例`。
libvlc_media_t：代表一个`可以播放`的`媒体`。
libvlc_media_player_t：代表一个`VLC媒体播放器`（一个视频播放器播放一个视频）。`注意VLC并不仅仅用于媒体播放。`
创建或者以上3个结构体通过以下6个函数：

```c
libvlc_new()：创建libvlc_instance_t。
libvlc_media_new_path()：创建libvlc_media_t。
libvlc_media_player_new_from_media()：创建libvlc_media_player_t。
libvlc_media_player_release()：释放libvlc_media_player_t
libvlc_media_release()：释放libvlc_media_t。
libvlc_release()：释放libvlc_instance_t
可以已通过下面的函数控制媒体的播放或者暂停，这些函数都需要使用libvlc_media_player_t作为参数。这里处于简化的目的，只使用了播放和停止函数：
libvlc_media_player_play()：播放。
libvlc_media_player_pause()：暂停。
libvlc_media_player_stop()：停止。
除了上述3个函数之外，还包括libvlc_media_player_set_position()等函数，这里不再一一记录。
```



#### 步骤

1. 包含头文件 #include ”vlc/vlc.h"
2. 创建vic实例 m_plnstance = libvlc_new(O, nullptr);`VLC实例`
3. 销毁：    libvlc release(m plnstance);
4. 仓ll建媒体播放器 m pMediaPlayer = libvlc_media_player_new(m_plnstance);`可播放的媒体`
5. 设置路径
   m pMedia = libvlc_media_new_path(m_plnstance, fileName.toStdString().c_str()); 
6. 解析media
   libvlc_media_parse(m_pMedia);
7. 设置media
   libvlc_media_player_set_media(m_pMediaPlayer;,m_pMedia);
8. 设置播放的窗口句柄
   libvlc一mediajDlayer—set_hwnd(m_pMediaPlayer; (void*)(ui.video_widget->winld()));
9. 释放media
   libvlc_media_release(m_pMedia); mpMedia 5 nullptr;





## 几点注意事项



### 路径问题

```c++
fileName=QDir::toNativeSeparators(fileName);
```

再不同平台下的路径不一样

```
d:/1/2/3
```

```
d:\1\2\3
```



### libvlc_media_t的创建

创建libvlc_media_t有两种方法：

libvlc_media_new_path():`打开文件`

和libvlc_media_new_location()。：`打开协议`

简单描述一下这两个函数的区别：libvlc_media_new_location()用于`打开协议`，而libvlc_media_new_path()用于`打开文件`。因而传递给libvlc_media_new_path()的就是普通的文件路径（绝对路径例如D:\xxx.flv，或者相对路径例如xxx.flv），而传递给libvlc_media_new_location()的就是`协议地址`（例如“udp://….”，“http://”）。但是这里有一点需要注意，在VLC中“文件”也属于一种广义上的“协议”。因此使用libvlc_media_new_location()也`可以打开文件`，但是必须在文件路径前面加上“文件协议”的标记“file:///”。例如打开“F:\movie\cuc_ieschool.flv”下的视频，实际使用的代码如下所示。
libvlc_media_new_location (inst, "file:///F:\\movie\\cuc_ieschool.flv");
此外，VLC还支持很多“神奇”的协议，`比如输入“screen://”协议就可以进行屏幕录制，`代码如下。
libvlc_media_new_location (inst, "screen://");

把libVLC的弹出窗口嵌入到程序中
在这里我只实践过Windows下把libVLC的弹出窗口嵌入到程序中。将`窗口或者控件的句柄传递`给libvlc_media_player_set_hwnd()函数即可。
这里有一点需要注意，如果把libVLC弹出窗口嵌入到程序中的话，“全屏”功能就不能使用了。

### 关于libVLC加载的问题

在libVLC中可以通过libvlc_media_player_get_length()，libvlc_video_get_width()，libvlc_video_get_height()等函数获取到视频的时长，宽，高等信息。但是有一个很奇怪的现象：如果在调用完libvlc_media_player_play()之后立即调用上述3个函数的话，返回的值都是0，只有“等待”一段时间（例如调用sleep()）后再调用上述函数，才能得到正确的数值。对于这种现象，我觉得可能是libVLC加载完成之后，才能通过上述几个函数获得正确的值（自己推测的，还没有深究）。



### error

引入vlc/vlc.h后，编译代码会出现如下error
error C2065: " libvlc_media_read_cb ":未声明的标识符
解决办法
在vic头文件上方声明SSIZE T类型 

```c
#ifdef_WIN32 
#include <basetsd.h> typedef SSIZE_T ssize_t； #endif
#include "vlc/vlc.h"
```

运行问题
libvlc_new执行失败
解决办法，需要把libvlc的plugins放到可执行文件的目录

![](readme.assets/image-20240122141308995.png)

## 类图

### Wideget

主窗口类

```tex
//继承自定义的五窗口类
Class: Widget
+-------------------+
| - m_pTitleBar     |
| - m_pVideoWidget  |
| - m_pBottomCtrlBar|
| - m_pRightPlayList|
| - m_pVlc          |
| - m_isPlay        |
+-------------------+
| - initUI()        |
+-------------------+
| - on_closed()     |
| - onTimeSliderMoved(value: int)|
| - onOpenFile(fileList: QStringList)|
| - onPlay()        |
+-------------------+
| + Widget(parent: QWidget* = nullptr)|
| + ~Widget()       |
+-------------------+
| # mouseDoubleClickEvent(event: QMouseEvent*)|
| # mouseMoveEvent(event: QMouseEvent*)      |
| # keyPressEvent(event: QKeyEvent*)         |
+-------------------+
```



### TitleBar

标题栏类

```
+-------------------+
|      TitleBar     |
+-------------------+
| - m_pLogoBtn      |
| - m_pMinimodeBtn  |
| - m_pSetBtn       |
| - m_pMinBtn       |
| - m_pMaxBtn       |
| - m_pCloseBtn     |
+-------------------+
| - initUI()        |
+-------------------+
| - mousePressEvent(event: QMouseEvent*)   |
| - mouseDoubleClickEvent(event: QMouseEvent*)|
+-------------------+
| + TitleBar(p: QWidget* = nullptr)         |
| + ~TitleBar()                              |
+-------------------+
| # onClicked()                              |
+-------------------+
| sig_close()                                |
+-------------------+

```



### VideoWidget 

播放窗口

```
+-------------------+
|    VideoWidget    |
+-------------------+
| - m_dPos           |
| - m_pTopWidget     |
| - m_isPlay         |
+-------------------+
| - onSliderMoved(value: int)           |
+-------------------+
| + showTopWidget(show: bool)           |
| + setPlayStatus(play: bool)           |
+-------------------+
| + VideoWidget(parent: QWidget* = nullptr)|
| + ~VideoWidget()                      |
+-------------------+
| - nativeEventFilter(eventType: QByteArray, message: void*, result: qintptr*)|
+-------------------+
| # resizeEvent(event: QResizeEvent*)  |
+-------------------+
| sig_OpenFile(fileList: QStringList)   |
| sig_OpenFloder(path: QString)         |
| sig_OpenPlayList()                    |
| sig_Totalms(duration: qint64)         |
| sig_VideoPositionChanged(pos: qint64)|
| sig_SliderMoved(value: int)           |
+-------------------+

```

#### BtnOpenFile

将两个btn合并为一个组件

效果如下：



![image-20240119191145141](readme.assets/image-20240119191145141.png)

```
+----------------------+
|     BtnOpenFile      |
+----------------------+
| - m_pBtnOpenFile     |
| - m_pBtnArrow         |
| - m_totalWidth        |
+----------------------+
| + BtnOpenFile(parent: QWidget* = nullptr)|
+----------------------+
| - on_openFile()       |
| - on_OpenFloder()     |
+----------------------+
| sig_openFile(fileList: QStringList)       |
| sig_OpenFloder(path: QString)              |
+----------------------+
| # eventFilter(watched: QObject*, event: QEvent*) |
+----------------------+
| # paintEvent(event: QPaintEvent*)          |
+----------------------+

```

#### TimeSlider

实现了播放条

```tex
+-------------------+
|    TimeSlider     |
+-------------------+
| - m_bSliderPressed|
| - m_SliderHeight   |
+-------------------+
| - slider_mouseLButtonPress(slider: QObject*, event: QEvent*) |
| - setTimeSlideerRange(value: qint64)  |
| - setSliderValue(value: qint64)       |
+-------------------+
| + TimeSlider(parent: QWidget* = nullptr)|
+-------------------+
| - onSliderMoved(pos: int)            |
+-------------------+
| sig_SliderMove(value: int)           |
+-------------------+
| # eventFilter(watched: QObject*, event: QEvent*) |
+-------------------+

```

### CBottomCtrlBar

实现底部播放控制器

```
+--------------------------------------+
|             CBottomCtrlBar            |
+--------------------------------------+
| - m_pLabCurPlayTime: QLabel*         |
| - m_pLabTotalTime: QLabel*            |
| - m_pPlayBtnWidget: PlayBtnGroupWidget*|
| - m_pBtnToolBox: QPushButton*        |
| - m_pBtnEffect: QPushButton*         |
| - m_pBtnFullscreen: QPushButton*     |
+--------------------------------------+
| + showTimeLabel(bool isShow): void   |
| + shetCurPlayTime(const qint64& curMs): void |
| + setTotalTime(const qint64& totalMs): void |
+--------------------------------------+
| # formatTimeMs(const qint64& timeMs): QString |
+--------------------------------------+
| SIGNAL sig_play(): void              |
| SIGNAL sig_stop(): void              |
| SIGNAL sig_fullScreen(): void        |
+--------------------------------------+

+---------------------------+
|    PlayBtnGroupWidget     |
+---------------------------+
| // Contents of the class  |
+---------------------------+


```

#### playBtnGroupWidget

```
+------------------------+
|  PlayBtnGroupWidget    |
+------------------------+
| - m_pBtnStop: QPushButton* |
| - m_pBtnPrev: QPushButton* |
| - m_pBtnPlay: QPushButton* |
| - m_pBtnNext: QPushButton* |
| - m_pBtnAudio: BtnVolume*  |
+------------------------+
| + sig_play(): void     |
| + sig_stop(): void     |
| + sig_next(): void     |
| + sig_prev(): void     |
+------------------------+

+------------------------+
|      BtnVolume         |
+------------------------+
| // Contents of the class|
+------------------------+

+------------------------+
|  DlgVolumeSilder       |
+------------------------+
| // Contents of the class|
+------------------------+

```



### cVlckits

vlc工具类

```
+------------------------+
|      cVlcKits          |
+------------------------+
| - m_pInstance: libvlc_instance_t* |
| - m_pMediaPlayer: libvlc_media_player_t* |
| - m_pMedia: libvlc_media_t* |
| - m_pEvent_manager: libvlc_event_manager_t* |
| - m_timeVector: std::vector<libvlc_time_t> |
| - m_pMediaPlayerList: libvlc_media_list_player_t* |
| - m_pMediaList: libvlc_media_list_t* |
| - curIndex: int |
| - curDuration: libvlc_time_t |
+------------------------+
| + getCurDuration(): libvlc_time_t |
| + setCurDuration(val: libvlc_time_t): void |
| + addCurindex(): void |
| + getCurIndex(): int |
| + getTimeVector(): std::vector<libvlc_time_t> |
| + media_player(): libvlc_media_player_t* |
| + setSliderSoundPos(val: int): void |
| + setLabTime(text: const QString&): void |
| + setVideoPos(value: int): void |
| + initVLC(): int |
| + setTimeSliderPos(val: int): void |
| + play(fileNames: const QStringList&, hwndd: void*): int |
| + playing(): void |
| + pause(): void |
| + stop(): void |
+------------------------+
| SIGNAL sig_setTimeSliderPos(val: int): void |
| SIGNAL sig_setSliderSound(val: int): void |
| SIGNAL sig_setLabTime(text: const QString&): void |
+------------------------+
| cVlcKits() |
| ~cVlcKits() |
+------------------------+

```

