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
    /*
    if((globalX - mousePressXOrd - parentX) >= 0 &&
        (globalX - mousePressXOrd - parentX + width()) < parentW &&
        (globalY - mousePressYOrd - parentY) >= 0 &&
        (globalY - mousePressYOrd - parentY + height()) < parentH)
    {
        move(globalX - mousePressXOrd - parentX,
             globalY - mousePressYOrd - parentY);
    }
    */

    // Just move the window without restricting the boundaries
    move(globalX - mousePressXOrd - parentX - WINDOW_BORDER_OFFSET,
         globalY - mousePressYOrd - parentY - WINDOW_BORDER_OFFSET);
}

void LoginForm::mouseReleaseEvent(QMouseEvent *event)
{
    if(x() != prevX ||
        y() != prevY)
    {
        int curX = x();
        int curY = y();

        //See if the form is out of the screen boundary-
        //if yes then bring it inside the screen
        if(curX < parentX)
        {
            curX = parentX;
        }else if(curX > (parentW - width() - WINDOW_BORDER_OFFSET))
        {
            curX =parentW - width() - WINDOW_BORDER_OFFSET;
        }

        if(curY < parentY)
        {
            curY = parentY;
        }else if(curY > (parentH - height() - WINDOW_BORDER_OFFSET))
        {
            curY = parentH - height() - WINDOW_BORDER_OFFSET;
        }

        //move to corrected position
        move(curX, curY);

        prevX = x();
        prevY = y();
        emit positionChanged(x(),y());
#ifdef QT_DEBUG
        qDebug() << "CHANGE" << x() << y();
#endif
    }
}
