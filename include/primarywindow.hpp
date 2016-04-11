#ifndef PRIMARYWINDOW_HPP
#define PRIMARYWINDOW_HPP

#include <QWidget>
#include <abstractmainwindow.hpp>
#include <loginform.hpp>
#include <QTimer>
#include <QDateTime>
#include <QLightDM/greeter.h>
#include <QLightDM/power.h>
#include <QLightDM/sessionsmodel.h>
#include <QLightDM/usersmodel.h>

class PrimaryWindow : public AbstractMainWindow
{
        Q_OBJECT
    public:
        PrimaryWindow(int screenIndex =0, QWidget * parent = 0);
        ~PrimaryWindow();

    signals:
        void background_changed(QString image);

    private slots:
        void on_image_name_changed(QString image);
        void on_timeout();
        void on_input_focus_changed(QWidget * _old, QWidget * _new);
        void on_shutdown_clicked(bool state);
        void on_restart_clicked(bool state);
        void on_current_user_changed(QString userName);
        void set_current_session(QString hint);
        void on_prompted(QString prompt,QLightDM::Greeter::PromptType promptType);
        void on_greeter_message(QString msg,QLightDM::Greeter::MessageType msgType);
        void on_password_given();
        void on_authentication_complete();

    private:
        LoginForm form;
        QTimer * timer;
        QDateTime dateTimeProvider;

        QLightDM::Greeter greeter;
        QLightDM::PowerInterface powerInterface;
        QLightDM::SessionsModel sessionModel;
        QLightDM::UsersModel userModel;

    private:
        void save_user_n_session_data();
};

#endif // PRIMARYWINDOW_HPP
