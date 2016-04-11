#ifndef ABSTRACTMAINWINDOW_HPP
#define ABSTRACTMAINWINDOW_HPP

#include <QWidget>
#include <QSettings>
#include <QVector>
#include <QMap>

class AbstractMainWindow : public QWidget
{
        Q_OBJECT
    public:
        explicit AbstractMainWindow(int screenIndex = 0, QWidget *parent = 0);
        ~AbstractMainWindow();    

    public slots:
        /*
         * Only the absolute file path will be sent here
         */
        void set_background(QString image);

    public:
        static QSettings settings;
        static QVector<AbstractMainWindow *> windowList;
        static QMap<QString, QString> imageFileMap;
        static void setup_n_start();
        bool is_primary_window();

    protected:
        int screenIndex;
};

#endif // ABSTRACTMAINWINDOW_HPP
