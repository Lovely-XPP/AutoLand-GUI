#include "mathassist.h"


double mathassist::trunc4(double x){//保留四位小数
    double ans=(int)(x*10000)/10000.0;
    return ans;
}
double mathassist::tmax(double x,double y){//取最大值
    if (x>=y) return(x);
    else return(y);
}
double mathassist::tmin(double x,double y){//取最小值
    if (x<=y) return(x);
    else return(y);
}
double mathassist::angle(double x)
{
    double pi=3.1415926535898;
    double y=x/180.0*pi;
    return(y);
}
