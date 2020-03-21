#ifndef _NMEA_H_
#define _NMEA_H_

#include <stdint.h>

typedef struct {
    uint8_t svid;		//卫星编号
	uint8_t ele;	    //卫星仰角 单位是度
	uint16_t az;	    //卫星方位角 单位是度
	uint8_t cn;		    //信噪比	
} nmea_slmsg_t;//卫星信息

typedef struct {
    uint8_t year;   //年 不包括世纪
    uint8_t month;  //月
    uint8_t date;   //日
    uint8_t hour;   //时
    uint8_t min;    //分
    uint8_t sec;    //秒
} nmea_utc_t;//nmea 时间

typedef struct {
    //时间信息
    nmea_utc_t utc;             //utc时间                   RMC

    //卫星信息
    nmea_slmsg_t gpslmsg[12];   //GPS可见卫星信息           GPGSV
    nmea_slmsg_t bdslmsg[12];   //BD可见卫星信息            BDGSV
    uint8_t gpsvnum;            //GPS可见卫星数             GPGSV
    uint8_t bdsvnum;            //BD可见卫星数              BDGSV

    uint8_t gppossl[12];        //GPS用于定位的卫星编号     GPGSA
    uint8_t bdpossl[12];        //BD用于定位的卫星编号      BDGSA
    
    uint8_t posslnum;           //用于定位的卫星数           GGA
    uint8_t fixmode;            //定位状态 : 2D/3D          GGA

    //定位信息
    uint8_t gpssta;             //定位信息状态 'A' 数据有效/'V' 数据无效    RMC
    uint32_t latitude;			//纬度 分扩大100000倍,实际要除以100000      RMC
	uint8_t nshemi;				//北纬/南纬,N:北纬;S:南纬			        RMC
	uint32_t longitude;		    //经度 分扩大100000倍,实际要除以100000      RMC
	uint8_t ewhemi;				//东经/西经,E:东经;W:西经                   RMC

    //精度因子
    uint16_t pdop;              //位置精度因子 0~500,对应实际值0~50.0           GPGSA/BDGSA
    uint16_t hdop;              //水平精度因子 0~500,对应实际值0~50.0           GGA
    uint16_t vdop;              //垂直精度因子 0~500,对应实际值0~50.0           GPGSA/BDGSA

    //其他信息
    uint16_t altitude;			//海拔高度,放大了10倍	        GGA
	uint16_t speed;				//地面速率,放大了1000倍	        VTG
} nmea_t;//nmea数据

/* GGA */
extern void NMEA_GGA_Analysis(nmea_t *gpsx, uint8_t *buf);

/* GSA */
extern void NMEA_GPGSA_Analysis(nmea_t *gpsx, uint8_t *buf);
extern void NMEA_BDGSA_Analysis(nmea_t *gpsx, uint8_t *buf);

/* GSV */
extern void NMEA_GPGSV_Analysis(nmea_t *gpsx, uint8_t *buf);
extern void NMEA_BDGSV_Analysis(nmea_t *gpsx, uint8_t *buf);

/* RMC */
extern void NMEA_RMC_Analysis(nmea_t *gpsx, uint8_t *buf);

/* VTG */
extern void NMEA_VTG_Analysis(nmea_t *gpsx, uint8_t *buf);

/* ALL */
extern void NMEA_Analysis(nmea_t *gpsx, uint8_t *buf);

#endif
