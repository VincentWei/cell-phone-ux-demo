#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#   define snprintf _snprintf
#endif

static int get_dot_pos(char *result)
{
    int i;
    if (result == NULL)
        return -1;
    for (i=0;i<strlen(result);i++)
        if (result[i] == '.')
            return i;
    return -1;
}

static void calc_result_adjust(char *src,int len,char *dst,int num)
{
    int i = len,j = 0;
    int dot_pos = get_dot_pos(src);
    if (dot_pos <= 0)
        return;
    memcpy(dst,src,len);
    while(i > 0)
    {
        if (src[i - 1] == '9')
        {
            j++;
        }
        else
        {
            break;
        }
        i--;
    }
    if (j >= num)
    {
        if (src[i - 1] >='0' && src[i - 1] <= '8')
        {
            dst[i - 1] = src[i - 1] + 1;
            dst[i] = '\0'; 
        }
    }
    return ;
}

static int phone_calc_add(double left,double right,char *result,int len)
{
    double val = left + right;

    snprintf(result,len,"%.*lf",len,val);
    return strlen(result);
}

static int phone_calc_sub(double left,double right,char *result,int len)
{
    double val = left - right;

    snprintf(result,len,"%.*lf",len,val);
    return strlen(result);
}

static int phone_calc_mul(double left,double right,char *result,int len)
{
    double val = left*right;

    snprintf(result,len,"%.*lf",len,val);
    return strlen(result);
}

static int phone_calc_div(double left,double right,char *result,int len)
{
    double val = left/right;

    snprintf(result,len,"%.*lf",len,val);
    return strlen(result);
}
int phone_calc_compute(const char* left,char op,const char *right,char *result,size_t len)
{
    int i;
    char *endptr;
    double d_l;
    double d_r;
    int act_len;
    int dot_pos;

    if (left == NULL || right == NULL || result == NULL)
        return -1;

    d_l = strtod(left,&endptr);
    if (left == endptr)
        d_l = 0;
    d_r = strtod(right,&endptr);
    if (right == endptr)
        d_r = 0;

    switch(op)
    {
        case '+':
            act_len = phone_calc_add(d_l,d_r,result,len);
            break;
        case '-':
            act_len = phone_calc_sub(d_l,d_r,result,len);
            break;
        case 'x':
            act_len = phone_calc_mul(d_l,d_r,result,len);
            break;
        case '/':
            act_len = phone_calc_div(d_l,d_r,result,len);
            break;
        default:
            strncpy(result,"0",len);
            return -1;
    }

    calc_result_adjust(result,act_len,result,6);

    dot_pos = get_dot_pos(result);
    if (dot_pos >= 0)
    {
        for (i=act_len-1;i>=dot_pos ;i--)
        {
            if (result[i] == '0'||result[i] == '.')
            {
                result[i] = '\0';
            }
            else
            {
                break;
            }
        }
        if (i < 0)
            result[0] = '0';
    }
    return 0;
}

