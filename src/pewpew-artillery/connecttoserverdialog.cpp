#include "connecttoserverdialog.h"
#include "ui_connecttoserverdialog.h"

ConnectToServerDialog::ConnectToServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToServerDialog)
{
    ui->setupUi(this);
}

ConnectToServerDialog::~ConnectToServerDialog() {
    delete ui;
}

QString ConnectToServerDialog::getNickname() const {
    return ui->nicknameField->text();
}

QString ConnectToServerDialog::getAddress() const {
    return ui->addressField->text();
}

int ConnectToServerDialog::getPort() const {
    return ui->portField->value();
}
