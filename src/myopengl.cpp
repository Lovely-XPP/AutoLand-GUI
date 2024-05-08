#include "myopengl.h"
#include <QDebug>
#include <QTimer>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <moc_myopengl.cpp>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#define GL_SILENCE_DEPRECATION 1
/*************************************************************************************************
 * receive from X-Plane
 * *******************************************************************************************************/
/*
struct Flight_Data
{
    float air_speed = 0;                          // 空速 Va
    float angular_velocity[3] = { '0','0','0' };  // 角速率 P Q R
    float attitude_angle[3] = { '0','0','0' };    // 欧拉态角 pitch(theta) roll(phi) heading(psi)
    float flow_angle[2] = { '0','0' };            // 气流角 AOA(alpha) side-slip(beta)
    float position[3] = { '0','0','0' };          // 位置 lon lat altitude

};
Flight_Data Flight_Data_this_time;                // 存储解析后的飞行数据的全局结构体，用于线程共享（只读）
Flight_Data decode_frame(char* udp_raw_data);*/

/*****************************************8
 * receive from MATLAB
 *****************************************/
struct Flight_Data
{
    float attitude[3] = {'0', '0', '0'}; // 欧拉角 pitch(theta),y    roll(phi),x    heading(psi),z
    float position[3] = {'0', '0', '0'}; // 位置 x y z
};
Flight_Data Flight_Data_; // 申明全局结构体Flight_Data_
Flight_Data decode(char *buff);

myOpenGL::myOpenGL(QWidget *parent)
{
    // 初始化该class中的private参数
    show_curveReal = true;
    show_curveCalcul = true;
    // stepValue = 20;

    zscale = 250;
    verticalAngle = 7.6f;
    horizonAngle = 12.0F;
    center.setX(0);
    center.setZ(0);
    center.setY(0);
    /*the coordinate blew need to coordinated with the input flight*/
    eye.setX(0.f);
    eye.setY(0.f);
    eye.setZ(100.f);

    upside.setX(1.f);
    upside.setY(1.f);
    upside.setZ(1.f);
    upside.normalize();

    TempTranslateVec.setX(0.0);
    TempTranslateVec.setY(0.0);
    TempTranslateVec.setZ(0.0);

    viewport.setX(0.f);
    viewport.setY(0.f);

    //**********************************************************************
    // from Windows
    //m_receiver = new QUdpSocket(this);
    //m_receiver->bind(10086, QUdpSocket::ShareAddress);
    //connect(m_receiver, SIGNAL(readyRead()), this, SLOT(processReceiveData()));

    // from ROS
    //m_ROSreceiver = new QUdpSocket(this);
    //m_ROSreceiver->bind(4000);
    //connect(m_ROSreceiver, SIGNAL(readyRead()), this, SLOT(processReceiveData()));

    //********************************************************************
    // 以时间为触发条件，刷新视图显示
    // 在构造函数中引入时间节点，用于触发signal从而启动槽函数
    // QTimer *timer = new QTimer(this);
    // 设定计时间隔，单位milliseconds
    // timer -> setInterval(10);
    // connect信号与槽函数
    // 由于此槽函数作用是刷新Widget窗口，而openGL的机制是，每次调用时都会自动
    // 隐士调用initializeGL(), resizeGL(), paintGL()函数，从而实现动态显示
    // connect(timer, SIGNAL(timeout()), this, SLOT(updateSlots()));
    // 重新计时
    // timer->start();
    //***********************************************************************
}

myOpenGL::~myOpenGL()
{
}

void myOpenGL::GLreset()
{
    zscale = 250;
    verticalAngle = 7.6f;
    horizonAngle = 12.0F;
    center.setX(0);
    center.setZ(0);
    center.setY(0);
    eye.setX(0.f);
    eye.setY(0.f);
    eye.setZ(100.f);
    upside.setX(1.f);
    upside.setY(1.f);
    upside.setZ(1.f);
    upside.normalize();
    TempTranslateVec.setX(0.0);
    TempTranslateVec.setY(0.0);
    TempTranslateVec.setZ(0.0);
    viewport.setX(0.f);
    viewport.setY(0.f);
    // viewport.setWidth(600);
    // viewport.setHeight(600);
    update();
}

void myOpenGL::enableShowReal()
{
    if (show_curveReal == true)
        show_curveReal = false;
    else
        show_curveReal = true;

    update();
}

void myOpenGL::enableShowCalcul()
{
    if (show_curveCalcul == true)
        show_curveCalcul = false;
    else
        show_curveCalcul = true;

    update();
}

void myOpenGL::initializeGL()
{
    setGeometry(0, 0, 700, 700); // 设置窗口大小
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    setGeometry(this->x(), this->y(), this->width(),this->height());
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(0.5);
    glClearColor(255 / 255, 255 / 255, 255 / 255, 0.0); // the same color as paraview's background
    glEnable(GL_CULL_FACE);                       //启用剔除操作效果
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); // 启用深度测试
    glDepthFunc(GL_LEQUAL);
}

void myOpenGL::resizeGL(int w, int h)
{
    setGeometry(this->x(), this->y(), w, h);
    glViewport(0, 0,
               w, h);
    glMatrixMode(GL_PROJECTION);                   // 选择投影矩阵
    glLoadIdentity();                              // 重置投影矩阵
    gluPerspective(2.5, (double)w / h, 0.1, 1000); // origin parma 1, (double) w / h, 0.1, 100000
    glMatrixMode(GL_MODELVIEW);
}

void myOpenGL::paintGL()
{
    resizeGL(this->width(), this->height());
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    mathassist *assist;
    assist = new (mathassist);

    eye.setX(zscale * cos(assist->angle(verticalAngle)) * cos(assist->angle(horizonAngle)) + center.x());
    eye.setY(zscale * sin(assist->angle(verticalAngle)) + center.y());
    eye.setZ(zscale * cos(assist->angle(verticalAngle)) * sin(assist->angle(horizonAngle)) + center.z());

    glTranslatef(TempTranslateVec.x(), TempTranslateVec.y(), TempTranslateVec.z());
    gluLookAt(eye.x(), eye.y() + 10, eye.z() - 100, center.x(), center.y(), center.z(), 0.0, 0.5, 0.0); // 3*3三组数据，第一组数相机坐标，第二组是物体坐标，第三组是歪头坐标0.0,1.0,0.0

    if (num > 0)
    {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(1.5); // 设置线宽
        glBegin(GL_LINES);
        glVertex3f(leny / 2, lenz / 2, -lenx / 2);
        glVertex3f(leny / 2, -lenz / 2, -lenx / 2);
        glVertex3f(leny / 2, -lenz / 2, -lenx / 2);
        glVertex3f(leny / 2, -lenz / 2, lenx / 2);
        glVertex3f(-leny / 2, lenz / 2, lenx / 2);
        glVertex3f(-leny / 2, lenz / 2, -lenx / 2);
        glVertex3f(-leny / 2, lenz / 2, lenx / 2);
        glVertex3f(-leny / 2, -lenz / 2, lenx / 2);
        glVertex3f(-leny / 2, -lenz / 2, -lenx / 2);
        glVertex3f(-leny / 2, lenz / 2, -lenx / 2); // 原y轴 现z轴 方向相同
        glVertex3f(-leny / 2, -lenz / 2, lenx / 2);
        glVertex3f(-leny / 2, -lenz / 2, -lenx / 2); // 原z轴 现x轴 方向相反
        glVertex3f(leny / 2, lenz / 2, -lenx / 2);
        glVertex3f(-leny / 2, lenz / 2, -lenx / 2);
        glVertex3f(leny / 2, -lenz / 2, lenx / 2);
        glVertex3f(-leny / 2, -lenz / 2, lenx / 2);
        glVertex3f(leny / 2, -lenz / 2, -lenx / 2);
        glVertex3f(-leny / 2, -lenz / 2, -lenx / 2); // 原x轴 现y轴 方向相反
        glEnd();

        glLineWidth(0.5); // 设置线宽
        glBegin(GL_LINES);
        double temp;
        int step = 100;
        for (double i = (int)(minx / step) * step; i <= maxx; i += step)
        {
            if (minx <= i)
            {
                temp = lenx / 2 - (i - minx) / xx * lenx; // x轴方向取反//画网格
                glVertex3f(-leny / 2, -lenz / 2, temp);
                glVertex3f(-leny / 2, lenz / 2, temp);
                glVertex3f(-leny / 2, -lenz / 2, temp);
                glVertex3f(leny / 2, -lenz / 2, temp);
            }
        }
        step = 100;
        for (double i = (int)(miny / step) * step; i <= maxy; i += step)
        {
            if (miny <= i)
            {
                temp = leny / 2 - (i - miny) / yy * leny; // y轴方向取反//画网格
                glVertex3f(temp, -lenz / 2, -lenx / 2);
                glVertex3f(temp, -lenz / 2, lenx / 2);
                glVertex3f(temp, -lenz / 2, -lenx / 2);
                glVertex3f(temp, lenz / 2, -lenx / 2);
            }
        }
        step = 10;
        for (double i = (int)(minz / step) * step; i <= maxz; i += step)
        {
            if (minz <= i)
            {
                temp = (i - minz) / zz * lenz - lenz / 2; // z轴方向相同//画网格
                glVertex3f(-leny / 2, temp, -lenx / 2);
                glVertex3f(leny / 2, temp, -lenx / 2);
                glVertex3f(-leny / 2, temp, -lenx / 2);
                glVertex3f(-leny / 2, temp, lenx / 2);
            }
        }
        glEnd();

        int i;
        double x, y, z;
        // draw 1st-group points
        if (show_curveReal == true)
        {
            glColor3f(0, 0, 1); // 蓝色
            glPointSize(2);
            glBegin(GL_POINTS);
            for (i = 1; i <= num; i++)
            {
                x = line[i][1];
                y = line[i][2];
                z = line[i][3];
                glVertex3f(leny / 2 - (y - miny) / yy * leny, (z - minz) / zz * lenz - lenz / 2, lenx / 2 - (x - minx) / xx * lenx); // 画点
            }
            glEnd();
        }

        // draw 2nd-group points
        if (show_curveCalcul == true)
        {
            glColor3f(1, 0, 0);
            glPointSize(5);
            glBegin(GL_POINTS);
            for (i = 10000; i <= num; i++)
            {
                x = line2[i][1];
                y = line2[i][2];
                z = line2[i][3];
                glVertex3f(leny / 2 - (y - miny) / yy * leny, (z - minz) / zz * lenz - lenz / 2, lenx / 2 - (x - minx) / xx * lenx); // 画点
            }
            glEnd();
        }
        /*
            glColor3f(205.0/255,38.0/255,38.0/255);//红色
            glPointSize(2);
            glBegin(GL_POINTS);
            for(i=1;i<=num;i++){
              x=line3[i][1];y=line3[i][2];z=line3[i][3];
              glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画点
            }
            glEnd();
        */

        // glColor3f(131.0/255,139.0/255,139.0/255);
        /*
            if(show_curveReal==true)
            {
            double llen=2/smallrate;
            glLineWidth(2);
            glBegin(GL_LINES);
                for(i=1;i<=num;i+=stepValue){
                  double a,b,r,x1,y1,z1,x2,y2,z2;
                  x=line[i][1];y=line[i][2];z=line[i][3];
                  a=line[i][4];b=line[i][5];r=line[i][6];
                  x1=x+(cos(b)*cos(r))*llen*2;x2=2*x-x1;
                  y1=y+(cos(b)*sin(r))*llen*2;y2=2*y-y1;
                  z1=z+(-sin(b))*llen*2;z2=2*z-z1;
                  glColor3f(1,0,0);
                  glVertex3f(leny/2-(y1-miny)/yy*leny,(z1-minz)/zz*lenz-lenz/2,lenx/2-(x1-minx)/xx*lenx);
                  glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画姿态角
                  glVertex3f(leny/2-(y2-miny)/yy*leny,(z2-minz)/zz*lenz-lenz/2,lenx/2-(x2-minx)/xx*lenx);
                  glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画姿态角
                  x1=x+(-cos(a)*sin(r)+sin(a)*sin(b)*cos(r))*llen;x2=2*x-x1;
                  y1=y+(cos(a)*cos(r)+sin(a)*sin(b)*sin(r))*llen;y2=2*y-y1;
                  z1=z+(sin(a)*cos(b))*llen;z2=2*z-z1;
                  glColor3f(0,1,0);
                  glVertex3f(leny/2-(y1-miny)/yy*leny,(z1-minz)/zz*lenz-lenz/2,lenx/2-(x1-minx)/xx*lenx);
                  glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画姿态角
                  glVertex3f(leny/2-(y2-miny)/yy*leny,(z2-minz)/zz*lenz-lenz/2,lenx/2-(x2-minx)/xx*lenx);
                  glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画姿态角
                  x1=x+(sin(a)*sin(r)+cos(a)*sin(b)*cos(r))*llen/3;x2=2*x-x1;
                  y1=y+(-sin(a)*cos(r)+cos(a)*sin(b)*sin(r))*llen/3;y2=2*y-y1;
                  z1=z+(cos(a)*cos(b))*llen/3;z2=2*z-z1;
                  glColor3f(1,1,0);
                  glVertex3f(leny/2-(y1-miny)/yy*leny,(z1-minz)/zz*lenz-lenz/2,lenx/2-(x1-minx)/xx*lenx);
                  glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画姿态角
                  glVertex3f(leny/2-(y2-miny)/yy*leny,(z2-minz)/zz*lenz-lenz/2,lenx/2-(x2-minx)/xx*lenx);
                  glVertex3f(leny/2-(y-miny)/yy*leny,(z-minz)/zz*lenz-lenz/2,lenx/2-(x-minx)/xx*lenx);//画姿态角
                }
            glEnd();
            }
        */

        glColor3f(0.0f, 0.0f, 0.0f);
        QString stl;
        glRasterPos3f(leny / 4, -lenz / 2 - 0.05, lenx / 2 + 1.3); // 设置y轴文字显示的位置
        drawString("Y Global Axis(m)");                            // 显示文字
        // glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,10);
        glRasterPos3f(-leny / 2, lenz / 2 + 0.25, lenx / 2 + 0.2);       // 设置z轴文字显示的位置
        drawString("Z Global Axis(m)");                                  // 显示文字
        glRasterPos3f(leny / 2 + 1.5, -lenz / 2 - 0.05, lenx / 4 - 0.1); // 设置x轴文字显示的位置
        drawString("X Global Axis(m)");                                  // 显示文字

        step = 50;
        while (xx / step >= 10)
            step += 50; // 10,8,10
        for (double i = (int)(minx / step) * step; i <= maxx; i += step)
        {
            if (minx <= i)
            {
                temp = lenx / 2 - (i - minx) / xx * lenx; // x轴方向取反//标刻度
                glRasterPos3f(leny / 2 + 0.35, -lenz / 2 - 0.05, temp + 0.1);
                std::string varAsString = std::to_string(i);
                varAsString = varAsString.substr(0, varAsString.length() - 4);
                //        stl=QString::number(i);drawString(stl);
                drawString(varAsString);
            }
        }
        step = 50;
        while (yy / step >= 10)
            step += 50;
        for (double i = (int)(miny / step) * step + step; i <= maxy; i += step)
        {
            if (miny <= i)
            {
                temp = leny / 2 - (i - miny) / yy * leny; // y轴方向取反//标刻度
                glRasterPos3f(temp + 0.3, -lenz / 2 - 0.05, lenx / 2 + 0.25);
                std::string varAsString = std::to_string(i);
                varAsString = varAsString.substr(0, varAsString.length() - 4);
                //        stl=QString::number(i);drawString(stl);
                drawString(varAsString);
            }
        }
        step = 5;
        while (zz / step >= 5)
            step += 5;
        for (double i = (int)(minz / step) * step; i <= maxz; i += step)
        {
            if (minz <= i)
            {
                temp = (i - minz) / zz * lenz - lenz / 2; // z轴方向相同//标刻度
                glRasterPos3f(-leny / 2, temp, lenx / 2 + 0.2);
                std::string varAsString = std::to_string(i);
                varAsString = varAsString.substr(0, varAsString.length() - 4);
                //        stl=QString::number(i);drawString(stl);
                drawString(varAsString);
            }
        }
    }
}

void myOpenGL::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    if (event->buttons() & Qt::LeftButton)
    {
        // if(VM==SELECTMODE)
        // selectObjects();
    }
    else if (event->buttons() & Qt::RightButton)
    {
    }
    else if (event->buttons() & Qt::MidButton)
    {
    }
    lastPos = event->pos();
    update();
}

void myOpenGL::mouseMoveEvent(QMouseEvent *event)
{
    QPoint qPoint = event->pos(); // - lastPos; // mouse move right x+, down y+
    if (event->buttons() & Qt::LeftButton)
    {
        xyRotate(qPoint.x(), qPoint.y());
    }
    else if (event->buttons() & Qt::RightButton)
    {
        xyTranslate(qPoint.x(), qPoint.y());
    }
    else
    {
    }
    lastPos = event->pos(); // Update the mouse coordinate
    update();
}

void myOpenGL::wheelEvent(QWheelEvent *event)
{
    int qwheel = event->delta(); // positive when wheel up, delta +/- 120 when wheel +/- 1
    zScale(qwheel * 120.0);
    update();
}

void myOpenGL::xyTranslate(int dx, int dy)
{
    currPos.setX(dx);
    currPos.setY(dy);
    QVector3D vec(-lastPos.x() + currPos.x(), lastPos.y() - currPos.y(), 0);
    TempTranslateVec = TempTranslateVec + vec * (eye - center).length() / 35000;
    lastPos = currPos;
}

void myOpenGL::xyRotate(int dx, int dy)
{
    currPos.setX(dx);
    currPos.setY(dy);
    dx -= lastPos.x();
    dy -= lastPos.y();
    if (dx < 4.0 && dx > -4 && dy < 4.0 && dy > -4)
        return;
    horizonAngle += dx / 5.0;
    if (verticalAngle + dy < 90 && verticalAngle + dy > -90)
        verticalAngle += dy / 5.0;
    lastPos = currPos; // Update the mouse coordinate
}

void myOpenGL::zScale(float dz)
{
    if (zscale - dz * 1e-3 > 0)
        zscale -= dz * 1e-3;
    update();
}

void myOpenGL::uuppdate(int ii, double x, double y, double z, double a, double b, double r, double x2, double y2, double z2, double x3, double y3, double z3)
{
    num = ii;
    line[ii][1] = x;
    line[ii][2] = y;
    line[ii][3] = z;
    line[ii][4] = a;
    line[ii][5] = b;
    line[ii][6] = r;

    // for line2, not begin from line2[0][]
    line2[ii][1] = x2;
    line2[ii][2] = y2;
    line2[ii][3] = z2;
    line3[ii][1] = x3;
    line3[ii][2] = y3;
    line3[ii][3] = z3;

    /*
        if (ii==1){
          minx=0;maxx=x+50;xx=maxx-minx;
          miny=0;maxy=y+50;yy=maxy-miny;
          minz=0;maxz=z+5;zz=maxz-minz;neww();
        }
        if (maxx<x+10) {
          maxx=x+10;xx=maxx-minx;neww();
        }
        if ((x-minx-3)/xx<=0.05){
          minx=x-20;xx=maxx-minx;neww();
        }
        if (maxy<y+2) {
          maxy=y+50;yy=maxy-miny;neww();
        }
        if ((y-miny-3)/yy<=0.05){
          miny=y-200;yy=maxy-miny;neww();
        }
        if (maxz<z+3){
          maxz=z+10;zz=maxz-minz;neww();
        }
        if (minz>z-3){
          minz=z-8;zz=maxz-minz;neww();
        }
    */

    // focus filed
    minx = -1000;
    maxx = 10;
    xx = maxx - minx;
    miny = 0;
    maxy = 1000;
    yy = maxy - miny;
    minz = 0;
    maxz = 80;
    zz = maxz - minz;
    neww();
    // update();
}

void myOpenGL::setVectorNULL()
{

    // resizeGL(this->width(),this->height());
    glClearColor(255 / 255, 255 / 255, 255 / 255, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // glLoadIdentity();

    // update();

    m_vectorx = {0};
    m_vectory = {0};
    m_vectorz = {0};
    m_vectora = {0};
    m_vectorb = {0};
    m_vectorc = {0};

    m_ROSvectorx = {0};
    m_ROSvectory = {0};
    m_ROSvectorz = {0};
    m_ROSvectora = {0};
    m_ROSvectorb = {0};
    m_ROSvectorc = {0};

    // line[][] = {0};
    // line2[][] = {0};
    update();
}

void myOpenGL::drawString(std::string str)
{
    // 要显示的字符
    const char *strtemp = str.c_str();
    int n = strlen(strtemp);
    // 设置要在屏幕上显示字符的起始位置
    //    glRasterPos2i(0,0);
    // 逐个显示字符串中的每个字符
    //for (int i = 0; i < n; i++)
    //    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(strtemp + i));
}

void myOpenGL::neww()
{
    mathassist *assist;
    assist = new (mathassist);
    double mm = assist->tmax(assist->tmax(xx, yy), zz) / 10;
    smallrate = 9 / mm;
    lenx = xx / 10 * smallrate;
    leny = yy / 10 * smallrate;
    // lenz=zz/10*smallrate;
    lenz = zz / 2 * smallrate;
}

void myOpenGL::processReceiveData()
{
    // sleep(100);
    while (false) //m_receiver->hasPendingDatagrams()
    {

        QByteArray datagram;
        //datagram.resize(m_receiver->pendingDatagramSize());

        //m_receiver->readDatagram(datagram.data(), datagram.size());

        QString data = datagram;
        // std::cout << data.toStdString() << std::endl;
        // std::string stdString

        char *dataStr = datagram.data();

        /*int N = strlen(dataStr);
        int Num[6];
        int NumCounter = 0;
        for (int i = 0; i <= N; i++)
        {
            if (dataStr[i] == ':')
            {
                Num[0] = i;
                NumCounter = NumCounter + 1;
            }
            if (dataStr[i] == ';')
            {
                Num[NumCounter] = i;
                NumCounter = NumCounter + 1;
            }
        }*/

        Flight_Data_ = decode(dataStr);

        // qDebug() << "x: " << Flight_Data_.position[0] << " y: " << Flight_Data_.position[1] << " z: " << Flight_Data_.position[2]
        //          << " a: " << Flight_Data_.attitude[0] <<  " b: " << Flight_Data_.attitude[1] <<  " c: " << Flight_Data_.attitude[2] << endl;

        m_valuex = double(Flight_Data_.position[0]);
        m_valuey = double(Flight_Data_.position[1]);
        m_valuez = -double(Flight_Data_.position[2]);
        m_valuea = double(Flight_Data_.attitude[0]);
        m_valueb = double(Flight_Data_.attitude[1]);
        m_valuec = double(Flight_Data_.attitude[2]);

        // 在向量末尾添加新的值
        m_vectorx.push_back(m_valuex);
        m_vectory.push_back(m_valuey);
        m_vectorz.push_back(m_valuez);
        m_vectora.push_back(m_valuea);
        m_vectorb.push_back(m_valueb);
        m_vectorc.push_back(m_valuec);

        if (abs(int(m_valuez)) > 100)
            m_valuez = m_vectorz.back();
        if (abs(int(m_valuex)) > 1000)
            m_valuex = m_vectorx.back();
        if (abs(int(m_valuey)) > 1000)
            m_valuey = m_vectory.back();

        std::cout << "vector size: " << m_vectorx.size() << std::endl;

        if (false)//m_ROSreceiver->hasPendingDatagrams()
        {
            QByteArray datagram;
            //datagram.resize(m_ROSreceiver->pendingDatagramSize());
            //m_ROSreceiver->readDatagram(datagram.data(), datagram.size());
            // qDebug() << "ROS_datagram" << datagram << endl;

            QString ROSdata = datagram;
            QStringList ROSdataList = ROSdata.split(' ');

            m_ROSvaluex = ROSdataList[0].toDouble();
            m_ROSvaluey = ROSdataList[1].toDouble();
            m_ROSvaluez = ROSdataList[2].toDouble();
            m_ROSvaluea = ROSdataList[3].toDouble();
            m_ROSvalueb = ROSdataList[4].toDouble();
            m_ROSvaluec = ROSdataList[5].toDouble();
            // qDebug() << "ROS_x:" << m_ROSvaluex << " ROS_y: "<< m_ROSvaluey << " ROS_z: " << m_ROSvaluez
            //          << " ROS_a: "<< m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;

            // m_ROSvectorx.push_back(m_ROSvaluex);
            // m_ROSvectory.push_back(m_ROSvaluey);
            // m_ROSvectorz.push_back(m_ROSvaluez);
            // m_ROSvectora.push_back(m_ROSvaluea);
            // m_ROSvectorb.push_back(m_ROSvalueb);
            // m_ROSvectorc.push_back(m_ROSvaluec);

            qDebug() << "******************************************************************************************" << endl;
            qDebug() << "Origin -- ROS_x:" << m_ROSvaluex << " ROS_y: " << m_ROSvaluey << " ROS_z: " << m_ROSvaluez
                     << " ROS_a: " << m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;

            m_AirsimCamb = -atan2(fabs(m_valuex), fabs(m_valuey));
            // m_AirsimCamb = atan2(m_valuex,m_valuey);
            m_AirsimCama = atan2(fabs(m_valuez), sqrt(pow(m_valuex, 2) + pow(m_valuey, 2)));
            qDebug() << "Camera picth: " << m_AirsimCama << "Camera yaw: " << m_AirsimCamb << endl;

            double m_ROSvaluex_ = m_ROSvaluex;
            double m_ROSvaluey_ = m_ROSvaluey;
            double m_ROSvaluez_ = m_ROSvaluez;
            double m_ROSvaluea_ = m_ROSvaluea;
            double m_ROSvalueb_ = m_ROSvalueb;
            double m_ROSvaluec_ = m_ROSvaluec;
            // calculate transed ROS values

            // m_ROSvaluex = m_ROSvaluex*cos(m_AirsimCamb)+m_ROSvaluez*cos(m_AirsimCamb);
            // m_ROSvaluey = -m_ROSvaluex*cos(m_AirsimCama)*sin(m_AirsimCamb)-m_ROSvaluey*sin(m_AirsimCamb)+m_ROSvaluez*cos(m_AirsimCama)*cos(m_AirsimCamb);
            // m_ROSvaluez = m_ROSvaluex*cos(m_AirsimCamb)*sin(m_AirsimCama)-m_ROSvaluey*cos(m_AirsimCama)+m_ROSvaluez*sin(m_AirsimCama)*cos(m_AirsimCamb);

            //            m_ROSvaluex = m_ROSvaluex_*cos(m_AirsimCamb)-m_ROSvaluey_*sin(m_AirsimCamb)*sin(m_AirsimCama)+m_ROSvaluez_*sin(m_AirsimCamb)*cos(m_AirsimCama);
            //            m_ROSvaluey = -m_ROSvaluex_*sin(m_AirsimCamb)-m_ROSvaluey_*cos(m_AirsimCamb)*sin(m_AirsimCama)+m_ROSvaluez_*cos(m_AirsimCamb)*cos(m_AirsimCama);
            //            m_ROSvaluez = -(-m_ROSvaluey_*cos(m_AirsimCama)-m_ROSvaluez_*sin(m_AirsimCama));
            m_ROSvaluex = m_ROSvaluex_ * cos(m_AirsimCamb) + m_ROSvaluez_ * sin(m_AirsimCamb);
            m_ROSvaluey = -m_ROSvaluex_ * sin(m_AirsimCamb) * cos(m_AirsimCama) - m_ROSvaluey_ * sin(m_AirsimCama) + m_ROSvaluez_ * cos(m_AirsimCamb) * cos(m_AirsimCama);
            m_ROSvaluez = -m_ROSvaluey_ * sin(m_AirsimCama) * sin(m_AirsimCamb) - m_ROSvaluey_ * cos(m_AirsimCama) + m_ROSvaluez_ * sin(m_AirsimCama) * cos(m_AirsimCamb);

            qDebug() << "Calculated -- ROS_x:" << m_ROSvaluex << " ROS_y: " << m_ROSvaluey << " ROS_z: " << m_ROSvaluez
                     << " ROS_a: " << m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;
            qDebug() << "x: " << m_valuex << " y: " << m_valuey << " z: " << m_valuez
                     << " a: " << m_valuea << " b: " << m_valueb << " c: " << m_valuec << endl;
        }
        else
        {
            m_ROSvaluex = 0;
            m_ROSvaluey = 0;
            m_ROSvaluez = 0;
            m_ROSvaluea = 0;
            m_ROSvalueb = 0;
            m_ROSvaluec = 0;
        }

        m_ROSvectorx.push_back(m_ROSvaluex);
        m_ROSvectory.push_back(m_ROSvaluey);
        m_ROSvectorz.push_back(m_ROSvaluez);
        m_ROSvectora.push_back(m_ROSvaluea);
        m_ROSvectorb.push_back(m_ROSvalueb);
        m_ROSvectorc.push_back(m_ROSvaluec);

        // add ||a||
        double value_xyz = pow(m_vectorx.back(), 2) + pow(m_vectory.back(), 2) + pow(m_vectorz.back(), 2);
        double value_ROSxyz = pow(m_ROSvectorx.back(), 2) + pow(m_ROSvectory.back(), 2) + pow(m_ROSvectorz.back(), 2);

        if (fabs(value_xyz - value_ROSxyz) < 100)
        {
            m_vectorx.back() = 0;
            m_vectory.back() = 0;
            m_vectorz.back() = 0;
            m_ROSvectorx.back() = 0;
            m_ROSvectory.back() = 0;
            m_ROSvectorz.back() = 0;
        }

        /*
        if(m_ROSreceiver->hasPendingDatagrams())
        {
            if(m_vectorx.size()<10000){
                m_ROSvaluex = 0;
                m_ROSvaluey = 0;
                m_ROSvaluez = 0;
                m_ROSvaluea = 0;
                m_ROSvalueb = 0;
                m_ROSvaluec = 0;
            }
            else {
                QByteArray datagram;
                datagram.resize(m_ROSreceiver->pendingDatagramSize());
                m_ROSreceiver -> readDatagram(datagram.data(), datagram.size());
                //qDebug() << "ROS_datagram" << datagram << endl;

                QString ROSdata = datagram;
                QStringList ROSdataList = ROSdata.split(' ');

                m_ROSvaluex = ROSdataList[0].toDouble();
                m_ROSvaluey = ROSdataList[1].toDouble();
                m_ROSvaluez = ROSdataList[2].toDouble();
                m_ROSvaluea = ROSdataList[3].toDouble();
                m_ROSvalueb = ROSdataList[4].toDouble();
                m_ROSvaluec = ROSdataList[5].toDouble();
                //qDebug() << "ROS_x:" << m_ROSvaluex << " ROS_y: "<< m_ROSvaluey << " ROS_z: " << m_ROSvaluez
                //         << " ROS_a: "<< m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;

                //m_ROSvectorx.push_back(m_ROSvaluex);
                //m_ROSvectory.push_back(m_ROSvaluey);
                //m_ROSvectorz.push_back(m_ROSvaluez);
                //m_ROSvectora.push_back(m_ROSvaluea);
                //m_ROSvectorb.push_back(m_ROSvalueb);
                //m_ROSvectorc.push_back(m_ROSvaluec);

                qDebug() << "******************************************************************************************" << endl;
                qDebug() << "Origin -- ROS_x:" << m_ROSvaluex << " ROS_y: "<< m_ROSvaluey << " ROS_z: " << m_ROSvaluez
                         << " ROS_a: "<< m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;

                m_AirsimCamb = -atan2(fabs(m_valuex),fabs(m_valuey));
                //m_AirsimCamb = atan2(m_valuex,m_valuey);
                m_AirsimCama = atan2(fabs(m_valuez),sqrt(pow(m_valuex,2)+pow(m_valuey,2)));
                qDebug() << "Camera picth: " << m_AirsimCama << "Camera yaw: " << m_AirsimCamb << endl;

                double m_ROSvaluex_ = m_ROSvaluex;
                double m_ROSvaluey_ = m_ROSvaluey;
                double m_ROSvaluez_ = m_ROSvaluez;
                double m_ROSvaluea_ = m_ROSvaluea;
                double m_ROSvalueb_ = m_ROSvalueb;
                double m_ROSvaluec_ = m_ROSvaluec;
                // calculate transed ROS values

                //m_ROSvaluex = m_ROSvaluex*cos(m_AirsimCamb)+m_ROSvaluez*cos(m_AirsimCamb);
                //m_ROSvaluey = -m_ROSvaluex*cos(m_AirsimCama)*sin(m_AirsimCamb)-m_ROSvaluey*sin(m_AirsimCamb)+m_ROSvaluez*cos(m_AirsimCama)*cos(m_AirsimCamb);
                //m_ROSvaluez = m_ROSvaluex*cos(m_AirsimCamb)*sin(m_AirsimCama)-m_ROSvaluey*cos(m_AirsimCama)+m_ROSvaluez*sin(m_AirsimCama)*cos(m_AirsimCamb);

    //            m_ROSvaluex = m_ROSvaluex_*cos(m_AirsimCamb)-m_ROSvaluey_*sin(m_AirsimCamb)*sin(m_AirsimCama)+m_ROSvaluez_*sin(m_AirsimCamb)*cos(m_AirsimCama);
    //            m_ROSvaluey = -m_ROSvaluex_*sin(m_AirsimCamb)-m_ROSvaluey_*cos(m_AirsimCamb)*sin(m_AirsimCama)+m_ROSvaluez_*cos(m_AirsimCamb)*cos(m_AirsimCama);
    //            m_ROSvaluez = -(-m_ROSvaluey_*cos(m_AirsimCama)-m_ROSvaluez_*sin(m_AirsimCama));
                m_ROSvaluex = m_ROSvaluex_*cos(m_AirsimCamb)+m_ROSvaluez_*sin(m_AirsimCamb);
                m_ROSvaluey = -m_ROSvaluex_*sin(m_AirsimCamb)*cos(m_AirsimCama)-m_ROSvaluey_*sin(m_AirsimCama)+m_ROSvaluez_*cos(m_AirsimCamb)*cos(m_AirsimCama);
                m_ROSvaluez = -m_ROSvaluey_*sin(m_AirsimCama)*sin(m_AirsimCamb)-m_ROSvaluey_*cos(m_AirsimCama)+m_ROSvaluez_*sin(m_AirsimCama)*cos(m_AirsimCamb);
            }

            qDebug() << "Calculated -- ROS_x:" << m_ROSvaluex << " ROS_y: "<< m_ROSvaluey << " ROS_z: " << m_ROSvaluez
                     << " ROS_a: "<< m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;
            qDebug() << "x: " << m_valuex << " y: " << m_valuey << " z: " << m_valuez
                     << " a: " << m_valuea <<  " b: " << m_valueb <<  " c: " << m_valuec << endl;



        }
        else
        {
            m_ROSvaluex = 0;
            m_ROSvaluey = 0;
            m_ROSvaluez = 0;
            m_ROSvaluea = 0;
            m_ROSvalueb = 0;
            m_ROSvaluec = 0;
        }
        */
        m_ROSvectorx.push_back(m_ROSvaluex);
        m_ROSvectory.push_back(m_ROSvaluey);
        m_ROSvectorz.push_back(m_ROSvaluez);
        m_ROSvectora.push_back(m_ROSvaluea);
        m_ROSvectorb.push_back(m_ROSvalueb);
        m_ROSvectorc.push_back(m_ROSvaluec);

        /*
                m_ROSvectorx.push_back(m_ROSvaluex);
                m_ROSvectory.push_back(m_ROSvaluey);
                m_ROSvectorz.push_back(m_ROSvaluez);
                m_ROSvectora.push_back(m_ROSvaluea);
                m_ROSvectorb.push_back(m_ROSvalueb);
                m_ROSvectorc.push_back(m_ROSvaluec);

                // add ||a||
                double value_xyz = pow(m_vectorx.back(),2) + pow(m_vectory.back(),2) + pow(m_vectorz.back(),2);
                double value_ROSxyz = pow(m_ROSvectorx.back(),2) + pow(m_ROSvectory.back(),2) + pow(m_ROSvectorz.back(),2);

                if(fabs(value_xyz-value_ROSxyz) < 100)
                {
                    m_vectorx.back()=0;
                    m_vectory.back()=0;
                    m_vectorz.back()=0;
                    m_ROSvectorx.back()=0;
                    m_ROSvectory.back()=0;
                    m_ROSvectorz.back()=0;
                }
                */
        uuppdate(int(m_vectorx.size()), m_valuex, m_valuey, m_valuez, m_valuea, m_valueb, m_valuec,
                 m_ROSvectorx.back(), m_ROSvectory.back(), m_ROSvectorz.back(), m_ROSvectora.back(), m_ROSvectorb.back(), m_ROSvectorc.back());
        // sleep(1);
        // qDebug() << "REAL: " << m_vectorx.size() << " ROS: " << m_ROSvectorx.size() << endl;
        update();
    }

    /*
    while(m_ROSreceiver->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_ROSreceiver->pendingDatagramSize());
        m_ROSreceiver -> readDatagram(datagram.data(), datagram.size());
        qDebug() << "ROS_datagram" << datagram << endl;

        QString ROSdata = datagram;
        QStringList ROSdataList = ROSdata.split(' ');

        m_ROSvaluex = ROSdataList[0].toDouble();
        m_ROSvaluey = ROSdataList[1].toDouble();
        m_ROSvaluez = ROSdataList[2].toDouble();
        m_ROSvaluea = ROSdataList[3].toDouble();
        m_ROSvalueb = ROSdataList[4].toDouble();
        m_ROSvaluec = ROSdataList[5].toDouble();
        qDebug() << "ROS_x:" << m_ROSvaluex << " ROS_y: "<< m_ROSvaluey << " ROS_z: " << m_ROSvaluez
                 << " ROS_a: "<< m_ROSvaluea << " ROS_b: " << m_ROSvalueb << " ROS_c: " << m_ROSvaluec << endl;

        //uuppdate(int(m_vectorx.size()),m_valuex,m_valuey,m_valuez,m_valuea,m_valueb,m_valuec,ROS_x,ROS_y,ROS_z,m_valuex,m_valuey,m_valuez);
        //sleep(1);
        //update();
    }
    */
}
/*
        char* dataStr = datagram.data();
        Flight_Data_this_time = decode_frame(dataStr);

        std::cout << "x: " << Flight_Data_this_time.position[0] << std::endl;
        std::cout << "y: " << Flight_Data_this_time.position[1] << std::endl;
        std::cout << "z: " << Flight_Data_this_time.position[2] << std::endl;
        std::cout << "a: " << Flight_Data_this_time.attitude_angle[0] << std::endl;
        std::cout << "b: " << Flight_Data_this_time.attitude_angle[1] << std::endl;
        std::cout << "c: " << Flight_Data_this_time.attitude_angle[2] << std::endl;

        m_valuex = double(Flight_Data_this_time.position[0]);
        m_valuey = double(Flight_Data_this_time.position[1]);
        m_valuez = double(Flight_Data_this_time.position[2]);

        update();
*/
/*
        // 将数据转化为 string 进行处理
        QString data = datagram;
        std::cout << data.toStdString() << std::endl;

        // 应当注意：以这种方式会导致，当数据长度改变时，会出错
        // 最优应当以 特有的分割符进行划分
        m_valuex = data.right(54).mid(1,8).toDouble();
        //std::cout << "x: " << m_valuex << std::endl;
        m_valuey = data.right(54).mid(10,8).toDouble();
        //std::cout << "y: " << m_valuey << std::endl;
        m_valuez = data.right(54).mid(19,8).toDouble();
        //std::cout << "z: " << m_valuez << std::endl;

        update();
*/
void myOpenGL::setStepValue(int a)
{
    // std::cout<<__FILE__<<' '<<__LINE__<<' '<<this<<std::endl;
    // std::cout<<"stepValue: "<<stepValue<<std::endl;
    // std::cout<<"a: "<<a<<std::endl;
    stepValue = a;

    // std::cout<<"stepValue: "<<stepValue<<std::endl;
    update();
}

Flight_Data decode(char *buff)
{
    Flight_Data data;
    if (buff[0] == 'D' && buff[1] == 'A') // 数据帧头"DATA@"校验
    {
        double *Data[6];     // 共计6个float需要读取
        int Bytecounter = 5; // 除去帧头，从第6个byte开始
        for (int N = 0; N <= 5; N++)
        {
            Data[N] = (double *)&buff[Bytecounter];
            Bytecounter = Bytecounter + 8;
        }
        data.position[0] = *Data[0];  // X
        data.position[1] = *Data[1];  // Y
        data.position[2] = -*Data[2]; // Z
        data.attitude[0] = *Data[3];  // theta
        data.attitude[1] = *Data[4];  // phi
        data.attitude[2] = *Data[5];  // psi
    }
    return data;
}
/*
Flight_Data decode_frame(char* udp_raw_data)
{
    Flight_Data Data_temp;

    if (udp_raw_data[0] == 'D' && udp_raw_data[1] == 'A')// 数据帧头校验
    {
        unsigned int* Data_index[5]; // 5个int
        float* Data[40];             // 5*8个float
        int Bytecounter = 5;         // 除去帧头，从第6个字节开始

        for (int N = 0; N <= 4; N++)
        {
            Data_index[N] = (unsigned int*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[0 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[1 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[2 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[3 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[4 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[5 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[6 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
            Data[7 + 8 * N] = (float*)&udp_raw_data[Bytecounter];
            Bytecounter = Bytecounter + 4;
        }
        Data_temp.air_speed = 0.44704 * (*Data[5]);// Va, mph2m
        Data_temp.angular_velocity[0] = *Data[9];  // P
        Data_temp.angular_velocity[1] = *Data[8];  // Q
        Data_temp.angular_velocity[2] = *Data[10];  // R
        Data_temp.attitude_angle[0] = *Data[16];    // theta
        Data_temp.attitude_angle[1] = *Data[17];    // phi
        Data_temp.attitude_angle[2] = *Data[18];    // psi
        Data_temp.flow_angle[0] = *Data[24];        // alpha
        Data_temp.flow_angle[1] = *Data[25];        // beta
        Data_temp.position[0] = *Data[33];          // lon
        Data_temp.position[1] = *Data[32];          // lat
        //Data_temp.position[2] = 0.3048 * ((*Data[34]) + 68);// altitude,feet2m
        Data_temp.position[2] = 0.3048 * (*Data[34]) - 28.29625;// altitude,feet2m
    }

    return Data_temp;
}
*/
/*
Flight_Data1 decode(char* buff, int* Num)
{
    Flight_Data1 data;
    char num_char[20];
    float num_float = 0.0;
    std::string num_string = "waiting for data";
    // x
    for (int i = 0; i <= Num[1] - Num[0] - 2; i++)
    {
        num_char[i] = buff[Num[0] + 1 + i];
    }
    num_char[Num[1] - Num[0] - 1] = '\0';
    num_string = num_char;
    num_float = atof(num_string.c_str()); // string转float
    data.position[0] = num_float;
    // y
    for (int i = 0; i <= Num[2] - Num[1] - 2; i++)
    {
        num_char[i] = buff[Num[1] + 1 + i];
    }
    num_char[Num[2] - Num[1] - 1] = '\0';
    num_string = num_char;
    num_float = atof(num_string.c_str());
    data.position[1] = num_float;
    // z
    for (int i = 0; i <= Num[3] - Num[2] - 2; i++)
    {
        num_char[i] = buff[Num[2] + 1 + i];
    }
    num_char[Num[3] - Num[2] - 1] = '\0';
    num_string = num_char;
    num_float = atof(num_string.c_str());
    data.position[2] = num_float;
    // theta
    for (int i = 0; i <= Num[4] - Num[3] - 2; i++)
    {
        num_char[i] = buff[Num[3] + 1 + i];
    }
    num_char[Num[4] - Num[3] - 1] = '\0';
    num_string = num_char;
    num_float = atof(num_string.c_str());
    data.attitude[0] = num_float;
    // phi
    for (int i = 0; i <= Num[5] - Num[4] - 2; i++)
    {
        num_char[i] = buff[Num[4] + 1 + i];
    }
    num_char[Num[5] - Num[4] - 1] = '\0';
    num_string = num_char;
    num_float = atof(num_string.c_str());
    data.attitude[1] = num_float;
    // psi
    int end = strlen(buff) - 1;
    for (int i = 0; i <= end - Num[5] - 2; i++)
    {
        num_char[i] = buff[Num[5] + 1 + i];
    }
    num_char[end - Num[5] - 1] = '\0';
    num_string = num_char;
    num_float = atof(num_string.c_str());
    data.attitude[2] = num_float;
    return data;
}
*/
