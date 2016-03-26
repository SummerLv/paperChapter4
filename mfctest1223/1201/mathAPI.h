#ifndef MATHAPI_H
#define MATHAPI_H

#define pi 3.1415926
#include "math.h"
struct float_point
{
float x;
float y;
};
extern double distancePointToLine(double x1, double y1, double z1
                                                    , double x2, double y2, double z2
                                                    , double xe, double ye, double ze);

extern double minDistanceCalculate(double Ax,double Ay,double Bx,double By,
                                           double Cx,double Cy,double Dx,double Dy);

extern double pointOnRightOrLeft(double x1,double y1,double x2,double y2,double xe,double ye);

extern  void select_sort(float *x, int n);
extern float angle_pan(struct float_point x1,struct float_point x2);
extern int cacu_line_circle(struct float_point x1, struct float_point x2, float x, float y, float r, float starangle, float endangl,struct float_point returndata[]);
extern char suit_angle(float angle,float stangle,float endangl);
extern char cacu_cross(struct float_point x1,struct float_point x2,struct float_point y1,struct float_point y2,float *temp);

#endif // MATHAPI_H
