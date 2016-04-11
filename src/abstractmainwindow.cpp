#include <abstractmainwindow.hpp>
#include <settings.hpp>
#include <QDirIterator>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <primarywindow.hpp>

#ifdef QT_DEBUG
    QSettings AbstractMainWindow::settings( QSettings::NativeFormat,
                                            QSettings::UserScope,
                                            COMPANY_TITLE,
                                            PRODUCT_NAME);
#else
    QSettings AbstractMainWindow::settings(DEF_CONFIG_FILE,
                                           QSettings::NativeFormat);
#endif

QVector<AbstractMainWindow *> AbstractMainWindow::windowList;
QMap<QString, QString> AbstractMainWindow::imageFileMap;

AbstractMainWindow::AbstractMainWindow(int _screenIndex, QWidget *parent) :
    QWidget(parent),
    screenIndex(_screenIndex)
{
    /*
     * Enlist all image files from /usr/share/backgrounds or
     * the directory set from ini file
     * and map them as -
     * baseName --> absolutePath
     */
    AbstractMainWindow::imageFileMap.insert(DEF_IMG_BASE_NAME,
                                            DEF_IMG_ABS_PATH);
    QDirIterator it(AbstractMainWindow::settings.value(KEY_IMG_DIR,DEF_IMG_DIR).toString(),
                    QStringList() << "*.jpg" << "*.png" << "*.jpeg",
                    QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        //qDebug()<<it.next();//mandatory to iterate :-<
        QString tmp = it.next();
        QFileInfo info = it.fileInfo();
        AbstractMainWindow::imageFileMap.insert(info.baseName(),
                                                info.absoluteFilePath());
    }
    qDebug()<<"Total " << imageFileMap.count() << " images found";

    /*
     * Set Object name and Geometry according to the monitor index
     */
    this->setObjectName(QString("LoginWindow_%1").arg(screenIndex));
    this->setGeometry(QApplication::desktop()->screenGeometry(screenIndex));
}

AbstractMainWindow::~AbstractMainWindow(){}

void AbstractMainWindow::setup_n_start()
{
    AbstractMainWindow * primaryWindow = Q_NULLPTR;
    for(int monitorCount = 0; monitorCount < QApplication::desktop()->screenCount(); monitorCount++)
    {
        AbstractMainWindow * win;
        if(monitorCount == QApplication::desktop()->primaryScreen())
        {
            //do inititalize a primary window
            win = new PrimaryWindow(monitorCount);
            primaryWindow = win;
        }else
        {
            //create the other windows
            win = new AbstractMainWindow(monitorCount);
        }
        AbstractMainWindow::windowList.push_back(win);
    }

    /*
     * Walk through the list again and do -
     * 1. connect primary's background changed signal to all's set_background
     * 2. show em up
     */
    if(primaryWindow != Q_NULLPTR)
    {
        for(QVector<AbstractMainWindow*>::iterator it = AbstractMainWindow::windowList.begin();
            it != AbstractMainWindow::windowList.end(); it++)
        {
            AbstractMainWindow * win = *it;
            qDebug()<<QWidget::connect(primaryWindow,SIGNAL(background_changed(QString)),win,SLOT(set_background(QString)));
            win->show();
            // has to set the back ground explicitly for the first time
            win->set_background(AbstractMainWindow::settings.value(KEY_IMG_ABS_PATH,
                                                                   DEF_IMG_ABS_PATH).toString());
        }
    }else
    {
        qDebug() << "Could not determine Primary Screen. Exitng ...";
        qApp->exit(-1);
    }
}


bool AbstractMainWindow::is_primary_window()
{
    return this->screenIndex == QApplication::desktop()->primaryScreen();
}


void AbstractMainWindow::set_background(QString image)
{
    qDebug() << "Setting background on "<< this->objectName();
    QPalette palatte = this->palette();
    QBrush brush(Qt::black);
    QFile imageFile(image);
    if(imageFile.exists())
    {
        brush.setTexture(QPixmap(image).scaled(QApplication::desktop()->screenGeometry(screenIndex).width(),
                                               QApplication::desktop()->screenGeometry(screenIndex).height()));
    }
    palatte.setBrush(this->backgroundRole(),brush);
    this->setPalette(palatte);
}
