#include "dialogauthorization.h"
#include "ui_dialogauthorization.h"

DialogAuthorization::DialogAuthorization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAuthorization)
{
    ui->setupUi(this);
    client.reset(new Client(HostAddres::LOOPBACK, 3334));
    connect(client.get(), SIGNAL(ConnectionToServerSignal(bool)), this, SLOT(checkConnectionSlot(bool)));
    connect(client.get(), SIGNAL(InformationSignal(QString)), this, SLOT(informationSlot(QString)));
    connect(client.get(), SIGNAL(AuthorizationSignal(bool)), this, SLOT(checkAuthorizationSlot(bool)));
    connect(client.get(), SIGNAL(CloseConnectionSignal()), this, SLOT(closeConnectionSlot()));
    client->connectToServerAutoMode();
}

DialogAuthorization::~DialogAuthorization()
{
    delete ui;
}

// SLOTS


void DialogAuthorization::on_closePushButton_clicked()
{
    emit reject();
}

void DialogAuthorization::on_okPushButton_clicked()
{
    if(ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty()){
        QMessageBox::information(this, "Error", "Enter login and password");
        return;
    }

    // Check login and password
    client->setLoginAndPassword(ui->lineEdit->text().toStdString(), ui->lineEdit_2->text().toStdString());
    client->authorization();
}

void DialogAuthorization::checkConnectionSlot(bool result){
    if(result){
        ui->okPushButton->setEnabled(true);
    }
}

void DialogAuthorization::informationSlot(QString str){
    ui->textEdit->append("information: ");
    ui->textEdit->append(str);
}

void DialogAuthorization::checkAuthorizationSlot(bool result){
    if(result){
        emit sendClientPointSignal(client);
        done(1);
    }
}

void DialogAuthorization::closeConnectionSlot(){
    qDebug() << "closeConnectionSlot()\n";
    client->connectToServerAutoMode();
}
