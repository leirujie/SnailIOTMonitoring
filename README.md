#### 介绍

QT/蜗牛物联网监控平台

#### 软件架构

软件架构说明

```
SnailIOTMonitoring：
├──
    ├── alertManager  // 告警规则管理模块
        ├── alertManager.h
        ├── alertManager.cpp
        ├── alertManager.ui
    ├── dataMonitor  // 数据监测管理模块
        ├── historytimedata.h
        ├── historytimedata.cpp
        ├── historytimedata.ui
        ├── realtimedata.h
        ├── realtimedata.cpp
        ├── realtimedata.ui
    ├── deviceManager  // 设备管理模块
        ├── adddevicelocationgroupdialog.h
        ├── adddevicelocationgroupdialog.cpp
        ├── adddevicetypegroupdialog.h
        ├── adddevicetypegroupdialog.cpp
        ├── admindevicemanager.h
        ├── admindevicemanager.cpp
        ├── admindevicemanager.ui
        ├── devicedialog.h
        ├── devicedialog.cpp
        ├── userdevicemanager.h
        ├── userdevicemanager.cpp
        ├── userdevicemanager.ui
    ├── FrameWork  // 框架管理模块
        ├── centerwidget.h
        ├── centerwidget.cpp
        ├── centerwidget.ui
        ├── statusbar.h
        ├── statusbar.cpp
        ├── statusbar.ui
        ├── titlebar.h
        ├── titlebar.cpp
        ├── titlebar.ui
    ├── LogManager  // 日志管理模块
        ├── logdata.h
        ├── logdata.cpp
        ├── logdata.ui
        ├── logmanager.h
        ├── logmanager.cpp
    ├── userManager  // 用户管理模块
        ├── adduserdialog.h
        ├── adduserdialog.cpp
        ├── adduserdialog.ui
        ├── adminmenu.h
        ├── adminmenu.cpp
        ├── adminmenu.ui
        ├── adminmenuframe.h
        ├── adminmenuframe.cpp
        ├── adminmenuframe.ui
        ├── adminupdateuserinfodialog.h
        ├── adminupdateuserinfodialog.cpp
        ├── adminupdateuserinfodialog.ui
        ├── forgotpassworddialog.h
        ├── forgotpassworddialog.cpp
        ├── forgotpassworddialog.ui
        ├── global.h
        ├── global.cpp
        ├── launchscrren.h
        ├── launchscrren.cpp
        ├── launchscrren.ui
        ├── loginpage.h
        ├── loginpage.cpp
        ├── loginpage.ui
        ├── registerpage.h
        ├── registerpage.cpp
        ├── registerpage.ui
        ├── UserDatabase.h
        ├── UserDatabase.cpp
        ├── usermenu.h
        ├── usermenu.cpp
        ├── usermenu.ui
        ├── validator.h
        ├── validator.cpp
    ├── widget.h
    ├── widget.cpp
    ├── widget.ui
    ├── main.cpp // 程序运行主函数
    ├── image.qrc // 配置资源文件
    ├── README.md
```

#### 项目背景

  随着物联网技术的发展，越来越多的物联网设备被应用到各个领域。为了方便对这些设备进行实时监控和管理，特开发一款基于 Qt6 的蜗牛物联网监控平台。该平台将提供设备的实时数据展示、历史数据分析、告警通知和设备管理等功能。物联网技术的发展带来了海量设备数据的产生和多样化应用的需求。及时监控和管理这些设备不仅能够提升设备的运行效率，还能保障其正常工作，减少因为设备故障带来的损失。因此，开发一款高效、便捷、安全的监控平台尤为重要!

#### 使用说明

1.  程序运行时会初始化admin为管理员账号，密码为<admin123>，登录后可以进行用户管理、设备管理、数据监测、告警管理、日志管理等操作.
2.  普通用户账号密码需要自己注册，登录后可以查看设备信息、数据信息、用户信息和个人日志等操作.
