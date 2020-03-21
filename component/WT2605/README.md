**2020.3.211**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**WT2605B01 串口模块简易驱动**

# 功能介绍
wt2605b01 音频模块串口驱动

# 函数接口

## 播放音频
> void wt2605b01_play_index(uint16_t index);  

根目录索引播放

> void wt2605b01_play_folder_index(const char *folder, uint16_t index);  

指定目录索引播放

> void wt2605b01_play_folder_name(const char *folder, const char *name);  

指定目录内的文件播放

> void wt2605b01_pause(void);  

播放暂停
> void wt2605b01_stop(void);  

播放终止

---

## 录制音频
> void wt2605b01_record_stop(void);  

结束录音

> void wt2605b01_record_index_start(uint16_t index);  

在根目录指定索引录音

> void wt2605b01_record_folder_index(const char *folder, uint16_t index);  

在指定目录索引录音

---

## 模块配置
> void wt2605b01_set_noret(void);  

设置不需要返回值

> void wt2605b01_set_volume(uint8_t vol);  

指定音量 0 - 30
> void wt2605b01_set_volume_max(void);  

设置最大音量

