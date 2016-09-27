#ifndef LOGINFORM_HPP
#define LOGINFORM_HPP

#include <QWidget>


#ifdef QT_DEBUG
    /**
      * Happens to have a window border when the window manager is running
      * in debug mode.
      */
    #define WINDOW_BORDER_OFFSET 30
#else
    #define WINDOW_BORDER_OFFSET 0
#endif

namespace Ui {
    class LoginForm;
}

class LoginForm : public QWidget
{
        Q_OBJECT

    public:
        explicit LoginForm(QWidget *parent = 0);
        ~LoginForm();

        Ui::LoginForm *ui;

    signals :
        void positionChanged(int x, int y);

    private:
        void mousePressEvent(QMouseEvent * event);
        void mouseMoveEvent(QMouseEvent * event);
        void mouseReleaseEvent(QMouseEvent * event);

        int mousePressXOrd, mousePressYOrd , parentX, parentY, parentW, parentH;
        int prevX, prevY;
};

#endif // LOGINFORM_HPP
