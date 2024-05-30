#include "AddContactDialog.h"
#include <QLabel>
#include <QRegExpValidator>
#include <QMessageBox>

AddContactDialog::AddContactDialog(QWidget* parent)
    : QDialog(parent), buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel))
{
    QFormLayout* layout = new QFormLayout(this);

    inputFields["name"] = new QLineEdit(this);
    inputFields["phone"] = new QLineEdit(this);
    inputFields["email"] = new QLineEdit(this);
    inputFields["birthday"] = new QLineEdit(this);

    layout->addRow(new QLabel("Name:"), inputFields["name"]);
    layout->addRow(new QLabel("Phone:"), inputFields["phone"]);
    layout->addRow(new QLabel("Email:"), inputFields["email"]);
    layout->addRow(new QLabel("Birthday:"), inputFields["birthday"]);

    inputFields["phone"]->setValidator(new QRegExpValidator(QRegExp("^1[3-9]\\d{9}$"), this)); // 中国标准手机号
    inputFields["email"]->setValidator(new QRegExpValidator(QRegExp("\\S+@\\S+\\.\\S+"), this));
    inputFields["birthday"]->setValidator(new QRegExpValidator(QRegExp("\\d{4}-\\d{2}-\\d{2}"), this));

    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddContactDialog::validateInput);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QMap<QString, QVariant> AddContactDialog::getContactInfo() const
{
    QMap<QString, QVariant> contactInfo;
    for (const auto& key : inputFields.keys()) {
        contactInfo.insert(key, inputFields[key]->text());
    }
    return contactInfo;
}

void AddContactDialog::validateInput()
{
    for (const auto& key : inputFields.keys()) {
        QLineEdit* lineEdit = inputFields[key];
        if (lineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "输入错误", "必须把信息填完！");
            return;
        }

        // Validate input with its validator
        QString input = lineEdit->text();
        int pos = 0;
        if (lineEdit->validator() != nullptr) {
            QValidator::State state = lineEdit->validator()->validate(input, pos);
            if (state != QValidator::Acceptable) {
                QMessageBox::warning(this, "输入错误", QString("输入框 '%1' 非法！").arg(key));
                return;
            }
        }

        // Additional validation for birthday
        if (key == "birthday") {
            QDate date = QDate::fromString(input, "yyyy-MM-dd");
            if (!date.isValid() || date > QDate::currentDate()) {
                QMessageBox::warning(this, "输入错误", "生日日期非法！");
                return;
            }
        }
    }
    accept();
}

void AddContactDialog::setContactInfo(const QMap<QString, QVariant>& contactInfo)
{
    // 设置每个输入字段的文本为联系人信息中对应的值
    for (const auto& key : inputFields.keys()) {
        if (contactInfo.contains(key)) {
            // 如果当前字段不是姓名字段，设置文本
            if (key != "name") {
                inputFields[key]->setText(contactInfo[key].toString());
            }
            else {
                // 如果当前字段是姓名字段，设置为只读
                inputFields[key]->setText(contactInfo[key].toString());
                inputFields[key]->setReadOnly(true);
            }
        }
    }
}