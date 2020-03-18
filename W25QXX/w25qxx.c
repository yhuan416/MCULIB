#include "w25qxx.h"

//4KbytesΪһ��Sector/����
//16������Ϊ1��Block/��
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector 

/* ָ��� */
#define W25X_WriteEnable		(0x06) 
#define W25X_WriteDisable		(0x04) 
#define W25X_ReadStatusReg		(0x05) 
#define W25X_WriteStatusReg		(0x01) 
#define W25X_ReadData			(0x03) 
#define W25X_FastReadData		(0x0B) 
#define W25X_FastReadDual		(0x3B) 
#define W25X_PageProgram		(0x02) 
#define W25X_BlockErase			(0xD8) 
#define W25X_SectorErase		(0x20) 
#define W25X_ChipErase			(0xC7) 
#define W25X_PowerDown			(0xB9) 
#define W25X_ReleasePowerDown	(0xAB) 
#define W25X_DeviceID			(0xAB) 
#define W25X_ManufactDeviceID	(0x90) 
#define W25X_JedecDeviceID		(0x9F)
/* ָ��� */

/* ��ֲ�궨�� */
uint16_t W25QXX_TYPE = W25Q128;//Ĭ����W25Q128

//W25QXXģ�����ʱ����
#include "delay.h"
#define W25QXX_DELAY_US			delay_us

//W25QXXʹ�õ�SPI��д�ӿ�
#include "soft_spi.h"
#define W25QXX_ReadWriteByte	SOFTSPI_ReadWriteByte

//W25QXX��Ƭѡ�ź�
#define W25QXX_CS_H()			PBout(14)=1
#define W25QXX_CS_L()			PBout(14)=0

//GPIO��ʼ���ṹ��
static GPIO_InitTypeDef GPIO_InitStructure;
/* ��ֲ�궨�� */

//��ʼ��SPI FLASH��ƬѡIO��
void W25QXX_Init(void)
{
	/* Ƭѡ���ų�ʼ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//��ʱ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//PB14
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ������
	/* Ƭѡ���ų�ʼ�� */
	
	W25QXX_CS_H();//����Ƭѡ
	W25QXX_TYPE = W25QXX_ReadID();//��ȡFLASH ID
}  

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;                             
	W25QXX_CS_L();							  //ʹ������
	W25QXX_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������    
	byte=W25QXX_ReadWriteByte(0XFF);          //��ȡһ���ֽ�  
	W25QXX_CS_H();                            //ȡ��Ƭѡ     
	return byte;   
} 

//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д
static void W25QXX_Write_SR(uint8_t sr)   
{   
	W25QXX_CS_L();                            //ʹ������   
	W25QXX_ReadWriteByte(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������    
	W25QXX_ReadWriteByte(sr);               //д��һ���ֽ�  
	W25QXX_CS_H();                            //ȡ��Ƭѡ     	      
}

//W25QXXдʹ��	
//��WEL��λ   
static void W25QXX_Write_Enable(void)   
{
	W25QXX_CS_L();                          	//ʹ������   
    W25QXX_ReadWriteByte(W25X_WriteEnable); 	//����дʹ��  
	W25QXX_CS_H();                           	//ȡ��Ƭѡ     	      
}

//W25QXXд��ֹ	
//��WEL����  
static void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS_L();                            //ʹ������   
    W25QXX_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��    
	W25QXX_CS_H();                            //ȡ��Ƭѡ     	      
}

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	W25QXX_CS_L();				    
	W25QXX_ReadWriteByte(W25X_ManufactDeviceID);//���Ͷ�ȡID����	    
	W25QXX_ReadWriteByte(0x00); 	    
	W25QXX_ReadWriteByte(0x00); 	    
	W25QXX_ReadWriteByte(0x00); 	 			   
	Temp|=W25QXX_ReadWriteByte(0xFF)<<8;//��ȡ�����ֽڵ�ID
	Temp|=W25QXX_ReadWriteByte(0xFF);	 
	W25QXX_CS_H();				    
	return Temp;
}

//�ȴ�����
static void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);// �ȴ�BUSYλ���
}

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	W25QXX_CS_L();                            		//ʹ������   
    W25QXX_ReadWriteByte(W25X_ReadData);         	//���Ͷ�ȡ����   
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr)>>16));//����24bit��ַ    
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    W25QXX_ReadWriteByte((uint8_t) (ReadAddr));   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=W25QXX_ReadWriteByte(0XFF);   	//ѭ��������
    }
	W25QXX_CS_H();  				    	      
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���	 
static void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  			//SET WEL дʹ��
	W25QXX_CS_L();                            			//ʹ������   
    W25QXX_ReadWriteByte(W25X_PageProgram);      		//����дҳ����   
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr)>>16));	//����24bit��ַ    
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    W25QXX_ReadWriteByte((uint8_t) (WriteAddr));   
    for(i=0;i<NumByteToWrite;i++)W25QXX_ReadWriteByte(pBuffer[i]);//ѭ��д���� 
	W25QXX_CS_H();                            			//ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   				//�ȴ�д�����
}

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
static void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t pageremain;	   
	pageremain=256-(WriteAddr%256); 	//��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);//ֱ����ҳд������
		if(NumByteToWrite==pageremain)break;//д�������
	 	else
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256) pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}	    
}

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535) 
static uint8_t W25QXX_BUFFER[4096];//����������		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;//������ַ(/4096)
	uint16_t secoff;//�����ڵ�ƫ��
	uint16_t secremain;//����ʣ��ռ�Ĵ�С
 	uint16_t i;    
	uint8_t* W25QXX_BUF=W25QXX_BUFFER;//�������׵�ַ
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
	//DEBUG("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
	if(NumByteToWrite<=secremain) secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0
		   	pBuffer+=secremain;  				//ָ��ƫ��
			WriteAddr+=secremain;				//д��ַƫ��	   
		   	NumByteToWrite-=secremain;			//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;//��һ����������д����
			else secremain=NumByteToWrite;		//��һ����������д����
		}
	}
}

//��������оƬ
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                 	//SET WEL дʹ��
    W25QXX_Wait_Busy();   					//�ȴ�æ�ź�
  	W25QXX_CS_L();                         	//ʹ������   
    W25QXX_ReadWriteByte(W25X_ChipErase);  	//����Ƭ����������������Զ���ֹдʹ��  
	W25QXX_CS_H();                         	//ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   	//�ȴ�оƬ��������
}  

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  	//SET WEL дʹ��
    W25QXX_Wait_Busy();   						//�ȴ�æ
  	W25QXX_CS_L();                            	//ʹ������   
    W25QXX_ReadWriteByte(W25X_SectorErase);     //������������ָ�� 
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr)>>16));//����24bit��ַ    
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    W25QXX_ReadWriteByte((uint8_t) (Dst_Addr));  
	W25QXX_CS_H();                            	//ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				   		//�ȴ��������
}

//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS_L();                        	//ʹ������   
    W25QXX_ReadWriteByte(W25X_PowerDown); 	//���͵�������  
	W25QXX_CS_H();                        	//ȡ��Ƭѡ     	      
    W25QXX_DELAY_US(3);                   	//�ȴ�TPD  
}   

//����
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS_L();                            		//ʹ������   
    W25QXX_ReadWriteByte(W25X_ReleasePowerDown);	//���ͻ���ָ��    
	W25QXX_CS_H();                            		//ȡ��Ƭѡ     	      
    W25QXX_DELAY_US(3);                            	//�ȴ�TRES1
}
