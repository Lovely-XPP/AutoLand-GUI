#include <MainWindow.h>
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    // 设置应用程序的 DPI 适应性，这可以使应用程序在高 DPI 屏幕上看起来更清晰
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    QMainWindow *main_win;
    AutoLandMainWindow *autoland_gui = new AutoLandMainWindow(main_win);
    return autoland_gui->exec(); // 主事件循环
}