#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QIcon>
#include <QLineEdit>
#include <QDateTime>
#include <QComboBox>
#include <QProgressDialog>
#include <QCheckBox>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QHeaderView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileDialog>
#include <string>
#include <vector>
#include <iostream>
#include <time.h>
#include <iostream>
#include <sstream>
#include <util.h>
#include <myopengl.h>
#include <thread>
#include <ctime>

class AutoLandMainWindow : public QDialog
{
    public:
        AutoLandMainWindow(QWidget *parent_widget)
        {
            setup();
        }

    private:
        // 设置
        std::string version = "V1.0.1";
        int ip_test_result = 0;

        // 组件初始化
        // 标签类
        QLabel *title_label = new QLabel(this);
        QLabel *send_ip_addr_label = new QLabel(this);
        QLabel *send_ip_port_label = new QLabel(this);
        QLabel *recive_ip_addr_label = new QLabel(this);
        QLabel *recive_ip_port_label = new QLabel(this);
        QLabel *estimator_label = new QLabel(this);
        QLabel *controller_label = new QLabel(this);
        QLabel *export_label = new QLabel(this);
        // 输入框
        QLineEdit *send_ip_addr_edit = new QLineEdit(this);
        QLineEdit *send_ip_port_edit = new QLineEdit(this);
        QLineEdit *recive_ip_addr_edit = new QLineEdit(this);
        QLineEdit *recive_ip_port_edit = new QLineEdit(this);
        QPlainTextEdit *info_edit = new QPlainTextEdit(this);
        // 信息框
        QMessageBox *msg_box = new QMessageBox(this);
        // 按钮
        QPushButton *send_ip_test_button = new QPushButton(this);
        QPushButton *recive_ip_test_button = new QPushButton(this);
        QPushButton *start_button = new QPushButton(this);
        QPushButton *clear_fig_button = new QPushButton(this);
        QPushButton *reset_fig_button = new QPushButton(this);
        // 下拉菜单
        QComboBox *estimator_combobox = new QComboBox(this);
        QComboBox *controller_combobox = new QComboBox(this);
        // OpenGL
        myOpenGL *opengl = new myOpenGL(this);
        // 复选框
        QCheckBox *fig_export_checkbox = new QCheckBox(this);
        QCheckBox *data_export_checkbox = new QCheckBox(this);
        QCheckBox *export_checkbox = new QCheckBox(this);

        void setup()
        {
            // 初始化
            QIcon *icon = new QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            this->setWindowIcon(*icon);
            this->setFixedSize(1080, 500);
            this->setWindowTitle(("AutoLand GUI [Version: " + version + "]").c_str());
            msg_box->setStyleSheet("QLineEdit{min-width: 300px;}");

            // 初始化组件
            // 标题设置
            title_label->setText("Auto Land GUI");
            title_label->setStyleSheet("font-size: 28pt; font-weight: bold");
            title_label->setAlignment(Qt::AlignCenter);
            title_label->setMaximumHeight(100);

            // ip设置
            recive_ip_addr_label->setText("从服务器接收数据的 IP 地址：");
            recive_ip_port_label->setText("端口：");
            recive_ip_test_button->setText("测试连通性");
            send_ip_addr_label->setText("发送数据到服务器的 IP 地址：");
            send_ip_port_label->setText("端口：");
            send_ip_test_button->setText("测试连通性");

            // 算法设置
            estimator_label->setText("位姿估计算法：");
            estimator_label->setMaximumWidth(100);
            controller_label->setText("飞行控制算法：");
            controller_label->setMaximumWidth(100);
            estimator_combobox->addItem("PoseNet");
            controller_combobox->addItem("PID");
            start_button->setText("开始运行");

            // 输出信息设置
            info_edit->appendPlainText("*** 欢迎使用Auto Land GUI ***");
            info_edit->setReadOnly(true);

            // 导出数据模块
            fig_export_checkbox->setText("导出图片");
            fig_export_checkbox->setChecked(true);
            data_export_checkbox->setText("导出轨迹");
            data_export_checkbox->setChecked(true);
            export_label->setText("数据导出设置：");
            export_checkbox->setText("开启数据保存");
            export_checkbox->setChecked(true);

            // 组件排版
            // 排版初始化
            QVBoxLayout *vbox = new QVBoxLayout();
            QVBoxLayout *vbox_1 = new QVBoxLayout();
            QHBoxLayout *hbox_1_1 = new QHBoxLayout();
            QHBoxLayout *hbox_1_2 = new QHBoxLayout();
            QHBoxLayout *hbox_1_3 = new QHBoxLayout();
            QVBoxLayout *vbox_2 = new QVBoxLayout();
            QHBoxLayout *hbox_2_1 = new QHBoxLayout();
            QHBoxLayout *hbox_2_2 = new QHBoxLayout();
            QHBoxLayout *hbox_2_3 = new QHBoxLayout();
            QVBoxLayout *vbox_3 = new QVBoxLayout();
            QHBoxLayout *hbox_export_setting = new QHBoxLayout();
            QHBoxLayout *hbox_view_setting = new QHBoxLayout();
            QVBoxLayout *vbox_4 = new QVBoxLayout();
            QHBoxLayout *hbox_setting = new QHBoxLayout();
            QHBoxLayout *hbox_bottom = new QHBoxLayout();
            // 标题行
            vbox->addWidget(title_label);
            // 设置
            hbox_1_1->addWidget(recive_ip_addr_label);
            hbox_1_1->addWidget(recive_ip_addr_edit);
            hbox_1_2->addWidget(send_ip_addr_label);
            hbox_1_2->addWidget(send_ip_addr_edit);
            hbox_1_3->addWidget(estimator_label);
            hbox_1_3->addWidget(estimator_combobox);
            vbox_1->addLayout(hbox_1_1);
            vbox_1->addLayout(hbox_1_2);
            vbox_1->addLayout(hbox_1_3);
            hbox_2_1->addWidget(recive_ip_port_label);
            hbox_2_1->addWidget(recive_ip_port_edit);
            hbox_2_2->addWidget(send_ip_port_label);
            hbox_2_2->addWidget(send_ip_port_edit);
            hbox_2_3->addWidget(controller_label);
            hbox_2_3->addWidget(controller_combobox);
            vbox_2->addLayout(hbox_2_1);
            vbox_2->addLayout(hbox_2_2);
            vbox_2->addLayout(hbox_2_3);
            vbox_3->addWidget(recive_ip_test_button);
            vbox_3->addWidget(send_ip_test_button);
            vbox_3->addWidget(start_button);
            hbox_setting->addLayout(vbox_1, 2);
            hbox_setting->addLayout(vbox_2, 1);
            hbox_setting->addLayout(vbox_3, 1);
            vbox->addLayout(hbox_setting);
            // 导出设置
            hbox_export_setting->addWidget(export_label);
            hbox_export_setting->addWidget(export_checkbox);
            hbox_export_setting->addWidget(fig_export_checkbox);
            hbox_export_setting->addWidget(data_export_checkbox);
            vbox->addLayout(hbox_export_setting);
            // 命令框以及绘图
            hbox_bottom->addWidget(info_edit, 1);
            hbox_bottom->addWidget(opengl, 1);
            vbox->addLayout(hbox_bottom);
            // 设置布局
            this->setLayout(vbox);

            // 信号-槽 连接
            QObject::connect(recive_ip_test_button, &QPushButton::clicked, this, &AutoLandMainWindow::recive_ip_test_slot);
            QObject::connect(send_ip_test_button, &QPushButton::clicked, this, &AutoLandMainWindow::send_ip_test_slot);
        }

        // 发送ip槽函数
        void send_ip_test_slot()
        {
            info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性...");
            QString ip = send_ip_addr_edit->text();
            QString port = send_ip_port_edit->text();
            // 显示进度条
            QProgressDialog *progress_win = new QProgressDialog(this);
            progress_win->setAttribute(Qt::WA_DeleteOnClose);
            progress_win->setMinimumWidth(600);                // 设置最小宽度
            progress_win->setWindowModality(Qt::NonModal);     // 非模态，其它窗口正常交互  Qt::WindowModal 模态
            progress_win->setMinimumDuration(0);               // 等待0秒后显示
            progress_win->setWindowTitle(tr("IP 连通性测试")); // 标题名
            progress_win->setLabelText(tr("IP 连通性测试"));   // 标签的
            progress_win->setMinimum(0);
            progress_win->setMaximum(0);
            progress_win->open();
            // 单独线程测试
            ip_test_result = -1;
            std::thread ip_test_thread(&AutoLandMainWindow::ip_test_slot, this, ip.toStdString(), port.toStdString());
            ip_test_thread.detach();
            usleep(500000);
            while (ip_test_result == -1)
            {
                usleep(200000);
            }
            progress_win->close();
            // 测试结果显示
            msg_box->setIcon(QMessageBox::Icon::Critical);
            msg_box->setText("Error");
            msg_box->setWindowTitle("Error");
            msg_box->setInformativeText("连接失败!");
            switch (ip_test_result)
            {
            case TCPING_ERROR:
                msg_box->setText("连接失败：输入格式错误");
                msg_box->setInformativeText("请检查输入ip格式是否正确！");
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 失败");
                break;
            case TCPING_OPEN:
                msg_box->setIcon(QMessageBox::Icon::Information);
                msg_box->setText("连接成功");
                msg_box->setWindowTitle("Info");
                msg_box->setInformativeText("成功连接至" + ip + ":" + port);
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 成功");
                break;
            case TCPING_CLOSED:
                msg_box->setText("连接失败：目标端口关闭");
                msg_box->setInformativeText("连接目标主机ip成功，请检查端口设置是否正确！");
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 失败");
                break;
            case TCPING_TIMEOUT:
                msg_box->setText("连接失败：目标主机超时");
                msg_box->setInformativeText("连接目标主机ip失败，请检查ip是否输入正确，通信是否设置正确！");
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 失败");
                break;
            }
            msg_box->exec();
        }

        void recive_ip_test_slot()
        {
            info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性...");
            QString ip = recive_ip_addr_edit->text();
            QString port = recive_ip_port_edit->text();
            // 显示进度条
            QProgressDialog *progress_win = new QProgressDialog(this);
            progress_win->setAttribute(Qt::WA_DeleteOnClose);
            progress_win->setMinimumWidth(600);                // 设置最小宽度
            progress_win->setWindowModality(Qt::NonModal);     // 非模态，其它窗口正常交互  Qt::WindowModal 模态
            progress_win->setMinimumDuration(0);               // 等待0秒后显示
            progress_win->setWindowTitle(tr("IP 连通性测试")); // 标题名
            progress_win->setLabelText(tr("IP 连通性测试"));   // 标签的
            progress_win->setMinimum(0);
            progress_win->setMaximum(0);
            progress_win->open();
            // 单独线程测试
            ip_test_result = -1;
            std::thread ip_test_thread(&AutoLandMainWindow::ip_test_slot, this, ip.toStdString(), port.toStdString());
            ip_test_thread.detach();
            usleep(500000);
            while (ip_test_result == -1)
            {
                usleep(200000);
            }
            progress_win->close();
            // 测试结果显示
            msg_box->setIcon(QMessageBox::Icon::Critical);
            msg_box->setText("Error");
            msg_box->setWindowTitle("Error");
            msg_box->setInformativeText("连接失败!");
            switch (ip_test_result)
            {
            case TCPING_ERROR:
                msg_box->setText("连接失败：输入格式错误");
                msg_box->setInformativeText("请检查输入ip格式是否正确！");
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 失败");
                break;
            case TCPING_OPEN:
                msg_box->setIcon(QMessageBox::Icon::Information);
                msg_box->setText("连接成功");
                msg_box->setWindowTitle("Info");
                msg_box->setInformativeText("成功连接至" + ip + ":" + port);
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 成功");
                break;
            case TCPING_CLOSED:
                msg_box->setText("连接失败：目标端口关闭");
                msg_box->setInformativeText("连接目标主机ip成功，请检查端口设置是否正确！");
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 失败");
                break;
            case TCPING_TIMEOUT:
                msg_box->setText("连接失败：目标主机超时");
                msg_box->setInformativeText("连接目标主机ip失败，请检查ip是否输入正确，通信是否设置正确！");
                info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性 - 失败");
                break;
            }
            msg_box->exec();
        }

        // 工具函数：测试ip是否可达
        void ip_test_slot(const std::string ip, const std::string port)
        {
            ip_test_result = check_connection(ip, port, 0, 200);
        }

        QString get_current_time()
        {
            return "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "] ";
        }
};

#endif