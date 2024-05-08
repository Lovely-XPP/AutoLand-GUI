#ifndef MYOPENGL_H
#define MYOPENGL_H

#include <QOpenGLWidget>
#include <QWidget>
#include <QtOpenGL/QGL>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector3D>
#include <vector>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "mathassist.h"


class myOpenGL : public QOpenGLWidget
{
    Q_OBJECT
    
public:
    explicit myOpenGL(QWidget *parent=nullptr);
    ~myOpenGL();


    // 绘制网格需要的线条，在paintGL()中被调用
    void GLGrid(float pt1x, float pt1y, float pt1z,
                float pt2x, float pt2y, float pt2z, int num);
    // 绘制3维网格
    void GL3Grid(int pt, int num);
    // 重置视图为初始视角
    void GLreset();
    // 用于显示和隐藏绘图
    void enableShowReal();
    void enableShowCalcul();

    void resizeGL(int w, int h);
    void initializeGL();
    void paintGL();
    //上面三个函数是OpenGL必备函数，其中paintGl是重点，所有绘图工作都在其中完成
    void mousePressEvent(QMouseEvent *event);//监测鼠标点击
    void mouseMoveEvent(QMouseEvent *event);//监测鼠标移动
    //上面两个函数用来监测鼠标拖动，进行旋转或移动操作
    void wheelEvent(QWheelEvent *event);//监测滚轮
    //用来放大缩小
    void xyTranslate(int dx, int dy);
    void xyRotate(int dx, int dy);
    void zScale(float dz);
    void uuppdate(int ii,double x,double y,double z,double a,double b,double r,double x2,double y2,double z2,double x3,double y3,double z3);
    //上面这个uuppdate用来导入一组数据，其中依次为第一组坐标、姿态角、第二组坐标、第三组坐标
    //void newview(bool tt);//三维切换
    //void restart();//三维重置
    void setVectorNULL();

    int stepValue;

private:
    GLfloat horizonAngle;
    GLfloat verticalAngle;
    GLfloat zscale;
    QPoint lastPos,currPos;
    QVector3D eye, upside;
    QVector3D center;
    QVector3D TempTranslateVec;
    QRectF viewport;
    std::vector<QVector3D> normal;
    int num=0,isFirstCall=1,MAX_CHAR=128;
    double xx,yy,zz,minx,maxx,miny,maxy,minz,maxz,lenx,leny,lenz,smallrate,xxori,yyori,zzori,minxori,minyori,minzori;
    bool ttfull=false;
    double line[20000][7];//存放坐标
    double line2[20000][4];//存放随机坐标
    double line3[20000][4];//存放随机坐标二
//    void drawString(QString str);
    void drawString(std::string str);
   void neww();


public slots:
    // 槽函数，用于更新数据，同时刷新界面
    // 从而激发openGL反复调用paintGL()函数绘图
    // 触发的signal是计时时间对象
    void processReceiveData();
    void setStepValue(int a);

private:
    // 用于缩放视图 wheelEvent
    int m_scloe;
    // 用于点的平移步长
    int m_isize;
    // 用于显示绘图
    bool show_curveReal;
    bool show_curveCalcul;

    // 用于转动视角 mouseMoveEvent
    QPoint m_rotPosOld;
    double m_rotx;
    double m_roty;
    double m_rotz;

    // 每个点的坐标x,y,z
    double m_valuex=0;
    double m_valuey=0;
    double m_valuez=0;
    double m_valuea;
    double m_valueb;
    double m_valuec;
    // 将每个点的坐标分开存储在一个向量中
    std::vector<double> m_vectorx;
    std::vector<double> m_vectory;
    std::vector<double> m_vectorz;
    std::vector<double> m_vectora;
    std::vector<double> m_vectorb;
    std::vector<double> m_vectorc;

    // 每个ROS点的坐标x,y,z
    double m_ROSvaluex=0;
    double m_ROSvaluey=0;
    double m_ROSvaluez=0;
    double m_ROSvaluea;
    double m_ROSvalueb;
    double m_ROSvaluec;
    // 将每个点的坐标分开存储在一个向量中
    std::vector<double> m_ROSvectorx;
    std::vector<double> m_ROSvectory;
    std::vector<double> m_ROSvectorz;
    std::vector<double> m_ROSvectora;
    std::vector<double> m_ROSvectorb;
    std::vector<double> m_ROSvectorc;

    double m_AirsimCama=0;  // gunzhuan
    double m_AirsimCamb=0;  // pianhang

    //QUdpSocket *m_receiver;
    //QUdpSocket *m_ROSreceiver;
};

#endif // MYOPENGL_H
