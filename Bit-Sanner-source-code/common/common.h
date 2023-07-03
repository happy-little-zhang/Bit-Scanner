#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <cstdlib>
#include <stdio.h>					// 标准C输入输出
#include <stdint.h>                 // C99中标准C库头文件（定义了 int64_t、uint8_t等数据类型）
#include <unistd.h>
#include <time.h>
#include <string.h>                 // 字符串库
#include <math.h>                   // 数学计算相关库
#include <signal.h>                 // 信号中断库
#include <linux/can.h>              // Linux CAN 相关
#include <linux/can/raw.h>          // Linux CAN 相关
#include <string.h>
#include <memory.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>              // 基本系统数据类型
#include <inttypes.h>
#include <limits.h>                 // 最大最小值
#include <float.h>                  // 浮点运算

#include <iostream>
#include <vector>
#include <map>
#include <bitset>              //C++的 bitset 在 bitset 头文件中，它是一种类似数组的结构，它的每一个元素只能是０或１，每个元素仅用１bit空间。


#include "log.h"               // 日志模块库函数
#include "SimpleIni.h"         // 模型参数存储与读取相关库

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif


#define TRUE            true
#define FALSE           false
typedef bool            BOOL;
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef unsigned char   BYTE;
typedef unsigned char   *PBYTE;
typedef unsigned short  WORD;
typedef unsigned short  USHORT;
typedef int             INT;
typedef unsigned int    UINT;
typedef unsigned int    DWORD;
typedef unsigned int    *LPDWORD;
typedef unsigned long        ULONG;
typedef unsigned long long   ULONG64;
typedef void            *PVOID;
typedef void            *LPVOID;

// 返回值 
#define RET_OK                            0
#define RET_ERROR                         -1

// 文件打开方式
#define MODE_A_PLUS                "a+"       // 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留
#define MODE_R_PLUS                "r+"       // 以可读写方式打开文件，该文件必须存在
#define MODE_W_PLUS                "w+"       // 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。


// 打印不同字体颜色的颜色宏定义 
#define NONE                              "\e[0m"
#define BLACK                             "\e[0;30m"
#define L_BLACK                           "\e[1;30m"
#define RED                               "\e[0;31m"
#define L_RED                             "\e[1;31m"
#define GREEN                             "\e[0;32m"
#define L_GREEN                           "\e[1;32m"
#define BROWN                             "\e[0;33m"
#define YELLOW                            "\e[1;33m"
#define BLUE                              "\e[0;34m"
#define L_BLUE                            "\e[1;34m"
#define PURPLE                            "\e[0;35m"
#define L_PURPLE                          "\e[1;35m"
#define CYAN                              "\e[0;36m"
#define L_CYAN                            "\e[1;36m"
#define GRAY                              "\e[0;37m"
#define WHITE                             "\e[1;37m"

#define BOLD                              "\e[1m"
#define UNDERLINE                         "\e[4m"
#define BLINK                             "\e[5m"
#define REVERSE                           "\e[7m"
#define HIDE                              "\e[8m"
#define CLEAR                             "\e[2J"
#define CLRLINE                           "\r\e[K" /* or "\e[1K\r" */


// 自定义宏
#define FRAME_DLC                     8                               // 报文数据长度(byte)
#define BIT_STREAM_NUM                127                             // 64+63
#define FRAME_DLC_BIT                 64                              // 报文数据长度(bit)
#define BYTE_VALUE_NUM                256                             // 报文字节值范围
#define TRAIN_LOADING_CYCLE           5000                           // 5s = 5000ms
#define BASIC_DEBUG                   0                              // 调试使用

// 训练文件数目
#define MIN_FILE_NUM                    1                     // 最大文件序号([1,35])
#define MAX_FILE_NUM                    35                     // 最大文件序号([1,35])

#define MIN_SLIDING_WINDOW_SIZE         1                      // bit位尝试的最小长度1
#define MAX_SLIDING_WINDOW_SIZE         16                      // bit位尝试的最小长度8
#define DETECTOR_INVALID_THRESHOLD     0.5                     // 二进制检测器 无效阈值      (stable_num/total_num < 0.5)
#define READ_PROFILE_BUFFER_SIZE       ((65536) * (8))         // 读取配置文件或存储配置文件时，values的缓存大小,之所以开这么大是为了保证0~65536能够全部存储


// 文件路径
#define PROFILE_PATH                 "../dataset/profile/"                         // 模型参数文件路径前缀
#define TRAIN_PATH                   "../dataset/train/"                           // 训练集文件路径前缀
#define TEST_PATH                    "../dataset/test/"                            // 测试集文件路径前缀
#define RESULT_PATH                  "../dataset/simulation/result/"               // 模拟攻击的检测结果文件路径前缀
#define LABEL_PATH                   "../dataset/simulation/label/"                // 模拟攻击的标签文件路径前缀
#define ATTACK_PATH                  "../dataset/simulation/attack/"               // 模拟攻击的合成文件路径前缀
#define ANALYSIS_PATH                "../dataset/analysis/"                        // 数据分析文件路径前缀


// 对比方法个数
#define COMPARSION_METHOD_NUM         5             // 0-min_max 1-hamming_detect, 2-id_matrix, 3-bit_scanner, 4-sim_bit_scanner 

#define VALID           1                            // 有效
#define INVALID         0                            // 无效
#define NOT_EXIST       -1                           // 值不存在
#define NORMAL          0                            // 正常
#define ABNORMAL        1                            // 异常


//  配置文件相关宏定义
#define SECTION_NAME_BYTE_RPOFILE               "byte_profile"
#define SECTION_NAME_ID_RPOFILE                 "id_profile"
#define SECTION_NAME_BIT_RPOFILE                "bit_profile"
#define KEY_NAME_CAN_ID_LIST                    "can_id_list"



// 模拟恶意攻击时使用的宏定义
#define ATTACK_NUM                               10    // 每个文件独立攻击实施的次数[1, 10]
#define ATTACK_DURATION                 (10*1000*1000) // 攻击持续时间10s=10*1000*1000us
#define MIN_INJECTION_STRENGTH                   1     // 最小报文注入强度，见get_injection_interval函数
#define MAX_INJECTION_STRENGTH                   8     // 最大报文注入强度，见get_injection_interval函数
#define MIN_TAMPER_NUM                           1     // 最小报文字节篡改数目
#define MAX_TAMPER_NUM                           8     // 最大报文字节篡改数目
#define DELETE_TEMP_FILE                         1     // 是否删除模拟攻击过程中产生的临时文件


#define BIT_OP_NUM                               5     // 一共5种位运算
#define ATTACK_TYPE_NUM                          5     // 一共5种攻击
#define ATTACK_NAME_MAX_SIZE                     50    // 攻击名称的字符串最大长度

// 定义两个相邻比特位运算方式 djecent_bit_operation
enum BIT_OP
{
    BIT_OP_XOR  = 0,                   // 异或运算^:参加运算的两个对象，如果两个位为“异”（值不同），则该位结果为1，否则为0。
	BIT_OP_AND  = 1,                   // 与运算（AND）：参加运算的两个对象，只有当它们的对应位都是1时，结果才为1，否则结果为0。
	BIT_OP_OR   = 2,                   // 或运算（OR）：参加运算的两个对象，只有当它们的对应位都是0时，结果才为0，否则结果为1。
	BIT_OP_NAND = 3,                   // 非与运算（NAND）：参加运算的两个对象，只有当它们的对应位都是1时，结果才为0，否则结果为1。
	BIT_OP_NOR  = 4                    // 非或运算（NOR）：参加运算的两个对象，只有当它们的对应位都是0时，结果才为1，否则结果为0。
};


// 定义攻击方式
enum ATTACK_TYPE
{
    ATTACK_TYPE_RANDOM_INJECTION   = 0,                 // 随机注入攻击   （随机负载）
	ATTACK_TYPE_TARGETED_INJECTION = 1,                 // 目标注入攻击（合法负载 ）
	ATTACK_TYPE_REPLAY             = 2,                 // 重放攻击 
	ATTACK_TYPE_RANDOM_TAMPER      = 3,                 // 随机篡改（随机负载） 
	ATTACK_TYPE_TARGETED_TAMPER    = 4                  // 目标篡改攻击（合法负载）
};



class ID_Profile {
public:
	int min_hamming_dist;                 // 最小汉明距离
	int max_hamming_dist;                 // 最大汉明距离

    int inner_product;
	
	vector<UINT> legal_next_id;       // 合法后继ID,用于ID_matrix

    int previous_payload_valid;       // 上一个负载是否有效
	BYTE previous_payload[FRAME_DLC]; // 上一个负载内容

	// 构造函数
    ID_Profile()
	{
		min_hamming_dist = 64;
		max_hamming_dist = 0;
	};

	// 析构函数
    ~ID_Profile()
	    {};	
};


// 字节轮廓类
class Byte_Profile {
public:

    // 存储字节值是否出现过 [0,255] 
    bitset<256> legal_data;
	
    // 最小最大字节值
	int min_byte_value;
	int max_byte_value;

    // 构造函数
    Byte_Profile()
	{
		min_byte_value = 255;
		max_byte_value = 0;
	};
	// 析构函数
	~Byte_Profile()
		{};	

};



class Bit_Profile 
{
public:
    bitset<65536> tm_data;                                         // pow(2, 16)用于存储不同滑动窗口下的whitelist
	bitset<MAX_SLIDING_WINDOW_SIZE> old_data;                      // 用于缓存滑动窗口内的 bit segment                                    
	bitset<MAX_SLIDING_WINDOW_SIZE> old_data_valid;                // 缓存的bit位是否有效

	int  anomaly_count;                          // 异常次数，用于统计误报

	int total_num;                               // 有效检测的总次数
	int stable_num;                              // 最新保持稳定的数目
	int invalid_flag;                            // 该检测器是否失效的标志

	Bit_Profile()
	{
	    tm_data.reset();
		old_data.reset();
		old_data_valid.reset();      
		anomaly_count = INVALID;
		total_num = INVALID;
		stable_num = INVALID;
		invalid_flag = INVALID;
	};
	~Bit_Profile()
	{
	
	};	
};



// 获取系统毫秒时间ms
long long int get_system_ms_time();

// 获取系统微秒时间us
long long int get_system_us_time();

// select 精确睡眠,单位ms 
int select_sleep(int ms_time);

// 标准时间(2021-08-04 11:43:07.123)转换成时间戳(ms);
long long int stand2tick_time(char *str_time);


// Linux socket can 格式
// struct can_frame {
//		canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
//		__u8	can_dlc; /* frame payload length in byte (0 .. 8) */
//		__u8	__pad;	 /* padding */
//		__u8	__res0;  /* reserved / padding */
//		__u8	__res1;  /* reserved / padding */
//		__u8	data[8] __attribute__((aligned(8)));
// };

// 打印输出CAN报文 
int can_frame_show(can_frame *frame);

// 复制CAN报文 
int can_frame_duplicate(struct can_frame *destination, struct can_frame  *source);

// 根据报文ID获取下标映射
int get_map_id2num(vector<UINT> &vec_can_id, UINT frame_id);

// 根据下标映射获取报文ID
UINT get_map_num2id(vector<UINT> &vec_can_id, int num);

// 计算两个报文负载的距离,曼哈顿距离（Manhattan Distance）
int frame_payload_distance(const struct can_frame *frame1, const struct can_frame  *frame2);

// 读取字节指定bit位的值
int read_bit_value(uint8_t *data, int bit_pos);

// 给字节指定bit位赋值(这里的value不是0就是1)
int write_bit_value(uint8_t *data, int bit_pos, int value);

// 从配置文件读取id轮廓
int read_id_profile(vector <ID_Profile> & id_profile, vector <UINT> & vec_can_id, CSimpleIniA & configurator);

// 打印id轮廓
int show_id_profile(vector <ID_Profile> & id_profile, vector <UINT> & vec_can_id);

// 保存id轮廓数据
int save_id_profile(vector <UINT> & vec_can_id, vector <ID_Profile> & id_profile, CSimpleIniA & configurator);

// 从配置文件读取字节轮廓
int read_byte_profile(vector< vector<Byte_Profile> > &byte_profile, vector<UINT> &vec_can_id, CSimpleIniA &configurator);

// 打印字节轮廓
int show_byte_profile(vector< vector<Byte_Profile> > &byte_profile, vector<UINT> &vec_can_id);

// 保存轮廓数据
int save_byte_profile(vector<UINT> &vec_can_id, vector< vector<Byte_Profile> > &byte_profile, CSimpleIniA & configurator);

// 读取bit轮廓
int read_bit_profile(vector< vector<Bit_Profile> > &bit_profile, vector<UINT> &vec_can_id, CSimpleIniA &configurator);

// 打印bit轮廓
int show_bit_profile(vector< vector <Bit_Profile> > & bit_profile, vector <UINT> & vec_can_id);

// 保存bit轮廓数据
int save_bit_profile(vector<UINT> &vec_can_id, vector< vector <Bit_Profile> > & bit_profile, CSimpleIniA & configurator, int sliding_window_size);

// 最大最小字节值检测
int max_min_byte_value_detect(Byte_Profile &byte_profile, BYTE byte_value, int &is_anomaly);

// 根据字节轮廓对当前报文进行检测
int bit_scanner_detect(Bit_Profile &bit_profile, int bit_value, int sliding_window_size, int &is_anomaly);

// 合法ID后继检测器
int id_matrix_detect(ID_Profile &id_profile, UINT can_id, int &is_anomaly);

// 计算两个8字节报文汉明距离
// @ 输入两个字节序列
int calculate_hamming_distance(uint8_t *data_previous, uint8_t *data_current);

// hamming距离检测器
int hamming_distance_detect(ID_Profile &id_profile, BYTE *current_payload, int &is_anomaly);


// 比对真实值和检测值的结果,返回本次篡改样本的tp数目 tn数目 fp 数目 fn 数目(1列label VS 1列result)
int compare_result_one_by_one(  FILE *label_file, FILE *detect_result, int *tp, int *tn, int *fp, int *fn);

// 比对真实值和检测值的结果,返回本次篡改样本的tp数目 tn数目 fp 数目 fn 数目(以攻击的方式度量性能)
int compare_result_by_attack_hit(  FILE *label_file, FILE *detect_result, int *tp, int *tn, int *fp, int *fn, int *delay);


// 标记数组若干个不重复的位置，最大8个
int mark_pos(int *mark, int mark_num);

// 解析从csv文件读取的CAN报文   
// timestamp,id,data0 data1 data2 data3 data4 data5 data6 data 7
int parse_read_buffer(char *data_buffer, int64_t *timestamp, struct can_frame *new_can);

// 将CAN报文以固定格式存储到文件中
int save_read_buffer(FILE *data_file, int64_t *timestamp, struct can_frame *new_can);

// 将报文数据解析成实际数值（解析原子函数
int can_to_data(uint8_t *data, int start_bit, int length);

// 逆解析，将实际数值转换到相对应的字节数组中
void data_to_can(int value, uint8_t *data, int start_bit, int length);

// 根据二进制预算操作处理两个01字符
int adjecent_bit_operation(int a, int b, BIT_OP bit_op);

// 根据攻击类型获取字符串类型的攻击名称
int get_attack_name(char *attack_name, ATTACK_TYPE attack_type);

// 根据攻击强度获取注入报文的间隔
int get_injection_interval(int injection_strength);

#ifdef __cplusplus
}       // extern "C"
#endif


#endif  // COMMON_H_INCLUDED
