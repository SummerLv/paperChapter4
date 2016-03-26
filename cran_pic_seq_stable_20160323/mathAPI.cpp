#include "mathAPI.h"

/*点到直线的距离*/
double distancePointToLine(double x1,double y1,double z1
                                         ,double x2,double y2,double z2
                                         ,double xe,double ye,double ze)
{
    double t,min;
    double a = (x1 - xe)*(x1 - x2) + (y1 - ye)*(y1 - y2) + (z1 - ze)*(z1 - z2),
            b = pow( (x1 - x2) , 2) + pow( (y1 - y2) , 2) + pow( (z1 - z2) , 2 );
    t = a/b;
    if ( (0<t || 0 == t) && (t < 1 || 1 == t) ) {                                                //最近的点在线段上
        double Qx,Qy,Qz;
        Qx = x1 + t*(x2 - x1);
        Qy = y1 + t*(y2 - y1);
        Qz = z1 + t*(z2 - z1);
        min = sqrt(pow((xe - Qx),2) + pow((ye - Qy),2) + pow((ze - Qz),2));     //最近点在BA延长线上，min距离为QA
    }else if (t < 0) {
        min = sqrt(pow((xe - x1),2) + pow((ye -y1),2) + pow((ze - z1),2));      //最近点在AB延长线上，min距离为QB
    }else if (t > 1) {
        min = sqrt(pow((xe - x2),2) + pow((ye - y2),2) + pow((ze - z2),2));
    }
    return min;
}

/*两条线段AB与CD的最短距离（Ax,Ay,Bx,By）----(Cx,Cy,Dx,Dy)*/
double minDistanceCalculate(double Ax,double Ay,double Bx,double By,
                                           double Cx,double Cy,double Dx,double Dy)
{
    double r,s,min_d;
    r = ((Ay - Cy)*(Dx - Cx) - (Ax - Cx)*(Dy - Cy))*1.0 / ((Bx - Ax)*(Dy - Cy) - (By - Ay)*(Dx - Cx));
    s = ((Ay - Cy)*(Bx - Ax) - (Ax - Cx)*(By - Ay))*1.0 / ((Bx - Ax)*(Dy - Cy) - (By - Ay)*(Dx - Cx));

    if(((0 < r || 0 == r) &&(r < 1 || 1== r)) &&
            ((0 < s || 0 == s) && (s < 1 || 1 == s))) {                     //
        double Px,Py,Qx,Qy;
        Px = Ax + r*(Bx - Ax);
        Py = Ay + r*(By - Ay);

        Qx = Cx + s*(Dx - Cx);
        Qy = Cy + s*(Dy - Cy);
        min_d = sqrt(pow((Px-Qx),2) + pow((Py - Qy),2));
    }else{
        double Da_cd,Db_cd,Dd_ab,Dc_ab;
        Da_cd = distancePointToLine(Cx,Cy,0,Dx,Dy,0,Ax,Ay,0);   //A到CD的最短距离
        Db_cd = distancePointToLine(Cx,Cy,0,Dx,Dy,0,Bx,By,0);   //B到CD的最短距离

        Dc_ab = distancePointToLine(Ax,Ay,0,Bx,By,0,Cx,Cy,0);       //C到AB的最短距离
        Dd_ab = distancePointToLine(Ax,Ay,0,Bx,By,0,Dx,Dy,0);       //D到AB的最短距离

        //---------------找出4段最短距离者------------------
        min_d = Da_cd < Db_cd ? Da_cd : Db_cd;
        min_d = min_d < Dc_ab ? min_d : Dc_ab;
        min_d = min_d < Dd_ab ? min_d : Dd_ab;

        if (min_d == Da_cd) {

        }else if (min_d == Db_cd) {

        }else if (min_d == Dc_ab) {

        }else{

        }

    }
    return min_d;
}



/*判断点在直线左边还是右边*/
double pointOnRightOrLeft(double x1,double y1,double x2,double y2,double xe,double ye)
{
    double temp;
    temp = (x2 - x1) * (ye - y2) - (y2 - y1)*(xe - x2);
    return temp;

}



/*
**********************************************************************
º¯Êý£ºcacu_cross
²ÎÊý£ºx1 :L1_P1  x2:L1_P2  y1£ºL2_P3 y2:L2_P4
·µ»Ø£ºÓÐœ»µãÔò·µ»Ø1£¬ÎÞÔò·µ»Ø0
¹ŠÄÜ£ºŒÆËãÖ±Ïßœ»µã
**********************************************************************
*/
    char cacu_cross(struct float_point x1,struct float_point x2,struct float_point y1,struct float_point y2,float *temp)
    {

        float max1,max2,max3,max4;
        float min1,min2,min3,min4;
        max1=fmax(x1.x,x2.x);  //·µ»Øx1.x,x2.xÖÐµÄ×îŽó
        min1=fmin(x1.x,x2.x);  //·µ»Øx1.x,x2.xÖÐµÄ×îÐ¡

        max2=fmax(x1.y,x2.y);
        min2=fmin(x1.y,x2.y);

        max3=fmax(y1.x,y2.x);
        min3=fmin(y1.x,y2.x);

        max4=fmax(y1.y,y2.y);
        min4=fmin(y1.y,y2.y);


        if (x1.x==x2.x)
        {
            temp[0]=x1.x;
            float temp1=fmax(y1.x,y2.x);
            float temp2=fmin(y1.x,y2.x);
            if (temp[0]<=temp1&&temp[0]>=temp2)
            {
                if (y1.x==y2.x)
                {
                    return 0;
                }
                else
                {
                    float k=(y2.y-y1.y)/(y2.x-y1.x) ;		 //ŒÆËãÐ±ÂÊ
                    temp[1]=k*(temp[0]-y2.x)+y2.y;
                    if (temp[0]>=min1&&temp[0]<=max1&&temp[0]>=min3&&temp[0]<=max3&&temp[1]>=min2&&temp[1]<=max2&&temp[1]>=min4&&temp[1]<=max4)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }

                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (y1.x==y2.x)
            {
                temp[0]=y2.x;
                float temp1=fmax(x1.x,x2.x);
                float temp2=fmin(x1.x,x2.x);
                if (temp[0]<=temp1&&temp[0]>=temp2)
                {
                    float k=(x2.y-x1.y)/(x2.x-x1.x);		 //ŒÆËãÐ±ÂÊ
                    temp[1]=k*(temp[0]-x2.x)+x2.y;
                    if (temp[0]>=min1&&temp[0]<=max1&&temp[0]>=min3&&temp[0]<=max3&&temp[1]>=min2&&temp[1]<=max2&&temp[1]>=min4&&temp[1]<=max4)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                float k1=(y2.y-y1.y)/(y2.x-y1.x);		  //ŒÆËãÐ±ÂÊ
                float k2=(x2.y-x1.y)/(x2.x-x1.x);		  //ŒÆËãÐ±ÂÊ
                if (k1==k2)
                {
                    return 0;
                }
                else
                {
                    temp[0]=(-k2*x2.x+x2.y+k1*y2.x-y2.y)/(k1-k2);
                    temp[1]=k2*(temp[0]-x2.x)+x2.y;
                    if (temp[0]>=min1&&temp[0]<=max1&&temp[0]>=min3&&temp[0]<=max3&&temp[1]>=min2&&temp[1]<=max2&&temp[1]>=min4&&temp[1]<=max4)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }

                }
            }

        }
    }




    /*
    **********************************************************************
    º¯Êý£º
    ²ÎÊý£º
    ·µ»Ø£º
    ¹ŠÄÜ£º
    **********************************************************************
    */

    char suit_angle(float angle,float stangle,float endangl)
    {

        if (stangle>=endangl)
        {
            if ((angle>=stangle&&angle<=360)||(angle>=0&&angle<=endangl))
            {
                return 1;
            }
            else return 0;
        }
        else
        {
            if (angle>=stangle&&angle<=endangl)
            {
                return 1;
            }
            else if (angle==0&&endangl==360)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

    }



    ///ŒÆËãÖ±Ïß¶ÎÓëÔ²»¡µÄœ»µã
    int cacu_line_circle(struct float_point x1, struct float_point x2, float x, float y, float r, float starangle, float endangl,struct float_point returndata[])
    {
        float y1,y2;
        struct float_point point1,point2;
        int m=0;		   //·µ»ØµãµÄžöÊý
        if (x1.x==x2.x)
        {
            returndata[0].x=x1.x;
            float d=r*r-(returndata[0].x-x)*(returndata[0].x-x);
            if (d<0)
            {
                return 0;
            }
            else
            {
                y1=y-sqrt(d);
                y2=y+sqrt(d);
                float may1=fmax(x1.y,x2.y);
                float may2=fmin(x1.y,x2.y);
                if (x==returndata[0].x)
                {
                    float angle1,angle2;

                    angle1=90;
                    angle2=180;
                    if (suit_angle(angle1,starangle,endangl)&&y1>=may2&&y1<=may1)
                    {
                        returndata[m].y=y1;
                        m++;

                    }
                    if (suit_angle(angle2,starangle,endangl)&&y1>=may2&&y1<=may1)
                    {
                        returndata[m].x=returndata[0].x;
                        returndata[m].y=y1;
                        m++;
                    }
                    return m;
                }
                else
                {
                    point1.x=x;
                    point2.y=y;

                    point2.x=returndata[0].x;
                    point2.y=y1;

                    float angle1=angle_pan(point1,point2);


                    if (suit_angle(angle1,starangle,endangl)&&y1>=may2&&y1<=may1)
                    {
                        returndata[m].y=y1;
                        returndata[m].x=returndata[0].x;
                        m++;
                    }
                    point2.y=y2;

                    angle1=angle_pan(point1,point2);

                    if (suit_angle(angle1,starangle,endangl)&&y1>=may2&&y2<=may1)
                    {
                        returndata[m].y=y2;
                        returndata[m].x=returndata[0].x;
                        m++;
                    }

                }

            }
        }
        else
        {
            float k=(x2.y-x1.y)/(x2.x-x1.x);		 //ŒÆËãÐ±ÂÊ
            float temp=1+k*k;
            float b=-k*x2.x+x2.y;
            float temp1=x-k*(b-y);

            float d=(r*r-x*x-(b-y)*(b-y))/temp+pow((temp1/temp),2);
            if (d<0)
            {
                return 0;
            }
            else
            {
                returndata[0].x=temp1/temp-sqrt(d);
                returndata[0].y=k*(returndata[0].x-x2.x)+x2.y;
                returndata[1].x=temp1/temp+sqrt(d);
                returndata[1].y=k*(returndata[1].x-x2.x)+x2.y;
                float max1,max2;
                float min1,min2;
                max1=fmax(x1.x,x2.x);
                min1=fmin(x1.x,x2.x);

                max2=fmax(x1.y,x2.y);
                min2=fmin(x1.y,x2.y);

                point1.x=x;
                point1.y=y;
                if (returndata[0].x>=min1&&returndata[0].x<=max1&&returndata[0].y>=min2&&returndata[0].y<=max2)
                {
                    float angle =angle_pan(point1,returndata[0]);
                    if (suit_angle(angle,starangle,endangl))
                    {
                        m++;
                    }
                }
                if (returndata[1].x>=min1&&returndata[1].x<=max1&&returndata[1].y>=min2&&returndata[1].y<=max2)
                {
                    float angle =angle_pan(point1,returndata[1]);
                    if (suit_angle(angle,starangle,endangl))
                    {
                        returndata[m].x=returndata[1].x;
                        returndata[m].y=returndata[1].y;
                        m++;
                    }
                }

            }
        }
        return m;
    }

    /*
    **********************************************************************
    º¯Êý£ºangle_pan(struct float_point x1,struct float_point x2)
    ²ÎÊý£º
    ·µ»Ø£º
    ¹ŠÄÜ£ºŒÆËã2Ö±ÏßµÄŒÐœÇ
    **********************************************************************
    */
    float angle_pan(struct float_point x1,struct float_point x2)   //x1ÎªÆðµã×ø±ê £¬x2ÎªÖÕµã×ø±ê
    {
        float angle;
        if (x1.x==x2.x)
        {
            if (x2.y-x1.y>0)
            {
                angle=90;
                return angle;
            }
            else if (x2.y-x1.y<0)
            {
                angle=270;
            }
            else
            {
                angle=0;
            }
        }
        else
        {
            float dale_x,dale_y;
            dale_y=x2.y-x1.y;
            dale_x=x2.x-x1.x;
            float k=(dale_y)/(dale_x);
            if (dale_x>0&&dale_y>=0)
            {
                angle=atan(k)*180/pi;
            }
            else if (dale_x<0&&dale_y<=0)
            {
                angle=atan(k)*180/pi+180;
            }

            else if (dale_x>0&&dale_y<0)
            {
                angle=atan(k)*180/pi+360;
            }
            else if (dale_x<0&&dale_y>0)
            {
                angle=atan(k)*180/pi+180;
            }

        }
        return angle;
    }


    /*
    **********************************************************************
    º¯Êý£ºselect_sort
    ²ÎÊý£º
    ·µ»Ø£º
    ¹ŠÄÜ£ºÅÅÐò
    **********************************************************************
    */

    void select_sort(float *x, int n)
    {
        int i, j, min;
            float t;
        for (i=0; i<n-1; i++) /*ÒªÑ¡ÔñµÄŽÎÊý£º0~n-2¹²n-1ŽÎ*/
        {
            min = i; /*ŒÙÉèµ±Ç°ÏÂ±êÎªiµÄÊý×îÐ¡£¬±ÈœÏºóÔÙµ÷Õû*/
            for (j=i+1; j<n; j++)/*Ñ­»·ÕÒ³ö×îÐ¡µÄÊýµÄÏÂ±êÊÇÄÄžö*/
            {
                if (*(x+j) < *(x+min))
                {
                    min = j; /*Èç¹ûºóÃæµÄÊý±ÈÇ°ÃæµÄÐ¡£¬ÔòŒÇÏÂËüµÄÏÂ±ê*/
                }
            }
            if (min != i) /*Èç¹ûminÔÚÑ­»·ÖÐžÄ±äÁË£¬ŸÍÐèÒªœ»»»ÊýŸÝ*/
            {
                t = *(x+i);
                *(x+i) = *(x+min);
                *(x+min) = t;
            }
        }
    }
