#include "updatewizard.h"
#include "ui_updatewizard.h"
#include "shinebutton.h"
#include <math.h>
#include <QPushButton>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>
#include <queue>
#include <time.h>
#include <windows.h>
#include <QGraphicsOpacityEffect>
#include <QGraphicsItem>

/* sqlite3 connections about kospi, kosdaq */
QSqlDatabase db_kospi[TYPE_NUM_STOCK_DB];
QSqlDatabase db_kosdaq[TYPE_NUM_STOCK_DB];

/* DB file open fail flag */
#define DB_OPEN_ALL_CLEAR (1)
#define DB_OPEN_NOT_ALL_CLEAR (0)

/* Commit count */
#define COMMIT_COUNT (0)

/* StockChart Object's specifics. SetInputData API's type argument */
#define COMPANY_CODE (0)
#define REQUEST_CATEGORY (1)
#define REQUEST_END_DATE (2)
#define REQUEST_START_DATE (3)
#define REQUEST_COUNT (4)
#define REQUEST_FIELD (5)
#define CHART_TYPE (6)
#define CHART_CYCLE (7)
#define IS_GAP_CORRECTION (8)
#define IS_REVISED_STOCK_PRICE (9)
#define VOLUME_TYPE (10)

/* REQUEST_CATEGORY's values */
#define REQUEST_CATEGORY_PREIOD ('1')
#define REQUEST_CATEGORY_COUNT ('2')

/* REQUEST_FIELD's values */
#define REQUEST_DATE (0) // 날짜
#define REQUEST_TIME (1) // 시간
#define REQUEST_OPEN (2) // 시가
#define REQUEST_HIGH (3) // 고가
#define REQUEST_LOW (4) // 저가
#define REQUEST_CLOSE (5) // 종가
#define REQUEST_COMPARED_PREV (6) // 전일 대비
#define REQUEST_VOLUME (8) // 거래량
#define REQUEST_VOLUME_VALUE (9) // 거래대금
#define REQUEST_BUY_ACCUMULATION (10) // Only offered in minutes or ticks, 누적 매수 체결
#define REQUEST_SELL_ACCUMULATION (11) // Only offered in minutes or ticks, 누적 매도 체결
#define REQUEST_SHARE_QUANTITY (12) // 상장 주식 수
#define REQUEST_COMPANY_PRICE (13) // 시가 총액
#define REQUEST_FOREIGN_OFFER_LIMIT (14) // 외국인 주문 한도 수량
#define REQUEST_FOREIGN_OFFER_POSSIBLE (15) // 외국인 주문 가능 수량
#define REQUEST_FOREIGN_HOLDING_QUANTITY (16) // 외국인 보유 수량
#define REQUEST_FOREIGN_HOLDING_RATIO (17) // 외국인 보유 비율
#define REQUEST_REVISE_DATE (18) // 수정 주가 일자
#define REQUEST_REVISE_RATIO (19) // 수정 주가 비율
#define REQUEST_INSTITUTION_NETBUY (20) // 기관 순매수
#define REQUEST_INSTITUTION_NETBUY_ACCUMULATION (21) // 기관 누적 순매수
// I don't know meaning of 22, 23
#define REQUEST_DEPOSIT (24) // 예탁금
#define REQUEST_ROTATION_RATIO (25) // 주식 회전률
#define REQUEST_SUCCESS_VOLUME_RATIO (26) // 거래 성립률
#define REQUEST_COMPARED_PREV_SIGN (37) // 대비 부호

/* CHART_TYPE's values */
#define TYPE_DAY ('D') // Day
#define TYPE_WEEK ('W') // Week
#define TYPE_MONTH ('M') // Month
#define TYPE_MINIUTE ('m') // Minitue
#define TYPE_TICK ('T') // Tick

/* IS_GAP_CORRECTION's values */
#define GAP_CORRETION_OFF ('0')
#define GAP_CORRETION_ON ('1')

/* IS_REVISED_STOCK_PRICE's values */
#define REVISED_STOCK_PRICE_OFF ('0')
#define REVISED_STOCK_PRICE_ON ('1')

/* VOLUME_TYPE's values */
#define VOLUME_TYPE_ALL ('0')
#define VOLUME_TYPE_ONLY_AFTER_END ('1')
#define VOLUME_TYPE_EXCEPT_OUT_OF_OPEN ('2')
#define VOLUME_TYPE_ONLY_BEFORE_START ('3')


using namespace std;

/* Update wizard's initializer */
updateWizard::updateWizard(QWidget *parent) : QWizard(parent), ui(new Ui::updateWizard)
{
    /* Apply .ui file into Object */
    ui->setupUi(this);

    /* Selection window of selection page */
    selectWindow = new SelectWindow;

    /* Change style of default buttons */
    QPushButton* nextButton = (QPushButton*)QWizard::button(QWizard::NextButton);
    nextButton->setStyleSheet("background-color: black; color: green; border-style: outset; border-width: 2px; border-color: green; min-width: 10em; min-height: 3em");

    QPushButton* backButton = (QPushButton*)QWizard::button(QWizard::BackButton);
    backButton->setStyleSheet("background-color: black; color: green; border-style: outset; border-width: 2px; border-color: green; min-width: 10em; min-height: 3em");

    QPushButton* cancelButton = (QPushButton*)QWizard::button(QWizard::CancelButton);
    cancelButton->setStyleSheet("background-color: black; color: green; border-style: outset; border-width: 2px; border-color: green; min-width: 10em; min-height: 3em");

    QPushButton* finishButton = (QPushButton*)QWizard::button(QWizard::FinishButton);
    finishButton->setStyleSheet("background-color: black; color: green; border-style: outset; border-width: 2px; border-color: green; min-width: 10em; min-height: 3em");

    /* Initialize COM Objects */
    codeManager = new QAxWidget("CpUtil.CpCodeMgr");
    chartDataManager = new QAxWidget("CpSysDib.StockChart");

    /* Set the page 3's label's content be center */
    ui->stateLabel->setAlignment(Qt::AlignCenter);

    /* Set the progressbar to 0 */
    ui->progressBar->setValue(0);

    ShineButton* loginButton = new ShineButton(ui->loginButton);
    loginButton->setText("Log-In");
    loginButton->setStyleSheet("border-radius: 80px; border: 3px solid rgb(61,185,127); \
                                color: rgb(255, 255, 255); font: 75 48pt ""Agency FB"";");
    loginButton->setFont("color: rgb(255, 255, 255); font: 75 48pt ""Agency FB"";");
    loginButton->resize(280, 180);

    connect(loginButton, &QPushButton::clicked, this, &updateWizard::on_loginButton_clicked);

}

updateWizard::~updateWizard()
{
    delete ui;
}

/* When log in button is clicked */
void updateWizard::on_loginButton_clicked()
{
    /* Execute Cybos for using API */
    system("C:\\DAISHIN\\STARTER\\ncStarter.exe /prj:cp");
}

/* When select button is clicked */
void updateWizard::on_selectButton_clicked()
{
    /* Show select window for selecting what kinds of data will be downloaded */
    selectWindow->show();
}

/* When open button is clicked */
void updateWizard::on_openButton_clicked()
{
    /* Directory path in line edit of select window*/
    QString dirPath = ui->dirPath->text();
    /* Directory path to be changed as argument */
    QString dirPath_arg = "";

    /* Transform directory path into usable form ( e.g. c:\ => c:\\ ) */
    for(int i = 0; i < dirPath.length(); i++){
        /* Append dirPath into dirPath_arg */
        dirPath_arg.append(dirPath[i]);
        if (dirPath[i] == QChar('\\')){
            /* If path has '\', then make it "\\" */
            dirPath_arg.append(dirPath[i]);
        }
    }

    /* Using directory path argument to open selected data files */

    /* Open KOSPI DB and KOSDAQ DB */
    /* First, get the ticket */
    bool* kospi_ticket = selectWindow->get_ticket_KOSPI();
    bool* kosdaq_ticket = selectWindow->get_ticket_KOSDAQ();

    /* Using ticket, open database */
    int flag = DB_OPEN_ALL_CLEAR; // If file open is failed, flag tell us */
    QString filePath;
    for(int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        if (kospi_ticket[i]){
            get_file_path(filePath, dirPath_arg, KOSPI, i);

            /* Initialize connection with sqlite3 */
            /* file path becomes the name of the connection */
            db_kospi[i] = QSqlDatabase::addDatabase("QSQLITE", filePath);

            /* Open database */
            db_kospi[i].setDatabaseName(filePath);
            if (db_kospi[i].open()){
                cout << filePath.toStdString() << " open success" << endl;
            }
            else{
                cout << filePath.toStdString() << " open fail" << endl;
                flag = DB_OPEN_NOT_ALL_CLEAR;
            }
        }
        if (kosdaq_ticket[i]){
            get_file_path(filePath, dirPath_arg, KOSDAQ, i);

            /* Initialize connection with sqlite3 */
            /* file path becomes the name of the connection */
            db_kosdaq[i] = QSqlDatabase::addDatabase("QSQLITE", filePath);

            /* Open database */
            db_kosdaq[i].setDatabaseName(filePath);
            if (db_kosdaq[i].open()){
                cout << filePath.toStdString() << " open success" << endl;
            }
            else{
                cout << filePath.toStdString() << " open fail" << endl;
                flag = DB_OPEN_NOT_ALL_CLEAR;
            }
        }
    }

    /* If all database's open is successful, make next button green */
    if (flag == DB_OPEN_ALL_CLEAR){

    }
}

void updateWizard::on_setDefault_clicked()
{
    /* Create pathListWindow including .db file's directory path */
    pathListWindow = new PathList;

    /* Connect directory line edit of page 2 with path list window's seleted path */
    connect(pathListWindow, &PathList::diretoryPath_seleted, ui->dirPath, &QLineEdit::setText);

    /* Insert entered text of directory path into setting's line edit */
    QLineEdit* pathLine = pathListWindow->get_pathLine();
    pathLine->insert(ui->dirPath->text());

    /* Show default path setting window */
    pathListWindow->show();
}

void updateWizard::get_file_path(QString& filePath, QString dirPath, int market, int type){
    filePath.clear();
    filePath.append(dirPath);
    filePath.append("\\\\");

    if (market == KOSPI){
        switch (type)
        {
        case MONTH:
            filePath.append("KOSPI_MONTH.db");
            break;
        case WEEK:
            filePath.append("KOSPI_WEEK.db");
            break;
        case DAY:
            filePath.append("KOSPI_DAY.db");
            break;
        case _30MIN:
            filePath.append("KOSPI_30MIN.db");
            break;
        case _5MIN:
            filePath.append("KOSPI_5MIN.db");
            break;
        case _3MIN:
            filePath.append("KOSPI_3MIN.db");
            break;
        case _1MIN:
            filePath.append("KOSPI_1MIN.db");
            break;
        case INDEX_MONTH:
            filePath.append("KOSPI_INDEX_MONTH.db");
            break;
        case INDEX_WEEK:
            filePath.append("KOSPI_INDEX_WEEK.db");
            break;
        case INDEX_DAY:
            filePath.append("KOSPI_INDEX_DAY.db");
            break;
        case INDEX_30MIN:
            filePath.append("KOSPI_INDEX_30MIN.db");
            break;
        case INDEX_5MIN:
            filePath.append("KOSPI_INDEX_5MIN.db");
            break;
        }
    }
    else{
        switch (type)
        {
        case MONTH:
            filePath.append("KOSDAQ_MONTH.db");
            break;
        case WEEK:
            filePath.append("KOSDAQ_WEEK.db");
            break;
        case DAY:
            filePath.append("KOSDAQ_DAY.db");
            break;
        case _30MIN:
            filePath.append("KOSDAQ_30MIN.db");
            break;
        case _5MIN:
            filePath.append("KOSDAQ_5MIN.db");
            break;
        case _3MIN:
            filePath.append("KOSDAQ_3MIN.db");
            break;
        case _1MIN:
            filePath.append("KOSDAQ_1MIN.db");
            break;
        case INDEX_MONTH:
            filePath.append("KOSDAQ_INDEX_MONTH.db");
            break;
        case INDEX_WEEK:
            filePath.append("KOSDAQ_INDEX_WEEK.db");
            break;
        case INDEX_DAY:
            filePath.append("KOSDAQ_INDEX_DAY.db");
            break;
        case INDEX_30MIN:
            filePath.append("KOSDAQ_INDEX_30MIN.db");
            break;
        case INDEX_5MIN:
            filePath.append("KOSDAQ_INDEX_5MIN.db");
            break;
        }
    }
}

void updateWizard::on_updateButton_clicked()
{
    /* Use API to get the list of code */
    /* API is used with COM object */
    QVariant code = codeManager->dynamicCall("GetStockListByMarket(2)"); // code list of kosdaq
    this->kosdaqList = code.toStringList();

    code = codeManager->dynamicCall("GetStockListByMarket(1)"); // code list of kospi
    this->kospiList = code.toStringList();

    bool* tickect_kospi = selectWindow->get_ticket_KOSPI();
    bool* tickect_kosdaq = selectWindow->get_ticket_KOSDAQ();

    QString updateMsg;

    for(int i = 0; i < TYPE_NUM_STOCK_DB; i++){
        if (tickect_kospi[i]){
            switch (i)
            {
            case MONTH:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_MONTH.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_MONTH.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, MONTH);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_MONTH.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case WEEK:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_WEEK.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_WEEK.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, WEEK);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_WEEK.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case DAY:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_DAY.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_DAY.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, DAY);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_DAY.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _30MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_30MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_30MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, _30MIN, 30);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_30MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _5MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_5MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_5MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, _5MIN, 5);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_5MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _3MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_3MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_3MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, _3MIN, 3);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_3MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _1MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_1MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_1MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, _1MIN, 1);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_1MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case INDEX_MONTH:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_INDEX_MONTH.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_MONTH.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, INDEX_MONTH);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_MONTH.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case INDEX_WEEK:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_INDEX_WEEK.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_WEEK.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, INDEX_WEEK);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_WEEK.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
                break;
            case INDEX_DAY:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_INDEX_DAY.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_DAY.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, INDEX_DAY);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_DAY.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
                break;
            case INDEX_30MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_INDEX_30MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_30MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, INDEX_30MIN, 30);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_30MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
                break;
            case INDEX_5MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSPI_INDEX_5MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_5MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSPI, INDEX_5MIN, 5);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSPI_INDEX_5MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            }
        }

        if (tickect_kosdaq[i]){
            switch (i)
            {
            case MONTH:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_MONTH.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_MONTH.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, MONTH);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_MONTH.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case WEEK:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_WEEK.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_WEEK.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, WEEK);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_WEEK.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case DAY:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_DAY.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_DAY.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, DAY);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_DAY.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _30MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_30MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_30MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, _30MIN, 30);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_30MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _5MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_5MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_5MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, _5MIN, 5);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_5MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _3MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_3MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_3MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, _3MIN, 3);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_3MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case _1MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_1MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_1MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, _1MIN, 1);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_1MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case INDEX_MONTH:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_INDEX_MONTH.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_MONTH.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, INDEX_MONTH);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_MONTH.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            case INDEX_WEEK:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_INDEX_WEEK.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_WEEK.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, INDEX_WEEK);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_WEEK.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
                break;
            case INDEX_DAY:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_INDEX_DAY.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_DAY.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, INDEX_DAY);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_DAY.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
                break;
            case INDEX_30MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_INDEX_30MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_30MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, INDEX_30MIN, 30);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_30MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
                break;
            case INDEX_5MIN:
                /* Change content of state label*/
                ui->stateLabel->setText("KOSDAQ_INDEX_5MIN.db is updating...");

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_5MIN.db is updating...\n");
                ui->updateLog->setText(updateMsg);

                /* Update db */
                update_db(KOSDAQ, INDEX_5MIN, 5);

                /* Show complete message on the text edit */
                updateMsg = ui->updateLog->toPlainText();
                updateMsg.append("KOSDAQ_INDEX_5MIN.db is completed...\n");
                ui->updateLog->setText(updateMsg);
                break;
            }
        }
    }
    ui->stateLabel->setText("All Clear");
}

void updateWizard::update_db(int market, int type, unsigned short period){
    /* Code list of market */
    QStringList marketList;

    /* Update week database both kospi */
    /* Initialize query object */
    QSqlQuery* query;

    if (market == KOSPI){
        /* Set the query to the kospi with type argument */
        query = new QSqlQuery(db_kospi[type]);
        /* Set the code list to the kospi */
        marketList = kospiList;

        /* If target is index */
        if (type == INDEX_MONTH || type == INDEX_WEEK || type == INDEX_DAY || type == INDEX_30MIN || type == INDEX_5MIN){
            marketList.clear();
            marketList.append("U001"); // Kospi index's code
        }
    }
    else{
        /* Set the query to the kosdaq with type argument */
        query = new QSqlQuery(db_kosdaq[type]);
        /* Set the code list to the kosdaq */
        marketList = kosdaqList;

        /* If target is index */
        if (type == INDEX_MONTH || type == INDEX_WEEK || type == INDEX_DAY || type == INDEX_30MIN || type == INDEX_5MIN){
            marketList.clear();
            marketList.append("U201"); // Kosdaq index's code
        }
    }

    /* Set the progressbar to 0 */
    ui->progressBar->setValue(0);

    /* Update kospi week database */
    for(int i = 0; i < marketList.size(); i++){
        /* Set default start date and request count */
        unsigned int startdate = 20150625;
        unsigned int count = 0;

        /* Select max date(latest date in table) */
        /* Make start date to the max date + 1 */
        QString selectSQL = QString("SELECT MAX(date) FROM %1").arg(marketList[i]);
        query->prepare(selectSQL);
        query->exec();
        query->next(); // To pointing first data

        /* Is table exist? */
        if (!query->isValid()){
            /* If there are no table, crate table */
            QString createSQL;
            if (type == MONTH || type == WEEK){
                /* If type is not minute, set the schema to the no minute schema */
                createSQL = get_createSQL_noMintue(marketList[i]);
            }
            else if ( type == DAY){
                /* If type is DAY, there are more columns */
                createSQL = get_createSQL_noMintue(marketList[i], true);
            }
            else if (type == INDEX_MONTH || type == INDEX_WEEK){
                createSQL = get_createSQL_Index_noMinute(marketList[i]);
            }
            else if (type == INDEX_DAY){
                createSQL = get_createSQL_Index_noMinute(marketList[i], true);
            }
            else if ( type == INDEX_30MIN || type == INDEX_5MIN){
                createSQL = get_createSQL_Index_Minute(marketList[i]);
            }
            else{
                /* If type is minute, set the schema to the minute schema */
                createSQL = get_createSQL_Minute(marketList[i]);
            }

            first_foreign_netbuy = 0;

            /* Create the table */
            query->prepare(createSQL);
            query->exec();
        }
        else if (query->value(0).toUInt() != 0){
            /* If the table exist, set the start date to the latest date of existing table */
            startdate = query->value(0).toUInt();

            /* If updating happens more than twice a day, pct_change and foreign netbuy may be 0
             * So, the startdate is set to the date immediately before the most recent date */

            if (type == DAY){
                QString selectSQL = QString("SELECT MAX(date), foreign_netbuy FROM %1 WHERE %1.date < %2").arg(marketList[i], QString("%1").arg(startdate));
                query->prepare(selectSQL);
                query->exec();
                query->next(); // To pointing first data

                startdate = query->value(0).toUInt();

                first_foreign_netbuy = query->value(QString("foreign_netbuy")).toInt();
            }
        }

        /* Request field of no minute type */
        QList<QVariant> field_noMinute = {REQUEST_DATE, REQUEST_OPEN, REQUEST_HIGH, REQUEST_LOW, REQUEST_CLOSE,
                                         REQUEST_VOLUME, REQUEST_VOLUME_VALUE};
        // 날짜, 시가, 고가, 저가, 종가
        // 거래량, 거래대금

        /* Day type has more columns */
        QList<QVariant> field_noMinute_day = {REQUEST_DATE, REQUEST_OPEN, REQUEST_HIGH, REQUEST_LOW, REQUEST_CLOSE,
                                         REQUEST_VOLUME, REQUEST_VOLUME_VALUE,
                                         REQUEST_SHARE_QUANTITY, REQUEST_COMPANY_PRICE,
                                         REQUEST_FOREIGN_OFFER_LIMIT, REQUEST_FOREIGN_OFFER_POSSIBLE,
                                         REQUEST_FOREIGN_HOLDING_QUANTITY, REQUEST_FOREIGN_HOLDING_RATIO,
                                         REQUEST_INSTITUTION_NETBUY, REQUEST_INSTITUTION_NETBUY_ACCUMULATION};
        // 상장 주식 수, 시가 총액
        // 외국인 주문 한도 수량, 외국인 주문 가능 수량
        // 외국인 현 보유량, 외국인 현 보유 비율
        // 기관 순매수, 기관 누적 순매수 추가

        /* Request field of minute type */
        QList<QVariant> field_Minute = {REQUEST_DATE, REQUEST_TIME, REQUEST_OPEN, REQUEST_HIGH, REQUEST_LOW, REQUEST_CLOSE,
                                     REQUEST_VOLUME, REQUEST_VOLUME_VALUE,
                                     REQUEST_BUY_ACCUMULATION, REQUEST_SELL_ACCUMULATION};
        // 기본 field에 시간, 누적 매수 체결량, 누적 매도 체결량 추가

        /* Request field of index type */
        QList<QVariant> field_Index_noMinute = {REQUEST_DATE, REQUEST_OPEN, REQUEST_HIGH, REQUEST_LOW, REQUEST_CLOSE,
                                       REQUEST_VOLUME, REQUEST_VOLUME_VALUE};

        QList<QVariant> field_Index_noMinute_day = {REQUEST_DATE, REQUEST_OPEN, REQUEST_HIGH, REQUEST_LOW, REQUEST_CLOSE,
                                                    REQUEST_VOLUME, REQUEST_VOLUME_VALUE,
                                                    REQUEST_SHARE_QUANTITY, REQUEST_COMPANY_PRICE,
                                                    REQUEST_DEPOSIT, REQUEST_ROTATION_RATIO, REQUEST_SUCCESS_VOLUME_RATIO};

        QList<QVariant> field_Index_Minute = {REQUEST_DATE, REQUEST_TIME, REQUEST_OPEN, REQUEST_HIGH, REQUEST_LOW, REQUEST_CLOSE,
                                              REQUEST_VOLUME, REQUEST_VOLUME_VALUE};


        QList<QVariant> requestField;
        /* diff function get difference between now and argument date. (20210101 -  20201231 = 1, no 2) */
        /* But latest in table date is needed. So add it. ( + 1 ) */
        if (type == MONTH){
            count = get_diff_month(startdate) + 1;
            requestField = field_noMinute;
        }
        else if (type == WEEK){
            count = get_diff_week(startdate) + 1;
            requestField = field_noMinute;
        }
        else if (type == INDEX_MONTH){
            count = get_diff_month(startdate) + 1;
            requestField = field_Index_noMinute;
        }
        else if (type == INDEX_WEEK){
            count = get_diff_week(startdate) + 1;
            requestField = field_Index_noMinute;
        }
        /* If type is day or minute, count will be not used */
        else if (type == DAY){
            requestField = field_noMinute_day;
        }
        else if (type == INDEX_DAY){
            requestField = field_Index_noMinute_day;
        }
        else if (type == INDEX_30MIN || type == INDEX_5MIN){
            requestField = field_Index_Minute;
        }
        else{
            requestField = field_Minute;
        }

        /* 권리정보 변경으로 인해서 기준 가격이 바뀔 수도 있음.
         * 이러한 변경이 일어났다면 수정된 주가를 처음부터 다시 받아야함
         * StockAdj 객체를 사용하여 현재 DB에 적힌 가장 최근의 날짜가
         * 가장 최근 권리정보가 변경된 날짜보다 이전이라면 기존의 DB에 최근 정보만 덧붙이는 것이 아니라
         * 해당 테이블 전체 레코드를 수정주가로 변경해야하기 때문에 startdate를 다시 20150625로 초기화 */
        // 예를 들어 가장 최근 권리정보가 변경된 날짜가 2021.02.09이고, DB에 적힌 최신날짜가 2021.01.26이라면
        // 별다른 조치 없이 startdate를 20210126으로 설정 시 수정된 주가가 20210209~20210126까지 DB에 반영되지만
        // 20210126 이전 데이터는 수정되지 않은 주가가 DB에 여전히 남아있음
        // 반대로 권리정보가 변경된 날짜가 DB에 적힌 최신날짜보다 더 이전이라면 수정된 주가로 DB에 반영된 상태
        QAxWidget* stockAdj = new QAxWidget("CpSysDib.StockAdj");
        stockAdj->dynamicCall("SetInputValue(QString, QString)", "0", marketList[i]); // 종목코드 세팅
        stockAdj->dynamicCall("BlockRequest()");
        Sleep(250); // To avoid warning message due to request limit

        int changedNum = stockAdj->dynamicCall("GetHeaderValue(QString)", "2").toInt(); // 권리정보 변경된 횟수를 구함
        /* 만약 변경된 경우가 존재한다면 startdate 검사 필요
         * 변경된 경우가 존재하지 않는다면 기존의 startdate 사용 */
        if (changedNum != 0){
            int date = stockAdj->dynamicCall("GetDataValue(int, int)", 0, 0).toInt(); // 가장 최근 권리정보가 변경된 날짜

            /* 권리정보 변경일이 DB의 가장 최신 날짜보다 최근일 때 테이블 전체 수정 필요 */
            /* 원래 두 값이 같다면 초기화할 필요가 없지만
             * 만약 DB를 장중에 업데이트하고 권리정보가 그 업데이트 이후에 변경된 것일 수도 있기 때문에
             * 같은 것 까지 초기화 기준을 강화 */
            if (date >= (int)startdate){
                startdate = 20150625;
            }
        }


        /* Settings of Request */
        /* SetInputValue(company code, start date, request field, type of chart, period) */
        SetInputValue(marketList[i], count, requestField, type, period, startdate);

        do {
            /* Data requesting API */
            chartDataManager->dynamicCall("BlockRequest()");
            Sleep(250); // To avoid warning message due to request limit

            /* Count of recieved data */
            QVariant c = chartDataManager->dynamicCall("GetHeaderValue(int)", 3);

            /* count_partial is part of total requested count. because BlockRequest() partially get the requseted data */
            int count_partial = c.toInt();

            /* Store the datas into the deque */
            for(int j = 0; j < count_partial; j++){
                if (type == MONTH || type == WEEK){
                    /* If type is no mintue */
                    GetDataValue_noMinute(j);
                }
                else if (type == DAY){
                    /* If type is DAY, there are more columns */
                    GetDataValue_noMinute(j, true);
                }
                else if (type == INDEX_MONTH || type == INDEX_WEEK){
                    GetDataValue_Index_noMinute(j);
                }
                else if (type == INDEX_DAY){
                    GetDataValue_Index_noMinute(j, true);
                }
                else if (type == INDEX_30MIN || type == INDEX_5MIN){
                    GetDataValue_Index_Minute(j);
                }
                else{
                    /* If type is minute */
                    GetDataValue_Minute(j);
                }
            }

        }while(chartDataManager->dynamicCall("Continue").toInt()); // If Continue member is true, there are more data waiting.

        /* Make insert query */
        QString insertSQL;
        if (type == MONTH || type == WEEK){
            insertSQL = get_insertSQL_noMinute(marketList[i]);
        }
        else if (type == DAY){
            insertSQL = get_insertSQL_noMinute(marketList[i], true); // true means that insert sql is about day db
        }
        else if (type == INDEX_MONTH || type == INDEX_WEEK){
            insertSQL = get_insertSQL_Index_noMinute(marketList[i]);
        }
        else if (type == INDEX_DAY){
            insertSQL = get_insertSQL_Index_noMinute(marketList[i], true); // true means that insert sql is about day db
        }
        else if (type == INDEX_30MIN || type == INDEX_5MIN){
            insertSQL = get_insertSQL_Index_Minute(marketList[i]);
        }
        else{
            insertSQL = get_insertSQL_Mintue(marketList[i]);
        }

        /* Insert data to the table */
        query->prepare(insertSQL);
        query->exec();

        /* Change progress bar */
        int percentage = 100 * (i + 1) / marketList.size();
        ui->progressBar->setValue(percentage);
    }

    delete query;
}


void updateWizard::SetInputValue(QString code, unsigned int count, QList<QVariant> field, int type, unsigned short period, unsigned int date){
    /* Change data type of type argument(int) to the char */
    char requestType;
    switch (type)
    {
    case MONTH:
        requestType = TYPE_MONTH;
        break;
    case WEEK:
        requestType = TYPE_WEEK;
        break;
    case DAY:
        requestType = TYPE_DAY;
        break;
    case INDEX_MONTH:
        requestType = TYPE_MONTH;
        break;
    case INDEX_WEEK:
        requestType = TYPE_WEEK;
        break;
    case INDEX_DAY:
        requestType = TYPE_DAY;
        break;
    default:
        requestType = TYPE_MINIUTE;
        break;
    }

    /* Set the company's code */
    chartDataManager->dynamicCall("SetInputValue(int, string)", COMPANY_CODE, code);

    /* Set the category to period */
    /* If type is day or mintues, use period */
    /* Else, use count */
    if (type == MONTH || type == WEEK || type == INDEX_MONTH || type == INDEX_WEEK){
        /* Set the count */
        chartDataManager->dynamicCall("SetInputValue(int, char)", REQUEST_CATEGORY, REQUEST_CATEGORY_COUNT);
        chartDataManager->dynamicCall("SetInputValue(int, unsigned int)", REQUEST_COUNT, count);
    }
    else{
        /* Set the start, end date */
        chartDataManager->dynamicCall("SetInputValue(int, char)", REQUEST_CATEGORY, REQUEST_CATEGORY_PREIOD);
        chartDataManager->dynamicCall("SetInputValue(int, unsigned int)", REQUEST_END_DATE, 0);
        chartDataManager->dynamicCall("SetInputValue(int, unsigned int)", REQUEST_START_DATE, date);
    }

    /* Set the field what we want */
    chartDataManager->dynamicCall("SetInputValue(int, int*)", REQUEST_FIELD, field);

    /* Set what type of data */
    chartDataManager->dynamicCall("SetInputValue(int, char)", CHART_TYPE, requestType);

    /* Set the cycle of data */
    chartDataManager->dynamicCall("SetInputValue(int, unsigned short)", CHART_CYCLE, period);

    /* Set the revised stock price */
    chartDataManager->dynamicCall("SetInputValue(int, char)", IS_REVISED_STOCK_PRICE, REVISED_STOCK_PRICE_ON);
}

void updateWizard::GetDataValue_noMinute(int index, bool day){
    /* Date */
    date.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 0, index).toUInt());

    /* Price info */
    open.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 1, index).toInt());
    high.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 2, index).toInt());
    low.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 3, index).toInt());
    close.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 4, index).toInt());

    /* Volume info */
    volume.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 5, index).toULongLong());
    volume_value.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 6, index).toULongLong());

    if (day){
        /* If db type is day, there are more columns */

        /* Company info */
        share_quantity.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 7, index).toULongLong());
        company_price.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 8, index).toULongLong());

        /* Foreign info */
        foreign_offer_limit.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 9, index).toUInt());
        foreign_offer_possible.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 10, index).toUInt());
        foregin_holding_quantity.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 11, index).toUInt());
        foreign_holding_ratio.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 12, index).toFloat());

        /* Institution info */
        institution_netbuy.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 13, index).toInt());
        institution_netbuy_accumulation.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 14, index).toInt());

        /* Foreign net buy */
        int size = foregin_holding_quantity.size();
        if (size > 1){
            int today_holdingQuantity = (int)foregin_holding_quantity[size - 2];
            int yesterday_holdingQuantity = (int)foregin_holding_quantity[size - 1];

            foreign_netbuy.push_back(today_holdingQuantity - yesterday_holdingQuantity);
        }
    }
}

void updateWizard::GetDataValue_Minute(int index){
    /* Date and time */
    date.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 0, index).toUInt());
    time.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 1, index).toInt());

    /* Price info */
    open.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 2, index).toInt());
    high.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 3, index).toInt());
    low.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 4, index).toInt());
    close.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 5, index).toInt());

    /* Volume info */
    volume.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 6, index).toULongLong());
    volume_value.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 7, index).toULongLong());

    /* buy, sell accumulation */
    buy_accumulation.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 8, index).toULongLong());
    sell_accumulation.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 9, index).toULongLong());
}

void updateWizard::GetDataValue_Index_noMinute(int index, bool day){
    /* Date */
    date.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 0, index).toUInt());

    /* Price info */
    open.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 1, index).toInt());
    high.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 2, index).toInt());
    low.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 3, index).toInt());
    close.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 4, index).toInt());

    /* Volume info */
    volume.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 5, index).toULongLong());
    volume_value.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 6, index).toULongLong());


    if (day){
        /* Company info */
        share_quantity.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 7, index).toULongLong());
        company_price.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 8, index).toULongLong());

        /* etc */
        deposit.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 9, index).toULongLong());
        rotationRatio.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 10, index).toFloat());
        successRatio.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 11, index).toFloat());
    }
}

void updateWizard::GetDataValue_Index_Minute(int index){
    /* Date */
    date.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 0, index).toUInt());
    time.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 1, index).toInt());

    /* Price info */
    open.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 2, index).toInt());
    high.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 3, index).toInt());
    low.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 4, index).toInt());
    close.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 5, index).toInt());

    /* Volume info */
    volume.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 6, index).toULongLong());
    volume_value.push_back(chartDataManager->dynamicCall("GetDataValue(int, int)", 7, index).toULongLong());
}

QString updateWizard::get_createSQL_noMintue(QString code, bool day){
    /* If data type is not minute */
    QString createSQL = QString("CREATE TABLE %1 ").arg(code);
    QString schemaSQL_noMinute = QString("(date INT UNSIGNED, \
                                          open INT, \
                                          high INT, \
                                          low INT, \
                                          close INT, \
                                          volume LONGLONG UNSIGNED, \
                                          volume_value LONGLONG UNSIGNED, ");

    if (day){
        /* If db is day type, there is more columns */
        schemaSQL_noMinute.append("share_quantity LONGLONG UNSIGNED, \
                                   company_price LONGLONG UNSIGNED, \
                                   foreign_offer_limit INT UNSIGNED, \
                                   foreign_offer_possible INT UNSIGNED, \
                                   foregin_holding_quantity INT UNSIGNED, \
                                   foreign_holding_ratio FLOAT, \
                                   foreign_netbuy INT, \
                                   institution_netbuy INT, \
                                   institution_netbuy_accumulation INT, ");
    }

    /* Set the primary key to date column */
    schemaSQL_noMinute.append("PRIMARY KEY (date) );");

    createSQL.append(schemaSQL_noMinute);

    return createSQL;
}

QString updateWizard::get_createSQL_Minute(QString code){
    /* If data type is minute */
    QString createSQL = QString("CREATE TABLE %1 ").arg(code);
    QString schemaSQL_Minute = QString("(date INT UNSIGNED, \
                                      time INT, \
                                      open INT, \
                                      high INT, \
                                      low INT, \
                                      close INT, \
                                      volume LONGLONG UNSIGNED, \
                                      volume_value LONGLONG UNSIGNED, \
                                      buy_accumulation LONGLONG UNSIGNED, \
                                      sell_accumulation LONGLONG UNSIGNED, \
                                      PRIMARY KEY (date, time) );");

    createSQL.append(schemaSQL_Minute);

    return createSQL;
}

QString updateWizard::get_createSQL_Index_noMinute(QString code, bool day){
    /* If data type is not minute */
    QString createSQL = QString("CREATE TABLE %1 ").arg(code);
    QString schemaSQL_noMinute = QString("(date INT UNSIGNED, \
                                          open INT, \
                                          high INT, \
                                          low INT, \
                                          close INT, \
                                          volume LONGLONG UNSIGNED, \
                                          volume_value LONGLONG UNSIGNED, ");

    if (day){
        /* If db is day type, there is more columns */
        schemaSQL_noMinute.append("share_quantity LONGLONG UNSIGNED, \
                                   company_price LONGLONG UNSIGNED, \
                                   deposit LONGLONG UNSIGNED, \
                                   rotationRatio Float, \
                                   successRatio Float, ");
    }

    /* Set the primary key to date column */
    schemaSQL_noMinute.append("PRIMARY KEY (date) );");

    createSQL.append(schemaSQL_noMinute);

    return createSQL;
}

QString updateWizard::get_createSQL_Index_Minute(QString code){
    /* If data type is not minute */
    QString createSQL = QString("CREATE TABLE %1 ").arg(code);
    QString schemaSQL_Minute = QString("(date INT UNSIGNED, \
                                          time INT, \
                                          open INT, \
                                          high INT, \
                                          low INT, \
                                          close INT, \
                                          volume LONGLONG UNSIGNED, \
                                          volume_value LONGLONG UNSIGNED, ");

    /* Set the primary key to date, time column */
    schemaSQL_Minute.append("PRIMARY KEY (date, time) );");

    createSQL.append(schemaSQL_Minute);

    return createSQL;
}

QString updateWizard::get_insertSQL_noMinute(QString code, bool day){
    /* Make insert SQL */
    QString insertSQL;
    QTextStream(&insertSQL) << "INSERT OR REPLACE INTO "
                            << code << " "
                            << "(date, open, high, low, close, volume, volume_value";

    if (day){
        /* If db type is day, there are more columns */
        insertSQL.append(", share_quantity, company_price, \
                          foreign_offer_limit, foreign_offer_possible, foregin_holding_quantity, foreign_holding_ratio, foreign_netbuy, \
                          institution_netbuy, institution_netbuy_accumulation");

        foreign_netbuy.push_back(first_foreign_netbuy); // for first date
    }

    insertSQL.append(") VALUES");

    while(!date.empty()){
        QString value;
        QTextStream(&value) << " (" << date.back() << ", "
                            << open.back() << ", " << high.back() << ", " << low.back() << ", " << close.back() << ", "
                            << volume.back() << ", " << volume_value.back();
        insertSQL.append(value);

        if (day){
            /* If db type is day, there are more columns */
            QString additionalValue;
            QTextStream(&additionalValue) << ", "
                                          << share_quantity.back() << ", " << company_price.back() << ", "
                                          << foreign_offer_limit.back() << ", " << foreign_offer_possible.back() << ", "
                                          << foregin_holding_quantity.back() << ", " << foreign_holding_ratio.back() << ", "
                                          << foreign_netbuy.back() << ","
                                          << institution_netbuy.back() << ", " << institution_netbuy_accumulation.back();
            insertSQL.append(additionalValue);

            insertSQL.append("),");

            pop_back_all(false, true); // min is false, day is true
        }
        else{
            insertSQL.append("),");

            pop_back_all();
        }
    }

    /* Replace the end of insert SQL. , to ; */
    /* ), => ); */
    int lastIndex = insertSQL.size() - 1;
    insertSQL.replace(lastIndex, 1, ";");

    return insertSQL;
}

QString updateWizard::get_insertSQL_Mintue(QString code){
    /* Make insert SQL */
    QString insertSQL;
    QTextStream(&insertSQL) << "INSERT OR REPLACE INTO "
                            << code << " "
                            << "(date, time, open, high, low, close, volume, volume_value, buy_accumulation, sell_accumulation"
                            << ") VALUES";

    while(!date.empty()){
        QString value;
        QTextStream(&value) << " (" << date.back() << ", " << time.back() << ", "
                            << open.back() << ", " << high.back() << ", " << low.back() << ", " << close.back() << ", "
                            << volume.back() << ", " << volume_value.back() << ", "
                            << buy_accumulation.back() << ", " << sell_accumulation.back() <<  "),";
        insertSQL.append(value);
        pop_back_all(true, false); // min is true, day is false
    }

    /* Replace the end of insert SQL. , to ; */
    /* ), => ); */
    int lastIndex = insertSQL.size() - 1;
    insertSQL.replace(lastIndex, 1, ";");

    return insertSQL;
}

QString updateWizard::get_insertSQL_Index_noMinute(QString code, bool day){
    /* Make insert SQL */
    QString insertSQL;
    QTextStream(&insertSQL) << "INSERT OR REPLACE INTO "
                            << code << " "
                            << "(date, open, high, low, close, volume, volume_value";


    if (day){
        /* If db type is day, there are more columns */
        insertSQL.append(", share_quantity, company_price, deposit, rotationRatio, successRatio");
    }

    insertSQL.append(") VALUES");

    while(!date.empty()){
        QString value;
        QTextStream(&value) << " (" << date.back() << ", "
                            << open.back() << ", " << high.back() << ", " << low.back() << ", " << close.back() << ", "
                            << volume.back() << ", " << volume_value.back();
        insertSQL.append(value);

        if (day){
            /* If db type is day, there are more columns */
            QString additionalValue;
            QTextStream(&additionalValue) << ", "
                                          << share_quantity.back() << ", " << company_price.back() << ", "
                                          << deposit.back() << ", " << rotationRatio.back() << ", " << successRatio.back();
            insertSQL.append(additionalValue);

            insertSQL.append("),");

            pop_back_all();
            share_quantity.pop_back(); company_price.pop_back();
            deposit.pop_back(); rotationRatio.pop_back(); successRatio.pop_back();
        }
        else{
            insertSQL.append("),");

            pop_back_all();
        }
    }

    /* Replace the end of insert SQL. , to ; */
    /* ), => ); */
    int lastIndex = insertSQL.size() - 1;
    insertSQL.replace(lastIndex, 1, ";");

    return insertSQL;
}

QString updateWizard::get_insertSQL_Index_Minute(QString code){
    /* Make insert SQL */
    QString insertSQL;
    QTextStream(&insertSQL) << "INSERT OR REPLACE INTO "
                            << code << " "
                            << "(date, time, open, high, low, close, volume, volume_value";


    insertSQL.append(") VALUES");

    while(!date.empty()){
        QString value;
        QTextStream(&value) << " (" << date.back() << ", " << time.back() << ", "
                            << open.back() << ", " << high.back() << ", " << low.back() << ", " << close.back() << ", "
                            << volume.back() << ", " << volume_value.back();
        insertSQL.append(value);

        insertSQL.append("),");

        pop_back_all();
        time.pop_back();
    }

    /* Replace the end of insert SQL. , to ; */
    /* ), => ); */
    int lastIndex = insertSQL.size() - 1;
    insertSQL.replace(lastIndex, 1, ";");

    return insertSQL;
}


void updateWizard::pop_back_all(bool min, bool day){
    /* Pop all deques */
    date.pop_back();

    open.pop_back();
    high.pop_back();
    low.pop_back();
    close.pop_back();

    volume.pop_back();
    volume_value.pop_back();

    if (day){
        /* If db type is day, pop more deques */
        share_quantity.pop_back();
        company_price.pop_back();

        foreign_offer_limit.pop_back();
        foreign_offer_possible.pop_back();
        foregin_holding_quantity.pop_back();
        foreign_holding_ratio.pop_back();

        foreign_netbuy.pop_back();

        institution_netbuy.pop_back();
        institution_netbuy_accumulation.pop_back();
    }

    if (min){
        /* If db type is minute, pop more deques */
        time.pop_back();
        buy_accumulation.pop_back();
        sell_accumulation.pop_back();
    }
}

unsigned int get_diff_month(unsigned int date){
    /* Dynamic allocation for tm struct for parsing argument */
    /* date is YYYYMMDD */
    struct tm* tm_date = new tm;

    /* Set the year value and month value */
    tm_date->tm_year = date / 10000 - 1900; // tm struct's year value recognize 1900 is 0
    tm_date->tm_mon = (date % 10000) / 100 - 1; // tm struct's month value is 0 ~ 11

    /* Dynamic allocation for tm struct and get the time_t about now */
    struct tm* tm_now = new tm;
    time_t time_now = time(NULL);

    /* Make time_t to the tm struct */
    *tm_now = *localtime(&time_now);

    /* Get the difference between the month of now and argument's date */
    unsigned int diff_year = tm_now->tm_year - tm_date->tm_year;
    unsigned int count_month = (diff_year * 12) + tm_now->tm_mon - tm_date->tm_mon;

    /* Delete dynamic allocation */
    delete tm_date;
    delete tm_now;

    return count_month;
}

unsigned int get_diff_week(unsigned int date){
    /* List of days corresponding month */
    int lastDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Dynamic allocation for tm struct for parsing argument */
    /* date is YYYYMMDD */
    struct tm* tm_date = new tm;

    /* Set the year value and month value */
    tm_date->tm_year = date / 10000 - 1900; // tm struct's year value recognize 1900 is 0
    tm_date->tm_mon = (date % 10000) / 100 - 1; // tm struct's month value is 0 ~ 11

    /* Dynamic allocation for tm struct and get the time_t about now */
    struct tm* tm_now = new tm;
    time_t time_now = time(NULL);

    /* Make time_t to the tm struct */
    *tm_now = *localtime(&time_now);

    /* Get the difference between start date and now date */
    /* First, get the year between startdate's next year and now date's previous year and multiply 365 */
    unsigned int diff_day = (tm_now->tm_year - tm_date->tm_year - 1) * 365;

    /* Second, get the sum of days of startdate's year */
    for(int i = 11; i >= tm_date->tm_mon; i--){
        diff_day += lastDay[i];
    }

    /* Third, get the sum of days of now date's year */
    diff_day += tm_now->tm_yday;

    /* Delete dynamic allocation */
    delete tm_date;
    delete tm_now;

    /* return total count of week with some overhead for no missing data */
    return diff_day / 7 + 1;
}
