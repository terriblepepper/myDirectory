#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_Directory.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QMap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include<QTextCodec>
#include <QStackedWidget>
#include <QVariantMap>
#include<QTextEdit>
#include<QMessageBox>
#include "AddContactDialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class DirectoryClass; }
QT_END_NAMESPACE

class Directory : public QMainWindow
{
    Q_OBJECT

public:
    Directory(QWidget* parent = nullptr);
    ~Directory();

private slots:
    void addContact();
private:
    Ui::DirectoryClass* ui;//ui
    QMap<QString, QVariant> contacts;//存储联系人信息用于读和取
    QMap<QString, QPushButton*> contactsButtons;//存储联系人按钮
    QMap<QString, QWidget*> contactInfoPages; //存储联系人页面
    QWidget* preWidget = nullptr;
    int contactInfoOffset = 264;//联系人按钮偏移量
    int contactInfoSpace = 30;//联系人按钮间隔
    int infoHeight = 50;//信息页，每行信息标签高度
    int btn_Height = 100;//联系人按钮高度
    int counts = 0;//记录联系人个数
    int inputDialogMode = -1;//修改：1；删除：2；搜索：3
    QMap<QString, QVariant> readJsonToQMap(const QString& filePath);
    void writeQMapToJsonFile(const QMap<QString, QVariant>& contactsMap, const QString& filePath);
    void inputNameDialog();
    void loadContactsPages();
    void addContactsButtons();
    void moveButtons();
    void flashPages(int mode, const QString& contactName);
    void modifyContact(const QString& contactName);
    void deleteContact(const QString& contactName);
    void searchContact(const QString& contactName);
};
