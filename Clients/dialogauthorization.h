#ifndef DIALOGAUTHORIZATION_H
#define DIALOGAUTHORIZATION_H

#include <QDialog>
#include <QMessageBox>
#include <memory>
#include "client.h"

namespace Ui {
class DialogAuthorization;
}

class DialogAuthorization : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAuthorization(QWidget *parent = 0);
    ~DialogAuthorization();

private slots:
    void on_closePushButton_clicked();
    void on_okPushButton_clicked();

    void checkConnectionSlot(bool result);
    void informationSlot(QString str);
    void checkAuthorizationSlot(bool result);
    void closeConnectionSlot();

private:
    Ui::DialogAuthorization *ui;
    std::shared_ptr<Client> client;

signals:
    void sendClientPointSignal(std::shared_ptr<Client>&);

};

#endif // DIALOGAUTHORIZATION_H
