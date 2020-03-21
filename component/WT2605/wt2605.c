#include "wt2605.h"

#include <string.h>
#include <stdio.h>

//发送指令
static void send_cmd_data(uint8_t *cmd)
{
	uint8_t i;
	uint8_t len = cmd[1] + 2;//获取指令总长度
	
	for(i = 0; i < len; i++)
	{
		printf("%1c", cmd[i]);
	}
}

//计算累加和
static uint8_t get_cmd_sum(uint8_t *cmd)
{
	uint8_t i;
	uint8_t len = cmd[1];//获取指令长度
	uint8_t ret = 0;//返回值
	
	//计算累加和
    for(i = 1; i < len; i++)
	{
		ret += cmd[i];
	}

    return ret;//返回结果
}

//播放指定索引的音乐
void wt2605b01_play_index(uint16_t index)
{
	uint8_t cmd[7];
	cmd[0] = 0x7E;//开始码
	cmd[1] = 0x05;//长度
	cmd[2] = WT2605B01_PLAY_INDEX_CMD;//索引播放指令
	cmd[3] = index >> 8;//索引高位
	cmd[4] = index;//索引低位
	cmd[5] = get_cmd_sum(cmd);//计算校验和
	cmd[6] = 0xEF;//结束码
	
	//发送命令
	send_cmd_data(cmd);
}

//播放文件夹索引
void wt2605b01_play_folder_index(const char *folder, uint16_t index)
{
	uint8_t cmd[12];
	cmd[0] = 0x7E;//开始码
	cmd[1] = 0x0A;//长度
	cmd[2] = WT2605B01_PLAY_FOLDER_INDEX_CMD;//目录索引播放指令

	cmd[3] = folder[0];
	cmd[4] = folder[1];
	cmd[5] = folder[2];
	cmd[6] = folder[3];
	cmd[7] = folder[4];//文件夹名

	cmd[8] = index >> 8;//索引高位
	cmd[9] = index;//索引低位

	cmd[10] = get_cmd_sum(cmd);//计算校验和
	cmd[11] = 0xEF;//结束码

	//发送命令
	send_cmd_data(cmd);
}

//播放文件夹内指定文件
void wt2605b01_play_folder_name(const char *folder, const char *name)
{
	uint8_t cmd[13];
	cmd[0] = 0x7E;//开始码
	cmd[1] = 0x0A;//长度
	cmd[2] = WT2605B01_PLAY_FOLDER_NAME_CMD;//目录文件名播放指令
	
	cmd[3] = folder[0];
	cmd[4] = folder[1];
	cmd[5] = folder[2];
	cmd[6] = folder[3];
	cmd[7] = folder[4];//5位文件夹名

	cmd[8] = name[0];
	cmd[9] = name[1];
	cmd[10] = name[2];//3位文件名

	cmd[11] = get_cmd_sum(cmd);//计算校验和
	cmd[12] = 0xEF;//结束码

	//发送命令
	send_cmd_data(cmd);
}

//暂停播放
void wt2605b01_pause(void)
{
	uint8_t cmd[5];
	cmd[0] = 0x7E;
	cmd[1] = 0x03;
	cmd[2] = WT2605B01_PAUSE_CMD;//暂停命令
	cmd[3] = 0xAD;//校验
	cmd[4] = 0xEF;
	
	//发送命令
	send_cmd_data(cmd);
}

//停止播放
void wt2605b01_stop(void)
{
	uint8_t cmd[5];
	cmd[0] = 0x7E;
	cmd[1] = 0x03;
	cmd[2] = WT2605B01_STOP_CMD;//停止命令
	cmd[3] = 0xAE;//校验
	cmd[4] = 0xEF;
	
	//发送命令
	send_cmd_data(cmd);
}

//设置不需要返回信息
void wt2605b01_set_noret(void)
{
	uint8_t cmd[6];
	cmd[0] = 0x7E;//起始
	cmd[1] = 0x04;//长度
	cmd[2] = WT2605B01_SET_RETURN_CMD;//设置是否需要返回值指令
	cmd[3] = 0x00;//不需要返回值
	cmd[4] = 0xBE;//校验
	cmd[5] = 0xEF;//结束
	
	//发送命令
	send_cmd_data(cmd);
}

//停止录音
void wt2605b01_record_stop(void)
{
	uint8_t cmd[5];
	cmd[0] = 0x7E;//起始
	cmd[1] = 0x03;//长度
	cmd[2] = WT2605B01_RECORD_STOP_CMD;//停止录音
	cmd[3] = 0xDD;//校验
	cmd[4] = 0xEF;//结束
	
	//发送命令
	send_cmd_data(cmd);
}

//指定索引录音
void wt2605b01_record_index_start(uint16_t index)
{
	uint8_t cmd[8];
	cmd[0] = 0x7E;//起始
	cmd[1] = 0X06;//长度
	cmd[2] = WT2605B01_RECORD_INDEX_CMD;//索引录音

	cmd[3] = 0x01;//索引在SD卡

	cmd[4] = index >> 8;
	cmd[5] = index;//索引值
	
	cmd[6] = get_cmd_sum(cmd);//计算校验和
	cmd[7] = 0xEF;//结束
	
	//发送命令
	send_cmd_data(cmd);
}

//指定目录索引录音
void wt2605b01_record_folder_index(const char *folder, uint16_t index)
{
	uint8_t cmd[13];
	cmd[0] = 0x7E;//起始
	cmd[1] = 0X0B;//长度
	cmd[2] = WT2605B01_RECORD_FOLDER_INDEX_CMD;//索引录音
	
	cmd[3] = 0x01;//索引在SD卡
	
	cmd[4] = folder[0];
	cmd[5] = folder[1];
	cmd[6] = folder[2];
	cmd[7] = folder[3];
	cmd[8] = folder[4];//文件夹名
	
	cmd[9] = index>>8;
	cmd[10] = index;//索引

	cmd[11] = get_cmd_sum(cmd);//计算校验和
	cmd[12] = 0xEF;//结束
	
	//发送命令
	send_cmd_data(cmd);
}

//设置音量
void wt2605b01_set_volume(uint8_t vol)
{
	uint8_t	cmd[13];

	//判断音量是否超出界限
	if(vol>30)vol=30;
	if(vol<0)vol=0;

	cmd[0] = 0x7E;//开始

	cmd[1] = 0x04;//长度
	cmd[2] = WT2605B01_SET_VOLUME_CMD;//设置音量
	cmd[3] = vol;//音量
	cmd[4] = get_cmd_sum(cmd);//计算校验和

	cmd[5] = 0XEF;//结束符

	//发送命令
	send_cmd_data(cmd);
}

//设置最大音量
void wt2605b01_set_volume_max(void)
{
	wt2605b01_set_volume(30);
}

//设置播放模式
void wt2605b01_set_play_mode(uint8_t mode)
{
	uint8_t	cmd[13];

	cmd[0] = 0x7E;//开始

	cmd[1] = 0x04;//长度
	cmd[2] = WT2605B01_SET_PLAY_MODE_CMD;//设置播放模式
	cmd[3] = mode;//模式
	cmd[4] = get_cmd_sum(cmd);//计算校验和

	cmd[5] = 0XEF;//结束符

	//发送命令
	send_cmd_data(cmd);
}
