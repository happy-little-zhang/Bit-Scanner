#include "common.h"
#include "log.h"

// 获取系统时间
long long int get_system_ms_time()
{
    struct timeval tv;
	memset(&tv, 0, sizeof(struct timeval));
    gettimeofday(&tv,NULL);    //该函数在sys/time.h头文件中
    //printf("sys time: %ld s, %ld us\n", tv.tv_sec , tv.tv_usec);
    //printf("sys time: %lld ms\n", (unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
    return (unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// 获取系统微秒时间us
long long int get_system_us_time()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);    //该函数在sys/time.h头文件中
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

// select 精确睡眠,单位ms 
int select_sleep(int ms_time)
{
    struct timeval msgrcv_timeout={0, ms_time * 1000};   //{秒,微秒}
    select(0, NULL, NULL, NULL, &msgrcv_timeout);
    return RET_OK;
}


// 标准时间(2021-08-04 11:43:07.123)转换成时间戳(ms)
long long int stand2tick_time(char *str_time)
{
    struct tm stm;  
    int iY, iM, iD, iH, iMin, iS;  
	double dS;     // 秒钟的小数部分 
  
    memset(&stm,0,sizeof(stm));  
  
    iY = atoi(str_time);  
    iM = atoi(str_time + 5);  
    iD = atoi(str_time + 8);  
    iH = atoi(str_time + 11);  
    iMin = atoi(str_time + 14);  
    iS = atoi(str_time + 17);  

	// 获取小数点后面的ms
	dS = atof(str_time + 17);
	dS = dS - int(dS);
  
    stm.tm_year=iY-1900;  
    stm.tm_mon=iM-1;  
    stm.tm_mday=iD;  
    stm.tm_hour=iH;  
    stm.tm_min=iMin;  
    stm.tm_sec=iS;  
  
    printf("%d-%0d-%0d %0d:%0d:%0d\n", iY, iM, iD, iH, iMin, iS);  

	long long int res = mktime(&stm);

	// printf("system_time: %ld", res);

	res = res * 1000 + (int)(dS * 1000);
  
    return res;
}



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
int can_frame_show(can_frame *frame)
{
    if (!frame)
    {
        LOG_ERROR("invalid input!");
        return RET_ERROR;
    }

    char szPrintInfo[512] = {0};
    char szInfo[64] = {0};
    sprintf(szInfo, "[%lld %08X]", get_system_ms_time(), frame->can_id);
    strcat(szPrintInfo, szInfo);
    
    for (int i = 0; i < frame->can_dlc; ++i)
    {
        char szTemp[8] = {0};
        sprintf(szTemp, " %02X", frame->data[i]);
        strcat(szPrintInfo, szTemp);
    }
    LOG_DEBUG("%s", szPrintInfo);

    return RET_OK;
}


// 复制CAN报文 
int can_frame_duplicate(struct can_frame *destination, struct can_frame  *source)
{
    if(!destination || !source)
    {
        LOG_ERROR("invalid input: destination[%p], source[%p]!", destination, source);
        return RET_ERROR;
    }

    memcpy(destination, source, sizeof(struct can_frame));
    return RET_OK;
}


// 根据报文ID获取下标映射
int get_map_id2num(vector<UINT> &vec_can_id, UINT frame_id)
{
    int res = RET_ERROR;

	// 遍历容器，寻找frameID
    for(size_t i = 0; i < vec_can_id.size(); i++)
    {
        if(vec_can_id[i] == frame_id)
        {
            res = i;
		    break;
        }
    }

    return res;    
}

// 根据下标映射获取报文ID
UINT get_map_num2id(vector<UINT> &vec_can_id, int num)
{
    int ret = RET_ERROR;

    if(num < 0 || num >= (int)vec_can_id.size())
    {
        LOG_ERROR("invalid num: num[%d] !", num);
        return ret; 
    }

    return vec_can_id[num];        
}

// 计算两个报文负载的距离,曼哈顿距离（Manhattan Distance）
int frame_payload_distance(const struct can_frame *frame1, const struct can_frame  *frame2)
{
    if(!frame1 || !frame2)
    {
        LOG_ERROR("invalid input: frame1[%p], frame2[%p]!", frame1, frame2);
        return RET_ERROR;
    }    

    int res = 0;
    for(int i = 0; i < FRAME_DLC; i++)
    {
        int a = (int)(frame1->data[i]);
		int b = (int)(frame2->data[i]);
        res += abs(a-b);
    }

    return res;
}


// 读取字节指定bit位的值
int read_bit_value(uint8_t *data, int bit_pos)
{
    if (bit_pos < 0 || bit_pos > 7) 
	{
        printf("Error: bit_pos out of range.\n");
        return RET_ERROR;
    }

	uint8_t temp_data = *data;

	int ret = 0;		   // 返回的结果 
	temp_data = temp_data << (bit_pos);       // 字节左移pos位，将需访问的bit移至最高位以清空多余数值
	temp_data = temp_data >> 7;               // 右移7位，将需访问的bit移至最低位以读取数值
	ret = (int)temp_data;
	return ret;

}

// 给字节指定bit位赋值(这里的value不是0就是1)
int write_bit_value(uint8_t *data, int bit_pos, int value)
{
    if(value == 0)
    {
        uint8_t temp_data = 1;        
	
		temp_data = temp_data << (7-bit_pos);     // 00010000
		temp_data = ~temp_data;                   // ~按位取反
		*data = *data & temp_data;               // &与操作   
    }
	else if(value == 1)
	{
        uint8_t temp_data = 1;
 		temp_data = temp_data << (7-bit_pos);       // 字节左移7-bit_pos位，将需访问的bit移至最高位以清空多余数值
		*data = *data | temp_data;                  // |或操作  	
	}


    return RET_OK;

}


// 读取ID轮廓
int read_id_profile(vector<ID_Profile> &id_profile, vector<UINT> &vec_can_id, CSimpleIniA &configurator)
{
	char section_name[20] = SECTION_NAME_ID_RPOFILE; 					   
	char keys_name[36]; 										   
	char values[1024 * 8];    


	// 读取报文ID列表
	memset(keys_name, 0, sizeof(keys_name));
    memset(values, 0, sizeof(values));
	snprintf(keys_name, sizeof(keys_name), KEY_NAME_CAN_ID_LIST);
	const char *res;
	res = configurator.GetValue(section_name, keys_name);
	if(res != NULL)
	{
		strcpy(values, res);
	}
	else
	{
		LOG_DEBUG(RED "configurator getvalue faild!" NONE );
	
		return RET_ERROR;
	}
	// 使用分割函数
	char *ptr = NULL;
    char *saveptr = NULL;
    char *endptr = NULL;
	ptr = strtok_r(values, ",", &saveptr);
	while(ptr != NULL)
	{
	    UINT can_id = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制
        vec_can_id.push_back(can_id);
		ptr = strtok_r(NULL, ",", &saveptr);
	}


    int length_of_white_list = (int)vec_can_id.size();
    for(int i = 0; i < length_of_white_list; i++)
    {
        ID_Profile temp_id_profile;
		id_profile.push_back(temp_id_profile);
    }


    // 读取配置文件ID转移矩阵
	for(int i = 0; i < length_of_white_list; i++)
	{
		memset(keys_name, 0, sizeof(keys_name));
		memset(values, 0, sizeof(values));
		snprintf(keys_name, sizeof(keys_name), "0x%08x.legal_next_id", get_map_num2id(vec_can_id, i));
		const char *res;
		res = configurator.GetValue(section_name, keys_name);
		if(res != NULL)
		{
			strcpy(values, res);
		}
		else
		{
			LOG_DEBUG(RED "configurator getvalue faild!" NONE );
		
			return RET_ERROR;
		}
		
		// 使用分割函数
		char *ptr = NULL;
		char *saveptr = NULL;
		char *endptr = NULL;
		ptr = strtok_r(values, ",", &saveptr);
		while(ptr != NULL)
		{
			UINT can_id = strtol(ptr, &endptr, 16); 		   // 参数16表示转化为16进制
			id_profile[i].legal_next_id.push_back(can_id);
			ptr = strtok_r(NULL, ",", &saveptr);
		} 
	
	}
   
    
	// 读取配置文件hamming_dist
	for(int i = 0; i < length_of_white_list; i++)
	{
		memset(keys_name, 0, sizeof(keys_name));
		memset(values, 0, sizeof(values));
		snprintf(keys_name, sizeof(keys_name), "0x%08x.hamming_dist", get_map_num2id(vec_can_id, i));
		const char *res;
        res = configurator.GetValue(section_name, keys_name);
		if(res != NULL)
		{
			strcpy(values, res);
		}
		else
		{
			LOG_DEBUG(RED "configurator getvalue faild!" NONE );
		
			return RET_ERROR;
		}		

		// 使用分割函数
		char *ptr = NULL;
		char *saveptr = NULL;
		char *endptr = NULL;
		ptr = strtok_r(values, ",", &saveptr);
		int min_value = strtol(ptr, &endptr, 10);
		ptr = strtok_r(NULL, ",", &saveptr);
		int max_value = strtol(ptr, &endptr, 10);
		id_profile[i].min_hamming_dist = min_value;
		id_profile[i].max_hamming_dist = max_value;

	}

    
    return RET_OK;
}

// 打印ID轮廓
int show_id_profile(vector <ID_Profile> & id_profile, vector <UINT> & vec_can_id)
{
    char buffer[65500] = {0};
	
    int length_of_white_list = (int)vec_can_id.size();
	
    // 打印ID转移矩阵
//    for(int i = 0; i < length_of_white_list; i++)
//    {
//	    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "0x%08x.legal_next_id = ", get_map_num2id(vec_can_id, i));
//
//		int legal_next_id_size = id_profile[i].legal_next_id.size();
//	    for(int j = 0; j < legal_next_id_size; j++)
//		{
//			// 第一次写入
//			if(j == 0)
//			{
//				snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%x", id_profile[i].legal_next_id[j]);
//			}
//			else
//			{
//				snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), ",%x", id_profile[i].legal_next_id[j]);
//			}			     
//		}    
//
//		snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "\n");
//    } 

    for(int i = 0; i < length_of_white_list; i++)
    {
        for(int j = 0; j < length_of_white_list; j++)
        {
           
            UINT can_id = get_map_num2id(vec_can_id, j);
			
            int find = NOT_EXIST;
			find = get_map_id2num(id_profile[i].legal_next_id, can_id);
			if(find == NOT_EXIST)
			{
			    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%d", INVALID);
			}
			else
			{
				snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%d", VALID);
			}
        }
    
		snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "\n");
    } 
	
    // 打印配置文件hamming距离
    for(int i = 0; i < length_of_white_list; i++)
    {
		snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), 
			"0x%08x.hamming_dist = [%d, %d]\n", get_map_num2id(vec_can_id, i), id_profile[i].min_hamming_dist, id_profile[i].max_hamming_dist);
    } 

	LOG_DEBUG("\n%s", buffer);

    return RET_OK;
}

// 保存轮廓数据
int save_id_profile(vector<UINT> &vec_can_id, vector<ID_Profile> &id_profile, CSimpleIniA & configurator)
{
	// 将字节值相关参数转换成结构化字符串
	char section_name[20] = SECTION_NAME_ID_RPOFILE;       						  // 配置文件中的“节名”
	char keys_name[100] = {0};                                            // 键名
	char values[1024 * 8]= {0};                                           // 键名对应的值

    int length_of_white_list = (int)vec_can_id.size(); 

    // 存储报文ID列表
	memset(keys_name, 0, sizeof(keys_name));
    memset(values, 0, sizeof(values));
	snprintf(keys_name, sizeof(keys_name), KEY_NAME_CAN_ID_LIST);
	
	for(int i = 0; i < length_of_white_list; i++)
	{
	    // 第一次写入
	    if(i == 0)
	    {
	        snprintf(values + strlen(values), sizeof(values) - strlen(values), "%x", vec_can_id[i]);
	    }
		else
		{
			snprintf(values + strlen(values), sizeof(values) - strlen(values), ",%x", vec_can_id[i]);
		}
	    
	}
	configurator.SetValue(section_name, keys_name, values);



    // 保存合法后继ID
	for(int i = 0; i < length_of_white_list; i++)
	{
		memset(keys_name, 0, sizeof(keys_name));
		memset(values, 0, sizeof(values));
	    snprintf(keys_name, sizeof(keys_name), "0x%08x.legal_next_id", get_map_num2id(vec_can_id, i));
		
		int legal_next_id_size = id_profile[i].legal_next_id.size();
		
	    for(int j = 0; j < legal_next_id_size; j++)
		{
			// 第一次写入
			if(j == 0)
			{
				snprintf(values + strlen(values), sizeof(values) - strlen(values), "%x", id_profile[i].legal_next_id[j]);
			}
			else
			{
				snprintf(values + strlen(values), sizeof(values) - strlen(values), ",%x", id_profile[i].legal_next_id[j]);
			}			     
		}
		configurator.SetValue(section_name, keys_name, values);
	}
	
    // 保存hanmming距离信息
	for(int i = 0; i < length_of_white_list; i++)
	{
		memset(keys_name, 0, sizeof(keys_name));
		memset(values, 0, sizeof(values));
		snprintf(keys_name, sizeof(keys_name), "0x%08x.hamming_dist", get_map_num2id(vec_can_id, i));
		snprintf(values, sizeof(values), "%d,%d", id_profile[i].min_hamming_dist, id_profile[i].max_hamming_dist);
		configurator.SetValue(section_name, keys_name, values);
	}

    return RET_OK;
}



// 从配置文件读取字节轮廓
int read_byte_profile(vector< vector<Byte_Profile> > &byte_profile, vector<UINT> &vec_can_id, CSimpleIniA &configurator)
{
    
	char section_name[20] = SECTION_NAME_BYTE_RPOFILE; 					   
	char keys_name[36]; 										   
	char values[1024 * 8];    

	
    int length_of_white_list = (int)vec_can_id.size();
    for(int i = 0; i < length_of_white_list; i++)
    {
        // LOG_DEBUG("0x%08x", vec_can_id[i]);
        
		// 增加轮廓数目
		vector<Byte_Profile> temp_byte_profile(FRAME_DLC);
		byte_profile.push_back(temp_byte_profile);
    }

    
    // -------------------------------------------------从配置文件中读取字节轮廓------------------------------------------------- //

	
    // 读取字节合法值
    for(int i = 0; i < length_of_white_list; i++)
    {
        for(int j = 0; j < FRAME_DLC; j++)
        {
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			
			snprintf(keys_name, sizeof(keys_name), "0x%08X.byte%d_legal_value", get_map_num2id(vec_can_id, i), j);
			
			const char *res_temp;
			res_temp = configurator.GetValue(section_name, keys_name);	 
			if(res_temp != NULL)
			{
				strcpy(values, res_temp);
			}
			else
			{
				//LOG_DEBUG(RED "configurator getvalue faild!" NONE );
			
				//return RET_ERROR;
			}
			
			// 使用分割函数
			char *ptr = NULL;
			char *saveptr = NULL;
			char *endptr = NULL;
			ptr = strtok_r(values, ",", &saveptr);
			while ( ptr != NULL )
			{
				int m = (int)strtol(ptr, &endptr, 10);			  // 参数10表示转化为10进制, 将参数nptr字符串根据参数base来转换成长整型数
				byte_profile[i][j].legal_data[m] = VALID;
				
				ptr = strtok_r(NULL, ",", &saveptr);
			}

        }
		
    } 


    // 读取字节最小最大值
    for(int i = 0; i < length_of_white_list; i++)
    {
        for(int j = 0; j < FRAME_DLC; j++)
        {
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			
			snprintf(keys_name, sizeof(keys_name), "0x%08" PRIX32 ".byte%d_range", vec_can_id[i], j);

			const char *res_temp;
			res_temp = configurator.GetValue(section_name, keys_name);	 
			if(res_temp != NULL)
			{
				strcpy(values, res_temp);
			}
			else
			{
				//LOG_DEBUG(RED "configurator getvalue faild!" NONE );
			
				//return RET_ERROR;
			}
			
			// 使用分割函数
			char *ptr = NULL;
			char *saveptr = NULL;
			char *endptr = NULL;
			ptr = strtok_r(values, ",", &saveptr);
			byte_profile[i][j].min_byte_value = (int)strtol(ptr, &endptr, 10);
			ptr = strtok_r(NULL, ",", &saveptr);
			byte_profile[i][j].max_byte_value = (int)strtol(ptr, &endptr, 10);

        }
    } 


	// -------------------------------------------------参数读取完毕------------------------------------------------- //

	return RET_OK;
}


// 打印字节轮廓
int show_byte_profile(vector< vector<Byte_Profile> > &byte_profile, vector<UINT> &vec_can_id)
{
	int length_of_white_list = (int)vec_can_id.size();
	char buffer[65500] = {0};
	
	snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "length_of_white_list: %d\n", length_of_white_list);
	
 	// -------------------------------------------------打印字节轮廓------------------------------------------------- //


    // 打印最小最大字节值
    memset(buffer, 0, sizeof(buffer));
	for(int i = 0; i < length_of_white_list; i++)
	{
		for(int j = 0; j < FRAME_DLC; j++)
		{
			snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), 
			    "0x%08X.byte%d_range = [%d, %d]", get_map_num2id(vec_can_id, i), j, byte_profile[i][j].min_byte_value, byte_profile[i][j].max_byte_value);			

			snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "\n");
		}
	}
	LOG_DEBUG("\n%s", buffer);    


    return RET_OK;
}

// 保存轮廓数据
int save_byte_profile(vector<UINT> &vec_can_id, vector< vector<Byte_Profile> > &byte_profile, CSimpleIniA & configurator)
{
	// 将字节值相关参数转换成结构化字符串
	char section_name[20] = SECTION_NAME_BYTE_RPOFILE;       						  // 配置文件中的“节名”
	char keys_name[100] = {0};                                            // 键名
	char values[1024 * 8]= {0};                                           // 键名对应的值

    int can_id_size = (int)vec_can_id.size();


    // 存储字节合法值
    for(int i = 0; i < can_id_size; i++)
    {
        for(int j = 0; j < FRAME_DLC; j++)
        {
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));

			int  is_first_value = 1;			
			for(int m = 0; m < BYTE_VALUE_NUM; m++)
			{
				int value = byte_profile[i][j].legal_data[m];
				if(value == VALID)
				{
					// 是否是第一次填入(第一次写入不加逗号，后续写入加逗号，保证存储为csv格式)
					if(is_first_value)
					{
						snprintf(keys_name, sizeof(keys_name), "0x%08" PRIX32 ".byte%d_legal_value", vec_can_id[i], j);
						snprintf(values + strlen(values), sizeof(values) - strlen(values), "%d", m);
						is_first_value = 0;
					}
					else
					{
						snprintf(values + strlen(values), sizeof(values) - strlen(values), ",%d", m);
					}
						
				}
					
			} 
			
			configurator.SetValue(section_name, keys_name, values);

        }
		
    }

    // 存储字节最小最大值
    for(int i = 0; i < can_id_size; i++)
    {
        for(int j = 0; j < FRAME_DLC; j++)
        {
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			
			snprintf(keys_name, sizeof(keys_name), "0x%08" PRIX32 ".byte%d_range", vec_can_id[i], j);
			snprintf(values + strlen(values), sizeof(values) - strlen(values), "%d,%d", byte_profile[i][j].min_byte_value, byte_profile[i][j].max_byte_value);
			
			configurator.SetValue(section_name, keys_name, values);

        }
    }    

    return RET_OK;
}



// 读取bit轮廓
int read_bit_profile(vector< vector<Bit_Profile> > &bit_profile, vector<UINT> &vec_can_id, CSimpleIniA &configurator)
{
	char section_name[20] = SECTION_NAME_BIT_RPOFILE; 					   
	char keys_name[100] = {0}; 										   
	char values[READ_PROFILE_BUFFER_SIZE] = {0};    

    int length_of_white_list = (int)vec_can_id.size();
    for(int i = 0; i < length_of_white_list; i++)
    {
        vector<Bit_Profile> temp_bit_profile(BIT_STREAM_NUM);
		bit_profile.push_back(temp_bit_profile);
    }

	for(int i = 0; i < length_of_white_list; i++)
	{
		for(int j = 0; j < BIT_STREAM_NUM; j++)
		{
			// 读取二进制检测器的合法值
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			snprintf(keys_name, sizeof(keys_name), "0x%08x.stream%d.legal_value", get_map_num2id(vec_can_id, i), j);
			
			const char *res_temp;
			res_temp = configurator.GetValue(section_name, keys_name);	 
			if(res_temp != NULL)
			{
				strcpy(values, res_temp);
			}
			else
			{
				//LOG_DEBUG(RED "configurator getvalue faild!" NONE );
			
				//return RET_ERROR;
			}
			
			// 使用分割函数
			char *ptr = NULL;
			char *saveptr = NULL;
			char *endptr = NULL;
			ptr = strtok_r(values, ",", &saveptr);				
			while ( ptr != NULL )
			{
				int m = (int)strtol(ptr, &endptr, 10);			  // 参数10表示转化为10进制, 将参数nptr字符串根据参数base来转换成长整型数
	
				bit_profile[i][j].tm_data[m] = VALID;
				
				ptr = strtok_r(NULL, ",", &saveptr);
			}		
	
	
			// 读取二进制检测器的辅助信息
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			snprintf(keys_name, sizeof(keys_name), "0x%08x.stream%d.parameter", get_map_num2id(vec_can_id, i), j);
			
			res_temp = NULL;
			res_temp = configurator.GetValue(section_name, keys_name);	 
			if(res_temp != NULL)
			{
				strcpy(values, res_temp);
			}
			else
			{
				//LOG_DEBUG(RED "configurator getvalue faild!" NONE );
			
				//return RET_ERROR;
			}
			
			// 使用分割函数
			ptr = NULL;
			saveptr = NULL;
			endptr = NULL;
			ptr = strtok_r(values, ",", &saveptr);
			for(int tt = 0; tt < 3; tt++)
			{
				if(ptr != NULL)
				{
					int m = (int)strtol(ptr, &endptr, 10);			  // 参数10表示转化为10进制, 将参数nptr字符串根据参数base来转换成长整型数
					ptr = strtok_r(NULL, ",", &saveptr);
					if(tt == 0)
					{
						bit_profile[i][j].total_num = m;
					}
					else if (tt == 1)
					{
						bit_profile[i][j].stable_num = m;
					}
					else if (tt == 2)
					{
						bit_profile[i][j].invalid_flag = m;
					}
				}
			}
	
		}
	
	} 


    return RET_OK;
}


// 打印bit轮廓
int show_bit_profile(vector< vector<Bit_Profile> > &bit_profile, vector <UINT> & vec_can_id)
{
    int length_of_white_list = (int)vec_can_id.size();


	// 统计二进制检测器的有效性
  	int valid_detector_num = 0;
    int invalid_detector_num = 0;
    for(int i = 0; i < length_of_white_list; i++)
    {
	    for(int j = 0; j < BIT_STREAM_NUM; j++)
	    {
		    if(bit_profile[i][j].invalid_flag == VALID)
		    {
			    invalid_detector_num++;
		    }
		    else
		    {
			    valid_detector_num++;
		    }
	    }	  
    }
   
    printf("valid/invalid_detector: [%d/%d]\n", valid_detector_num, invalid_detector_num);




	return RET_OK;
}

// 保存bit轮廓数据
int save_bit_profile(vector<UINT> &vec_can_id, vector< vector<Bit_Profile> > &bit_profile, CSimpleIniA & configurator, int sliding_window_size)
{
	// 将字节值相关参数转换成结构化字符串
	char section_name[20] = SECTION_NAME_BIT_RPOFILE;       			  // 配置文件中的“节名”
	char keys_name[100] = {0};                                            // 键名
	char values[READ_PROFILE_BUFFER_SIZE]= {0};                                           // 键名对应的值

    int length_of_white_list = (int)vec_can_id.size(); 

	for(int i = 0; i < length_of_white_list; i++)
	{
		for(int j = 0; j < BIT_STREAM_NUM; j++)
		{
			// 存储字节检测器的合法值
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			snprintf(keys_name, sizeof(keys_name), "0x%08x.stream%d.legal_value", get_map_num2id(vec_can_id, i), j);
			
			int num_len = pow(2, sliding_window_size);
			int  is_first_value = 1;
			for(int m = 0; m < num_len; m++)
			{
				int value = bit_profile[i][j].tm_data[m];
				if(value == VALID)
				{
					// 是否是第一次填入(第一次写入不加逗号，后续写入加逗号，保证存储为csv格式)
					if(is_first_value)
					{
						snprintf(values + strlen(values), sizeof(values) - strlen(values), "%d", m);
						is_first_value = 0;
					}
					else
					{
						snprintf(values + strlen(values), sizeof(values) - strlen(values), ",%d", m);
					}
								
				}				
			}
			
			// 将字节值相关参数写入配置文件
			if(is_first_value == 0)
			{
				configurator.SetValue(section_name, keys_name, values);
			}		
	
			// 存储字节检测器的辅助信息
			memset(keys_name, 0, sizeof(keys_name));
			memset(values, 0, sizeof(values));
			snprintf(keys_name, sizeof(keys_name), "0x%08x.stream%d.parameter", get_map_num2id(vec_can_id, i), j);
			snprintf(values + strlen(values), sizeof(values) - strlen(values), "%d", bit_profile[i][j].total_num);
			snprintf(values + strlen(values), sizeof(values) - strlen(values), ",%d", bit_profile[i][j].stable_num);
	
			int invalid_flag = INVALID;
			double stable_rate = (double)bit_profile[i][j].stable_num/(double)bit_profile[i][j].total_num;
			if(stable_rate <  DETECTOR_INVALID_THRESHOLD )
			{
				invalid_flag = VALID;
			}
			snprintf(values + strlen(values), sizeof(values) - strlen(values), ",%d", invalid_flag);
			
			configurator.SetValue(section_name, keys_name, values);
	
			
		}
	
	}


    return RET_OK;
}



// 最大最小字节值检测
int max_min_byte_value_detect(Byte_Profile &byte_profile, BYTE byte_value, int &is_anomaly)
{
	if(byte_value >= byte_profile.min_byte_value && byte_value <= byte_profile.max_byte_value)
	{
	    is_anomaly = NORMAL;
	}
	else
	{
	    is_anomaly = ABNORMAL;
	}

    return RET_OK;
}


// 根据字节轮廓对当前报文进行检测
int bit_scanner_detect(Bit_Profile &bit_profile, int bit_value, int sliding_window_size, int &is_anomaly)
{

	bit_profile.old_data <<= 1;     // 自身左移1位
	bit_profile.old_data[0] = bit_value;
	
	bit_profile.old_data_valid <<= 1;   // 自身左移1位
	bit_profile.old_data_valid[0] = VALID;
	
	int valid = bit_profile.old_data_valid[sliding_window_size-1];
	if(valid == VALID)
	{
		bit_profile.old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
		bit_profile.old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
		int data = bit_profile.old_data.to_ulong();         // 存储值转化为整型
		if(bit_profile.tm_data[data] == INVALID)
		{
			is_anomaly = ABNORMAL;

		}
		else
		{
			is_anomaly = NORMAL;
		}
	}


    return 0;
}



// 合法ID后继检测器
int id_matrix_detect(ID_Profile &id_profile, UINT can_id, int &is_anomaly)
{
    int find_flag = NOT_EXIST;

	find_flag = get_map_id2num(id_profile.legal_next_id, can_id);

	if(find_flag == NOT_EXIST)
	{
	    is_anomaly = ABNORMAL;
	}
	else
	{
	    is_anomaly = NORMAL;
	}

    return RET_OK;
}

// 计算两个8字节报文汉明距离
// @ 输入两个字节序列
int calculate_hamming_distance(uint8_t *data_previous, uint8_t *data_current)
{
    int res = 0;    

    for(int i = 0; i < FRAME_DLC; i++)           // 8个字节
    {
        for(int j = 0; j < FRAME_DLC; j++)      // 1个字节8个bit位
        {
            int value_a = read_bit_value(&data_previous[i], j);
			int value_b = read_bit_value(&data_current[i], j);

			if(value_a != value_b)
			{
			    res++;
			}
        }
    }

    return res;
}


// hamming距离检测器
int hamming_distance_detect(ID_Profile &id_profile, BYTE *current_payload, int &is_anomaly)
{
    if(id_profile.previous_payload_valid == VALID)
    {
		int hamming_dist = calculate_hamming_distance(id_profile.previous_payload, current_payload);
		
		if(hamming_dist >= id_profile.min_hamming_dist && hamming_dist <= id_profile.max_hamming_dist)
		{
			is_anomaly = NORMAL;
			
			// 更新存储的新负载
			for(int i = 0; i < FRAME_DLC; i++)
			{
				id_profile.previous_payload[i] = current_payload[i];
			}
		}
		else
		{
			// LOG_DEBUG(RED "hamming_dist[%d] over range[%d, %d] !" NONE, hamming_dist, id_profile.min_hamming_dist, id_profile.max_hamming_dist);
		
			id_profile.previous_payload_valid = INVALID;
		
			is_anomaly = ABNORMAL;
		}

    
    }
	else
	{
	    id_profile.previous_payload_valid = VALID;

		// 更新存储的新负载
		for(int i = 0; i < FRAME_DLC; i++)
		{
			id_profile.previous_payload[i] = current_payload[i];
		}

	}

    return RET_OK;
}


// 字节度量方法




// 比对真实值和检测值的结果,返回本次篡改样本的tp数目 tn数目 fp 数目 fn 数目(1列label VS 1列result)
int compare_result_one_by_one(  FILE *label_file, FILE *detect_result, int *tp, int *tn, int *fp, int *fn)
{
    if(!label_file || !detect_result)
    {
        LOG_DEBUG(RED "invalid input! label_file[%p], detect_result[%p]" NONE, label_file, detect_result);
		return RET_ERROR;
    }

	int label = NOT_EXIST; 
	int result[COMPARSION_METHOD_NUM];
	
	// memset初始化为0或-1有效，为了保证初始化正确，这里用for循环进行初始化
	for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
	{
		result[i] = NOT_EXIST;
	}

	char data_buffer[1024] = "";

	while( (fscanf( label_file, "%d", &label) != EOF) && ( (fgets(data_buffer, sizeof(data_buffer), detect_result ) != NULL) ) )
	{

		// 分割函数获取各方法的检测结果
		char *ptr = NULL;
		char *saveptr = NULL;
		char *endptr = NULL;
		ptr = strtok_r(data_buffer, ",", &saveptr); 			
		int pos = 0;
		while(ptr != NULL)
		{
			result[pos] = strtol(ptr, &endptr, 10);
			ptr = strtok_r(NULL, ",", &saveptr);
			pos++;
		}

        for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
        {
            // printf("read value: %d, %d \n", label, result[i]);
			int sum = label * 10 + result[i];
			switch(sum)
			{
				case 0:
					// 0-0
					tn[i] ++;
					break;
				case 1:
					 // 0-1
					fp[i] ++;
					break;
				case 10:
					 // 1-0
					fn[i] ++;
					break;
				case 11:
					 // 1-1
					tp[i] ++;
					break;
				default:
					LOG_DEBUG(RED "unknown label-result: %d-%d" NONE, label, result[i]);
					break;
			}        
        }


	}

    return RET_OK;
}


// 比对真实值和检测值的结果,返回本次篡改样本的tp数目 tn数目 fp 数目 fn 数目(1列label VS 1列result)，检测攻击是否被检测到
int compare_result_by_attack_hit(  FILE *label_file, FILE *detect_result, int *tp, int *tn, int *fp, int *fn, int *delay)
{
    if(!label_file || !detect_result)
    {
        LOG_DEBUG(RED "invalid input! label_file[%p], detect_result[%p]" NONE, label_file, detect_result);
		return RET_ERROR;
    }

	int label = NOT_EXIST; 
	int	result[COMPARSION_METHOD_NUM];

	int count_label = 0;             //  ground_truth 文件行计数 
	int count_result = 0;            //  检测结果文件 行计数  
	int attack_num_begin_ground_truth = NOT_EXIST;
	int attack_num_end_ground_truth = NOT_EXIST;
	int attack_num_begin_detection[COMPARSION_METHOD_NUM];            
	int attack_num_end_detection[COMPARSION_METHOD_NUM];
	
	// memset初始化为0或-1有效，为了保证初始化正确，这里用for循环进行初始化
	for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
	{
		result[i] = NOT_EXIST;
	    attack_num_begin_detection[i] = NOT_EXIST;
		attack_num_end_detection[i] =  NOT_EXIST;
	}
	
	char data_buffer[1024] = "";

    // 查找ground_truth开始攻击的位置以及攻击结束的位置
    while(fscanf( label_file, "%d", &label) != EOF)
    {
        count_label++;
        if(label == ABNORMAL)
        {
            if(attack_num_begin_ground_truth == NOT_EXIST && attack_num_end_ground_truth == NOT_EXIST)
            {
             	attack_num_begin_ground_truth = count_label;
				attack_num_end_ground_truth = count_label;           
            }
			else
			{
			    attack_num_end_ground_truth = count_label;   
			}
			
        }
    }

	// 查找各检测方法检测结果中开始攻击的位置以及攻击结束的位置
	while( fgets(data_buffer, sizeof(data_buffer), detect_result ) != NULL)
	{
	    //printf("read_result_buffer: %s\n", data_buffer);
        count_result++;
		
        // 分割函数获取各方法的检测结果
		char *ptr = NULL;
		char *saveptr = NULL;
		char *endptr = NULL;
		ptr = strtok_r(data_buffer, ",", &saveptr); 			
		int pos = 0;
		while(ptr != NULL)
		{
			result[pos] = strtol(ptr, &endptr, 10);
			ptr = strtok_r(NULL, ",", &saveptr);
			pos++;
		}  
		
//		printf("read result:");
//        for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
//		{
//		    printf(" %d", result[i]);
//        }
//		printf("\n");
   
		for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
		{
	        if(result[i] == ABNORMAL)
	        {
	            // printf(YELLOW " %d\n" NONE, count_result);
	            if(attack_num_begin_detection[i] == NOT_EXIST)
	            {
	             	attack_num_begin_detection[i] = count_result;      
	            }

				attack_num_end_detection[i] = count_result;
	        }		
		}

    }

    // test printf
    printf("ground_truth (%d, %d)\n", attack_num_begin_ground_truth, attack_num_end_ground_truth);
    for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
    {
        printf("method [%d] detect (%d, %d)\n", i, attack_num_begin_detection[i], attack_num_end_detection[i]);
    }


	for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
	{
	    if(attack_num_begin_ground_truth == NOT_EXIST && attack_num_begin_detection[i] == NOT_EXIST)
	    {
	        tn[i]++;
	    }
		else if(attack_num_begin_ground_truth == NOT_EXIST && attack_num_begin_detection[i] != NOT_EXIST)
		{
		    fp[i]++;
		}
		else if(attack_num_begin_ground_truth != NOT_EXIST && attack_num_begin_detection[i] == NOT_EXIST)
		{
		    fn[i]++;
		}
		else if(attack_num_begin_ground_truth != NOT_EXIST && attack_num_begin_detection[i] != NOT_EXIST)
		{
		    // 检测区域在实际攻击区域之间
	        if( (attack_num_begin_detection[i] >= attack_num_begin_ground_truth) && (attack_num_end_detection[i] <= attack_num_end_ground_truth) )
	        {
	            tp[i]++;
				delay[i] = attack_num_begin_detection[i] - attack_num_begin_ground_truth;
	        }
			else
			{
			    fp[i]++;
			}
		
		}	    

		//printf("tp[%d] tn[%d] fp[%d] fn[%d]\n", tp[i], tn[i], fp[i], fn[i]);
	}


    return RET_OK;
}


// 标记数组若干个不重复的位置，最大8个
int mark_pos(int *mark, int mark_num)
{
	for(int i = 0; i < mark_num; i++)
	{
	    int mark_negative_size = 0;
		for(int j = 0; j < FRAME_DLC; j++)
		{
		    if(mark[j] == INVALID)
		    {
		        mark_negative_size++;
		    }
		}

		int pos = rand()%mark_negative_size;
        int temp_pos = 0;
		for(int j = 0; j < FRAME_DLC; j++)
		{
		    if(mark[j] == INVALID)
		    {
		        if(temp_pos == pos)
		        {
		            mark[j] = VALID;
					break;
		        }
				else
				{
					temp_pos++;
				}
		    }
		}		
	}

    return RET_OK;
}


// 解析从csv文件读取的CAN报文   
// timestamp,id,data0 data1 data2 data3 data4 data5 data6 data 7
int parse_read_buffer(char *data_buffer, int64_t *timestamp, struct can_frame *new_can)
{
	char *ptr = NULL;
    char *saveptr = NULL;
    char *endptr = NULL;
	// 报文时间戳
	ptr = strtok_r(data_buffer, ",", &saveptr);             
	*timestamp = strtol(ptr, &endptr, 10);            // 参数10表示转化为10进制

	// 报文ID
	ptr = strtok_r(NULL, ",", &saveptr);
	new_can->can_id = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制

    // 报文负载，8字节值
	ptr = strtok_r(NULL, " ", &saveptr);                  // 跳过第一个空格
	int byte_pos = 0;
	while(ptr != NULL)
	{
	    new_can->data[byte_pos] = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制
		ptr = strtok_r(NULL, " ", &saveptr);
		byte_pos++;
	}    

    return 0;
}

// 将CAN报文以固定格式存储到文件中
int save_read_buffer(FILE *data_file, int64_t *timestamp, struct can_frame *new_can)
{
	fprintf(data_file, "%ld,%08x,", *timestamp, new_can->can_id);
	for(int i = 0; i < FRAME_DLC; i++)
	{
		if(i == 0)
		{
			fprintf(data_file, "%02x", new_can->data[i]);
		}
		else
		{
			fprintf(data_file, " %02x", new_can->data[i]);
		}
	}
	fprintf(data_file, "\n"); 

    return RET_OK;
}


// 将报文数据解析成实际数值（解析原子函数
int can_to_data(uint8_t *data, int start_bit, int length)
{
    int data_length = 8;         /* 默认CAN报文的数据数组长度均为8 */
    uint32_t m_data = 0;         /* CAN报文的数值（总线上传输的数值） */
    uint8_t m_data_byte[8];      /* 暂存CAN报文数据的数组 */
    int ret = 0;                 /* 返回的结果 */
    int k = 0;                   

    int start_byte, start_bit_offset;
    int end_byte, end_bit_offset;

    start_byte = start_bit / 8;                           /* 起始字节 */
    start_bit_offset = start_bit % 8;                     /* 起始字节位偏移 */
    end_byte = (start_bit + length - 1) / 8;              /* 终了字节 */
    end_bit_offset = (start_bit + length - 1) % 8;        /* 终了字节位偏移 */

    for (int i = 0; i < data_length; i++)
	{
        m_data_byte[i] = data[i];
    }

    for (int j = end_byte; j >= start_byte; j--)
	{
        m_data = m_data | m_data_byte[j];
        if (j != start_byte)
            m_data = m_data << 8;
        k = k + 1;
    }

    if (k > 4)
	{
        LOG_ERROR("data length is bigger than 4, please check data!");      // 这里m_data 是int类型，最大长度仅有4个字节
    }
	else
	{
        m_data = m_data << (4 * 8 - k * 8 + (7 - end_bit_offset));
        m_data = m_data >> ((4 * 8 - k * 8 + (7 - end_bit_offset)) + start_bit_offset);
    }


    ret = m_data;
	
    return ret;
}

// 逆解析，将实际数值转换到相对应的字节数组中
void data_to_can(int value, uint8_t *data, int start_bit, int length)
{
    uint32_t m_data = 0;
    m_data = value;   

    int start_byte, start_bit_offset;
    int end_byte;
    //int end_bit_offset;

    start_byte = start_bit / 8;                          /* 起始字节 */
    start_bit_offset = start_bit % 8;                    /* 起始字节位偏移 */
    end_byte = (start_bit + length - 1) / 8;             /* 终了字节 */
    //end_bit_offset = (start_bit + length - 1) % 8;     /* 终了字节位偏移 */

    m_data = m_data << start_bit_offset;      /* 填充起始字节偏移 */

    for(int i = start_byte; i <= end_byte; i++){
        data[i] = m_data | data[i];
        if (i != end_byte)
            m_data = m_data >> 8;
    }

    return;
}


// 根据二进制预算操作处理两个01字符
int adjecent_bit_operation(int a, int b, BIT_OP bit_op)
{
    if( ( a != 0 && a != 1 ) || ( b != 0 && b != 1 ) )
    {
        LOG_DEBUG(RED "input error, a: %d, b: %d" NONE, a, b);
    }
	
	BYTE result = 0;

	switch (bit_op)
		{
		    case BIT_OP_XOR: result = a^b; break;
			case BIT_OP_AND: result = a&b; break;
			case BIT_OP_OR: result = a|b; break;
			case BIT_OP_NAND: result = ~(a&b); break;
			case BIT_OP_NOR: result = ~(a|b); break;
            default: LOG_DEBUG(RED "bit_op error: %d" NONE, bit_op);
		}
    result = read_bit_value(&result, 7);
    
    return result;
}

// 根据攻击类型获取字符串类型的攻击名称
int get_attack_name(char *attack_name, ATTACK_TYPE attack_type)
{
    char array_attack_names[][ATTACK_NAME_MAX_SIZE] = {"random_injection", "targeted_injection", "replay", "random_tamper", "targeted_tamper"};

    snprintf(attack_name, ATTACK_NAME_MAX_SIZE, "%s", array_attack_names[attack_type]);

    return RET_OK;
}

// 根据攻击强度获取注入报文的间隔us
int get_injection_interval(int injection_strength)
{
    int inject_interval[MAX_INJECTION_STRENGTH] = {1, 10, 20, 50, 100, 200, 500, 1000};

    return inject_interval[MAX_INJECTION_STRENGTH - injection_strength] * 1000;
}

