#pragma once

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QSize>
#include <QTimer>
#include "can_comm.h"


class Window : public QWidget
{
public:
    Window(QWidget *parent=nullptr)
    {
        labelTitle = new QLabel("Value", this);
        labelTitle->setStyleSheet("font: 16px");
        labelTitle->setAlignment(Qt::AlignCenter);

        labelValue = new QLabel("0", this);
        labelValue->setStyleSheet("font: 16px");
        labelValue->setAlignment(Qt::AlignCenter);

        slider = new QSlider(Qt::Orientation::Vertical, this);
        slider->setMinimumWidth(100);
        slider->setRange(0, 100);

        layout = new QVBoxLayout(this);
        layout->addWidget(labelTitle);
        layout->addWidget(slider);
        layout->addWidget(labelValue);
        setLayout(layout);
        setMinimumSize(QSize(100, 300));

        connect(slider, &QSlider::sliderMoved, this, &Window::onSliderMoved);
    }

    int getValue()
    {
        return value;
    }

private:
    QLabel *labelTitle;
    QLabel *labelValue;
    QSlider *slider;
    QVBoxLayout *layout;
    int value;

    void onSliderMoved(int position)
    {
        value = position;
    }
};


class SenderApp : public Window
{
public:
    SenderApp(QWidget *parent=nullptr) 
            : Window(parent),
            sender("can0", std::bind(&SenderApp::messageReceived, this, _1, _2))
    {
        can_frame.can_dlc = 1;
        can_frame.can_id = 100;
        can_frame.data[0] = 0;

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SenderApp::sendMessage);
        timer->start(100);
    }

private:
    CAN_Comm sender;
    CAN_frame can_frame;
    QTimer *timer;

    void messageReceived(CAN_frame &message, CAN_stamp &stamp)
    { }

    void sendMessage()
    {
        can_frame.data[0] = (uint8_t)getValue();
        sender.send_message(can_frame);
    }
};


int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    SenderApp window;

    window.setWindowTitle("Sender");
    window.show();

    return app.exec();
}