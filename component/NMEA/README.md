**2020.03.22**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**NMEA0183协议解析**

*根据开源代码修改*

# 功能介绍
实现NMEA0183数据包的解析  
支持GPS+BD双系统数据

# 结构体说明
typedef struct {
```
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
    uint8_t gpssta;             //定位信息状态 'A' 数据有效/'V' 数据无效     RMC
    uint32_t latitude;			//纬度 度 保存5位小数(*100000)              RMC
	uint8_t nshemi;				//北纬/南纬,N:北纬;S:南纬			        RMC
	uint32_t longitude;		    //经度 度 保存5位小数(*100000)              RMC
	uint8_t ewhemi;				//东经/西经,E:东经;W:西经                   RMC

    //精度因子
    uint16_t pdop;              //位置精度因子 *10                  GPGSA/BDGSA
    uint16_t hdop;              //水平精度因子 *10                  GGA
    uint16_t vdop;              //垂直精度因子 *10                  GPGSA/BDGSA

    //其他信息
    uint16_t altitude;			//海拔高度 *10                  GGA
	uint16_t speed;				//地面速率 *1000 (km/h)          VTG

```
} nmea_t;//nmea数据

# 函数接口说明
> void NMEA_Analysis(nmea_t *gpsx, uint8_t *buf);

对数据包进行解析
> void NMEA_GGA_Analysis(nmea_t *gpsx, uint8_t *buf);

解析--GGA语句,获取用于定位的卫星数/定位状态/海拔/水平精度因子
> void NMEA_RMC_Analysis(nmea_t *gpsx, uint8_t *buf);

解析--RMC语句,获取utc时间/定位信息
> void NMEA_VTG_Analysis(nmea_t *gpsx, uint8_t *buf);

解析--VTG语句,获取接收机的时间
> void NMEA_GPGSA_Analysis(nmea_t *gpsx, uint8_t *buf);
> void NMEA_BDGSA_Analysis(nmea_t *gpsx, uint8_t *buf);

解析--GSA语句,获取各个系统用于定位的卫星编号
> void NMEA_GPGSV_Analysis(nmea_t *gpsx, uint8_t *buf);
> void NMEA_BDGSV_Analysis(nmea_t *gpsx, uint8_t *buf);

解析--GSV语句,获取各个系统当前可见卫星
