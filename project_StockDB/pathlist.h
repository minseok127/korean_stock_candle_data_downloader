#ifndef PATHLIST_H
#define PATHLIST_H

#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QFile>
#include <QTextStream>
#include <QMap>

namespace Ui {
class PathList;
}

class PathList : public QMainWindow
{
    Q_OBJECT

public:
    explicit PathList(QWidget *parent = nullptr);
    ~PathList();

    /* Get path line edit */
    QLineEdit* get_pathLine();

private slots:
    void on_plusButton_clicked();

    void on_pathList_itemClicked(QListWidgetItem *item);

    void on_okButton_clicked();


    void on_minusButton_clicked();

    void on_cancelButton_clicked();

signals:
    void diretoryPath_seleted(QString& path);

private:
    Ui::PathList *ui;

    /* Path list file */
    QFile* path_file;
    /* Stream */
    QTextStream* in;
    QTextStream* out;

    /* Selected path */
    QString seletedPath;

    /* Row of list */
    int row = 0;

    /* QMap of path list file */
    QMap<QString, int> map;
};

#endif // PATHLIST_H
