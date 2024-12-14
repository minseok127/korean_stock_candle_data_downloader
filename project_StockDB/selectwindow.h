#ifndef SELECTWINDOW_H
#define SELECTWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>

/* Type of DataBase */
#define MONTH (0)
#define WEEK (1)
#define DAY (2)
#define _30MIN (3)
#define _5MIN (4)
#define _3MIN (5)
#define _1MIN (6)
#define INDEX_MONTH (7)
#define INDEX_WEEK (8)
#define INDEX_DAY (9)
#define INDEX_30MIN (10)
#define INDEX_5MIN (11)


#define TYPE_NUM_STOCK_DB (12)

/* Type of Market */
#define KOSPI (0)
#define KOSDAQ (1)

/* SelectWindow's definition */
namespace Ui {
class SelectWindow;
}

class SelectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SelectWindow(QWidget *parent = nullptr);
    ~SelectWindow();

    /* Get ticket */
    bool* get_ticket_KOSPI();
    bool* get_ticket_KOSDAQ();

private slots:
    void on_selectTree_itemChanged(QTreeWidgetItem *item, int column);

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::SelectWindow *ui;

    /* Tickets indicating whether to open */
    bool ticket_KOSPI[TYPE_NUM_STOCK_DB];
    bool ticket_KOSDAQ[TYPE_NUM_STOCK_DB];

    /* Temporary tickets logging the ticket when the selection window is turned on */
    bool tmpTicket_KOSPI[TYPE_NUM_STOCK_DB];
    bool tmpTicket_KOSDAQ[TYPE_NUM_STOCK_DB];

    /* Temporary checkbox array logging the checkbox when the selction window is turned on */
    Qt::CheckState tmpCheckState_KOSPI_arr[TYPE_NUM_STOCK_DB];
    Qt::CheckState tmpCheckState_KOSDAQ_arr[TYPE_NUM_STOCK_DB];

    Qt::CheckState tmpCheckState_KOSPI;
    Qt::CheckState tmpCheckState_KOSDAQ;
};

#endif // SELECTWINDOW_H
