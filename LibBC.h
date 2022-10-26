
#pragma once//只编译一次防止重复包含
#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define INIT_DEV_WITH_LIST
#define INIT_DEVLIST_WITH_PTHREAD

/*********************************************************************************************************************
*
*  以下为执法仪全功能的相关函数
*
**********************************************************************************************************************/


/*************************************************
*函数说明:  检测设备是否存在，通过查看PID和VID是否符合DLL的管理范围
*输入参数:   无 	    			
*输出参数:  无	   
*返回值     :  成功:BC_SUCCESS,  失败:BC_FAILED	
*************************************************/
BODYCAMDLL_API int BC_CheckDevExist(void);

/*************************************************
*函数说明:  设置DLL的日志是否开启
*输入参数:   bEnable: 1-开启,0-关闭 	    			
*输出参数:  无	   
*返回值     :  无
*************************************************/
BODYCAMDLL_API void BC_SetLibLog(IN int bEnable);

/*************************************************
*函数说明: 检测设备操作句柄是否存在
*输入参数:    dev: 设备的操作句柄	
*输出参数:   无
*返回值     :   存在:BC_SUCCESS,  不存在:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_CheckDevHandle(IN BCHandle *dev);


/*=============================================================================
单设备操作函数
==============================================================================*/

#ifndef INIT_DEV_WITH_LIST

/*************************************************
*函数说明:    探测设备是否存在，作为操作的第一个接口函数(已经使用BC_ProbeDevEx代替)，直接从DLL中获取IDCode
*输入参数:   无			
*输出参数:   IDCode:  设备的识别码(5个字节)，同类型设备的IDCode相同
*返回值     :   成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_ProbeDev(OUT char *IDCode);
 
/*************************************************
*函数说明:   初始化设备，在探测到设备成功后使用(已经使用BC_InitDevEx代替)，即BC_ProbeDev之后调用
*输入参数:   IDCode:  设备的识别码(5个字节)，同类型设备的IDCode相同			
*输出参数:   无
*返回值     :   成功:设备的操作句柄(BCHandle *),  失败:NULL
*************************************************/
BODYCAMDLL_API BCHandle * BC_InitDev(IN char *IDCode);

/*************************************************
*函数说明:   释放设备,在探测到设备成功后使用(已经使用BC_UnInitDevEx代替)，即BC_ProbeDev之后调用
*输入参数:   dev: 设备的操作句柄	，BC_InitDev成功的返回值
*输出参数:   无
*返回值     :   无
*************************************************/
BODYCAMDLL_API void  BC_UnInitDev(IN BCHandle *dev);


/*************************************************
*函数说明:   探测设备是否存在，作为操作的第一个接口函数，从设备中获取IDCode，
				与BC_ProbeDev的区别是BC_ProbeDev是从DLL中获取IDCode
*输入参数:   无
*输出参数:   IDCode:  设备的识别码(5个字节)，同类型设备的IDCode相同
*返回值     :  成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_ProbeDevEx(OUT char *IDCode);


/*************************************************
*函数说明:   初始化设备，在探测到设备成功后使用，即BC_ProbeDevEx之后调用
*输入参数:   IDCode:  设备的识别码(5个字节)，同类型设备的IDCode相同		
*输出参数:   无
*返回值     :  成功:设备的操作句柄(BCHandle *),  失败:NULL
*************************************************/
BODYCAMDLL_API BCHandle * BC_InitDevEx(IN char *IDCode);


/*************************************************
*函数说明:   释放设备,在探测到设备成功后使用，即BC_ProbeDevEx之后调用
*输入参数:   dev: 设备的操作句柄	BC_InitDevEx成功的返回值
*输出参数:   无
*返回值     :  无
*************************************************/
BODYCAMDLL_API void  BC_UnInitDevEx(IN BCHandle *dev);


#else //INIT_DEV_WITH_LIST
/*=============================================================================
多设备操作函数，通过设备的列表方式进行操作
=============================================================================*/
#ifdef INIT_DEVLIST_WITH_PTHREAD

/*************************************************
*函数说明:   DLL使用现内部线程的方式获取存在的所有设备并保存在设备列表中,
*				设备会获取USB热插拔事件自动更新列表。有这个函数就不需要使用BC_InitDevList函数了
*				列表是否有更新可以通过调用BC_GetDevListIsUpdate来获取
*				注意: BC_InitDevListPthread与BC_InitDevListPthreadEx 二选一，不能都使用，只能在初始化时仅调用一次
*输入参数:   无
*输出参数:   无
*返回值     :   成功:列表中设备数量,  失败:BC_FAILED
*************************************************/
//BODYCAMDLL_API void  BC_InitDevListPthread(void);


/*************************************************
*函数说明:   DLL使用现内部线程的方式获取存在的所有设备并保存在设备列表中,
*				设备会获取USB热插拔事件自动更新列表。有这个函数就不需要使用BC_InitDevList函数了
				注意: BC_InitDevListPthread与BC_InitDevListPthreadEx 二选一，不能都使用
					  如果FuncListUpgrade回调函数设置为空则可以通过定时调用BC_GetDevListIsUpdate函数获取设备列表是否有更新
*输入参数:   FuncListUpgrade: 列表更新回调函数（无回调可设置为NULL），参数为更新后的设备数量
*输出参数:   无
*返回值     :   成功:列表中设备数量,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API void  BC_InitDevListPthreadEx(PFCALLBACK FuncListUpgrade);



/*************************************************
*函数说明:   列表访问的互斥操作，进入互斥条件
*输入参数:   无
*输出参数:   无
*返回值     :   无
*************************************************/

BODYCAMDLL_API void  BC_ListEnterCriticalSection(void);

/*************************************************
*函数说明:   列表访问的互斥操作，退出互斥条件
*输入参数:   无
*输出参数:   无
*返回值     :   无
*************************************************/
BODYCAMDLL_API void  BC_ListLeaveCriticalSection(void);


/*************************************************
*函数说明:   获取设备列表中的设备数量
*输入参数:   无
*输出参数:   无
*返回值     :   成功:列表中设备数量,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevListSum(void);

#else  //INIT_DEVLIST_WITH_PTHREAD

/*************************************************
*函数说明:   获取存在的所有设备并保存在设备列表中,使用了这个函数就不能使用BC_InitDevListPthread 或BC_InitDevListPthreadEx函数
*				如果有新设备加入或者对设备进行了进入U盘的操作需要重新调用该函数
*输入参数:   无
*输出参数:   无
*返回值     :   成功:列表中设备数量,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_InitDevList(void);


/*************************************************
*函数说明:   删除设备列表中所有设备
*输入参数:   无
*输出参数:   无
*返回值     :  无
*************************************************/
BODYCAMDLL_API void BC_UnInitDevList(void);

#endif  //INIT_DEVLIST_WITH_PTHREAD

/*************************************************
*函数说明:  获取列表中所有设备的信息
*输入参数:  	ListSum: 数组的个数，即DevListInfo[ListSum]
*输出参数:    pDevListInfo:设备列表数组的地址(指针)
*返回值     :   成功:信息列表中设备的个数即填充数组的个数,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetDevAllListInfoFromList(OUT DEVLIST_INFO *pDevListInfo,IN int ListSum);

/*************************************************
*函数说明:  设置设备列表中对应设备的新增设备标识
*输入参数:   dev: 设备的操作句柄	
*输出参数:   无
*返回值      :  成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_SetNewDevFlagToList(BCHandle *dev,int NewDevFlag);

/*************************************************
*函数说明:  获取设备列表中对应设备是否为新增设备
*输入参数:   dev: 设备的操作句柄	
*输出参数:   NewDevFlag: 1-是新增设备，0-已存在的设备
*返回值      :  成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetNewDevFlagFromList(BCHandle *dev,int *NewDevFlag);

/*************************************************
*函数说明:  设置设备列表中对应设备的扩展标识位
*输入参数:   dev: 设备的操作句柄	
*				index: 需要设置的标识位在标识数组中的索引值
*				Flag: 设置值
*输出参数:   无
*返回值      :  成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_SetExFlagToList(BCHandle *dev,int index,char Flag);

/*************************************************
*函数说明:  获取设备列表中对应设备的扩展标识位
*输入参数:   dev: 设备的操作句柄	
*				index: 需要设置的标识位在标识数组中的索引值
*输出参数:   Flag: 该标识位的值
*返回值      :  成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetExFlagFromList(BCHandle *dev,int index,char *Flag);

	
/*************************************************
*函数说明:  从设备列表中通过列表的索引值获取对应设备的SN
*输入参数:   devidx:设备列表索引值
*输出参数:   DevSN: 设备的序列号(8个字节)，每台设备都不通，是设备的唯一性ID
*返回值     :   成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetDevSNFromList(IN int devidx,OUT char *DevSN);

/*************************************************
*函数说明:   从设备列表中通过列表的索引值获取对应设备的IDCode
*输入参数:   devidx:设备列表索引值
*输出参数:   IDCode:  设备的识别码(5个字节)，同类型设备的IDCode相同
*返回值     :   成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetDevIDCodeFromList(IN int devidx,OUT char *IDCode);

/*************************************************
*函数说明:   从设备列表中获取DevSN对应的列表索引值
*输入参数:    DevSN: 设备的序列号(8个字节)，每台设备都不通，是设备的唯一性ID
*输出参数:   devidx:设备列表索引值
*返回值     :   成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetDevIdxFromList(IN char *DevSN,OUT int *devidx);

/*************************************************
*函数说明:   从设备列表中通过列表的索引值获取对应设备的句柄
*输入参数:   devidx:设备列表索引值
*输出参数:   无
*返回值     :   成功:设备的操作句柄(BCHandle *),  失败:NULL
*************************************************/
BODYCAMDLL_API BCHandle *BC_GetDevHandleFromList(IN int devidx);

/*************************************************
*函数说明:   从设备列表中通过列表的索引值获取对应设备的U盘是否已经加载
*输入参数:   devidx:设备列表索引值
*输出参数:   DiskStatu : 1-在PC上已经形成U盘，0-在PC上未形成U盘
*返回值     :   成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetUdiskStatuFromList(int devidx,int *DiskStatu);

/*************************************************
*函数说明:   获取设备列表的更新状态
*输入参数:   无
*输出参数:   无 : 
*返回值     :   1-有更新，0-无更新
*************************************************/
BODYCAMDLL_API int  BC_GetDevListIsUpdate(void);

#endif  //INIT_DEV_WITH_LIST




/*=============================================================================
*   以下所有的dev为设备操作句柄  BC_InitDev或BC_InitDevEx或BC_GetDevHandle的返回值
*   sPwd为设备的登录密码
*   管理员登录的权限可以操作所有的功能
*   普通用户登录的权限只允许查询操作和部分控制操作
*   已登上的用户需要更改用户需要退出登录后重新用新的用户登录
*   设备的相关参数可以先获取，修改后再设置
=============================================================================*/
/*************************************************
*函数说明:   管理员登录设备
*输入参数:   dev: 设备的操作句柄	
*				sPwd: 登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_Login(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:   用户登录设备，之后的设备操作函数都是在该用户的拥有权限下操作
*输入参数:   dev: 设备的操作句柄	
*				User: 管理员:admin , 普通用户: user
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_LoginEx(IN BCHandle *dev,IN char *User,IN char *sPwd);


/*************************************************
*函数说明:   查询设备是否已经登录
*输入参数:   dev: 设备的操作句柄	
*输出参数:   无
*返回值     :    已登录:1,    未登录:0
*************************************************/
BODYCAMDLL_API int BC_IsLogin(BCHandle *dev);


/*************************************************
*函数说明:   获取临时登录设备的登录ID号，具备管理员用户的操作权限
*输入参数:   dev: 设备的操作句柄	
*输出参数:   LoginID: 临时登录设备的ID (不大于8字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_GetLoginID(IN BCHandle *dev,OUT char *LoginID);


/*************************************************
*函数说明:   用户登录设备
*输入参数:   dev: 设备的操作句柄	
*				Pwd:  通过临时登录设备的ID计算出来的登录设备的密码(8个字节长度),即BC_GetLoginID的输出参数LoginID
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_LoginIDPwd(IN BCHandle *dev,IN char *Pwd);


/*************************************************
*函数说明:   退出设备登录
*输入参数:   dev: 设备的操作句柄	
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int BC_Logout(IN BCHandle *dev);

/*************************************************
*函数说明:   获取设备端的识别码
*输入参数:   dev: 设备的操作句柄	
*输出参数:   IDCode, 小于8个字节长度
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetIDCode(BCHandle *dev,char *IDCode);

/*************************************************
*函数说明:   获取使用者信息,对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   info: 参考ZFY_INFO的结构体描述
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevInfo(IN BCHandle *dev,IN char *sPwd,OUT DEV_INFO *info);


/*************************************************
*函数说明:   设置使用者信息,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   info: 参考ZFY_INFO的结构体描述
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevInfo(IN BCHandle *dev,IN char *sPwd,IN DEV_INFO *info);


/*************************************************
*函数说明:   同步电脑时间到设备，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevTime(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:   获取设备当前时间(本地时间含时区)  ，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				timestr: 本地时间,格式为YYYYMMDDhhmmss (14个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevTime(IN BCHandle *dev,IN char *sPwd,OUT char *timestr);


/*************************************************
*函数说明:  设置设备时间、时区、夏令时配置  ，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bDst:  是否开启夏令时，0-不开启，1-开启
*				TZstr: 时区的名称(小于256个字节)，通过GetTimeZoneStr函数获取，在zoneinfo.h中
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevTimeCfg(IN BCHandle *dev,IN char *sPwd,IN int bDst,IN char *TZstr);


/*************************************************
*函数说明:  获取设备时间、时区、夏令时配置，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   localtimestr: 本地时间,格式为YYYYMMDDhhmmss (14个字节长度)
*				Timezone: 时区的差值，单位为分钟				
*				bDst:  是否开启夏令时，0-不开启，1-开启
*				TZstr: 时区的名称(小于256个字节)，可以通过GetTimeZoneIndex函数获取对应的结构体的索引值，在zoneinfo.h中
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevTimeCfg(IN BCHandle *dev,IN char *sPwd,OUT char *localtimestr,OUT int *Timezone,OUT int *bDst,OUT char *TZstr);


/*************************************************
*函数说明:  通知设备进入U盘模式，只是本次插入的USB才生效，
*				如果拔掉USB后需要再次调用该函数，对所有的用户生效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_EnterDiskMode(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:  设置设备是否始终为U盘模式,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
				bEnable: 1:需要授权才进入U盘模式，0:连上电脑就进入U盘模式
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevUDiskAuthEn(IN BCHandle *dev,IN char *sPwd,IN int bEnable);


/*************************************************
*函数说明:  获取设备是否始终为U盘模式，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 1:需要授权才进入U盘模式，0:连上电脑就进入U盘模式
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevUDiskAuthEn(IN BCHandle *dev,IN char *sPwd,OUT int * bEnable);


/*************************************************
*函数说明:   获取形成U盘的状态，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bUDisk 是否形成了U盘，1-已经形成U盘，0-未形成U盘
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetUdiskStatu(IN BCHandle *dev,IN char *sPwd,OUT int * bUDisk);


/*************************************************
*函数说明:  格式化SD卡，生效后设备会自动重启,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*				fstype: 格式化SD文件系统的类型，请参考FSTYPE_E
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_FormatUdisk(IN BCHandle *dev,IN char *sPwd,IN FSTYPE_E fstype);


/*************************************************
*函数说明:   获取主码流信息，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   Width: 视频
*				Height: 
*				Fps:  
*				Bps: 
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetMasterVEInfo(IN BCHandle *dev,IN char *sPwd,OUT int *Width,OUT int *Height,OUT int *Fps,OUT int *Bps);


/*************************************************
*函数说明:   获取从码流信息，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   Width: 视频
*				Height: 
*				Fps:  
*				Bps: 
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetSlaveVEInfo(IN BCHandle *dev,IN char *sPwd,OUT int *Width,OUT int *Height,OUT int *Fps,OUT int *Bps);


/*************************************************
*函数说明:  获取电池电量，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   Battery:电量的百分比0-100
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetBatVal(IN BCHandle *dev,IN char *sPwd,OUT int *Battery);


/*************************************************
*函数说明:  设置运营商接入点，,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				APN: 运营商接入点(小于64字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetApn(IN BCHandle *dev,IN char *sPwd,IN char *Apn);


/*************************************************
*函数说明:  获取运营商接入点，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   APN: 运营商接入点(小于64字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetApn(IN BCHandle *dev,IN char *sPwd,OUT char *Apn);


/*************************************************
*函数说明:  设置运营商接入点及用户名和密码 ，只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				APN: 运营商接入点(小于64字节)
*				VpnName: VPN用户名称(小于64字节)
*				VpnPwd:  VPN用户密码(小于64字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetVpn(IN BCHandle *dev,IN char *sPwd,IN char *Apn,IN char *VpnName,IN char *VpnPwd);


/*************************************************
*函数说明:  获取运营商接入点及用户名和密码 ，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   APN: 运营商接入点(小于64字节)
*				VpnName: VPN用户名称(小于64字节)
*  				VpnPwd:  VPN用户密码(小于64字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetVpn(IN BCHandle *dev,IN char *sPwd,OUT char *Apn,OUT char *VpnName,OUT char *VpnPwd);


/*************************************************
*函数说明:  修改设备登录密码,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				username: 用户名(admin或者user ,其他的不支持)
*				oldpwd: 旧密码(小于64字节)，可以不填充
*				newpwd:  新密码(小于64字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevUser(IN BCHandle *dev,IN char *sPwd,IN char *username,IN char *oldpwd,IN char *newpwd);


/*************************************************
*函数说明:  获取设备登录名称，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   username: 当前登录设备的用户名(admin或者user )				
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevUser(IN BCHandle *dev,IN char *sPwd,OUT char *username);

 
/*************************************************
*函数说明:  设置设备名称，会叠加到视频上及显示屏上,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				devname: 需要叠加在视频上的设备名称(小于20字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevName(IN BCHandle *dev,IN char *sPwd,IN char *devname);
 
/*************************************************
*函数说明:  获取设备名称，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度) 
*输出参数:  devname: 需要叠加在视频上的设备名称(小于20字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevName(IN BCHandle *dev,IN char *sPwd,OUT char *devname);

 
/*************************************************
*函数说明:  获取设备厂家ID   ，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度) 
*输出参数:  Vid: 设备厂家ID   ( 2 个节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevVID(IN BCHandle *dev,IN char *sPwd,OUT char *Vid);

 
/*************************************************
*函数说明:  获取设备型号ID   ，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度) 
*输出参数:  Did: 设备型号ID    ( 6个节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevDID(IN BCHandle *dev,IN char *sPwd,OUT char *Did);

 
/*************************************************
*函数说明:  设置设备的日志级别,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				devname: 需要叠加在视频上的设备名称(小于20字节)
*				loglevel: 0-7, 0为最高级别，即只有最高级别的信息才会在日志中存在，7为最低级别即所有的信息都会在日志中
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevLogLevel(IN BCHandle *dev,IN char *sPwd,IN int loglevel);


/*************************************************
*函数说明:  获取设备的日志级别，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				devname: 需要叠加在视频上的设备名称(小于20字节)
*输出参数:   loglevel: 0-7, 0为最高级别，即只有最高级别的信息才会在日志中存在，7为最低级别即所有的信息都会在日志中
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevLogLevel(IN BCHandle *dev,IN char *sPwd,OUT int * loglevel);


/*************************************************
*函数说明:  导出设备的日志到SD卡中，如果需要查看SD卡中的日志需要让设备进入U盘的模式，对所有的用户生效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_ExportDevLog(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:  增加一个AP热点 ，可以增加20个AP热点到设备中，设备会自动连接,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				Ssid:  AP热点名称(小于32字节)，不允许有单引号和双引号
*				passwd:  热点密码(小于32字节)，不允许有单引号和双引号
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_AddAp(IN BCHandle *dev,IN char *sPwd,IN char *Ssid,IN char *passwd);


/*************************************************
*函数说明:  从设备中删除指定AP热点  ，可以先获取设备中已存在的AP热点后再删除,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				Ssid:  需要删除的AP热点名称(小于32字节)，
*输出参数:   无				
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_DelAp(IN BCHandle *dev,IN char *sPwd,IN char *Ssid);

/*************************************************
*函数说明:  从设备中删除所有A热点  ，只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_DelAllAp(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:  选择连接的AP热点 ，可以先获取设备中已存在的AP热点后再选择设置,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				Ssid:  需要最先连接的AP热点名称(小于32字节)，
*输出参数:   无				
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetSelAp(IN BCHandle *dev,IN char *sPwd,IN char *Ssid);

/*************************************************
*函数说明:  获取当前选择的AP热点 ，，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   Ssid:  当前选择的AP热点名称(小于32字节)，
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetSelAp(IN BCHandle *dev,IN char *sPwd,OUT char *Ssid);

/*************************************************
*函数说明:  获取所有的存在的AP热点 列表，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   Ssids:  设备中存储的所有AP热点名称(小于640字节)，每个AP热点 的名称以逗号隔开
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetApList(IN BCHandle *dev,IN char *sPwd,OUT char *Ssids);

//
/*************************************************
*函数说明:  设置PTT功能,如果开启，CMSV6平台的对讲功能将变成半双工模式，只有管理员用户有效
*								如果关闭，CMSV6平台对讲为全双工
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable:  1-开启，0-关闭
*输出参数:   无				
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevPFPTTEn(IN BCHandle *dev,IN char *sPwd,IN int bEnable);


/*************************************************
*函数说明:  获取PTT功能状态，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable:  1-开启，0-关闭		
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevPFPTTEn(IN BCHandle *dev,IN char *sPwd,OUT int * bEnable);


/*************************************************
*函数说明:  设置服务器地址及端口(已经使用BC_SetCmsv6Cfg代替，最好不要使用该函数),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				ServIP: 服务器的地址(支持域名，小于128字节)
*				ServPort: 服务器端口
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetServAddr(IN BCHandle *dev,IN char *sPwd,IN char *ServIP,IN int ServPort);


/*************************************************
*函数说明:  获取服务器地址及端口(已经使用BC_GetCmsv6Cfg代替，最好不要使用该函数)，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   ServIP: 服务器的地址(支持域名，小于128字节)
*				ServPort: 服务器端口
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetServAddr(IN BCHandle *dev,IN char *sPwd,OUT char *ServIP,OUT int *ServPort);

//
/*************************************************
*函数说明:  设置 gps数据上报间隔(已经使用BC_SetCmsv6Cfg代替，最好不要使用该函数),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevReportTime(IN BCHandle *dev,IN char *sPwd,IN int ReportTime);

/*************************************************
*函数说明:  获取gps数据上报间隔(已经使用BC_SetCmsv6Cfg代替，最好不要使用该函数)，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   ReportTime: gps数据上报间隔(单位秒，5-360)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevReportTime(IN BCHandle *dev,IN char *sPwd,OUT int * ReportTime);


/*************************************************
*函数说明:  设置集群对讲服务器信息,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID(小于32字节)
*				Name: 设备登录服务器的用户名(小于32字节)
*				Passwd: 设备登录服务器的密码(小于32字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetTalkBackCfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,IN char * ServPort,IN char *DevNo,IN char *Name,IN char *Passwd);


/*************************************************
*函数说明:  获取集群对讲服务器信息，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID(小于32字节)
*				Name: 设备登录服务器的用户名(小于32字节)
*				Passwd: 设备登录服务器的密码(小于32字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetTalkBackCfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort,OUT char *DevNo,OUT char *Name,OUT char *Passwd);


/*************************************************
*函数说明:  设置GB28181服务器信息,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口 
*				DevNo: 设备登录服务器的ID (小于32字节)
*				ChnNo: 设备通道ID (小于32字节)
*				ServNo: 服务器ID (小于32字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetGb28181Cfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,IN char * ServPort,IN char *DevNo,IN char *ChnNo,IN char *ChnName,IN char *ServNo,IN char *Passwd);


/*************************************************
*函数说明:  获取GB28181服务器信息，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID (小于32字节)
*				ChnNo: 设备通道ID (小于32字节)
*				ServNo: 服务器ID (小于32字节)
*				Passwd: 设备登录服务器密码 (小于32字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetGb28181Cfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort,OUT char *DevNo,OUT char *ChnNo,OUT char *ChnName,OUT char *ServNo,OUT char *Passwd);


/*************************************************
*函数说明:  设置KH3989 GPS服务器信息,只允许KH3989的客户使用，只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID (小于32字节)
*				Passwd: 设备登录服务器密码 (小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetGpsServCfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,IN char * ServPort,IN char *DevNo,IN char *Passwd,IN int ReportTime);


/*************************************************
*函数说明:  获取KH3989 GPS服务器信息,只允许KH3989的客户使用，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID (小于32字节)
*				Passwd: 设备登录服务器密码 (小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetGpsServCfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort,OUT char *DevNo,OUT char *Passwd,OUT int *ReportTime);


/*************************************************
*函数说明:  设置Cmsv6服务器信息(这是设备默认使用的音视频服务器),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的地址(小于64字节,支持域名)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID (小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetCmsv6Cfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,IN char * ServPort,IN char *DevNo,IN int ReportTime);


/*************************************************
*函数说明:  获取Cmsv6服务器信息(这是设备默认使用的音视频服务器)，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的地址(小于64字节,支持域名)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID (小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetCmsv6Cfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort,OUT char *DevNo,OUT int *ReportTime);


/*************************************************
*函数说明:  设置Ptyt服务器信息(只针对Ptyt客户有效),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID(小于32字节)
*				Name: 设备登录服务器的用户名(小于32字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetPtytCfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,IN char * ServPort,IN char *DevNo,IN char *Name,IN char *Passwd,IN int ReportTime);


/*************************************************
*函数说明:  获取Ptyt服务器信息(只针对Ptyt客户有效)，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID(小于32字节)
*				Name: 设备登录服务器的用户名(小于32字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetPtytCfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort,OUT char *DevNo,OUT char *Name,OUT char *Passwd,OUT int *ReportTime);


/*************************************************
*函数说明:  设置KH0787服务器信息(只针对KH0787客户有效),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID(小于32字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_Set0787Serv01Cfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,IN char * ServPort,IN char *DevNo,IN char *Passwd,IN int ReportTime);


/*************************************************
*函数说明:  获取KH0787服务器信息(只针对KH0787客户有效)，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*				DevNo: 设备登录服务器的ID(小于32字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_Get0787Serv01Cfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort,OUT char *DevNo,OUT char *Passwd,OUT int *ReportTime);


/*************************************************
*函数说明:  设置LBS服务器信息(只针对KH3080客户有效),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				LoginUrl: 服务器的URL地址(小于256字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetLBSCfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *LoginUrl,IN char *Passwd,IN int ReportTime);


/*************************************************
*函数说明:  获取LBS服务器信息(只针对KH3080客户有效)，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				LoginUrl: 服务器的URL地址(小于256字节)
*				Passwd: 设备登录服务器密码(小于32字节)
*				ReportTime: gps数据上报间隔(单位秒，5-360)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetLBSCfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *LoginUrl,OUT char *Passwd,OUT int *ReportTime);



/*************************************************
*函数说明:  设置网络检测服务器信息,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetNetCheckServCfg(IN BCHandle *dev,IN char *sPwd,IN int bEnable,IN char *ServIP,OUT char * ServPort);


/*************************************************
*函数说明:  获取网络检测服务器信息，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*				ServIP: 服务器的IP地址(小于16字节)
*				ServPort: 服务器端口
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetNetCheckServCfg(IN BCHandle *dev,IN char *sPwd,OUT int *bEnable,OUT char *ServIP,OUT char * ServPort);




/*************************************************
*函数说明:   升级线程服务，用来升级设备的固件，必须在BC_SendDataPack函数调用之前调用,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable: 是否开启该功能，1-开启，0-关闭
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDataPthread(IN BCHandle *dev,IN char *sPwd,IN int bEnable);

/*************************************************
*函数说明:   向升级线程发送数固件数据，用来升级设备的固件，必须在BC_SetDataPthread函数调用之后调用,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				packindex:  将固件文件拆分成多个数据包，对应的数据包的序号值(从0开始)
*				data:  固件对应包的数据
*				datalen: 固件对应包的数据长度
*输出参数:   无				
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SendDataPack(IN BCHandle *dev,IN char *sPwd,IN int packindex,IN char *data,IN int datalen);


/*************************************************
*函数说明:  向设备发送命令行数据(命令行数据不对外开放，如有需要可单独定制),只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				bEnable: 是否开启该功能，1-开启，0-关闭
*				usercmd: 命令内容(小于32字节)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SendUserCmd(IN BCHandle *dev,IN char *sPwd,IN char *usercmd,char *respbuf);


/*************************************************
*函数说明:  设置GPS功能开关,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
				bEnable:  1-开启，0-关闭	
*输出参数:   无	
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetDevGpsEn(IN BCHandle *dev,IN char *sPwd,IN int bEnable);

/*************************************************
*函数说明:  获取GPS功能开关，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   bEnable:  1-开启，0-关闭		
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetDevGpsEn(IN BCHandle *dev,IN char *sPwd,OUT int * bEnable);


/*************************************************
*函数说明:  设置录像加密的密码,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*				RecordPwd:  录像加密密码(小于12字节)
*输出参数:   无	
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_SetRecordPwd(IN BCHandle *dev,IN char *sPwd,IN char *RecordPwd);


/*************************************************
*函数说明:  获取录像加密的密码，对所有的用户生效
*输入参数:   dev: 设备的操作句柄	
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   RecordPwd:  录像加密密码(小于12字节)
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetRecordPwd(IN BCHandle *dev,IN char *sPwd,OUT char *RecordPwd);


/*************************************************
*函数说明:  通知设备关机，对所有的用户生效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_CtrlPowerOff(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:  通知设备进入省电模式，对所有的用户生效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_CtrlPowerSave(IN BCHandle *dev,IN char *sPwd);


/*************************************************
*函数说明:  通知设备恢复出厂值，成功后设备会自动重启,只有管理员用户有效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_FReset(IN BCHandle *dev,IN char *sPwd);

/*************************************************
*函数说明:  通知设备重启
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_CtrlReboot(BCHandle *dev,char *sPwd);


//-------------------------------------------------------------------------------------
/*************************************************
*函数说明:   通过Json格式设置或获取设备的各项参数,json格式相关参数参考< 参数格式说明.txt>
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*				in_jsonstr:  Json格式的参数设置或获取
*				out_jsonstrsize:输出返回json格式数据buf的大小
*输出参数:   out_jsonstr:输出返回的json格式数据
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_DevCfgJsonStr(BCHandle *dev,char *sPwd,char *in_jsonstr,char *out_jsonstr,int out_jsonstrsize);


/*************************************************
*函数说明:   通过Ini格式设置或获取设备的各项参数,ini格式相关参数参考< 参数格式说明.txt>
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*				in_inistr: ini格式的参数设置或获取
*				out_inistrsize: 输出返回ini格式数据buf的大小
*输出参数:   out_inistr: 输出返回的ini格式数据
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_DevCfgIniStr(BCHandle *dev,char *sPwd,char *in_inistr,char *out_inistr,int out_inistrsize);

BODYCAMDLL_API int   BC_UmsIniStr2IniCmd(char *inistr,UMS_CMD_S *inicmd);




BODYCAMDLL_API int  BC_SetVideoOsd(BCHandle *dev,char *sPwd,int index,int enable,char *osdstr);
BODYCAMDLL_API int  BC_GetVideoOsd(BCHandle *dev,char *sPwd,int index,int *enable,char *osdstr,int osdstrsize);


/*************************************************
*函数说明:  解密一个加密的录像文件
*输入参数:   infile: 设备的录像文件
			 outfile: 解密后保存的文件
*			 key:  解密的密码(不大于8个字节长度)
*输出参数:   无
*返回值     :    成功:BC_SUCCESS,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int    BC_FileDecrypt(char *infile,char *outfile,unsigned char* key);

//---------------------------------------------------------------------------------------


/*************************************************
*函数说明:  获取错误码，返回值为错误码，对所有的用户生效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:返回错误码,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API int  BC_GetErrNo(IN BCHandle *dev);


/*************************************************
*函数说明:  获取错误描述，返回值为错误描述，对所有的用户生效
*输入参数:   dev: 设备的操作句柄
*				sPwd:  登录设备的密码(6个字节长度)
*输出参数:   无
*返回值     :    成功:返回错误信息字符串指针,  失败:BC_FAILED
*************************************************/
BODYCAMDLL_API char *  BC_GetErrStr(IN BCHandle *dev);


#ifdef __cplusplus
}
#endif

