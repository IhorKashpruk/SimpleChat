#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Create authorization dialog
    authorizationDialog();
    popUpMessager = new PopUp();
}

MainWindow::~MainWindow()
{
    delete ui;
}


// SLOTS
void MainWindow::copyClient(std::shared_ptr<Client> &obj){
    client = obj;
    connect(client.get(), SIGNAL(CloseConnectionSignal()), this, SLOT(autoConnectionSlot()));
    connect(client.get(), SIGNAL(FromServerTimeSignal(std::string)), this, SLOT(timeSlot(std::string)));
    connect(client.get(), SIGNAL(FromServerCustomersSignal(std::set<std::string>&)), this, SLOT(customersSlot(std::set<std::string>&)));
    connect(client.get(), SIGNAL(FromServerDataSignal(std::string)), SLOT(dataSlot(std::string)));
    connect(client.get(), SIGNAL(FromServerSendSignal(std::string,std::string)), this, SLOT(sendSlot(std::string,std::string)));
    connect(client.get(), SIGNAL(ConnectionToServerSignal(bool)), this, SLOT(connectionSlot(bool)));
    connect(client.get(), SIGNAL(AuthorizationSignal(bool)), this, SLOT(authorizationSlot(bool)));

    client->sendMessageToServer("customers");
    ui->label_3->setText(QString("Login: ") + client->getAccount().login.c_str());
    client->sendMessageToServer("time");
    message = "data";
    QTimer::singleShot(100, this, SLOT(singleShotSetMessag()));
    ui->statusBar->showMessage("Connection: true; Authorization: true.");

}

void MainWindow::authorizationDialog(){
    client.reset();
    DialogAuthorization dialogA;
    connect(&dialogA, SIGNAL(sendClientPointSignal(std::shared_ptr<Client>&)), this, SLOT(copyClient(std::shared_ptr<Client>&)));
    if(dialogA.exec() == QDialog::Rejected){
        exit(0);
    }
}


void MainWindow::autoConnectionSlot(){

    client->connectAndAuthorizationAutoMode();
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->textEdit_2->toPlainText().isEmpty())
        return;
    QString text = ui->textEdit_2->toPlainText();
    for(int i = text.size()-1; i >= 0; i--){
        if((int)text.at(i).toLatin1() == 10){
            text.remove(i, 1);
        }else
            break;
    }
    LM.text = text;
    LM.good = false;

    if(ui->listWidget->currentRow() >= 0){
        if(ui->listWidget->currentItem()->text().compare("Server") == 0){
            client->sendMessageToServer(text.toStdString());
            qDebug() << "send " << text;
        }
        else
        if(client->sendMessageToServer("send " + ui->listWidget->currentItem()->text().toStdString() +
                                std::string(" '") + text.toStdString() + std::string("'"))){

        LM.good = true;
        QFile file((pathBegin+ui->listWidget->currentItem()->text().toStdString()).c_str());
        if(file.open(QIODevice::Append | QIODevice::Text)){
            file.write(QString(QString("[") + QDateTime::currentDateTime().toString("dd.MM.yy hh:mm::ss") + QString("] 222I222: ")+ text).toStdString().c_str());
            file.write("\n");
            file.close();
        }
        ui->textEdit->moveCursor(QTextCursor::End);
        ui->textEdit->setTextColor(QColor("lightGray"));
        ui->textEdit->insertPlainText(QString("[") + QDateTime::currentDateTime().toString("dd.MM.yy hh:mm::ss") + QString("] "));
        ui->textEdit->setTextColor(QColor("green"));
        ui->textEdit->insertPlainText("Me: ");
        ui->textEdit->setTextColor(QColor("black"));
        ui->textEdit->insertPlainText(text + "\n");
        }
    }
    else{
         QMessageBox::information(this, "Information", "Choose client!");
         return;
    }
    ui->textEdit_2->clear();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionSign_Out_triggered()
{
    disconnect(client.get(), SIGNAL(CloseConnectionSignal()), this, SLOT(autoConnectionSlot()));
    client->closeConnection();
    authorizationDialog();
}

void MainWindow::addTextToFileAndTextEdit(std::string &name, std::string & text){
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    QFile file((pathBegin+name).c_str());
    if(file.open(QIODevice::Append | QIODevice::Text)){
        file.write(("[" + QDateTime::currentDateTime().toString("dd.MM.yy hh:mm::ss").toStdString() + "] 111YOU111: " +text).c_str());
        file.write("\n");
        file.close();
    }
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->setTextColor(QColor("lightGray"));
    ui->textEdit->insertPlainText(QString("[") + QDateTime::currentDateTime().toString("dd.MM.yy hh:mm::ss") + QString("] "));
    ui->textEdit->setTextColor(QColor("blue"));
    ui->textEdit->insertPlainText(name.c_str() + QString(": "));
    ui->textEdit->setTextColor(QColor("black"));
    ui->textEdit->insertPlainText(text.c_str() + QString("\n"));


    if(isHiddenmode){
        qDebug() << "is hidden";
        popUpMessager->setPopupText(name.c_str() + QString(": ") + text.c_str());
        popUpMessager->show();
    }
}

// SLOTS
void MainWindow::customersSlot(std::set<std::string>& customers){
    if(LM.text.compare("customers") == 0 && LM.good == false){
        std::string name = "Server";
        std::string text;
        for(auto it: customers){
            text += it + " ";
        }
        addTextToFileAndTextEdit(name, text);
        LM.good = true;
    }
    ui->listWidget->clear();
    ui->listWidget->addItem("Server");
    for(auto& it: customers){
        if(it.compare(client->getAccount().login) != 0)
            ui->listWidget->addItem(QString(it.c_str()));
    }

}

void MainWindow::sendSlot(std::string name, std::string text){
    addTextToFileAndTextEdit(name, text);
}
void MainWindow::dataSlot(std::string  data){
    if(LM.text.compare(data.c_str()) == 0){
        LM.good = true;
    }
    static int n = 0;
    if(n == 0){
        ui->label_2->setText(QString("Data: ") + data.c_str());
        n++;
        return;
    }
    std::string name = "Server";
    addTextToFileAndTextEdit(name, data);
}

void MainWindow::timeSlot(std::string time){
    if(LM.text.compare(time.c_str()) == 0){
        LM.good = true;
    }
    static int t = 0;
    if(t == 0){
        ui->label->setText(QString("Time: ") + time.c_str());
        t++;
        return;
    }
    std::string name = "Server";
    addTextToFileAndTextEdit(name, time);
}

void MainWindow::singleShotSetMessag(){
    client->sendMessageToServer(message.c_str());
}

void MainWindow::connectionSlot(bool resoult){
    if(resoult)
        ui->statusBar->showMessage("Connection: true");
    else
        ui->statusBar->showMessage("Connection: fasle");
}

void MainWindow::authorizationSlot(bool resoult){
    if(resoult)
        ui->statusBar->showMessage("Authorization: true");
    else
        ui->statusBar->showMessage("Authorization: fasle");
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    if(currentRow == -1)
        return;
    ui->textEdit->clear();
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    QFile file((pathBegin + ui->listWidget->item(currentRow)->text().toStdString()).c_str());
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream textStream(&file);
        ui->textEdit->setText(textStream.readAll());
        ui->textEdit->moveCursor(QTextCursor::Start);
        while(ui->textEdit->find("111YOU111")){
            ui->textEdit->clearFocus();
            ui->textEdit->setTextColor(QColor("blue"));
            ui->textEdit->insertPlainText(ui->listWidget->item(currentRow)->text());
            ui->textEdit->setTextColor(QColor("black"));
        }
        ui->textEdit->moveCursor(QTextCursor::Start);
        while(ui->textEdit->find("222I222")){
              ui->textEdit->clearFocus();
              ui->textEdit->setTextColor(QColor("green"));
              ui->textEdit->insertPlainText("Me");
              ui->textEdit->setTextColor(QColor("black"));
        }
        file.close();
    }
}


void MainWindow::changeEvent(QEvent *event){
    if(event->type() == QEvent::WindowStateChange)
        isHiddenmode = true;
    else
        isHiddenmode = false;

}
