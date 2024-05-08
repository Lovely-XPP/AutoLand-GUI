#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QIcon>
#include <QLineEdit>
#include <QDateTime>
#include <QComboBox>
#include <QProgressDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <string>
#include <vector>
#include <time.h>
#include <util.h>
#include <myopengl.h>
#include <thread>
#include <ctime>
#include <opencv2/highgui/highgui.hpp>

class AutoLandMainWindow : public QDialog
{
    Q_OBJECT

    public:
        AutoLandMainWindow(QWidget *parent_widget)
        {
            setup();
        }
    
    signals:
        void ip_test_terminate_signal();

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
        QLabel *image_label = new QLabel(this);
        QLabel *save_data_path_label = new QLabel(this);
        // 输入框
        QLineEdit *send_ip_addr_edit = new QLineEdit(this);
        QLineEdit *send_ip_port_edit = new QLineEdit(this);
        QLineEdit *recive_ip_addr_edit = new QLineEdit(this);
        QLineEdit *recive_ip_port_edit = new QLineEdit(this);
        QLineEdit *save_data_path_edit = new QLineEdit(this);
        QPlainTextEdit *info_edit = new QPlainTextEdit(this);
        // 信息框
        QMessageBox *msg_box = new QMessageBox(this);
        QProgressDialog *progress_win;
        // 按钮
        QPushButton *send_ip_test_button = new QPushButton(this);
        QPushButton *recive_ip_test_button = new QPushButton(this);
        QPushButton *start_button = new QPushButton(this);
        QPushButton *clear_fig_button = new QPushButton(this);
        QPushButton *reset_fig_button = new QPushButton(this);
        QPushButton *show_real_fig_button = new QPushButton(this);
        QPushButton *save_data_path_button = new QPushButton(this);
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
            this->setFixedSize(1080, 800);
            this->setWindowTitle(("AutoLand GUI [Version: " + version + "]").c_str());

            // 初始化组件
            // 信息框设置
            msg_box->setStyleSheet("QLineEdit{min-width: 300px;}");

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
            fig_export_checkbox->setText("导出图片数据");
            fig_export_checkbox->setChecked(true);
            data_export_checkbox->setText("导出轨迹数据");
            data_export_checkbox->setChecked(true);
            export_label->setText("数据导出设置：");
            export_checkbox->setText("开启数据保存");
            export_checkbox->setChecked(true);
            save_data_path_label->setText("数据保存路径：");
            save_data_path_button->setText("选择保存路径");
            save_data_path_edit->setReadOnly(true);

            // 轨迹绘图设置
            show_real_fig_button->setText("显示真实轨迹");
            clear_fig_button->setText("清除已有轨迹");
            reset_fig_button->setText("重置默认视图");

            // 图像显示设置
            image_label->setAlignment(Qt::AlignCenter);

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
            QHBoxLayout *hbox_save_path = new QHBoxLayout();
            QHBoxLayout *hbox_export_setting = new QHBoxLayout();
            QHBoxLayout *hbox_view_setting = new QHBoxLayout();
            QHBoxLayout *hbox_4 = new QHBoxLayout();
            QHBoxLayout *hbox_setting = new QHBoxLayout();
            QVBoxLayout *vbox_data_show = new QVBoxLayout();
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
            // 保存路径设置
            hbox_save_path->addWidget(save_data_path_label);
            hbox_save_path->addWidget(save_data_path_edit);
            hbox_save_path->addWidget(save_data_path_button);
            vbox->addLayout(hbox_save_path);
            // 绘图设置
            hbox_view_setting->addWidget(reset_fig_button);
            hbox_view_setting->addWidget(clear_fig_button);
            hbox_view_setting->addWidget(show_real_fig_button);
            hbox_4->addLayout(hbox_view_setting);
            vbox->addLayout(hbox_4);
            // 导出设置
            hbox_export_setting->addWidget(export_label);
            hbox_export_setting->addWidget(export_checkbox);
            hbox_export_setting->addWidget(fig_export_checkbox);
            hbox_export_setting->addWidget(data_export_checkbox);
            hbox_4->addLayout(hbox_export_setting);
            // 命令框以及绘图
            vbox_data_show->addWidget(opengl, 1);
            vbox_data_show->addWidget(image_label, 1);
            hbox_bottom->addLayout(vbox_data_show, 1);
            hbox_bottom->addWidget(info_edit, 1);
            vbox->addLayout(hbox_bottom);
            // 设置布局
            this->setLayout(vbox);

            // 信号-槽 连接
            QObject::connect(recive_ip_test_button, &QPushButton::clicked, this, &AutoLandMainWindow::recive_ip_test_slot);
            QObject::connect(send_ip_test_button, &QPushButton::clicked, this, &AutoLandMainWindow::send_ip_test_slot);
            QObject::connect(start_button, &QPushButton::clicked, this, &AutoLandMainWindow::run_slot);
            QObject::connect(export_checkbox, &QCheckBox::stateChanged, this, &AutoLandMainWindow::export_checkbox_slot);
            QObject::connect(show_real_fig_button, &QPushButton::clicked, this, &AutoLandMainWindow::show_real_fig_button_clicked_slot);
            QObject::connect(clear_fig_button, &QPushButton::clicked, this, &AutoLandMainWindow::clear_fig_button_clicked_slot);
            QObject::connect(reset_fig_button, &QPushButton::clicked, this, &AutoLandMainWindow::reset_fig_button_clicked_slot);
            QObject::connect(save_data_path_button, &QPushButton::clicked, this, &AutoLandMainWindow::save_data_path_button_clicked_slot);
            QObject::connect(this, &AutoLandMainWindow::ip_test_terminate_signal, this, &AutoLandMainWindow::ip_test_terminate_slot);
        }

        // 槽函数：发送ip服务器测试
        void send_ip_test_slot()
        {
            info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性...");
            QString ip = send_ip_addr_edit->text();
            QString port = send_ip_port_edit->text();
            // 单独线程测试
            ip_test_result = -1;
            std::thread ip_test_thread(&AutoLandMainWindow::ip_test_slot, this, ip.toStdString(), port.toStdString());
            ip_test_thread.detach();
            // 进度条设置
            progress_win = new QProgressDialog(this);
            progress_win->setAttribute(Qt::WA_DeleteOnClose);
            progress_win->setMinimumWidth(600);                // 设置最小宽度
            progress_win->setWindowModality(Qt::WindowModal);  // 模态，不允许其它窗口交互  Qt::WindowModal 模态
            progress_win->setMinimumDuration(0);               // 等待0秒后显示
            progress_win->setWindowTitle(tr("IP 连通性测试")); // 标题名
            progress_win->setLabelText(tr("IP 连通性测试"));   // 标签的
            progress_win->setMinimum(0);
            progress_win->setMaximum(0);
            progress_win->setCancelButton(0);
            // 显示进度条
            progress_win->open();
        }

        // 槽函数：接收ip服务器测试
        void recive_ip_test_slot()
        {
            info_edit->appendPlainText(get_current_time() + "测试数据发送到对应服务器的连通性...");
            QString ip = recive_ip_addr_edit->text();
            QString port = recive_ip_port_edit->text();
            // 单独线程测试
            ip_test_result = -1;
            std::thread ip_test_thread(&AutoLandMainWindow::ip_test_slot, this, ip.toStdString(), port.toStdString());
            ip_test_thread.detach();
            // 进度条设置
            progress_win = new QProgressDialog(this);
            progress_win->setAttribute(Qt::WA_DeleteOnClose);
            progress_win->setMinimumWidth(600);                // 设置最小宽度
            progress_win->setWindowModality(Qt::WindowModal);  // 模态，不允许其它窗口交互  Qt::WindowModal 模态
            progress_win->setMinimumDuration(0);               // 等待0秒后显示
            progress_win->setWindowTitle(tr("IP 连通性测试")); // 标题名
            progress_win->setLabelText(tr("IP 连通性测试"));   // 标签的
            progress_win->setMinimum(0);
            progress_win->setMaximum(0);
            progress_win->setCancelButton(0);
            // 显示进度条
            progress_win->open();
        }

        // 槽函数：执行结果结束
        void ip_test_terminate_slot()
        {
            // 关闭进度条
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
                    msg_box->setInformativeText("成功连接!");
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

        // 槽函数：运行执行函数
        void run_slot()
        {
            // 开始运行后端指令
            if (start_button->text() == "开始运行")
            {
                start_button->setText("停止运行");
            }
            else // 停止运行后端指令
            {
                start_button->setText("开始运行");
            }
        }

        // 槽函数：勾选 / 取消勾选保存数据
        void export_checkbox_slot()
        {
            // 勾选上执行后端指令
            if (export_checkbox->isChecked())
            {
                fig_export_checkbox->setEnabled(true);
                fig_export_checkbox->setChecked(true);
                data_export_checkbox->setEnabled(true);
                data_export_checkbox->setChecked(true);
                save_data_path_button->setEnabled(true);
                save_data_path_edit->setEnabled(true);
            }
            else // 取消勾选执行后端指令
            {
                fig_export_checkbox->setEnabled(false);
                fig_export_checkbox->setChecked(false);
                data_export_checkbox->setEnabled(false);
                data_export_checkbox->setChecked(false);
                save_data_path_button->setEnabled(false);
                save_data_path_edit->setEnabled(false);
            }
        }

        // 槽函数：按下显示/隐藏真实轨迹按钮
        void show_real_fig_button_clicked_slot()
        {
            // 显示真实轨迹后端指令
            if (show_real_fig_button->text() == "显示真实轨迹")
            {
                show_real_fig_button->setText("隐藏真实轨迹");
            }
            else // 隐藏真实轨迹后端指令
            {
                show_real_fig_button->setText("显示真实轨迹");
            }
        }

        // 槽函数：按下清除已有轨迹按钮
        void clear_fig_button_clicked_slot()
        {
            
        }

        // 槽函数：按下重置默认视图按钮
        void reset_fig_button_clicked_slot()
        {
            
        }

        // 槽函数：按下选择保存路径按钮
        void save_data_path_button_clicked_slot()
        {
            char *cwd = getenv("HOME");
            QString qfoldername = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), cwd, QFileDialog::ShowDirsOnly);
            save_data_path_edit->setText(qfoldername);
        }
        
        // 槽函数：更新图片
        void update_image_slot(cv::Mat &img)
        {
            QImage qimg = cvMat2QImage(img);
            QPixmap temp_pixmap = QPixmap::fromImage(qimg);
            QPixmap fit_pixmap = temp_pixmap.scaled(image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            image_label->setPixmap(fit_pixmap);
        }

        // 工具函数：测试ip是否可达
        void ip_test_slot(const std::string ip, const std::string port)
        {
            sleep(3);
            ip_test_result = check_connection(ip, port, 0, 200);
            emit ip_test_terminate_signal();
        }

        // 工具函数：获取当前时间
        QString get_current_time()
        {
            return "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "] ";
        }

        // 工具函数：将OpenCV图像转换为QImage
        QImage cvMat2QImage(const cv::Mat &mat) // Mat转换成QImage
        {
            // 注意,使用opencv Mat的buf给QImage赋值时,务必带入参数mat.step;因为step是计算生成的每一行元素的字节数,它是4字节对齐的;
            // 否则如果输入图片的分辨率宽度不是4的整数倍,那么QImage会出现显示错乱的问题.
            if (mat.type() == CV_8UC1) // 1通道
            {
                const uchar *pSrc = (const uchar *)mat.data;
                QImage image = QImage(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
                return image;
            }
            else if (mat.type() == CV_8UC3) // 3通道
            {
                const uchar *pSrc = (const uchar *)mat.data;
                QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888); // 格式R,G,B,对应0,1,2
                return image.rgbSwapped();                                               // rgbSwapped是为了将BGR格式转换为RGB格式
            }
            else if (mat.type() == CV_8UC4) // 4通道
            {
                const uchar *pSrc = (const uchar *)mat.data;
                // Create QImage with same dimensions as input Mat
                QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32); // 格式B,G,R,A对应0,1,2,3
                return image.copy();
            }
            else if (mat.type() == CV_16SC3)
            {
                cv::Mat normalize_mat;
                normalize(mat, normalize_mat, 0, 255, cv::NORM_MINMAX, -1);
                normalize_mat.convertTo(normalize_mat, CV_8U);
                const uchar *pSrc = (const uchar *)normalize_mat.data;
                // Create QImage with same dimensions as input Mat
                QImage image(pSrc, normalize_mat.cols, normalize_mat.rows, normalize_mat.step, QImage::Format_RGB888);
                return image.rgbSwapped();
            }
            else
            {
                return QImage();
            }
        }
};

#endif