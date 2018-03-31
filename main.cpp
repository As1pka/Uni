#include "mainwindow.h"
//#include "baseview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Слгаживание текстуры.
    QSurfaceFormat format;
    format.setSamples(16);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

//   BaseView w;
    MainWindow w;
    w.show();

    return a.exec();
}
