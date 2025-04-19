#pragma once

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QSize>
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
        labelValue->setStyleSheet("font: 64px");
        labelValue->setAlignment(Qt::AlignCenter);

        layout = new QVBoxLayout(this);
        layout->addWidget(labelTitle);
        layout->addWidget(labelValue);
        setLayout(layout);
        setMinimumSize(QSize(150, 80));
    }

    void setValue(int value)
    {
        labelValue->setText(QString::number(value));
    }

private:
    QLabel *labelTitle;
    QLabel *labelValue;
    QVBoxLayout *layout;
};


class ReaderApp : public Window
{
public:
    ReaderApp(QWidget *parent=nullptr) 
            : Window(parent),
            reader("can0", std::bind(&ReaderApp::messageReceived, this, _1, _2))
    { }

private:
    void messageReceived(CAN_frame &message, CAN_stamp &stamp)
    {
        if(message.can_id != 100)
            return;
        setValue(message.data[0]);
    }

    CAN_Comm reader;
};


int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    ReaderApp window;

    window.setWindowTitle("Reader");
    window.show();

    return app.exec();
}