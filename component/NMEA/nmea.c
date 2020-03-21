#include "nmea.h"

#include <string.h>

//从buf里面得到第cx个逗号所在的位置
//返回值:0~0XFE,代表逗号所在位置的偏移.
//       0XFF,代表不存在第cx个逗号							  
static uint8_t NMEA_Comma_Pos(uint8_t *buf, uint8_t cx)
{
    uint8_t *p;
    if(buf == NULL) return 0XFF;//断言

	p = buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n函数
//返回值:m^n次方.
static uint32_t NMEA_Pow(uint8_t m, uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
static int NMEA_Str2num(uint8_t *buf, uint8_t*dx)
{
	uint8_t *p=buf;
	uint32_t ires=0,fres=0;
	uint8_t ilen=0,flen=0,i;
	uint8_t mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*'))break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//最多取5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}

//分析GGA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GGA_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;

    if(buf == NULL) return;

	p1=(uint8_t *)strstr((const char *)buf,"GGA");          //GGA
    if(p1 == NULL) return;

    posx=NMEA_Comma_Pos(p1,6);								//得到定位类型
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);	
    
	posx=NMEA_Comma_Pos(p1,7);								//得到用于定位的卫星数
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 

    posx=NMEA_Comma_Pos(p1,8);								//得到水平精度因子
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx); 
	
    posx=NMEA_Comma_Pos(p1,9);								//得到海拔高度
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}
//分析GPGSA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGSA_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx; 
	uint8_t i;

    if(buf == NULL) return;

	p1=(uint8_t *)strstr((const char *)buf,"GPGSA");        //GPGSA
    if(p1 == NULL) return;
	
    for(i=0;i<12;i++)										//得到GPS系统定位卫星编号
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->gppossl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}
	
    posx=NMEA_Comma_Pos(p1,15);								//得到PDOP位置精度因子
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	
    posx=NMEA_Comma_Pos(p1,17);								//得到VDOP位置精度因子
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}
//分析BDGSA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_BDGSA_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx; 
	uint8_t i;

    if(buf == NULL) return;

	p1=(uint8_t *)strstr((const char *)buf,"BDGSA");        //BDGSA
    if(p1 == NULL) return;
	
    for(i=0;i<12;i++)										//得到北斗系统定位卫星编号
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->bdpossl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}
	
    posx=NMEA_Comma_Pos(p1,15);								//得到PDOP位置精度因子
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	
    posx=NMEA_Comma_Pos(p1,17);								//得到VDOP位置精度因子
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}
//分析GPGSV信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGSV_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p,*p1,dx;
	uint8_t len,i,j,slx=0;
	uint8_t posx;

    if(buf == NULL) return;

	p=buf;
	p1=(uint8_t *)strstr((const char *)p,"GPGSV");          //GPGSV
    if(p1 == NULL) return;

	len=p1[7]-'0';//得到GPGSV的条数
	
    posx=NMEA_Comma_Pos(p1,3); 					            //得到可见卫星总数
	if(posx!=0XFF)gpsx->gpsvnum+=NMEA_Str2num(p1+posx,&dx);
	
    for(i=0;i<len;i++)
	{
		p1=(uint8_t *)strstr((const char *)p,"GPGSV");
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->gpslmsg[slx].svid=NMEA_Str2num(p1+posx,&dx);	//得到卫星编号
			else break; 
			
            posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->gpslmsg[slx].ele=NMEA_Str2num(p1+posx,&dx);     //得到卫星仰角 
			else break;
			
            posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->gpslmsg[slx].az=NMEA_Str2num(p1+posx,&dx);      //得到卫星方位角
			else break; 
			
            posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->gpslmsg[slx].cn=NMEA_Str2num(p1+posx,&dx);	    //得到卫星信噪比
			else break;
			
            slx++;
		}

 		p=p1+1;//切换到下一个GPGSV信息
	}   
}
//分析BDGSV信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_BDGSV_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p,*p1,dx;
	uint8_t len,i,j,slx=0;
	uint8_t posx;   	 
	
    if(buf == NULL) return;

    p=buf;
	p1=(uint8_t *)strstr((const char *)p,"BDGSV");          //BDGSV
    if(p1 == NULL) return;

	len=p1[7]-'0';//得到BDGSV的条数
	
    posx=NMEA_Comma_Pos(p1,3); 					            //得到可见卫星总数
	if(posx!=0XFF)gpsx->bdsvnum+=NMEA_Str2num(p1+posx,&dx);
	
    for(i=0;i<len;i++)
	{
		p1=(uint8_t *)strstr((const char *)p,"BDGSV");      //获取下一条语句

		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->bdslmsg[slx].svid=NMEA_Str2num(p1+posx,&dx);	//得到卫星编号
			else break; 
			
            posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->bdslmsg[slx].ele=NMEA_Str2num(p1+posx,&dx);     //得到卫星仰角 
			else break;
			
            posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->bdslmsg[slx].az=NMEA_Str2num(p1+posx,&dx);      //得到卫星方位角
			else break; 
			
            posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->bdslmsg[slx].cn=NMEA_Str2num(p1+posx,&dx);	    //得到卫星信噪比
			else break;
			
            slx++;
		}

 		p=p1+1;//切换到下一个BDGSV信息
	}   
}
//解析RMC信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_RMC_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;     
	uint32_t temp;	   
	float rs;

    if(buf == NULL) return;

	p1=(uint8_t *)strstr((const char *)buf,"RMC");          //"$XXRMC",判断RMC
    if(p1 == NULL) return;

	posx=NMEA_Comma_Pos(p1,1);					            //得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}

    posx=NMEA_Comma_Pos(p1,2);								//得到定位状态
    if(posx!=0XFF)
    {
        gpsx->gpssta=*(p1+posx);                            // (A/V)
    }

	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//得到°
		rs=temp%NMEA_Pow(10,dx+2);				//得到'		 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为° 
	}

	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);

 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//得到°
		rs=temp%NMEA_Pow(10,dx+2);				//得到'		 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为° 
	}

	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);
    
	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=temp%100;
	} 
}
//解析VTG信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_VTG_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	uint8_t *p1,dx;			 
	uint8_t posx;

    if(buf == NULL) return;

	p1=(uint8_t*)strstr((const char *)buf,"VTG");           //获取VTG
    if(p1 == NULL) return;

	posx=NMEA_Comma_Pos(p1,7);								//得到地面速率
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//确保扩大1000倍
	}
}

//提取NMEA-0183信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_Analysis(nmea_t *gpsx, uint8_t *buf)
{
	NMEA_GGA_Analysis(gpsx, buf);

    NMEA_GPGSA_Analysis(gpsx, buf);
    NMEA_BDGSA_Analysis(gpsx, buf);

    NMEA_GPGSV_Analysis(gpsx, buf);
    NMEA_BDGSV_Analysis(gpsx, buf);

    NMEA_RMC_Analysis(gpsx, buf);
    NMEA_VTG_Analysis(gpsx, buf);
}
