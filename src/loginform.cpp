#include <loginform.hpp>
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include "ui_loginform.h"

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm),
    mousePressXOrd(0),
    mousePressYOrd(0)
{
    ui->setupUi(this);

    prevX = x();
    prevY = y();
    parentX = parent->geometry().x();
    parentY = parent->geometry().y();
    parentW = parent->geometry().width();
    parentH = parent->geometry().height();
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::mousePressEvent(QMouseEvent *event)
{
    mousePressXOrd = event->x();
    mousePressYOrd = event->y();
}

void LoginForm::mouseMoveEvent(QMouseEvent *event)
{
    int globalX = event->globalX();
    int globalY = event->globalY();

    /*
     * Check if the login form is within the screen
     * If yes then move it else dont move
     */
    if((globalX - mousePressXOrd - parentX) >= 0 &&
        (globalX - mousePressXOrd - parentX + width()) < parentW &&
        (globalY - mousePressYOrd - parentY) >= 0 &&
        (globalY - mousePressYOrd - parentY + height()) < parentH)
    {
        move(globalX - mousePressXOrd - parentX,
             globalY - mousePressYOrd - parentY);
    }
}

void LoginForm::mouseReleaseEvent(QMouseEvent *event)
{
    if(x() != prevX ||
        y() != prevY)
    {
        emit positionChanged(x(),y());
        qDebug() << "CHANGE" << x() << y();
    }
}
