#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class updateManager; }
QT_END_NAMESPACE

class updateManager : public QDialog
{
    Q_OBJECT

public:
    updateManager(QWidget *parent = nullptr);
    ~updateManager();

private:
    Ui::updateManager *ui;
};
#endif // UPDATEMANAGER_H
