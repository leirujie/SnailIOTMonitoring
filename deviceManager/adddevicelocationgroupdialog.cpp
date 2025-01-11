#include "adddevicelocationgroupdialog.h"

AddDeviceLocationGroupDialog::AddDeviceLocationGroupDialog(QWidget *parent, const QString &oldGroupName)
    : QDialog(parent), m_oldGroupName(oldGroupName)
{
    setupUi();
}

QString AddDeviceLocationGroupDialog::getGroupName() const {
    return m_groupNameEdit->text();
}

void AddDeviceLocationGroupDialog::setupUi()
{
    m_groupNameEdit = new QLineEdit(this);
    m_groupNameEdit->setPlaceholderText("输入设备位置分组名称");

    QLabel *label = new QLabel("设备位置 分组名称", this);
    QPushButton *okButton = new QPushButton("确定", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(label);
    layout->addWidget(m_groupNameEdit);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    setLayout(layout);

    if (!m_oldGroupName.isEmpty()) {
        m_groupNameEdit->setText(m_oldGroupName);  // 如果是修改分组，默认显示原分组名
    }

    connect(okButton, &QPushButton::clicked, this, &AddDeviceLocationGroupDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &AddDeviceLocationGroupDialog::reject);
}
