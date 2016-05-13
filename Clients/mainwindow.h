#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <iostream>
#include "dialogauthorization.h"
#include "Oerror.h"
#include "client.h"
#include <memory>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "popup.h"

namespace Ui {
class MainWindow;
}

struct lastMessage{
    QString text;
    bool good;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Client> client;
    std::string message;
    const std::string pathBegin = "conversation";
    PopUp *popUpMessager;
    void changeEvent(QEvent*event);
    bool isHiddenmode = false;
    lastMessage LM;

    void authorizationDialog();
    void addTextToFileAndTextEdit(std::string& name, std::string&text);
public slots:
        void copyClient(std::shared_ptr<Client>&);
        void autoConnectionSlot();
private slots:
        void on_pushButton_clicked();
        void on_actionQuit_triggered();
        void on_actionSign_Out_triggered();
        void singleShotSetMessag();
        // clients clots
        void timeSlot(std::string time);
        void customersSlot(std::set<std::string>& customers);
        void dataSlot(std::string data);
        void sendSlot(std::string name, std::string text);
        void connectionSlot(bool);
        void authorizationSlot(bool);
        void on_listWidget_currentRowChanged(int currentRow);
};

#endif // MAINWINDOW_H
