#ifndef CONNECTTOSERVERDIALOG_H
#define CONNECTTOSERVERDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectToServerDialog;
}

class ConnectToServerDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ConnectToServerDialog(QWidget *parent = 0);
        ~ConnectToServerDialog();

        QString getNickname() const;
        QString getAddress() const;
        int getPort() const;

    private:
        Ui::ConnectToServerDialog *ui;
};

#endif // CONNECTTOSERVERDIALOG_H
