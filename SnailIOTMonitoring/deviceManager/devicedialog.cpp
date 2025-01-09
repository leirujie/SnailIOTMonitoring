#include "devicedialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>

DeviceDialog::DeviceDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("设备信息");

    nameEdit = new QLineEdit(this);
    typeComboBox = new QComboBox(this);
    typeComboBox->addItem("温度检测器");
    typeComboBox->addItem("湿度检测器");
    typeComboBox->addItem("光照检测器");

    locationComboBox = new QComboBox(this);
    locationComboBox->addItem("办公室");
    locationComboBox->addItem("厂房");
    manufacturerEdit = new QLineEdit(this);
    modelEdit = new QLineEdit(this);
    installDateEdit = new QDateEdit(this);
    installDateEdit->setCalendarPopup(true);
    installDateEdit->setDate(QDate::currentDate());

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("名称:", nameEdit);
    formLayout->addRow("类型:", typeComboBox);
    formLayout->addRow("位置:", locationComboBox);
    formLayout->addRow("制造商:", manufacturerEdit);
    formLayout->addRow("型号:", modelEdit);
    formLayout->addRow("安装日期:", installDateEdit);

    saveButton = new QPushButton("保存", this);
    cancelButton = new QPushButton("取消", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(mainLayout);
    resize(400, 300);
}

void DeviceDialog::setDeviceInfo(const QList<QString> &deviceInfo)
{
    if (deviceInfo.size() != 6)
        return;

    nameEdit->setText(deviceInfo[0]);
    //typeEdit->setText(deviceInfo[1]);
      typeComboBox->setCurrentText(deviceInfo[1]); // 设置下拉框的选中项
    //locationEdit->setText(deviceInfo[2]);
       locationComboBox->setCurrentText(deviceInfo[2]); // 设置设备位置
    manufacturerEdit->setText(deviceInfo[3]);
    modelEdit->setText(deviceInfo[4]);
    installDateEdit->setDate(QDate::fromString(deviceInfo[5], "yyyy-MM-dd"));
}

QList<QString> DeviceDialog::getDeviceInfo() const
{
    return {
        nameEdit->text(),
       // typeEdit->text(),
       typeComboBox->currentText(),
        //locationEdit->text(),
         locationComboBox->currentText(), // 获取选择的设备位置
        manufacturerEdit->text(),
        modelEdit->text(),
        installDateEdit->date().toString("yyyy-MM-dd")
    };
}

void DeviceDialog::onSave()
{

    if (nameEdit->text().isEmpty()) {
           QMessageBox::warning(this, "警告", "设备名称不能为空！");
           return;
       }

       if (typeComboBox->currentText().isEmpty()) {
           QMessageBox::warning(this, "警告", "设备类型不能为空！");
           return;
       }


       // 检查设备位置是否合法
           if (locationComboBox->currentText().isEmpty()) {
               QMessageBox::warning(this, "警告", "设备位置不能为空！");
               return;
           }

       // 如果通过了上述验证，保存设备信息
       accept();
}

