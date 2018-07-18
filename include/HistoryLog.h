#ifndef __HISTORY_LOG_H
#define __HISTORY_LOG_H

#define HISTORY_LOG_LENGTH			   						6
#define MAX_HISTORY_LOG_NUM			   						30
#define MAX_ITEM_LOG_FRAME     								10
#define MAX_LOG_FRAME     									16

void SaveAlarmLog(unsigned char Name, unsigned char Case, unsigned char Zone);
#endif

