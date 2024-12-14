#include "pathlist.h"
#include "ui_pathlist.h"
#include <iostream>

using namespace std;

PathList::PathList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PathList)
{
    ui->setupUi(this);

    /* Initialize path file */
    path_file = new QFile("path_list.txt");
    if (!path_file->open(QIODevice::ReadWrite)){
        cout << "path_list.txt open fail" << endl;
    }
    /* Initialize stream */
    in = new QTextStream(path_file);
    out = new QTextStream(path_file);

    /* Upload path list of file into default path setting window's path list */
    QString line;
    while(!in->atEnd()){
        line = in->readLine();
        /* Mapping line of file and row */
        map.insert(line, row);
        /* Wirte the path to list widget */
        ui->pathList->insertItem(row++, line);
    }
}

PathList::~PathList()
{
    delete ui;
}

QLineEdit* PathList::get_pathLine(){
    return ui->pathLine;
}

void PathList::on_plusButton_clicked()
{
    /* Content of line edit */
    QString currentLine = ui->pathLine->text();
    /* Add current line into the last */
    if (!map.contains(currentLine)){
        map.insert(currentLine, row);
        ui->pathList->insertItem(row++, currentLine);
    }
}

void PathList::on_pathList_itemClicked(QListWidgetItem *item)
{
    /* If path list's item is clicked, it will be seleted path */
    seletedPath = item->text();

    /* Drag entire line. Then put new line into line edit with clicked item */
    ui->pathLine->selectAll();
    ui->pathLine->insert(item->text());
}

void PathList::on_okButton_clicked()
{
    /* Set seleted path into path line's text */
    seletedPath = ui->pathLine->text();

    /* Put seletedPath into directory path at wizard's page 2 */
    /* emit signal to the main wizard's directory path */
    emit diretoryPath_seleted(seletedPath);

    /* Resize file to 0 for overwriting */
    path_file->resize(0);

    /* Before close, write path of list into file */
    for(int i = 0; i < row; i++){
        QString line = ui->pathList->item(i)->text();
        *out << line << "\n";
    }

    /* Close path_list.txt */
    /* Inside the close(), flush() will be called */
    path_file->close();

    /* Close path setting window */
    close();

    /* Delete object */
    delete this;
}


void PathList::on_minusButton_clicked()
{
    /* Content of line edit */
    QString currentLine = ui->pathLine->text();

    /* Delete current line using map */
    QMap<QString, int>::iterator iter;
    iter = map.find(currentLine);
    ui->pathList->takeItem(*iter);

    /* Modify rows(value) of map objects behind the deleted map object */
    for(iter++; iter != map.end(); iter++){
        *iter -= 1;
    }

    /* Delete item in the map */
    map.erase(iter);
    row--;

    /* Set the seleted path to a empty */
    seletedPath = "";
    ui->pathLine->setText("");
}

void PathList::on_cancelButton_clicked()
{
    /* Set seleted path into path line's text */
    seletedPath = ui->pathLine->text();

    /* Close the window */
    close();
}
