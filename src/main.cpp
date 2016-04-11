#include <QApplication>
#include <QFontDatabase>
#include <abstractmainwindow.hpp>

void setup_font();

int main (int argc, char ** argv)
{
    QApplication * app = new QApplication(argc,argv);
    AbstractMainWindow::setup_n_start();
    return app->exec();
}

void setup_font()
{
    QFontDatabase::addApplicationFont(":/files/fonts/TitillionWeb.ttf");
}
