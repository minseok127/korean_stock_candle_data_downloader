#include "updatewizard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    updateWizard w;
    w.show();
    return a.exec();
}
