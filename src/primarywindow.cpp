#include <primarywindow.hpp>
#include <settings.hpp>
#include <QApplication>
#include <QDebug>
#include "ui_loginform.h"

using namespace QLightDM;

PrimaryWindow::PrimaryWindow(int screenIndex, QWidget * parent) :
    AbstractMainWindow(screenIndex,parent),
    form(this),
    dateTimeProvider(),
    greeter(),
    powerInterface(this),
    sessionModel(),
    userModel()
{
    if(!greeter.connectSync())
    {
#ifdef QT_DEBUG // we dont need to close the app in debug mode
        qDebug() << "Could not connect to lightdm";
    }else
    {
        qDebug() << "Connected to lightdm";
    }
#else
        qApp->exit(-1);
    }
#endif

    timer = new QTimer(this);

    /*
     * Make the connections first
     */
    connect(form.ui->imageComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_image_name_changed(QString)));
    connect(timer,SIGNAL(timeout()),this,SLOT(on_timeout()));
    connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(on_input_focus_changed(QWidget*,QWidget*)));
    connect(form.ui->shutdownButton,SIGNAL(clicked(bool)),this,SLOT(on_shutdown_clicked(bool)));
    connect(form.ui->restartButton,SIGNAL(clicked(bool)),this,SLOT(on_restart_clicked(bool)));
    connect(form.ui->users,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_current_user_changed(QString)));
    connect(form.ui->users,SIGNAL(currentIndexChanged(QString)),this,SLOT(set_current_session(QString)));
    connect(form.ui->sessionComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(set_current_session(QString)));
    connect(form.ui->passwordInput,SIGNAL(returnPressed()),this,SLOT(on_password_given()));
    connect(&greeter,SIGNAL(authenticationComplete()),this,SLOT(on_authentication_complete()));
    connect(&greeter,SIGNAL(showPrompt(QString,QLightDM::Greeter::PromptType)),this,SLOT(on_prompted(QString,QLightDM::Greeter::PromptType)));
    connect(&greeter,SIGNAL(showMessage(QString,QLightDM::Greeter::MessageType)),this,SLOT(on_greeter_message(QString,QLightDM::Greeter::MessageType)));

    form.move(AbstractMainWindow::settings.value(KEY_LOGIN_FORM_X_OFFSET,DEF_LOGIN_FORM_X_OFFSET).toInt(),
              AbstractMainWindow::settings.value(KEY_LOGIN_FORM_Y_OFFSET,DEF_LOGIN_FORM_Y_OFFSET).toInt());
    form.ui->imageComboBox->addItems(AbstractMainWindow::imageFileMap.keys());
    form.ui->imageComboBox->setCurrentText(AbstractMainWindow::settings.value(KEY_IMG_BASE_NAME,
                                                                              DEF_IMG_BASE_NAME).toString());

    form.ui->hostLabel->setText(greeter.hostname().toUpper());

    /*
     * Enlist all sessions and
     * Set the current session appear as default
     * This doesnt work in debug(not lightdm debug) mode !!!
     */
    form.ui->sessionComboBox->setModel(&sessionModel);
    //qDebug() << "Default session :" << greeter.defaultSessionHint();
    if(form.ui->sessionComboBox->currentText() != greeter.defaultSessionHint())
    {
        for(int sessionIndex = 0; sessionIndex < sessionModel.rowCount(QModelIndex()); sessionIndex++)
        {
            if(greeter.defaultSessionHint() ==
                    sessionModel.data(sessionModel.index(sessionIndex,0),SessionsModel::KeyRole).toString())
            {
                form.ui->sessionComboBox->setCurrentIndex(sessionIndex);
            }
        }
    }

    /*
     * Enlist the known users
     * Check for last user data if found set as default
     * Initiate first authentication
     */
    for(int userIndex = 0; userIndex < userModel.rowCount(QModelIndex()); userIndex++)
    {
        form.ui->users->addItem(userModel.data(userModel.index(userIndex,0),UsersModel::NameRole).toString());
    }
    int c_userIndex = 0;
    if(AbstractMainWindow::settings.value(KEY_LAST_USER,"").toString().isEmpty())
    {
        //get hint from greeter, find user in combo and set him as default
        c_userIndex = form.ui->users->findText(greeter.selectUserHint());
    }else
    {
        //get user from conf
        c_userIndex = form.ui->users->findText(AbstractMainWindow::settings.value(KEY_LAST_USER,"").toString());
    }
    form.ui->users->setCurrentIndex(c_userIndex);
    greeter.authenticate(form.ui->users->currentText());

    timer->start(1000);
}

PrimaryWindow::~PrimaryWindow(){}

void PrimaryWindow::on_image_name_changed(QString image)
{
    AbstractMainWindow::settings.setValue(KEY_IMG_BASE_NAME,image);
    AbstractMainWindow::settings.setValue(KEY_IMG_ABS_PATH,AbstractMainWindow::imageFileMap[image]);
    emit background_changed(AbstractMainWindow::imageFileMap[image]);
    qDebug() << "Setting image : " << AbstractMainWindow::imageFileMap[image];
}

void PrimaryWindow::on_timeout()
{
    form.ui->timeDateLabel->setText(dateTimeProvider.currentDateTime().toString(Qt::SystemLocaleLongDate));
}

void PrimaryWindow::on_input_focus_changed(QWidget *_old, QWidget *_new)
{
    if(_new->objectName() != "passwordInput")
    {
        form.ui->passwordInput->setFocus();
    }
}

void PrimaryWindow::on_shutdown_clicked(bool state)
{
#ifdef QT_DEBUG
    qDebug() << "shutdown requested";
#else
    powerInterface.shutdown();
#endif
}

void PrimaryWindow::on_restart_clicked(bool state)
{
#ifdef QT_DEBUG
    qDebug() << "restart requested";
#else
    powerInterface.restart();
#endif
}

void PrimaryWindow::on_current_user_changed(QString userName)
{
    /*
     * Cancel any previous authentication
     * and initiate a new one
     */
    if(greeter.inAuthentication())
    {
        greeter.cancelAuthentication();
    }
    greeter.authenticate(userName);
}

void PrimaryWindow::set_current_session(QString hint)
{
    /*
     * Set the settings file and current index of combobox
     *

    if(form.ui->sessionComboBox->currentText() != hint)
    {
        for(int sessionIndex = 0; sessionIndex < sessionModel.rowCount(QModelIndex()); sessionIndex++)
        {
            if(greeter.defaultSessionHint() ==
                    sessionModel.data(sessionModel.index(sessionIndex,0),SessionsModel::KeyRole).toString())
            {
                form.ui->sessionComboBox->setCurrentIndex(sessionIndex);
            }
        }
    }
    */

    /*
     * if the change comes from user combo -
     * check if the current selected session is the last session for him
     * if not save it
     */
    if(form.ui->users->currentText() == hint)
    {
        if(!AbstractMainWindow::settings.value(hint+"_lastSession","").toString().isEmpty())
        {
            //means this user has some preivious choice
            //form.ui->sessionComboBox->setCurrentText(AbstractMainWindow::settings.value(hint+"_lastSession",""));
            form.ui->sessionComboBox->setCurrentIndex(form.ui->sessionComboBox->findText(
                                                          AbstractMainWindow::settings.value(hint+"_lastSession","").toString()
                                                          )
                                                      );
        }
    }

    /*
     * Always save the user and session data
     */
    save_user_n_session_data();
}

void PrimaryWindow::save_user_n_session_data()
{
    AbstractMainWindow::settings.setValue(KEY_LAST_USER,
                                          form.ui->users->currentText());
    AbstractMainWindow::settings.setValue(form.ui->users->currentText()+"_lastSession",
                                          form.ui->sessionComboBox->currentText());
}

void PrimaryWindow::on_greeter_message(QString msg, Greeter::MessageType msgType)
{
    /*
     * BUG ?? This one never gets called
     */
    form.ui->msgLabel->setText("GREETER_MSG : " + msg);
}

void PrimaryWindow::on_prompted(QString prompt, Greeter::PromptType promptType)
{
    qDebug() << "Prompt : " << prompt;
    //form.ui->msgLabel->setText("GREETER_PROMPT "+prompt);
}

void PrimaryWindow::on_password_given()
{
    if(!form.ui->passwordInput->text().isEmpty())
    {
        greeter.respond(form.ui->passwordInput->text());
    }
}

void PrimaryWindow::on_authentication_complete()
{
    if(greeter.isAuthenticated())
    {
        save_user_n_session_data();
        QModelIndex index = sessionModel.index(form.ui->sessionComboBox->currentIndex(),0,QModelIndex());
        QString curSession = sessionModel.data(index,SessionsModel::KeyRole).toString();
        greeter.startSessionSync(curSession);
    }else
    {
        form.ui->passwordInput->clear();
        form.ui->msgLabel->setText("Wrong Password !!!");
        on_current_user_changed(form.ui->users->currentText());
    }
}
