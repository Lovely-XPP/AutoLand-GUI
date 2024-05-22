#ifndef DATAREPLAYWINDOW_HPP
#define DATAREPLAYWINDOW_HPP
#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDoubleValidator>
#include <QFileDialog>

#include <vector>

class DataReplayWindow : public QDialog
{
    public:
        DataReplayWindow(QWidget *parent_widget)
        {
            setup();
        }

    private:
        void setup()
        {
            this->setWindowTitle("Data Replay Setting");
            this->setFixedSize(800, 500);
            this->setAttribute(Qt::WA_DeleteOnClose);
        }
};

#endif