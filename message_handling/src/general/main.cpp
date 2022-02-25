#include "MainWindow.h"

#include <QApplication>

#include "applock.h"

int main(int argc, char *argv[])
{
    using namespace applock;

    const QString lockName = QString("Lock");

    LockResult applicationLock = tryLockApplication(lockName.toLocal8Bit().constData());
    if (applicationLock != kSuccess)
        return 123;

    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();

    w.hide();
    w.setEnabled(true);
    w.setAttribute(Qt::WA_DontShowOnScreen);
    w.setAttribute(Qt::WA_TransparentForMouseEvents);

    return a.exec();
}
