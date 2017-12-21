#include <stdio.h>
#include <math.h>

/*
float bezierCurve(float x[] , float y[], float pos) // pos is 0.0 ~ 1.0
{
    float xu = 0.0 , yu = 0.0 , u = 0.0 ;
    int i = 0 ;
    for(u = 0.0 ; u <= pos ; u += 0.0005) // resolution : 0.001
    {
        yu = pow(1-u,3)*y[0]+3*u*pow(1-u,2)*y[1]+3*pow(u,2)*(1-u)*y[2]
            +pow(u,3)*y[3];
    }

    return yu;
}
*/
int getPt( int n1 , int n2 , float perc )
{
    int diff = n2 - n1;
    return n1 + ( diff * perc );
}    

float bezierCurve(float x[] , float y[], float pos) // pos is 0.0 ~ 1.0
{
    float ret = 0.0;
    float i = 0.0;
    int ya, yb, yc, ym, yn = 0;

    for( i = 0.0; i < pos ; i += 0.01 )
    {
        // The Green Line
        ya = getPt( y[0], y[1] , i );
        yb = getPt( y[1] , y[2] , i );
        yc = getPt( y[2] , y[3] , i );
    
        // The Blue Line
        ym = getPt( ya , yb , i );
        yn = getPt( yb , yc , i );
    
        // The Black Dot
        ret = getPt( ym , yn , i );
    }   

    return ret;
}

void main(void) {
    printf("test\n");
    //cubic-bezier,  1,.01,.62,.93
    float x[4] , y[4];
    x[0] = 0.0;
    y[0] = 0.0;

    x[1] = 5.0;
    y[1] = 1.0;
    
    x[2] = 5.0;
    y[2] = 8.0;
    
    x[3] = 10.0;
    y[3] = 10.0;
    
    int second = 60*60; // 1 hour
    int interval = second/512; //7sec
    int runningSecond = 0;

    float timePos = 0.0;
    float ret = 0.0;

    for (; runningSecond < second ;)
    {
        ret = bezierCurve(x, y, timePos);
        //printf("%f = %f (%d) \n",timePos, ret, (int)(ret*255));
        //printf("%f,%f,%d\n",timePos, ret, (int)((ret*255)/10));   
        printf("%d\n",(int)((ret*255)/10));
        runningSecond += 14;
        timePos = (float)runningSecond/(float)second;
    }

/*
    float ret = bezierCurve(x,y, 0.1);
    printf("0.1 = %f\n", ret);

    ret = bezierCurve(x,y, 0.2);
    printf("0.2 = %f\n", ret);

    ret = bezierCurve(x,y, 0.3);
    printf("0.3 = %f\n", ret);   
 
    ret = bezierCurve(x,y, 0.5);
    printf("0.5 = %f\n", ret);   

    ret = bezierCurve(x,y, 0.7);
    printf("0.7 = %f\n", ret);   
   
    ret = bezierCurve(x,y, 1.0);
    printf("1.0 = %f\n", ret);   
   */
}
