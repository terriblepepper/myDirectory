#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMap>
#include <QVariant>
#include <QDate>

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    AddContactDialog(QWidget* parent = nullptr);
    QMap<QString, QVariant> getContactInfo() const;
    void setContactInfo(const QMap<QString, QVariant>& contactInfo);
private slots:
    void validateInput();

private:
    QMap<QString, QLineEdit*> inputFields;
    QDialogButtonBox* buttonBox;
};
