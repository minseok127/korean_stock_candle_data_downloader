#include "selectwindow.h"
#include "ui_selectwindow.h"
#include <QTreeWidget>
#include <QString>
#include <iostream>

SelectWindow::SelectWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SelectWindow)
{
    ui->setupUi(this);

    /* Default state of tickets is true(open) */
    for(int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        ticket_KOSDAQ[i] = true;
        ticket_KOSPI[i] = true;
    }

    /* Initialize temporary tickets */
    /* These temporary tickets will be changed only with OK button */
    for(int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        tmpTicket_KOSDAQ[i] = true;
        tmpTicket_KOSPI[i] = true;
    }

    /* Initialize temporary state array */
    /* These temporary states will be changed only with OK button */
    /* And these states will be used at cancel button for returning */
    /* Default state is Qt::Checked */
    for(int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        tmpCheckState_KOSPI_arr[i] = Qt::Checked;
        tmpCheckState_KOSDAQ_arr[i] = Qt::Checked;
    }
    tmpCheckState_KOSPI = Qt::Checked;
    tmpCheckState_KOSDAQ = Qt::Checked;
}

SelectWindow::~SelectWindow()
{
    delete ui;
}

void SelectWindow::on_selectTree_itemChanged(QTreeWidgetItem *item, int column)
{
    /* Response about changed item in selection window */

    /* If changed item is KOSAQ or KOSPI and checked or unchecked */
    if (item->text(column).compare("KOSDAQ") == 0){
        /* All kinds of stock data in KOSDAQ will be checked or unchecked */
        for(int i = 0; i < item->childCount(); i++){
            if (item->checkState(column) == Qt::Unchecked){
                /* If KOSDAQ is unchecked, do Uchecking */
                item->child(i)->setCheckState(column, Qt::Unchecked);

                /* Ticket unabled */
                ticket_KOSDAQ[i] = false;
            }
            else if (item->checkState(column) == Qt::Checked){
                /* If KOSDAQ is checked, do Checking */
                item->child(i)->setCheckState(column, Qt::Checked);

                /* Ticked able */
                ticket_KOSDAQ[i] = true;
            }
        }
    }
    else if (item->text(column).compare("KOSPI") == 0){
        /* All kinds of stock data in KOSPI will be checked or unchecked */
        for(int i = 0; i < item->childCount(); i++){
            if (item->checkState(column) == Qt::Unchecked){
                /* If KOSPI is unchecked, do Uchecking */
                item->child(i)->setCheckState(column, Qt::Unchecked);

                /* Ticket unabled */
                ticket_KOSPI[i] = false;
            }
            else if (item->checkState(column) == Qt::Checked){
                /* If KOSPI is checked, do Checking */
                item->child(i)->setCheckState(column, Qt::Checked);

                /* Ticked able */
                ticket_KOSPI[i] = true;
            }
        }
    }
    else if (item->childCount() == 0){
        /* Seleted item is not a KOSPI or KOSDAQ. Just individual data type */
        QTreeWidgetItem* parent = item->parent();

        /* Check the item's parent is whether KOSDAQ or KOSPI, then set the ticket */
        if (parent->text(column).compare("KOSDAQ") == 0){
            /* Reverse booltype of ticket */
            ticket_KOSDAQ[parent->indexOfChild(item)] = !ticket_KOSDAQ[parent->indexOfChild(item)];
        }
        else{
            /* Reverse booltype of ticket */
            ticket_KOSPI[parent->indexOfChild(item)] = !ticket_KOSPI[parent->indexOfChild(item)];
        }
    }
}

bool* SelectWindow::get_ticket_KOSPI(){
    return ticket_KOSPI;
}

bool* SelectWindow::get_ticket_KOSDAQ(){
    return ticket_KOSDAQ;
}

void SelectWindow::on_okButton_clicked()
{
    /* First item's cooridinate */
    int default_column = 0;

    /* Get the KOSPI, KOSDAQ item */
    QTreeWidgetItem* item_KOSPI = ui->selectTree->topLevelItem(KOSPI);
    QTreeWidgetItem* item_KOSDAQ = ui->selectTree->topLevelItem(KOSDAQ);

    /* If OK button is clicked, changed contents must be saved in temporary tickets, temporary check state */
    for(int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        tmpTicket_KOSDAQ[i] = ticket_KOSDAQ[i];
        tmpTicket_KOSPI[i] = ticket_KOSPI[i];

        tmpCheckState_KOSDAQ_arr[i] = item_KOSDAQ->child(i)->checkState(default_column);
        tmpCheckState_KOSPI_arr[i] = item_KOSPI->child(i)->checkState(default_column);
    }
    /* Temporary KOSPI, KOSDAQ item is also saved */
    tmpCheckState_KOSDAQ = item_KOSDAQ->checkState(default_column);
    tmpCheckState_KOSPI = item_KOSPI->checkState(default_column);

    /* Close the selection window */
    close();
}

void SelectWindow::on_cancelButton_clicked()
{
    /* If cancel button is clicked, changed contents must be canceled */

    /* First item's cooridinate */
    int default_column = 0;

    /* Get the KOSPI, KOSDAQ item */
    QTreeWidgetItem* item_KOSPI = ui->selectTree->topLevelItem(KOSPI);
    QTreeWidgetItem* item_KOSDAQ = ui->selectTree->topLevelItem(KOSDAQ);

    /* Returning Checkbox of KOSPI and KOSDAQ */
    item_KOSPI->setCheckState(default_column, tmpCheckState_KOSPI);
    item_KOSDAQ->setCheckState(default_column, tmpCheckState_KOSDAQ);

    for (int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        /* Returning Checkbox of array */
        item_KOSPI->child(i)->setCheckState(default_column, tmpCheckState_KOSPI_arr[i]);
        item_KOSDAQ->child(i)->setCheckState(default_column, tmpCheckState_KOSDAQ_arr[i]);

        /* Returning Tickets */
        ticket_KOSPI[i] = tmpTicket_KOSPI[i];
        ticket_KOSDAQ[i] = tmpTicket_KOSDAQ[i];
    }

    /* Close the selection window */
    close();
}
