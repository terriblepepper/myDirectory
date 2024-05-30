#include "Directory.h"

Directory::Directory(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::DirectoryClass())
{
    ui->setupUi(this);
    // 调整联系人列表窗口小部件的大小以适应内容
    ui->contactsWidget->adjustSize();
    //设置按钮样式
    ui->stackedWidget->setStyleSheet("background-color: white");
    ui->scrollAreaWidgetContents->setStyleSheet(
        "QPushButton {"
        "    background-color: white;" // 设置背景透明
        "    border-top: 1px solid gray;"    // 在上部边缘显示横线
        "font-weight: bold"
        "}"
        "QPushButton:hover {"
        "    background-color: rgb(245,245,245);" // 设置背景透明
        "    border-top: 1px solid gray;"    // 在上部边缘显示横线
        "}"
    );
    contacts = readJsonToQMap("./data/contacts.json");
    qInfo() << "---------------------" << contacts.size();
    //把联系人按钮加入
    addContactsButtons();
    //加载联系人页面
    loadContactsPages();

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
    connect(ui->addAction, &QAction::triggered, this, &Directory::addContact);
    connect(ui->modifyAction, &QAction::triggered, [this]() 
        {
            inputDialogMode = 1;
            inputNameDialog();
        });
    connect(ui->deleteAction, &QAction::triggered, [this]()
        {
            inputDialogMode = 2;
            inputNameDialog();
        });
    connect(ui->search, &QAction::triggered, [this]()
        {
            inputDialogMode = 3;
            inputNameDialog();
        });
    connect(ui->totalInfo, &QAction::triggered, [this]() {
        preWidget = ui->stackedWidget->currentWidget();
        ui->infoPage->findChild<QLabel*>("totalEdit")->setText(QString::number(counts));
        ui->stackedWidget->setCurrentWidget(ui->infoPage);
        });
    connect(ui->About_back, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentWidget(preWidget);
        });
}

Directory::~Directory()
{
    writeQMapToJsonFile(contacts, "./data/contacts.json");
    delete ui;
}

QMap<QString, QVariant> Directory::readJsonToQMap(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("无法打开文件");
        return QMap<QString, QVariant>();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (document.isNull()) {
        qWarning("无法解析JSON文件");
        return QMap<QString, QVariant>();
    }

    QVariantMap variantMap = document.toVariant().toMap();
    QMap<QString, QVariant> contactsMap ; 

    for (auto it = variantMap.begin(); it != variantMap.end(); ++it) {
        QString name = it.key();
        QJsonArray contactArray = it.value().toJsonArray();
        if (!contactArray.isEmpty()) {
            QJsonObject contactObject = contactArray.first().toObject();
            contactsMap.insert(name, contactObject.toVariantMap()); // 插入新读取的联系人信息
        }
    }
    return contactsMap;
}

void Directory::writeQMapToJsonFile(const QMap<QString, QVariant>& contactsMap, const QString& filePath)
{
    QJsonObject jsonObject;

    // 遍历联系人信息
    for (auto it = contactsMap.constBegin(); it != contactsMap.constEnd(); ++it) {
        QString name = it.key();
        QVariantMap contactInfo = it.value().toMap();

        // 创建联系人对象
        QJsonArray contactArray;
        QJsonObject contactObject;
        contactObject["name"] = name;
        contactObject["phone"] = contactInfo["phone"].toString();
        contactObject["email"] = contactInfo["email"].toString();
        contactObject["birthday"] = contactInfo["birthday"].toString();
        contactArray.append(contactObject);

        // 将联系人对象添加到 JSON 对象中
        jsonObject[name] = contactArray;
    }

    // 创建 JSON 文档
    QJsonDocument document(jsonObject);

    // 写入 JSON 文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("无法打开文件进行写入");
        return;
    }

    file.write(document.toJson());
    file.close();
}

void Directory::loadContactsPages()
{
    //把联系人页面加载
    for (auto& key :contacts.keys())
    {
        QVariantMap contactInfo = contacts[key].toMap();
        //设置联系人信息页
        QPushButton* back = new QPushButton("←", contactInfoPages[key]);
        back->setFixedSize(70, 40);
        back->setStyleSheet(
            "QPushButton{ background-color: transparent;color: #000000; "
            "font-weight: lighter;"
            "font-size: 48px"
            "}"
            "QPushButton:hover{color: #808080;}"
        );
        connect(back, &QPushButton::clicked, [this]() {
            ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
            });

        QLabel* headLabel = new QLabel(contactInfoPages[key]);
        // 原始 QPixmap 对象
        QPixmap originalPixmap(":/Directory/image/contactIcon.png");
        // 缩放 QPixmap 对象到指定大小
        QSize newSize(120, 120); // 新的大小
        QPixmap scaledHead = originalPixmap.scaled(newSize);
        headLabel->setPixmap(scaledHead);
        headLabel->setFixedSize(120, 120);
        headLabel->move(10, 70);

        QLabel* contactNameLabel = new QLabel(key, contactInfoPages[key]);
        contactNameLabel->setStyleSheet("QLabel"
            "{"
            "font-size: 54px;"
            "font-weight: bold"
            "}");
        contactNameLabel->setObjectName("nameLabel");
        contactNameLabel->move(0, 200);
        contactNameLabel->setFixedSize(540, 70);

        QLabel* contactInfoLabel1 = new QLabel("手机号码 | " + contactInfo["phone"].toString(), contactInfoPages[key]);
        contactInfoLabel1->setStyleSheet("QLabel"
            "{"
            "font-size: 36px;"
            "border-top: 1px solid gray; "
            "}");
        contactInfoLabel1->setObjectName("phoneLabel");
        contactInfoLabel1->move(0, contactInfoOffset + contactInfoSpace);
        contactInfoLabel1->setFixedSize(540, infoHeight);
        qInfo() << "----------height" << contactInfoLabel1->height() << contactNameLabel->height();
        QLabel* contactInfoLabel2 = new QLabel("邮箱 | " + contactInfo["email"].toString(), contactInfoPages[key]);
        contactInfoLabel2->setStyleSheet("QLabel"
            "{"
            "font-size: 36px;"
            "border-top: 1px solid gray; "
            "}");
        contactInfoLabel2->setObjectName("emailLabel");
        contactInfoLabel2->move(0, infoHeight + contactInfoOffset + 2 * contactInfoSpace);
        contactInfoLabel2->setFixedSize(540, infoHeight);

        QLabel* contactInfoLabel3 = new QLabel("生日 | " + contactInfo["birthday"].toString(), contactInfoPages[key]);
        contactInfoLabel3->setStyleSheet("QLabel"
            "{"
            "font-size: 36px;"
            "border-top: 1px solid gray;"
            "}");
        contactInfoLabel3->setObjectName("birthdayLabel");
        contactInfoLabel3->move(0, 2 * infoHeight + contactInfoOffset + 3 * contactInfoSpace);
        contactInfoLabel3->setFixedSize(540, infoHeight);
    }
}

void Directory::addContactsButtons()
{
    for (auto& key :contacts.keys())
    {
        QPushButton* btn_contacts = new QPushButton(ui->scrollAreaWidgetContents);
        btn_contacts->setObjectName(key);
        QWidget* contactsInfo = new QWidget;
        btn_contacts->setFixedSize(514, btn_Height);
        btn_contacts->setText(key);
        btn_contacts->setStyleSheet("text-align:left;");
        contactInfoPages.insert(key, contactsInfo);
        ui->stackedWidget->addWidget(contactsInfo);
        // 连接按钮点击事件到槽函数
        connect(btn_contacts, &QPushButton::clicked, this, [=]() {
            // 切换到联系人信息页面
            ui->stackedWidget->setCurrentWidget(contactInfoPages[key]);
            });
        contactsButtons.insert(key, btn_contacts);
        counts++;
    }
    moveButtons();
}

void Directory::moveButtons()
{
    int i = 0;
    // 获取键的列表并按首字母排序
    QList<QString> sortedKeys = contactsButtons.keys();
    std::sort(sortedKeys.begin(), sortedKeys.end(), [](const QString& a, const QString& b) {
        return a.toLower() < b.toLower(); // 不区分大小写进行排序
        });

    // 按排序后的顺序遍历contactsButtons
    for (const QString& key : sortedKeys) {
        contactsButtons[key]->move(0, i++ * btn_Height);
    }
    if (counts > 8)
    {
        ui->scrollAreaWidgetContents->setFixedHeight(counts * btn_Height+28);
    }
}

void Directory::modifyContact(const QString& contactName)
{
    // 检查联系人是否存在
    if (!contacts.contains(contactName)) 
    {
        QMessageBox::warning(this,"警告","联系人不存在");
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
        return;
    }

    // 创建一个修改联系人信息的对话框
    AddContactDialog dialog(this);

    // 获取当前联系人的信息
    QMap<QString, QVariant> contactData = contacts[contactName].toMap();

    // 将当前联系人的信息填充到对话框中
    dialog.setContactInfo(contactData);

    // 执行对话框，并等待用户操作
    if (dialog.exec() == QDialog::Accepted) 
    {
        // 获取修改后的联系人信息
        QMap<QString, QVariant> modifiedContactData = dialog.getContactInfo();

        // 更新联系人信息
        contacts[contactName] = modifiedContactData;
        
        // 更新 UI 显示等操作
        ui->stackedWidget->setCurrentWidget(ui->buffer);
        flashPages(1,contactName);
        ui->stackedWidget->setCurrentWidget(contactInfoPages[contactName]);
    }
    else if(dialog.exec() == QDialog::Rejected)
    {
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
    }
}

void Directory::deleteContact(const QString& contactName)
{
    if (contactInfoPages.contains(contactName)) 
    {
        ui->stackedWidget->setCurrentWidget(ui->buffer);
        contacts.remove(contactName);
        contactInfoPages.remove(contactName);
        contactsButtons.remove(contactName);
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
        flashPages(2,contactName);
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
    }
    else
    {
        // 没有找到该按钮
        QMessageBox::warning(this, "错误！", "找不到联系人");
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
    }
}

void Directory::searchContact(const QString& contactName)
{
    if(contactInfoPages.contains(contactName))
        ui->stackedWidget->setCurrentWidget(contactInfoPages[contactName]);
    else
    {
        // 没有找到该按钮
        QMessageBox::warning(this, "错误！", "找不到联系人");
    }
}

void Directory::flashPages(int mode,const QString& contactName)
{
    switch (mode)
    {
    case 0: //添加联系人
    {
        AddContactDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) 
        {
            QMap<QString, QVariant> contactData = dialog.getContactInfo();
            QString name = contactData["name"].toString();
            contactData.remove("name");
            if (!contacts.contains(name))
            {
                // 将联系人的姓名和其他信息一起插入新的 QMap 中
                QMap<QString, QVariant> newContactInfo;
                newContactInfo.insert("name", name); // 将姓名作为键值对应的值插入新的 QMap 中
                newContactInfo.insert("phone", contactData["phone"]);
                newContactInfo.insert("email", contactData["email"]);
                newContactInfo.insert("birthday", contactData["birthday"]);

                // 使用联系人姓名作为键，新的 QMap 作为值插入到 contacts 中
                contacts.insert(name, newContactInfo);
                // Create a new contact button
                QPushButton* btn_contacts = new QPushButton(ui->scrollAreaWidgetContents);
                //添加按钮到按钮地图
                contactsButtons.insert(name, btn_contacts);
                QWidget* contactsInfo = new QWidget;
                btn_contacts->setFixedSize(514, btn_Height);
                btn_contacts->setText(name);
                btn_contacts->setStyleSheet("text-align:left;");
                btn_contacts->setObjectName(name);
                contactInfoPages.insert(name, contactsInfo);//添加新的信息页没有问题
                //qInfo()<<"/////" << contactInfoPages;
                ui->stackedWidget->addWidget(contactsInfo);
                //qInfo() << ui->contactsWidget;
                connect(btn_contacts, &QPushButton::clicked, this, [=]() {
                    ui->stackedWidget->setCurrentWidget(contactInfoPages[name]);
                    });

                // 设置详细信息页
                QPushButton* back = new QPushButton("←", contactsInfo);
                back->setFixedSize(70, 40);
                back->setStyleSheet(
                    "QPushButton{ background-color: transparent;color: #000000; "
                    "font-weight: lighter;"
                    "font-size: 48px"
                    "}"
                    "QPushButton:hover{color: #808080;}"
                );
                connect(back, &QPushButton::clicked, [this]() {
                    ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
                    });

                QLabel* headLabel = new QLabel(contactsInfo);
                QPixmap originalPixmap(":/Directory/image/contactIcon.png");
                QSize newSize(120, 120);
                QPixmap scaledHead = originalPixmap.scaled(newSize);
                headLabel->setPixmap(scaledHead);
                headLabel->setFixedSize(120, 120);
                headLabel->move(10, 70);

                QLabel* contactNameLabel = new QLabel(name, contactsInfo);
                contactNameLabel->setStyleSheet("QLabel{font-size: 54px;font-weight: bold}");
                contactNameLabel->setObjectName("nameLabel");
                contactNameLabel->move(0, 200);
                contactNameLabel->setFixedSize(540, 54);

                QLabel* contactInfoLabel1 = new QLabel("手机号码 | " + contactData["phone"].toString(), contactsInfo);
                contactInfoLabel1->setObjectName("phoneLabel");
                contactInfoLabel1->setStyleSheet("QLabel{font-size: 36px;border-top: 1px solid gray;}");
                contactInfoLabel1->move(0, contactInfoOffset + contactInfoSpace);
                contactInfoLabel1->setFixedSize(540, 36);

                QLabel* contactInfoLabel2 = new QLabel("邮箱 | " + contactData["email"].toString(), contactsInfo);
                contactInfoLabel2->setObjectName("emailLabel");
                contactInfoLabel2->setStyleSheet("QLabel{font-size: 36px;border-top: 1px solid gray;}");
                contactInfoLabel2->move(0, contactInfoLabel1->height() + contactInfoOffset + 2 * contactInfoSpace);
                contactInfoLabel2->setFixedSize(540, 36);

                QLabel* contactInfoLabel3 = new QLabel("生日 | " + contactData["birthday"].toString(), contactsInfo);
                contactInfoLabel3->setObjectName("birthdayLabel");
                contactInfoLabel3->setStyleSheet("QLabel{font-size: 36px;border-top: 1px solid gray;}");
                contactInfoLabel3->move(0, contactInfoLabel2->height() + contactInfoLabel1->height() + contactInfoOffset + 3 * contactInfoSpace);
                contactInfoLabel3->setFixedSize(540, 36);

                //更新联系人计数和按钮位置
                counts++;
                moveButtons();
                ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
            }
            else
            {
                QMessageBox::warning(this,"警告", "联系人已经存在！");
            }
            
        }
    }   
    break;

    case 1: //修改联系人信息
    {
            QString name = contactName;
            // 更新联系人信息页中的 QLabel
        if (contactInfoPages.contains(contactName)) 
        {
            QWidget* contactPage = contactInfoPages[contactName];

            // 查找联系人信息页中的 QLabel
            QLabel* contactNameLabel = contactPage->findChild<QLabel*>("nameLabel");
            QLabel* phoneLabel = contactPage->findChild<QLabel*>("phoneLabel");
            QLabel* emailLabel = contactPage->findChild<QLabel*>("emailLabel");
            QLabel* birthdayLabel = contactPage->findChild<QLabel*>("birthdayLabel");

            // 更新 QLabel 的文本内容
            QMap<QString, QVariant> temp = contacts[contactName].toMap();
            if (contactNameLabel) contactNameLabel->setText(temp["name"].toString());
            if (phoneLabel) phoneLabel->setText("手机号码 | " + temp["phone"].toString());
            if (emailLabel) emailLabel->setText("邮箱 | " + temp["email"].toString());
            if (birthdayLabel) birthdayLabel->setText("生日 | " + temp["birthday"].toString());
        }
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
    }
    break;

    case 2:
    {
         // 在 scrollAreaWidgetContents 中查找名称为 contactName 的按钮
        QPushButton* button = ui->scrollAreaWidgetContents->findChild<QPushButton*>(contactName);
        if (button) 
        {
            delete button;
            moveButtons();
            ui->scrollAreaWidgetContents->setFixedHeight(ui->scrollAreaWidgetContents->height() - 100);
        } 
        else 
        {
        // 没有找到该按钮
        QMessageBox::warning(this, "错误！", "找不到联系人");
        }
    }
    break;

    default:
        break;
    }
    ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
}

void Directory::addContact()
{
    ui->stackedWidget->setCurrentWidget(ui->buffer);
    flashPages(0,"");
    ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
}
void Directory::inputNameDialog()
{
    preWidget = ui->stackedWidget->currentWidget();
    ui->stackedWidget->setCurrentWidget(ui->buffer);
    // 创建一个对话框以输入联系人名字
    QDialog dialog(this);
    QLineEdit* nameLineEdit = new QLineEdit(&dialog);
    QPushButton* okButton = new QPushButton("确定", &dialog);
    QPushButton* cancelButton = new QPushButton("取消", &dialog);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel("输入姓名:"));
    layout->addWidget(nameLineEdit);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) 
    {
        QString selectedContact = nameLineEdit->text();
        // 调用修改联系人信息的函数
        switch (inputDialogMode)
        {
        case 1:
        {
            modifyContact(selectedContact);
        }break;
        case 2:
        {
            deleteContact(selectedContact);
        }break;
        case 3:
        {
            ui->stackedWidget->setCurrentWidget(preWidget);
            searchContact(selectedContact);
        }break;
        default:
            break;
        }
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->contactsWidget);
    }
}


