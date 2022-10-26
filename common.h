
#pragma once//只编译一次防止重复包含


#ifndef BODYCAMDLL_API
#ifdef BODYCAMDLL_EXPORTS
#define BODYCAMDLL_API  __declspec(dllexport)
#else
#define BODYCAMDLL_API   __declspec(dllimport)
#endif
#define  BCHandle  void
#endif

#define WINAPI	__stdcall
typedef enum
{
	PARATYPE_LISTSUM=1,
	PARATYPE_DISK,
}PARA_TYPE;
typedef int (WINAPI *PFCALLBACK)(/*PARA_TYPE*/int paratype,int paraval) ;



#if 0
#define BC_SUCCESS	0
#define BC_FAILED	-1
#else
#define BC_SUCCESS	1
#define BC_FAILED	0
#endif

typedef enum BODYCAMDLL_API
{
	FS_FAT32=1,
	FS_EXFAT,
}FSTYPE_E;

#ifndef DEV_INFO
#if 0
typedef struct //DYCAMDLL_API
{
	char cSerial[7];   /*执法记录仪产品序号，不可为空*/
	char userNo[6];    /*执法记录仪使用者警号，不可为空*/
	char userName[32]; /*执法记录仪使用者姓名，管理系统使用警号关联时可为空*/
	char unitNo[12];   /*执法记录仪使用者单位编号，管理系统使用警号关联时可为空*/
	char unitName[32]; /*执法记录仪使用者单位名称，管理系统使用警号关联时可为空*/
}ZFY_INFO;
#else
typedef struct /*DYCAMDLL_API*/
{
	char cSerial[8];   /*执法记录仪产品序号(不可设置，只能获取)，获取时不可为空*/
	char userNo[7];    /*执法记录仪使用者警号，不可为空*/
	char userName[33]; /*执法记录仪使用者姓名，管理系统使用警号关联时可为空*/
	char unitNo[13];   /*执法记录仪使用者单位编号，管理系统使用警号关联时可为空*/
	char unitName[33]; /*执法记录仪使用者单位名称，管理系统使用警号关联时可为空*/
}ZFY_INFO;
#endif
#define DEV_INFO  ZFY_INFO
#endif

typedef struct
{
	BCHandle *dev;    //设备操作句柄
	int ListIndex;	// 设备在列表中的索引值
	int UsbBusNum;	//USB 总线号
	int bUDisk;		// 是否已经形成U盘。1-已形成U盘，0-未形成U盘
	int bNewDevFlag;	// 是否新加入的设备
	char SN[16];         // 设备ID，设备唯一性ID,不可更改
	char IDCode[8];	// 设备识别码。设备类型编码，不可更改
	char ExFlag[16];
	short Pid;
	short Vid;
}DEVLIST_INFO;

typedef enum
{
	UMS_ACT_BEGIN = -1,
	UMS_ACT_GET,
	UMS_ACT_SET,
	UMS_ACT_SGET,			// Set and Get		
	UMS_ACT_ADD,
	UMS_ACT_DEL,
	UMS_ACT_CLEAR,
	UMS_ACT_END
} UMS_AC_E;
typedef enum
{
	VAL_TYPE_VOID=0,
	VAL_TYPE_INT,
	VAL_TYPE_STR,
	VAL_TYPE_FLOAT,
	VAL_TYPE_DOUBLE,
}VAl_TYPE_E;
typedef struct
{
	VAl_TYPE_E valtype;
	char keystr[32];
	char* valstr;
}KEY_VAL_S;
#define KEYVAL_MAX	16
typedef struct
{
	int act;
	int keysum;
	char section[32];
	KEY_VAL_S keyval[KEYVAL_MAX];
}UMS_CMD_S;

#include <QObject>
typedef struct
{
    QString username;
    QString registration_time;
    QString last_online_time;
    int battery_lvl;
    int download_lvl;
}DB_DEV_INFO;

typedef struct
{
    QString dev_serial_number;
    QString record_time;
    QString download_time;
    QString local_url;
    QString size;
}DB_FILE_INFO;


#define IN
#define OUT

