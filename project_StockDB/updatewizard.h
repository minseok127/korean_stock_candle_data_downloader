#ifndef UPDATEWIZARD_H
#define UPDATEWIZARD_H

#include <QWizard>
#include <QAxWidget>
#include "selectwindow.h"
#include "pathlist.h"
#include <queue>

/* Update window's definition */
namespace Ui {
class updateWizard;
}

using namespace std;

class updateWizard : public QWizard
{
    Q_OBJECT

public:
    explicit updateWizard(QWidget *parent = nullptr);
    ~updateWizard();

private slots:
    void on_loginButton_clicked();

    void on_selectButton_clicked();

    void on_openButton_clicked();

    void on_setDefault_clicked();

    void on_updateButton_clicked();

private:
    Ui::updateWizard *ui;

    /* Default directory path of DB*/
    QString default_directory_Path;

    /* Selection page's window */
    SelectWindow* selectWindow;

    /* Path list window */
    PathList* pathListWindow;

    /* Make database file's path */
    void get_file_path(QString& filePath, QString dirPath, int market, int type);

    /* Update database file */
    void update_db(int market, int type, unsigned short period = 1);

    /* COM Objects */
    QAxWidget* codeManager; // Manage company's code
    QAxWidget* chartDataManager; // Manage chart data

    /* Company's code */
    QStringList kospiList;
    QStringList kosdaqList;

    /* Setting of Request */
    void SetInputValue(QString code, unsigned int count, QList<QVariant> field, int type, unsigned short period, unsigned int date);

    /* Make deque corresponding each columns */
    deque<unsigned int> date, foreign_offer_limit, foreign_offer_possible, foregin_holding_quantity;
    deque<int> open, high, low, close, institution_netbuy, institution_netbuy_accumulation, time, foreign_netbuy;
    deque<unsigned long long> volume, volume_value, share_quantity, company_price, buy_accumulation, sell_accumulation, deposit;
    deque<float> foreign_holding_ratio, rotationRatio, successRatio;

    int first_foreign_netbuy = 0;

    /* Pop all deque */
    void pop_back_all(bool min = false, bool day = false);

    /* Get the data */
    void GetDataValue_noMinute(int index, bool day = false);
    void GetDataValue_Minute(int index);
    void GetDataValue_Index_noMinute(int index, bool day = false);
    void GetDataValue_Index_Minute(int index);

    /* Get the create SQL */
    QString get_createSQL_noMintue(QString code, bool day = false);
    QString get_createSQL_Minute(QString code);
    QString get_createSQL_Index_noMinute(QString code, bool day = false);
    QString get_createSQL_Index_Minute(QString code);

    /* Get the insert SQL */
    QString get_insertSQL_noMinute(QString code, bool day = false);
    QString get_insertSQL_Mintue(QString code);
    QString get_insertSQL_Index_noMinute(QString code, bool day = false);
    QString get_insertSQL_Index_Minute(QString code);
};

/* diff function used in getting count of requestment */
/* Day, minute type not use count. So these have not diff function */
unsigned int get_diff_month(unsigned int date);
unsigned int get_diff_week(unsigned int date);

#endif // UPDATEWIZARD_H
