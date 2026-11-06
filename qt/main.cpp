// main.cpp — Qt6 entry point
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("dnschanger");
    app.setApplicationDisplayName("Changement DNS");

    MainWindow win;
    win.show();

    return app.exec();
}
