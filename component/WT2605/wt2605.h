#ifndef _WT2605_H_
#define _WT2605_H_
#include <stdint.h>

/* 指令表 */
#define WT2605B01_PLAY_INDEX_CMD			0XA2
#define WT2605B01_PLAY_FOLDER_INDEX_CMD		0XA4
#define WT2605B01_PLAY_FOLDER_NAME_CMD		0XA5

#define WT2605B01_PAUSE_CMD					0XAA
#define WT2605B01_STOP_CMD					0XAB

#define WT2605B01_SET_RETURN_CMD			0XBA

#define WT2605B01_RECORD_STOP_CMD			0XDA
#define WT2605B01_RECORD_INDEX_CMD			0XE9
#define WT2605B01_RECORD_FOLDER_INDEX_CMD	0XEB

#define WT2605B01_SET_VOLUME_CMD			0xAE
#define WT2605B01_SET_PLAY_MODE_CMD			0xAF
/* 指令表 */

enum {
	MODE_SIGNAL = 0,
	MODE_SIGNAL_CIRCLE = 1,
	MODE_ALL_CIRCLE = 2,
	MODE_RANDOM = 3
};//播放模式设置

/* 播放操作 */
extern void wt2605b01_play_index(uint16_t index);//根目录索引播放
extern void wt2605b01_play_folder_index(const char *folder, uint16_t index);//指定目录索引播放
extern void wt2605b01_play_folder_name(const char *folder, const char *name);//指定目录内的文件播放

/* 暂停播放 */
extern void wt2605b01_pause(void);//播放暂停
extern void wt2605b01_stop(void);//播放终止

/* 设置不需要返回值 */
extern void wt2605b01_set_noret(void);

/* 录音 */
extern void wt2605b01_record_stop(void);//结束录音
extern void wt2605b01_record_index_start(uint16_t index);//在根目录指定索引录音
extern void wt2605b01_record_folder_index(const char *folder, uint16_t index);//在指定目录索引录音

/* 设置音量 */
extern void wt2605b01_set_volume(uint8_t vol);//指定音量 0 - 30
extern void wt2605b01_set_volume_max(void);//设置最大音量

#endif
