#include "common.h"            // 公共模块库函数


// 全局变量
// 0-min_max 1-hamming_detect, 2-id_matrix, 3-bit_scanner 4-sim_bit_scanner 
char method_name[COMPARSION_METHOD_NUM][20] = {"min_max", "hamming", "id_matrix", "bit_scanner", "sim_bit_scanner"};   


// 生成随机ID (注，排除数据集中负载始终不变的的ID，通过汉明距离为0可以检测出来)
int get_rand_can_id(vector<UINT> &vec_can_id)
{
    UINT special_id[] = {0x0cfc1313, 0x0cff2a13, 0x18ffc021, 0x18f206f6, 0x18ff2df7, 0x19ffade9};
	int special_id_num = sizeof(special_id)/sizeof(special_id[0]);

    int can_id_size = (int)vec_can_id.size();
	
	int target_id_num = 0;

    while(1)
    {
    	target_id_num = rand()%can_id_size;
    	UINT target_id = get_map_num2id(vec_can_id, target_id_num); 

        int find_flag = INVALID;
		for(int i = 0; i < special_id_num; i++)
		{
		    if(target_id == special_id[i])
		    {
		        find_flag = VALID;
				break;
		    }
		}

		if(find_flag == INVALID)
		{
		    break;
		}
    }

	return target_id_num;
}


// 将原数据分割成两部分，前一半用于训练，后一半用于测试
int split_data_to_train_and_test()
{

	for(int file_num = 1; file_num <= MAX_FILE_NUM; file_num++)
    {
        char raw_data_path[100] = "";
	    snprintf(raw_data_path, sizeof(raw_data_path), "../dataset/raw/%03d.csv", file_num);
		char train_data_path[100] = "";
	    snprintf(train_data_path, sizeof(train_data_path), "../dataset/train/%03d.csv", file_num);
		char test_data_path[100] = "";
	    snprintf(test_data_path, sizeof(test_data_path), "../dataset/test/%03d.csv", file_num);

        LOG_DEBUG("split data: (%s)", raw_data_path);

		FILE *raw_data_set = fopen(raw_data_path, MODE_R_PLUS);
		if( raw_data_set == NULL )
		{
			LOG_DEBUG(RED "open raw_data_path file failed!" NONE);
		
			return RET_ERROR;
		}
		
		FILE *train_data_set = fopen(train_data_path, MODE_W_PLUS);
		if( train_data_set == NULL )
		{
			LOG_DEBUG(RED "open train_data_path file failed!" NONE);
		
			return RET_ERROR;
		}
		
		FILE *test_data_set = fopen(test_data_path, MODE_W_PLUS);
		if( test_data_set == NULL )
		{
			LOG_DEBUG(RED "open test_data_path file failed!" NONE);
		
			return RET_ERROR;
		}
		
		// 遍历数据
		int64_t current_readtime;			
		unsigned int can_id;
		unsigned int frame_data[FRAME_DLC];
		
		unsigned long read_count = 0;
		
		// 读取原始的CAN报文
	    char data_buffer[1024] = "";
		// 读取原始的CAN报文
		while( fgets(data_buffer, sizeof(data_buffer), raw_data_set ) != NULL)
		{
		    // printf("%s\n", data_buffer);
		    char *ptr = NULL;
	    	char *saveptr = NULL;
	    	char *endptr = NULL;
			ptr = strtok_r(data_buffer, ",", &saveptr);             
			current_readtime = strtol(ptr, &endptr, 10);            // 参数10表示转化为10进制
			
			ptr = strtok_r(NULL, ",", &saveptr);
			can_id = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制

			ptr = strtok_r(NULL, " ", &saveptr);                  // 跳过第一个空格
			int byte_pos = 0;
			while(ptr != NULL)
			{
		    	frame_data[byte_pos] = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制
				ptr = strtok_r(NULL, " ", &saveptr);
				byte_pos++;
			}		
			read_count++;
		}
		
		// 以1:1的比例拆分数据集为训练，测试
		unsigned long split_num = read_count/100*50;    // 1:1
		//unsigned long split_num = read_count;
		
		// 将文件重新定位到开头
		rewind(raw_data_set);  
		read_count = 0;

		// 读取原始的CAN报文
		while( fgets(data_buffer, sizeof(data_buffer), raw_data_set ) != NULL)
		{
		    //printf("%s\n", data_buffer);
		    char *ptr = NULL;
	    	char *saveptr = NULL;
	    	char *endptr = NULL;
			ptr = strtok_r(data_buffer, ",", &saveptr);             
			current_readtime = strtol(ptr, &endptr, 10);            // 参数10表示转化为10进制
			
			ptr = strtok_r(NULL, ",", &saveptr);
			can_id = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制

			ptr = strtok_r(NULL, " ", &saveptr);                  // 跳过第一个空格
			int byte_pos = 0;
			while(ptr != NULL)
			{
		    	frame_data[byte_pos] = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制
				ptr = strtok_r(NULL, " ", &saveptr);
				byte_pos++;
			}		
			read_count++;
			if(read_count <= split_num)
			{	
				// 前一半做训练
				fprintf(train_data_set, "%ld,%08x,", current_readtime, can_id);
				for(int i = 0; i < FRAME_DLC; i++)
				{
				    if(i == 0)
				    {
				        fprintf(train_data_set, "%02x", frame_data[i]);
				    }
					else
					{
						fprintf(train_data_set, " %02x", frame_data[i]);
					}
				}
				fprintf(train_data_set, "\n");
			}
			else
			{
				// 后一半做测试
				fprintf(test_data_set, "%ld,%08x,", current_readtime, can_id);
				for(int i = 0; i < FRAME_DLC; i++)
				{
				    if(i == 0)
				    {
				        fprintf(test_data_set, "%02x", frame_data[i]);
				    }
					else
					{
						fprintf(test_data_set, " %02x", frame_data[i]);
					}
				}
				fprintf(test_data_set, "\n");					
			}
		}
		
		
		// 关闭文件
		fclose(raw_data_set);
		fclose(train_data_set);
		fclose(test_data_set);

    }


    return RET_OK;

}





// 测试bit值读写函数
int test_bit_read()
{
    BYTE a = 0;
	for(int i = 0; i < 8; i++)
	{
	    printf("%d", read_bit_value(&a, i));
	}
	printf("\n");

	write_bit_value(&a, 0, 1);
	write_bit_value(&a, 1, 0);
	write_bit_value(&a, 2, 1);
	write_bit_value(&a, 3, 0);
	write_bit_value(&a, 4, 1);
	write_bit_value(&a, 5, 0);
	write_bit_value(&a, 6, 1);
	write_bit_value(&a, 7, 0);

	for(int i = 0; i < 8; i++)
	{
	    printf("%d", read_bit_value(&a, i));
	}
	printf("\n");

	write_bit_value(&a, 0, 0);
	write_bit_value(&a, 1, 1);
	write_bit_value(&a, 2, 0);
	write_bit_value(&a, 3, 1);
	write_bit_value(&a, 4, 0);
	write_bit_value(&a, 5, 1);
	write_bit_value(&a, 6, 0);
	write_bit_value(&a, 7, 1);

	for(int i = 0; i < 8; i++)
	{
	    printf("%d", read_bit_value(&a, i));
	}
	printf("\n");

    return RET_OK;
	
}



// 测试标记函数
int test_mark_array()
{
    int num = rand()%5 + 3;
	printf("mark_array %d times\n", num);	

	for(int j = 0; j < num; j++)
	{
	    int size = rand()%FRAME_DLC;
		int a[FRAME_DLC] = {0};
		mark_pos(a, size);
		printf("mark_size(%d):", size);
		for(int i = 0; i < FRAME_DLC; i++)
		{
		    printf(" %d", a[i]);
		}
		printf("\n");	
	}

    return RET_OK;
}

int test_hamming()
{
    uint8_t data_previous[8] = {0};
	uint8_t data_current[8] = {0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};

	printf("hamming_dist: %d\n", calculate_hamming_distance(data_previous, data_current));
    
    return RET_OK;
}


// 将CAN报文从byte转换成bit(数据分析使用)
int turn_file_from_byte_to_bit()
{
    char data_path[100] = "";
	char save_data_path[100] = "";
    snprintf(data_path, sizeof(data_path), "../dataset/train/%03d.csv", 1);	
	snprintf(save_data_path, sizeof(save_data_path), "../dataset/001.csv");	
	printf(RED "feed %s "  NONE "\n", data_path);

	// 打开数据文件
	FILE *data_set = fopen(data_path, MODE_R_PLUS);
	if( data_set == NULL )
	{
		LOG_DEBUG(RED "open data_path file failed!" NONE);
		
		return RET_ERROR;
	}

	FILE *save_data_set = fopen(save_data_path, MODE_W_PLUS);
	if( save_data_set == NULL )
	{
		LOG_DEBUG(RED "open save_data_path file failed!" NONE);
		
		return RET_ERROR;
	}


	// 遍历数据
	struct can_frame new_can;			   // 新读取的报文
	memset(&new_can, 0, sizeof(struct can_frame));
	new_can.can_dlc = FRAME_DLC;
		
	int64_t current_readtime;			// 当前记录的时间戳(采集的数据记录的时间戳)
		
		
	int64_t timestamp = get_system_ms_time();
	int frame_count = 0;
		
	char data_buffer[1024] = "";
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_set ) != NULL)
	{
		//printf("%s\n", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);
		
		frame_count++;
		if((get_system_ms_time() - timestamp) > TRAIN_LOADING_CYCLE)
		{
			LOG_DEBUG("load train data...");
			timestamp = get_system_ms_time();
		}
		
//		printf("%ld, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", current_readtime, new_can.can_id, new_can.data[0], new_can.data[1], new_can.data[2], new_can.data[3],
//		    new_can.data[4], new_can.data[5], new_can.data[6], new_can.data[7]);    

		fprintf(save_data_set, "%ld,%x", current_readtime, new_can.can_id);

		for(int i = 0; i < FRAME_DLC_BIT; i++)
		{
		    int byte_pos = i/8;
			int bit_pos = i%8;
			int value = read_bit_value(&new_can.data[byte_pos], bit_pos);
			fprintf(save_data_set, ",%x", value); 
		}
		fprintf(save_data_set, "\n");
	}

    return RET_OK;
}


// 测试bitset函数库
int test_bitset()
{
    bitset<8> bitset1;

    int postion = 6;

    int a = bitset1[postion];     // 测试取值
	printf("%d\n", a);

    bitset1[postion] = 1;
	a = bitset1.test(postion);       // 测试取值

	printf("%d\n", a);

    int bitset_value = bitset1.to_ulong();       // bitset.to_ulong()   将位容器转换为 unsigned long

	printf("%d\n", bitset_value);

    bitset1.set();      // 全部置1
    bitset1.reset();    // 全部置0

    unsigned long long temp = pow(2, 16);
	printf("%lld \n", temp);

	bitset1[0] = 1;
	bitset1[3] = 1;

	for(int i = 0; i < 8; i++)
	{
	    printf("%d", bitset1.test(i));       // 测试取值
	}
	printf("\n");

    bitset1 <<= 1;
	for(int i = 0; i < 8; i++)
	{
	    printf("%d", bitset1.test(i));       // 测试取值
	}
	printf("\n");

	

    return 0;
}


// 统计数据集的分布
// 报文总数目，每个ID对应的数目
int statistic_data()
{
	vector<UINT> vec_can_id;					        // CAN_ID列表
	vector<long long> vec_can_count;					// 各ID计数
	int total_count = 0;                          // 报文总体计数
    int64_t total_time = 0;                           // 报文总体时间

    int64_t file_single_time[MAX_FILE_NUM] = {0};
	int file_single_count[MAX_FILE_NUM] = {0};
	

	for(int file_num = MIN_FILE_NUM; file_num <= MAX_FILE_NUM; file_num++)
    {
        char data_path[100] = "";
		snprintf(data_path, sizeof(data_path), "../dataset/raw/%03d.csv", file_num);

		printf(RED "feed %s "  NONE "\n", data_path);

		// 打开数据文件
		FILE *data_set = fopen(data_path, MODE_R_PLUS);
		if( data_set == NULL )
		{
			LOG_DEBUG(RED "open data_path file failed!" NONE);
		
			return RET_ERROR;
		}
		
		// 遍历数据
		struct can_frame new_can;			   // 新读取的报文
		memset(&new_can, 0, sizeof(struct can_frame));
		new_can.can_dlc = FRAME_DLC;
		
		int64_t current_readtime;			// 当前记录的时间戳(采集的数据记录的时间戳)

        int64_t first_readtime = 0;
		int64_t last_readtime = 0;
		

        // 单个文件报文统计
		int count = 0;
		
		char data_buffer[1024] = "";
		// 读取原始的CAN报文
		while( fgets(data_buffer, sizeof(data_buffer), data_set ) != NULL)
		{
			//printf("%s\n", data_buffer);
			parse_read_buffer(data_buffer, &current_readtime, &new_can);

			// 记录单个文件的数目
			count++;

			// 记录文件起始时间
            if(first_readtime == 0)
            {
                first_readtime = current_readtime;
            }
			last_readtime = current_readtime;
		
		
	//		printf("%ld, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", current_readtime, new_can.can_id, new_can.data[0], new_can.data[1], new_can.data[2], new_can.data[3],
	//			new_can.data[4], new_can.data[5], new_can.data[6], new_can.data[7]);
		
		
			// 寻找报文ID对应的序号
			int can_id_num = NOT_EXIST;
			can_id_num = get_map_id2num(vec_can_id, new_can.can_id);
		
			if(can_id_num == NOT_EXIST)
			{
				// 新CAN_ID入队
				vec_can_id.push_back(new_can.can_id);
				LOG_DEBUG(YELLOW "new id: %x, id_size(%d)" NONE, new_can.can_id, vec_can_id.size());

				vec_can_count.push_back(0);

				// 更新can_id_num
				can_id_num = get_map_id2num(vec_can_id, new_can.can_id);
			}

			vec_can_count[can_id_num]++;
			total_count++;

		}

		int64_t temp_time = (last_readtime - first_readtime)/1000/1000;    // 1s = 1000 * 1000us 
		total_time += temp_time;
		
		printf("last_time: %ld, first_time: %ld, temp_time: %ld s, count: %d\n", last_readtime, first_readtime, temp_time, count);

		file_single_count[file_num-1] = count;
		file_single_time[file_num-1] = temp_time;


		fclose(data_set);

	}    

    // 打印数据分布
    char printf_buffer[10240] = "";
	snprintf(printf_buffer + strlen(printf_buffer), sizeof(printf_buffer) - strlen(printf_buffer), "total_count: %d, total_time: %ld\n", total_count, total_time);
	snprintf(printf_buffer + strlen(printf_buffer), sizeof(printf_buffer) - strlen(printf_buffer), "file_num: %d\n", MAX_FILE_NUM - MIN_FILE_NUM + 1);
	for(int file_num = MIN_FILE_NUM; file_num <= MAX_FILE_NUM; file_num++)
	{
	    snprintf(printf_buffer + strlen(printf_buffer), sizeof(printf_buffer) - strlen(printf_buffer), "file_index: %d, count: %d, time: %lds\n", file_num, file_single_count[file_num-1], file_single_time[file_num-1]);
	}
	int vec_size = (int)vec_can_id.size();
	snprintf(printf_buffer + strlen(printf_buffer), sizeof(printf_buffer) - strlen(printf_buffer), "id_size: %d\n", vec_size);
	for(int i = 0; i < vec_size; i++)
	{
	    snprintf(printf_buffer + strlen(printf_buffer), sizeof(printf_buffer) - strlen(printf_buffer), "id: %x, count: %lld\n", vec_can_id[i], vec_can_count[i]);
	}
	LOG_DEBUG("\n%s", printf_buffer);


    // 存储各文件报文总数量和总时长
	FILE *data_static_result = NULL;
	data_static_result = fopen(ANALYSIS_PATH "data_statistics_result_count_and_time.csv", MODE_W_PLUS);
	fprintf(data_static_result, "%d,%ld\n", total_count, total_time);
	fprintf(data_static_result, "%d\n", MAX_FILE_NUM - MIN_FILE_NUM + 1);
	for(int file_num = MIN_FILE_NUM; file_num <= MAX_FILE_NUM; file_num++)
	{
	    fprintf(data_static_result, "%d,%d,%ld\n", file_num, file_single_count[file_num-1], file_single_time[file_num-1]);
	}
	fclose(data_static_result);

    // 存储总体数据集各ID的数目
	data_static_result = fopen(ANALYSIS_PATH "data_statistics_result_ID_and_count.csv", MODE_W_PLUS);    
	fprintf(data_static_result, "%d\n", vec_size);
	for(int i = 0; i < vec_size; i++)
	{
	    fprintf(data_static_result, "%x,%lld\n", vec_can_id[i], vec_can_count[i]);
	}

	fclose(data_static_result);

    return RET_OK;
}

// 将CAN分析仪采集的数据转化成csv格式
int data_transform()
{
    FILE *input_file = NULL;
	FILE *output_file = NULL;
	input_file = fopen("../dataset/processed/20220101005.csv", "r+");        // 只读模式打开文件
	if (input_file == NULL)
	{
	    LOG_DEBUG("open input_file failed!");
		return RET_OK;
	}
	output_file = fopen("../dataset/processed/20220101005.csv", "w+");      // 只写模式打开文件
	if (output_file == NULL)
	{
	    LOG_DEBUG("open output_file failed!");
		return RET_OK;
	}

    char line_content_buff[1024];
    char timestamp_buff[256];
	char frame_id_buff[256];
	char fram_data_buff[256];
	char info_buff[256];
	memset(line_content_buff, 0 , sizeof(line_content_buff));
	memset(info_buff, 0 , sizeof(info_buff));

	unsigned int count = 0;
    size_t start;
	
	while( fgets(line_content_buff, 1024, input_file) != NULL )
	{
		
	    // 跳过第一行标签
	    if (count == 0)
	    {
	        count++;
	    	memset(line_content_buff, 0 , sizeof(line_content_buff));
	        continue;
	    }
	    
	    LOG_DEBUG("%s", line_content_buff);

		int str_num = 0;

		// 使用分割函数
        char *ptr = NULL;
		char *saveptr = NULL;
		ptr = strtok_r(line_content_buff, ",", &saveptr);
		while ( ptr != NULL )
		{
		    // LOG_DEBUG("%s", ptr);

            // 第0个字段为系统时间
            if (str_num == 0)
            {
                memset(timestamp_buff, 0 , sizeof(timestamp_buff));
                strcpy(timestamp_buff, ptr);
				// LOG_DEBUG("system time str: %s", timestamp_buff);

				// 标准时间转化成时间戳
				int64_t system_time = stand2tick_time(timestamp_buff + 1);        // 这里的 "+1"是去除"["号
				// LOG_DEBUG("system time value: %ld", system_time);

				start = strlen(info_buff);
				snprintf(info_buff + start, sizeof(info_buff) - start, "%ld", system_time);
            }

            // 第13个字段为报文ID
            if (str_num == 13)
            {
                memset(frame_id_buff, 0 , sizeof(frame_id_buff));
                strcpy(frame_id_buff, ptr);
				// LOG_DEBUG("frame id: %s", frame_id_buff);
				
				start = strlen(info_buff);
				snprintf(info_buff + start, sizeof(info_buff) - start, ",%s", frame_id_buff);

            }			

            // 第20个字段为8个字节报文数据
            if (str_num == 20)
            {
                memset(fram_data_buff, 0 , sizeof(fram_data_buff));
                strcpy(fram_data_buff, ptr);
				// LOG_DEBUG("frame data: %s", fram_data_buff);

				
                // 获取8个字节数据
		        char *frame_data_ptr = NULL;
				char *frame_data_saveptr = NULL;
				frame_data_ptr = strtok_r(fram_data_buff, " ", &frame_data_saveptr);  
                int byte_num = 0;
				
				while (frame_data_ptr != NULL)
				{
					// LOG_DEBUG("Byte[%d]: %s", byte_num, frame_data_ptr);
					
					start = strlen(info_buff);
					snprintf(info_buff + start, sizeof(info_buff) - start, ",%s", frame_data_ptr);

					
					
					frame_data_ptr = strtok_r(NULL, " ", &frame_data_saveptr);
					
					
					byte_num++;
					if (byte_num > 7)
					{
					    break;
					}
				}
				
            }


			str_num++;
			
			ptr = strtok_r(NULL, ",", &saveptr);
		}


		start = strlen(info_buff);
		snprintf(info_buff + start, sizeof(info_buff) - start, "\n");

		fprintf(output_file, "%s", info_buff);

		LOG_DEBUG("log: %s", info_buff);      

		
		count++;
		memset(line_content_buff, 0 , sizeof(line_content_buff));
		memset(info_buff, 0 , sizeof(info_buff));

	}
	
    fclose(input_file);
	fclose(output_file);

	return RET_OK;
}

// 复制数据测试
int copy_data_test()
{
	FILE *train_data_set = fopen("../dataset/processed/20220101003.csv", MODE_R_PLUS);
	if( train_data_set == NULL )
	{
		LOG_DEBUG(RED "open train_data_path file failed!" NONE);

		return RET_ERROR;
	}

	FILE *test_data_set = fopen("../dataset/processed/20220101006.csv", MODE_W_PLUS);
	if( test_data_set == NULL )
	{
		LOG_DEBUG(RED "open test_data_path file failed!" NONE);

		return RET_ERROR;
	}

    // 遍历数据
	int64_t current_readtime;	        
	unsigned int can_id;
    unsigned int frame_data[FRAME_DLC];

    unsigned long read_count = 0;

	// 读取原始的CAN报文
	while( (fscanf( train_data_set, "%ld, %x, %x, %x, %x, %x, %x, %x, %x, %x",
		&current_readtime, &can_id, &frame_data[0], &frame_data[1], &frame_data[2], &frame_data[3],
			&frame_data[4], &frame_data[5], &frame_data[6], &frame_data[7] ) ) != EOF )
	{
	    read_count++;
		char buffer[256] = "";
		snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%ld %ld 0x%08x", read_count, current_readtime, can_id);
		for(int i = 0; i < FRAME_DLC; i++)
		{
			snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), ",%02x", frame_data[i]);
		}
		printf("%s\n", buffer);
		
		if(read_count> 42021)
		{
		    break;
		}

        

		fprintf(test_data_set, "%ld,%08x", current_readtime, can_id);
		for(int i = 0; i < FRAME_DLC; i++)
		{
			fprintf(test_data_set, ",%02x", frame_data[i]);
		}
		fprintf(test_data_set, "\n");

		
	}


    fclose(train_data_set);
    fclose(test_data_set);
			

    return 0;
}


// 将报文划分成固定长度的报文序列(用于重放攻击的读取)
int split_data_to_set()
{

	int set_num = 1;
	int set_length = 50000;
	char split_data_path[100] = "";

	for(int file_num = 1; file_num <= MAX_FILE_NUM; file_num++)
	{
		char train_data_path[100] = "";
		snprintf(train_data_path, sizeof(train_data_path), "../dataset/train/%03d.csv", file_num);
	    FILE *train_data_set = fopen(train_data_path, MODE_R_PLUS);
		if( train_data_set == NULL )
		{
			LOG_DEBUG(RED "open train_data_set file failed!" NONE);

			return RET_ERROR;
		}

        int read_finish = INVALID;
   
		while(!read_finish)
		{
			snprintf(split_data_path, sizeof(split_data_path), "../dataset/attack_set/%06d.csv", set_num);
			FILE *new_data_set = fopen(split_data_path, MODE_W_PLUS);
			if( new_data_set == NULL )
			{
				LOG_DEBUG(RED "open new_data_set file failed!" NONE);
			
				return RET_ERROR;
			}
			
			// 遍历数据
			int64_t current_readtime;			
			unsigned int can_id;
			unsigned int frame_data[FRAME_DLC];
			
			int read_count = 0;
			
			// 读取原始的CAN报文
		    char data_buffer[1024] = "";
			// 读取原始的CAN报文
			while( fgets(data_buffer, sizeof(data_buffer), train_data_set ) != NULL)
			{
			    //printf("%s\n", data_buffer);
			    char *ptr = NULL;
		    	char *saveptr = NULL;
		    	char *endptr = NULL;
				ptr = strtok_r(data_buffer, ",", &saveptr);             
				current_readtime = strtol(ptr, &endptr, 10);            // 参数10表示转化为10进制
				
				ptr = strtok_r(NULL, ",", &saveptr);
				can_id = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制

				ptr = strtok_r(NULL, " ", &saveptr);                  // 跳过第一个空格
				int byte_pos = 0;
				while(ptr != NULL)
				{
			    	frame_data[byte_pos] = strtol(ptr, &endptr, 16);            // 参数16表示转化为16进制
					ptr = strtok_r(NULL, " ", &saveptr);
					byte_pos++;
				}			

				read_count++;
			
				if(read_count <= set_length)
				{	
					fprintf(new_data_set, "%ld,%08x,", current_readtime, can_id);
					for(int i = 0; i < FRAME_DLC; i++)
					{
					    if(i == 0)
					    {
					        fprintf(new_data_set, "%02x", frame_data[i]);
					    }
						else
						{
							fprintf(new_data_set, " %02x", frame_data[i]);
						}
					}
					fprintf(new_data_set, "\n");					
				}

                if(read_count == set_length)
                {
                    break;
                }
			
				
			}

		    fclose(new_data_set);
			if(read_count == set_length)
			{
				set_num++;
			}
			else
			{
			    read_finish = VALID;
			}


		}

		// 关闭文件
		fclose(train_data_set);
		
		LOG_DEBUG(RED "split data: (%s) to attack_set" NONE, train_data_set);
	}

    return RET_OK;
}



// 单一文件训练模型参数
int train_single_file(char *data_path, vector<ID_Profile> &id_profile, vector< vector<Byte_Profile> > &byte_profile, vector< vector<Bit_Profile> > &bit_profile, vector<UINT> &vec_can_id, BIT_OP bit_op, int sliding_window_size)
{
    int can_id_size = (int)vec_can_id.size();

    // 初始化参数前继等相关标记
    int last_can_id_valid = INVALID;
	UINT last_can_id = 0;    
    for(int i = 0; i < can_id_size; i++)
    {

		id_profile[i].previous_payload_valid = INVALID;

        for(int j = 0; j < BIT_STREAM_NUM; j++)
        {
			bit_profile[i][j].old_data_valid.reset();
        }

    }

	// 打开数据文件
    FILE *data_set = fopen(data_path, MODE_R_PLUS);
	if( data_set == NULL )
    {
        LOG_DEBUG(RED "open data_path file failed!" NONE);

        return RET_ERROR;
    }

    // 遍历数据
	struct can_frame new_can;              // 新读取的报文
	memset(&new_can, 0, sizeof(struct can_frame));
	new_can.can_dlc = FRAME_DLC;


	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳)

//	int64_t timestamp = get_system_ms_time();

    char data_buffer[1024] = "";


    long frame_count = 0;

	
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_set ) != NULL)
	{
	    //printf("%s\n", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);

		frame_count++;

//    	if((get_system_ms_time() - timestamp) > TRAIN_LOADING_CYCLE)
//    	{
//    	    LOG_DEBUG("load train data...");
//			timestamp = get_system_ms_time();
//    	}

//		printf("%ld, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", current_readtime, new_can.can_id, new_can.data[0], new_can.data[1], new_can.data[2], new_can.data[3],
//			new_can.data[4], new_can.data[5], new_can.data[6], new_can.data[7]);


	    // 寻找报文ID对应的序号
	    int can_id_num = NOT_EXIST;
		can_id_num = get_map_id2num(vec_can_id, new_can.can_id);

	    if(can_id_num == NOT_EXIST)
		{
			// 新CAN_ID入队
			vec_can_id.push_back(new_can.can_id);
			LOG_DEBUG(YELLOW "new id: %x, id_size(%d)" NONE, new_can.can_id, vec_can_id.size());


			// ID轮廓数目增加
			ID_Profile temp_id_profile;
			id_profile.push_back(temp_id_profile);			


			// byte轮廓数目增加
			vector<Byte_Profile> temp_byte_profile(FRAME_DLC);
			byte_profile.push_back(temp_byte_profile);


			// 时序bit轮廓数目增加
			vector<Bit_Profile> temp_bit_profile(BIT_STREAM_NUM);
			bit_profile.push_back(temp_bit_profile);

			// 更新CAN_ID_NUM
			can_id_num = get_map_id2num(vec_can_id, new_can.can_id);
		}

		// 更新报文字节的合法值、最小值、最大值
		for(int i = 0; i < FRAME_DLC; i++)
		{
					
			// 更新字节合法取值
			int byte_value = new_can.data[i];
			byte_profile[can_id_num][i].legal_data[byte_value] = VALID;
		
		
			// 更新字节最小最大取值
			if(new_can.data[i] > byte_profile[can_id_num][i].max_byte_value)
			{
				byte_profile[can_id_num][i].max_byte_value = new_can.data[i];
			}
			if(new_can.data[i] < byte_profile[can_id_num][i].min_byte_value)
			{
				byte_profile[can_id_num][i].min_byte_value = new_can.data[i];
			}
		
		}



		// 更新ID转移矩阵
        if(last_can_id_valid == VALID)
        {
            int last_id_num = get_map_id2num(vec_can_id, last_can_id);
		
            int legal_next_num = NOT_EXIST;
			legal_next_num = get_map_id2num(id_profile[last_id_num].legal_next_id, new_can.can_id);

            // 避免重复入队
			if(legal_next_num == NOT_EXIST)
			{
				id_profile[last_id_num].legal_next_id.push_back(new_can.can_id);
			}
        }
		last_can_id = new_can.can_id;
		last_can_id_valid = VALID;


        // 更新汉明距离轮廓
        if(id_profile[can_id_num].previous_payload_valid == VALID)
        {
		    // 计算新旧报文负载的汉明距离
			int hamming_dist = calculate_hamming_distance(id_profile[can_id_num].previous_payload, new_can.data);

		    if(hamming_dist > id_profile[can_id_num].max_hamming_dist)
		    {
		        id_profile[can_id_num].max_hamming_dist = hamming_dist;
		    }

		    if(hamming_dist < id_profile[can_id_num].min_hamming_dist)
		    {
		        id_profile[can_id_num].min_hamming_dist = hamming_dist;
		    }


        }       
		// 更新存储的新负载
		for(int i = 0; i < FRAME_DLC; i++)
		{
		    id_profile[can_id_num].previous_payload[i] = new_can.data[i];
		}
		id_profile[can_id_num].previous_payload_valid = VALID;

	
		// 时序缓存
		for(int bit_num = 0; bit_num < FRAME_DLC_BIT; bit_num++)
		{
			int byte_pos = bit_num/8;
			int bit_pos = bit_num%8;
			int bit_value = read_bit_value(&new_can.data[byte_pos], bit_pos);

			bit_profile[can_id_num][bit_num].old_data <<= 1;
			bit_profile[can_id_num][bit_num].old_data[0] = bit_value;
			bit_profile[can_id_num][bit_num].old_data_valid <<= 1;
			bit_profile[can_id_num][bit_num].old_data_valid[0] = VALID;
			
			int valid = bit_profile[can_id_num][bit_num].old_data_valid[sliding_window_size - 1];
			if(valid == VALID)
			{
				bit_profile[can_id_num][bit_num].old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
				bit_profile[can_id_num][bit_num].old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
		        int data = bit_profile[can_id_num][bit_num].old_data.to_ulong();         // 存储值转化为整型
		        
				bit_profile[can_id_num][bit_num].total_num++;    // 检测次数+1
				if(bit_profile[can_id_num][bit_num].tm_data[data] == INVALID)
				{
					bit_profile[can_id_num][bit_num].tm_data[data] = VALID;
					bit_profile[can_id_num][bit_num].stable_num = 0;	  // 稳定次数置0
				}
				else
				{
					bit_profile[can_id_num][bit_num].stable_num++;	// 检测次数+1
				}
			}
		
			
			if( (bit_num+1) < FRAME_DLC_BIT)
			{
				int next_byte_pos = (bit_num+1)/8;
				int next_bit_pos = (bit_num+1)%8;
				int next_bit_value = read_bit_value(&new_can.data[next_byte_pos], next_bit_pos);
				
				// 根据二进制运算操作计算邻里值
				int temp_value = adjecent_bit_operation(bit_value, next_bit_value, bit_op);
	
				bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data <<= 1;
				bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data[0] = temp_value;
				bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid <<= 1;
				bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid[0] = VALID;

				int valid = bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid[sliding_window_size-1];
				if(valid == VALID)
				{
					bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
					bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
		        	int data = bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data.to_ulong();         // 存储值转化为整型		
					
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].total_num++;    // 检测次数+1
					if(bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].tm_data[data] == INVALID)
					{
						bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].tm_data[data] = VALID;
						bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].stable_num = 0;	  // 稳定次数置0
					}
					else
					{
						bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].stable_num++;	// 稳定次数+1
					}
		
					
				}
		
		
			}
			
			
		}

		
	}

    
    // 关闭文件
    fclose(data_set);

    return RET_OK;
}

// 使用轮廓模型检测单一文件CAN数据
int detect_single_file(char *data_path, vector<ID_Profile> &id_profile, vector< vector<Byte_Profile> > &byte_profile, vector< vector<Bit_Profile> > &bit_profile, vector<UINT> &vec_can_id, BIT_OP bit_op, int sliding_window_size)
{
    int can_id_size = (int)vec_can_id.size();

    // 初始化参数前继等相关标记
    int last_can_id_valid = INVALID;
	UINT last_can_id = 0;    
    for(int i = 0; i < can_id_size; i++)
    {

		id_profile[i].previous_payload_valid = INVALID;
		
        for(int j = 0; j < BIT_STREAM_NUM; j++)
        {
			bit_profile[i][j].old_data_valid.reset();
        }

    }

    FILE *data_set = fopen(data_path, MODE_R_PLUS);
	if( data_set == NULL )
    {
        LOG_DEBUG(RED "open data_path file failed!" NONE);

        return RET_ERROR;
    }

    // 遍历数据
	struct can_frame new_can;              // 新读取的报文
	memset(&new_can, 0, sizeof(struct can_frame));
	new_can.can_dlc = FRAME_DLC;


	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳)

	int64_t timestamp = get_system_ms_time();
	int64_t start = 0;
	int64_t end = 0;
	int count = 0;
	int64_t time_sum = 0;

    int detection_result[COMPARSION_METHOD_NUM] = {NORMAL};     // 0-min_max 1-hamming_detect, 2-id_matrix, 3-bit_scanner, 4-sim_bit_scanner 
	int anomaly_count[COMPARSION_METHOD_NUM] = {0};

	char data_buffer[1024] = "";
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_set ) != NULL)
	{
	    //printf("%s\n", data_buffer);
	    parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count++;
		
    	if((get_system_ms_time() - timestamp) > TRAIN_LOADING_CYCLE)
    	{
    	    LOG_DEBUG("model detect ...");
			timestamp = get_system_ms_time();
    	}

//		printf("%ld, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", current_readtime, new_can.can_id, new_can.data[0], new_can.data[1], new_can.data[2], new_can.data[3],
//			new_can.data[4], new_can.data[5], new_can.data[6], new_can.data[7]);


        start = get_system_us_time();

	    // 寻找报文ID对应的序号
	    int can_id_num = NOT_EXIST;
		can_id_num = get_map_id2num(vec_can_id, new_can.can_id);

        int anomaly[COMPARSION_METHOD_NUM] = {NORMAL};
		memset(detection_result, 0, sizeof(detection_result));


	    if(can_id_num == NOT_EXIST)
		{
			LOG_DEBUG(RED "unknow can id(0x%08x)" NONE, new_can.can_id);

			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
			{
			    detection_result[i] = ABNORMAL;
			}
		}
		else
		{
			// 最大最小值检查
			for(int i = 0; i < FRAME_DLC; i++)
			{
				max_min_byte_value_detect(byte_profile[can_id_num][i], new_can.data[i], anomaly[0]);
				detection_result[0] += anomaly[0];
			}			
	 
			// hamming 距离检查
			// int64_t hamming_start_time = get_system_us_time();
			hamming_distance_detect(id_profile[can_id_num], new_can.data, anomaly[1]);
			detection_result[1] += anomaly[1];
            // int64_t hamming_end_time = get_system_us_time();
			// printf("hamming detect time: %ldus\n", hamming_end_time - hamming_start_time);
			
			// ID矩阵检查
			// int64_t id_matrix_start_time = get_system_us_time();
			if(last_can_id_valid == VALID)
			{
				int last_can_id_num = get_map_id2num(vec_can_id, last_can_id);
				
				id_matrix_detect(id_profile[last_can_id_num], new_can.can_id, anomaly[2]);
			
				if(anomaly[2] == NORMAL)
				{
					//last_can_id = new_can.can_id;
				}
				else
				{
					//last_can_id_valid = INVALID;
					//LOG_DEBUG(RED "anomaly id_matrix [0x%08x]->[0x%08x]", last_can_id, new_can.can_id);
				}
			}
			last_can_id_valid = VALID;
			last_can_id = new_can.can_id;
			detection_result[2] += anomaly[2];
            // int64_t id_matrix_end_time = get_system_us_time();
			// printf("id_matrix detect time: %ldus\n", id_matrix_end_time - id_matrix_start_time);			
			

            // 时序记忆检测
            // int64_t bitscanner_start_time = get_system_us_time();
			for(int bit_num = 0; bit_num < FRAME_DLC_BIT; bit_num++)
			{
				int byte_pos = bit_num/8;
				int bit_pos = bit_num%8;
				int bit_value = read_bit_value(&new_can.data[byte_pos], bit_pos);

				bit_profile[can_id_num][bit_num].old_data <<= 1;
				bit_profile[can_id_num][bit_num].old_data[0] = bit_value;
				bit_profile[can_id_num][bit_num].old_data_valid <<= 1;
				bit_profile[can_id_num][bit_num].old_data_valid[0] = VALID;
				
				int valid = bit_profile[can_id_num][bit_num].old_data_valid[sliding_window_size-1];
				
				if(valid == VALID && bit_profile[can_id_num][bit_num].invalid_flag == INVALID)
				{
					bit_profile[can_id_num][bit_num].old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
					bit_profile[can_id_num][bit_num].old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
		        	int data = bit_profile[can_id_num][bit_num].old_data.to_ulong();         // 存储值转化为整型				
					
					if(bit_profile[can_id_num][bit_num].tm_data[data] == INVALID)
					{
						// LOG_DEBUG(RED "id: %x, bit %d -illegal value %d" NONE, new_can.can_id, bit_num, data);
						bit_profile[can_id_num][bit_num].anomaly_count++;
						detection_result[3]++;
						detection_result[4]++;
					}
					else
					{
					
					}
				}
			
				// 2进制差分序列检测
				if( (bit_num+1) < FRAME_DLC_BIT)
				{
					int next_byte_pos = (bit_num+1)/8;
					int next_bit_pos = (bit_num+1)%8;
					int next_bit_value = read_bit_value(&new_can.data[next_byte_pos], next_bit_pos);
					
					// 根据二进制运算操作计算邻里值
					int temp_value = adjecent_bit_operation(bit_value, next_bit_value, bit_op);	
				
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data <<= 1;
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data[0] = temp_value;
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid <<= 1;
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid[0] = VALID;
					
					int valid = bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid[sliding_window_size-1];
					if(valid == VALID && bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].invalid_flag == INVALID)
					{
						bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
						bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
						int data = bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data.to_ulong(); 		// 存储值转化为整型 

						if(bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].tm_data[data] == INVALID)
						{
							// LOG_DEBUG(RED "id: %x, diff_bit %d -illegal value %d" NONE, new_can.can_id, bit_num, data);
							bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].anomaly_count++;
							detection_result[3]++;
						}
					}
			
			
					
				}
				
			}
			
            // int64_t bitscanner_end_time = get_system_us_time();
			// printf("bitscanner detect time: %ldus\n", bitscanner_end_time - bitscanner_start_time);
            
		}


		
        for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
        {
	        if(detection_result[i] > 0)
	        {
	            anomaly_count[i]++;         
            }

        }

        end = get_system_us_time();
		time_sum += end - start;
		
	}

    double average_time = (double)time_sum/ (double)count;
    printf(YELLOW "frame_count: %d, average_time: %lf us " NONE "\n", count, average_time);
	printf(YELLOW "min-max anomaly_count: %d " NONE "\n", anomaly_count[0]);
	printf(YELLOW "hamming anomaly_count: %d " NONE "\n", anomaly_count[1]);
	printf(YELLOW "id-matrix anomaly_count: %d " NONE "\n", anomaly_count[2]);
	printf(YELLOW "bit_scanner anomaly_count: %d " NONE "\n", anomaly_count[3]);
	printf(YELLOW "sim_bit_scanner anomaly_count: %d " NONE "\n", anomaly_count[4]);  

	
    // 记录检测结果至临时文件
	FILE *res_record = fopen("anomaly_count.csv", MODE_A_PLUS);
    fprintf(res_record, "%d, %lf", count, average_time);
	fprintf(res_record, ",%d", anomaly_count[0]);
	fprintf(res_record, ",%d", anomaly_count[1]);
	fprintf(res_record, ",%d", anomaly_count[2]);
	fprintf(res_record, ",%d", anomaly_count[3]);
	fprintf(res_record, ",%d", anomaly_count[4]);
	

    
	fprintf(res_record, "\n"); 

    
    // 关闭文件
    fclose(data_set);
	fclose(res_record);
	

    return RET_OK;
}

// 使用轮廓模型检测单一文件CAN数据,并返回检测结果
int detect_single_file_return_result(char *data_path, vector<ID_Profile> &id_profile, vector< vector<Byte_Profile> > &byte_profile, vector< vector<Bit_Profile> > &bit_profile, vector<UINT> &vec_can_id, BIT_OP bit_op, int sliding_window_size, int &count, int* anomaly_count)
{
    int can_id_size = (int)vec_can_id.size();

    // 初始化参数前继等相关标记
    int last_can_id_valid = INVALID;
	UINT last_can_id = 0;    
    for(int i = 0; i < can_id_size; i++)
    {

		id_profile[i].previous_payload_valid = INVALID;
		
        for(int j = 0; j < BIT_STREAM_NUM; j++)
        {
			bit_profile[i][j].old_data_valid.reset();
        }

    }

    FILE *data_set = fopen(data_path, MODE_R_PLUS);
	if( data_set == NULL )
    {
        LOG_DEBUG(RED "open data_path file failed!" NONE);

        return RET_ERROR;
    }

    // 遍历数据
	struct can_frame new_can;              // 新读取的报文
	memset(&new_can, 0, sizeof(struct can_frame));
	new_can.can_dlc = FRAME_DLC;


	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳)

	count = 0;

    int detection_result[COMPARSION_METHOD_NUM] = {NORMAL};     // 0-min_max 1-hamming_detect, 2-id_matrix, 3-bit_scanner, 4-sim_bit_scanner 

	char data_buffer[1024] = "";
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_set ) != NULL)
	{
	    //printf("%s\n", data_buffer);
	    parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count++;
		
	    // 寻找报文ID对应的序号
	    int can_id_num = NOT_EXIST;
		can_id_num = get_map_id2num(vec_can_id, new_can.can_id);

        int anomaly[COMPARSION_METHOD_NUM] = {NORMAL};
		memset(detection_result, 0, sizeof(detection_result));


	    if(can_id_num == NOT_EXIST)
		{
			// LOG_DEBUG(RED "unknow can id(0x%08x)" NONE, new_can.can_id);

			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
			{
			    detection_result[i] = ABNORMAL;
			}
		}
		else
		{
			// 最大最小值检查
			for(int i = 0; i < FRAME_DLC; i++)
			{
				max_min_byte_value_detect(byte_profile[can_id_num][i], new_can.data[i], anomaly[0]);
				detection_result[0] += anomaly[0];
			}			
	 
			// hamming 距离检查
			// int64_t hamming_start_time = get_system_us_time();
			hamming_distance_detect(id_profile[can_id_num], new_can.data, anomaly[1]);
			detection_result[1] += anomaly[1];
            // int64_t hamming_end_time = get_system_us_time();
			// printf("hamming detect time: %ldus\n", hamming_end_time - hamming_start_time);
			
			// ID矩阵检查
			// int64_t id_matrix_start_time = get_system_us_time();
			if(last_can_id_valid == VALID)
			{
				int last_can_id_num = get_map_id2num(vec_can_id, last_can_id);
				
				id_matrix_detect(id_profile[last_can_id_num], new_can.can_id, anomaly[2]);
			
				if(anomaly[2] == NORMAL)
				{
					//last_can_id = new_can.can_id;
				}
				else
				{
					//last_can_id_valid = INVALID;
					//LOG_DEBUG(RED "anomaly id_matrix [0x%08x]->[0x%08x]", last_can_id, new_can.can_id);
				}
			}
			last_can_id_valid = VALID;
			last_can_id = new_can.can_id;
			detection_result[2] += anomaly[2];
            // int64_t id_matrix_end_time = get_system_us_time();
			// printf("id_matrix detect time: %ldus\n", id_matrix_end_time - id_matrix_start_time);			
			

            // 时序记忆检测
            // int64_t bitscanner_start_time = get_system_us_time();
			for(int bit_num = 0; bit_num < FRAME_DLC_BIT; bit_num++)
			{
				int byte_pos = bit_num/8;
				int bit_pos = bit_num%8;
				int bit_value = read_bit_value(&new_can.data[byte_pos], bit_pos);

				bit_profile[can_id_num][bit_num].old_data <<= 1;
				bit_profile[can_id_num][bit_num].old_data[0] = bit_value;
				bit_profile[can_id_num][bit_num].old_data_valid <<= 1;
				bit_profile[can_id_num][bit_num].old_data_valid[0] = VALID;
				
				int valid = bit_profile[can_id_num][bit_num].old_data_valid[sliding_window_size-1];
				
				if(valid == VALID && bit_profile[can_id_num][bit_num].invalid_flag == INVALID)
				{
					bit_profile[can_id_num][bit_num].old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
					bit_profile[can_id_num][bit_num].old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
		        	int data = bit_profile[can_id_num][bit_num].old_data.to_ulong();         // 存储值转化为整型				
					
					if(bit_profile[can_id_num][bit_num].tm_data[data] == INVALID)
					{
						// LOG_DEBUG(RED "id: %x, bit %d -illegal value %d" NONE, new_can.can_id, bit_num, data);
						bit_profile[can_id_num][bit_num].anomaly_count++;
						detection_result[3]++;
						detection_result[4]++;
					}
					else
					{
					
					}
				}
			
				// 2进制差分序列检测
				if( (bit_num+1) < FRAME_DLC_BIT)
				{
					int next_byte_pos = (bit_num+1)/8;
					int next_bit_pos = (bit_num+1)%8;
					int next_bit_value = read_bit_value(&new_can.data[next_byte_pos], next_bit_pos);
					
					// 根据二进制运算操作计算邻里值
					int temp_value = adjecent_bit_operation(bit_value, next_bit_value, bit_op);	
				
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data <<= 1;
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data[0] = temp_value;
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid <<= 1;
					bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid[0] = VALID;
					
					int valid = bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].old_data_valid[sliding_window_size-1];
					if(valid == VALID && bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].invalid_flag == INVALID)
					{
						bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
						bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
						int data = bit_profile[can_id_num][FRAME_DLC_BIT +bit_num].old_data.to_ulong(); 		// 存储值转化为整型 

						if(bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].tm_data[data] == INVALID)
						{
							// LOG_DEBUG(RED "id: %x, diff_bit %d -illegal value %d" NONE, new_can.can_id, bit_num, data);
							bit_profile[can_id_num][FRAME_DLC_BIT + bit_num].anomaly_count++;
							detection_result[3]++;
						}
					}
			
			
					
				}
				
			}
			
            // int64_t bitscanner_end_time = get_system_us_time();
			// printf("bitscanner detect time: %ldus\n", bitscanner_end_time - bitscanner_start_time);
            
		}


		
        for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
        {
	        if(detection_result[i] > 0)
	        {
	            anomaly_count[i]++;         
            }

        }
		
	}

    // 关闭文件
    fclose(data_set);

	
    return RET_OK;
}


// 使用模型检测并记录
int detect_and_record(FILE *data_set, FILE *result_path, vector<ID_Profile> &id_profile, vector< vector<Byte_Profile> > &byte_profile, vector< vector<Bit_Profile> > &bit_profile, vector<UINT> &vec_can_id, BIT_OP bit_op, int sliding_window_size)
{
    int can_id_size = (int)vec_can_id.size();

    // 初始化参数前继等相关标记
    int last_can_id_valid = INVALID;
	UINT last_can_id = 0;    
    for(int i = 0; i < can_id_size; i++)
    {

		id_profile[i].previous_payload_valid = INVALID;

        for(int j = 0; j < BIT_STREAM_NUM; j++)
        {
			bit_profile[i][j].old_data_valid.reset();
        }
		
    }

    // 遍历数据
	struct can_frame new_can;              // 新读取的报文
	memset(&new_can, 0, sizeof(struct can_frame));
	new_can.can_dlc = FRAME_DLC;


	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳)

//	int64_t timestamp = get_system_ms_time();

    long frame_count = 0;

	char data_buffer[1024] = "";

	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_set ) != NULL)
	{
	    //LOG_DEBUG("%s", data_buffer);
	    parse_read_buffer(data_buffer, &current_readtime, &new_can);

		frame_count++;

//    	if((get_system_ms_time() - timestamp) > TRAIN_LOADING_CYCLE)
//    	{
//    	    LOG_DEBUG("Scanning message payload ...");
//			timestamp = get_system_ms_time();
//    	}

        // 0-min_max 1-hamming_detect, 2-id_matrix, 3-bit_scanner, 4-sim_bit_scanner
        int detection_result[COMPARSION_METHOD_NUM] = {NORMAL};    

        int is_anomaly[COMPARSION_METHOD_NUM] = {NORMAL};

	    // 寻找报文ID对应的序号
	    int can_id_num = NOT_EXIST;
		can_id_num = get_map_id2num(vec_can_id, new_can.can_id);

	    if(can_id_num == NOT_EXIST)
		{
			LOG_DEBUG(RED "unknow can id(0x%08x)" NONE, new_can.can_id);

			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
			{
			    detection_result[i] = ABNORMAL;
			}
		}
		else
		{
			// 最大最小值检查
			for(int i = 0; i < FRAME_DLC; i++)
			{
				max_min_byte_value_detect(byte_profile[can_id_num][i], new_can.data[i], is_anomaly[0]);
				detection_result[0] += is_anomaly[0];
			}		

			// hamming检测
			hamming_distance_detect(id_profile[can_id_num], new_can.data, is_anomaly[1]);
			detection_result[1] += is_anomaly[1];
			
			
			// id_matrix检测
			
			if(last_can_id_valid == VALID)
			{
				int last_can_id_num = get_map_id2num(vec_can_id, last_can_id);
				
				id_matrix_detect(id_profile[last_can_id_num], new_can.can_id, is_anomaly[2]);
			
				if(is_anomaly[2] == NORMAL)
				{
					//last_can_id = new_can.can_id;
				}
				else
				{
					//last_can_id_valid = INVALID;
					//is_anomaly[2] = ABNORMAL;
					//LOG_DEBUG(RED "anomaly id_matrix [0x%08x]->[0x%08x]", last_can_id, new_can.can_id);
				}
			}
			last_can_id_valid = VALID;
			last_can_id = new_can.can_id;		 
			detection_result[2] += is_anomaly[2];
			

            // 时序记忆检测
			for(int bit_num = 0; bit_num < FRAME_DLC_BIT; bit_num++)
			{
				int byte_pos = bit_num/8;
				int bit_pos = bit_num%8;
				int bit_value = read_bit_value(&new_can.data[byte_pos], bit_pos);

                bit_scanner_detect(bit_profile[can_id_num][bit_num], bit_value, sliding_window_size, is_anomaly[3]);

				detection_result[3] += is_anomaly[3];
				detection_result[4] += is_anomaly[3];               // sim_bit_scanner只是bit_scanner的一部分

			    // 2进制差分序列检测
				if( (bit_num+1) < FRAME_DLC_BIT)
				{
					int next_byte_pos = (bit_num+1)/8;
					int next_bit_pos = (bit_num+1)%8;
					int next_bit_value = read_bit_value(&new_can.data[next_byte_pos], next_bit_pos);

					// 根据二进制运算操作计算邻里值
					int temp_value = adjecent_bit_operation(bit_value, next_bit_value, bit_op);					

					bit_scanner_detect(bit_profile[can_id_num][FRAME_DLC_BIT + bit_num], temp_value, sliding_window_size, is_anomaly[3]);

					detection_result[3] += is_anomaly[3];
			
				}
				
			
			}  

            
			
		}
		

        // 记录各种对比方法的检测结果
        for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
        {
            detection_result[i] = detection_result[i]>0? ABNORMAL:NORMAL;
			if(i == 0)
			{
				fprintf(result_path, "%d", detection_result[i]);
			}
			else
			{
			    fprintf(result_path, ",%d", detection_result[i]);
			}
        }

		fprintf(result_path, "\n");

	}

    return RET_OK;
}



// 整体模型训练，包括字节轮廓和ID轮廓
int model_train(BIT_OP bit_op, int sliding_window_size)
{

	vector<UINT> vec_can_id;					        // CAN_ID列表
	vector<ID_Profile> id_profile;                      // ID轮廓
	vector< vector<Byte_Profile> > byte_profile;        // 字节轮廓
    vector< vector<Bit_Profile> > bit_profile;          // bit轮廓


	CSimpleIniA configurator;
	// 设置配置文件编码模式
	configurator.SetUnicode();

	for(int file_num = 1; file_num <= MAX_FILE_NUM; file_num++)
    {
        char data_path[100] = "";
		snprintf(data_path, sizeof(data_path), TRAIN_PATH "%03d.csv", file_num);
		printf(RED "feed %s "  NONE "\n", data_path);
		
		// 使用单一文件训练
    	train_single_file(data_path, id_profile, byte_profile, bit_profile, vec_can_id, bit_op, sliding_window_size);

	}
	
    // 打印ID轮廓参数
    // show_id_profile(id_profile, vec_can_id);

	// 将ID轮廓参数写入模型配置
    save_id_profile(vec_can_id, id_profile, configurator);


	// 将byte轮廓参数写入模型配置
	// show_byte_profile(byte_profile, vec_can_id);	
    save_byte_profile(vec_can_id, byte_profile, configurator);


	// 打印bit轮廓
	// show_bit_profile(bit_profile, vec_can_id);

	// 将bit轮廓参数写入模型配置
    save_bit_profile(vec_can_id, bit_profile, configurator, sliding_window_size);


    // 保存模型参数文件
    char configuration_path[100] = "";
	snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "%d_%03d.init", bit_op, sliding_window_size);
	configurator.SaveFile(configuration_path);

    return RET_OK;
}


int model_detect_attack_free(BIT_OP bit_op, int sliding_window_size)
{
	vector<UINT> vec_can_id;					        // CAN_ID列表
	vector<ID_Profile> id_profile;                      // id轮廓
	vector< vector<Byte_Profile> > byte_profile;        // 字节轮廓
 	vector<vector<Bit_Profile>> bit_profile;            // bit轮廓	 	
	
	CSimpleIniA configurator;                           // 配置文件

    // 打开模型参数文件
    char configuration_path[100] = "";
	snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "%d_%03d.init", bit_op, sliding_window_size);    
	SI_Error rc = configurator.LoadFile(configuration_path);
	if (rc < 0) 
	{
	    LOG_DEBUG(RED "load init file failed !" NONE);
	}
	else
	{
	    LOG_DEBUG(GREEN "load init file OK !" NONE);	
	}

    int res = RET_ERROR;


    // 从配置文件读取ID轮廓
    res = read_id_profile(id_profile, vec_can_id, configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_id_profile failed !" NONE);
		
	    return RET_ERROR;
	}
	
    // 从配置文件读取byte轮廓
    res = read_byte_profile(byte_profile, vec_can_id, configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_bit_profile failed !" NONE);
		
	    return RET_ERROR;
	}


    // 从配置文件读取bit轮廓
    res = read_bit_profile(bit_profile, vec_can_id, configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_bit_profile failed !" NONE);
		
	    return RET_ERROR;
	}


	for(int file_num = MIN_FILE_NUM; file_num <= MAX_FILE_NUM; file_num++)
    {
        char data_path[100] = "";
		snprintf(data_path, sizeof(data_path), "../dataset/test/%03d.csv", file_num);
		
		// 单一文件检测
    	detect_single_file(data_path, id_profile, byte_profile, bit_profile, vec_can_id, bit_op, sliding_window_size);
    }

	// 统计配置文件whitelist矩阵的占用比
	FILE *occupation_record = fopen(ANALYSIS_PATH "occupation_record.csv", MODE_A_PLUS);
	int max_occupation = pow(2, sliding_window_size);	 // 最大占用量
	int total_stream_num = 0;				             // bit流总数
	int valid_stream_num = 0;                            // 有效bit流数目
	int invalid_stream_num = 0; 			             // 无效bit流数目
	int full_occupy_stream_num = 0;                      // 占据全满的bit流数目
	int not_full_occupy_stream_num = 0;                  // 占据不是全满的bit流数目
	int constant_stream_num = 0;			             // 常值/仅占据1个位置的流
	int no_anomaly_stream_num = 0;                       // 检测无异常的bit流数目
	for(int i = 0; i < (int)vec_can_id.size(); i++)
	{
		for(int j = 0; j < BIT_STREAM_NUM; j++)
		{
			total_stream_num++;

			int occupation = 0; 	
			occupation = bit_profile[i][j].tm_data.count();    //count函数用来求bitset中1的位数

			if(occupation == max_occupation)
			{
				full_occupy_stream_num++;
			}
			else
			{
				not_full_occupy_stream_num++;

				if(bit_profile[i][j].invalid_flag == INVALID)
				{
					valid_stream_num++;
					
					if(occupation == 1)
					{
						constant_stream_num++;
					}
					
					if(bit_profile[i][j].anomaly_count == INVALID)
					{
						no_anomaly_stream_num++;
					}

				}
				else
				{
					invalid_stream_num++;
				}

			}

		}
	}

	// 写数据
	fprintf(occupation_record, "%d", bit_op);
	fprintf(occupation_record, ",%d", sliding_window_size);
	fprintf(occupation_record, ",%d", total_stream_num);         
	fprintf(occupation_record, ",%d", valid_stream_num);
	fprintf(occupation_record, ",%d", invalid_stream_num);
	fprintf(occupation_record, ",%d", full_occupy_stream_num);
	fprintf(occupation_record, ",%d", not_full_occupy_stream_num);
	fprintf(occupation_record, ",%d", constant_stream_num);
	fprintf(occupation_record, ",%d", no_anomaly_stream_num);
	fprintf(occupation_record, "\n");
	// 关闭文件
	fclose(occupation_record);        


	// 统计算法的误报率
	FILE *alarm_record = fopen(ANALYSIS_PATH "alarm_record.csv", MODE_A_PLUS);
    FILE *temp_file = fopen("anomaly_count.csv", MODE_R_PLUS);
    char read_buffer[1024] = "";

    int sum_frame_count = 0;     // 总报文数量
    int sum_anomaly_count[COMPARSION_METHOD_NUM] = {0};     // 各方法累积误报次数
	double false_alarm_rate[COMPARSION_METHOD_NUM] = {0};   // 各方法误报率
	while( fgets(read_buffer, sizeof(read_buffer), temp_file ) != NULL)
	{
		// 分割函数获取各方法的检测结果
		char *ptr = NULL;
		char *saveptr = NULL;
		char *endptr = NULL;
		ptr = strtok_r(read_buffer, ",", &saveptr); 			
		int pos = 0;
		while(ptr != NULL)
		{
		    if(pos == 0)
		    {
		        // 单文件报文数目
		        int c = strtol(ptr, &endptr, 10);
				sum_frame_count += c;
		    }
			else if(pos == 1)
			{
			    // 处理时间
			}
			else if(pos>1)
			{
			    // 各个方法的误报数目
			    int c = strtol(ptr, &endptr, 10);
				sum_anomaly_count[pos - 2] += c;
			}

			ptr = strtok_r(NULL, ",", &saveptr);
			pos++;
		}

	    
	}

    for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
    {
        false_alarm_rate[i] = (double)sum_anomaly_count[i]/(double) sum_frame_count;
    }
	// 记录误报结果至临时文件
	fprintf(alarm_record, "%d", bit_op);
	fprintf(alarm_record, ",%d", sliding_window_size);
	fprintf(alarm_record, ",%d", sum_frame_count);
	for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
    {
        fprintf(alarm_record, ",%d,%lf", sum_anomaly_count[i], false_alarm_rate[i]);
    }		
	fprintf(alarm_record, "\n");

	// 关闭文件
	fclose(temp_file);
	remove("anomaly_count.csv");    
	fclose(alarm_record);
	

    return RET_OK;
}


// 随机报文注入攻击测试,生成异常报文和标签(重复注入一种ID的报文，报文负载随机，以模拟不同的注入速率)
int random_injection_attack_test(FILE *data_file, FILE *tamper_file, FILE *label_file, vector<UINT> &vec_can_id, int injection_strength)
{
    if(!data_file || !tamper_file || !label_file)
    {
        LOG_DEBUG(RED "invalid input! data_file[%p], tamper_file[%p], label_file[%p]" NONE, data_file, tamper_file, label_file);
		return RET_ERROR;
    }
	srand((unsigned)time(0));

	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳) 
    int count = 0;
	char data_buffer[1024] = "";

    // 新接收的报文
	can_frame new_can;              
	memset(&new_can, 0, sizeof(can_frame));
	new_can.can_dlc = FRAME_DLC;


	int start_attack_pos = (rand()%10 + 1) * 10000;            // 上一次攻击结束的位置,初始化为第一次攻击的随机位置
	// printf("start_attack_pos: %d\n", start_attack_pos);
    // 获取注入间隔
    int inject_interval = get_injection_interval(injection_strength);

	int64_t last_attack_time = 0;	                // 上一次注入的时间
	int64_t first_attack_time = 0;	                // 第一次注入的时间

	// 构造注入报文
	can_frame targeted_can;              // 新接收的报文
	memset(&targeted_can, 0, sizeof(can_frame));
	targeted_can.can_dlc = FRAME_DLC;
	int targeted_can_id_num = get_rand_can_id(vec_can_id);
	targeted_can.can_id = get_map_num2id(vec_can_id, targeted_can_id_num);
	for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
	{
		// 随机选取 0~255 
		int value = rand()%256;
		targeted_can.data[byte_pos] = value;
	}


	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_file ) != NULL)
	{
		//printf("%s\n", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count ++;
	
        // 当前报文是否篡改的记号
		int is_tamper = 0;
		
   
        // 读取了一段报文之后，开始随机注入报文
        if(count >= start_attack_pos)
		{

		    if(last_attack_time == 0)
		    {
		        last_attack_time = current_readtime;
				first_attack_time = current_readtime;
				
		        // 相同时间比较ID
                if(targeted_can.can_id >= new_can.can_id)
                {
					save_read_buffer(tamper_file, &current_readtime, &new_can);
					is_tamper = 0;
					fprintf(label_file, "%d\n", is_tamper); 

					for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
					{
						// 随机选取 0~255 
						int value = rand()%256;
						targeted_can.data[byte_pos] = value;
					}
					save_read_buffer(tamper_file, &current_readtime, &targeted_can);
					is_tamper = 1;
					fprintf(label_file, "%d\n", is_tamper); 	
                }
				else
				{
					for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
					{
						// 随机选取 0~255 
						int value = rand()%256;
						targeted_can.data[byte_pos] = value;
					}
					save_read_buffer(tamper_file, &last_attack_time, &targeted_can);
					is_tamper = 1;
					fprintf(label_file, "%d\n", is_tamper); 
					
					
					save_read_buffer(tamper_file, &current_readtime, &new_can);
					is_tamper = 0;
					fprintf(label_file, "%d\n", is_tamper); 
				}

				count += 2;
		    }
			else if(current_readtime > last_attack_time + inject_interval)
			{

			    while(current_readtime > last_attack_time + inject_interval)
			    {
					last_attack_time += inject_interval;
					
					for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
					{
						// 随机选取 0~255 
						int value = rand()%256;
						targeted_can.data[byte_pos] = value;
					}
					save_read_buffer(tamper_file, &last_attack_time, &targeted_can);
					is_tamper = 1;
					fprintf(label_file, "%d\n", is_tamper); 
					count++;
			    }

				save_read_buffer(tamper_file, &current_readtime, &new_can);
				is_tamper = 0;
				fprintf(label_file, "%d\n", is_tamper); 
				count++;
			}
			else
			{
				save_read_buffer(tamper_file, &current_readtime, &new_can);
				is_tamper = 0;
				fprintf(label_file, "%d\n", is_tamper); 
				count++;
			}

		}
		else
		{
			save_read_buffer(tamper_file, &current_readtime, &new_can);
			is_tamper = 0;
			fprintf(label_file, "%d\n", is_tamper); 
			count++;
		}

		
        // 恶意攻击时间持续10s
        if( (current_readtime > first_attack_time + ATTACK_DURATION) && (first_attack_time > 0) )
        {
            break;
        }

		
	}	

    printf("finish injection, count: %d\n", count);


    return RET_OK;
}


// 特定报文注入攻击测试,生成异常报文和标签（重复注入一种ID的报文，报文值均合法）
int targeted_injection_attack_test(FILE *data_file, FILE *tamper_file, FILE *label_file, vector<UINT> &vec_can_id, vector< vector<Byte_Profile> > &byte_profile, int injection_strength)
{
    if(!data_file || !tamper_file || !label_file)
    {
        LOG_DEBUG(RED "invalid input! data_file[%p], tamper_file[%p], label_file[%p]" NONE, data_file, tamper_file, label_file);
		return RET_ERROR;
    }
	srand((unsigned)time(NULL));

	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳) 
    int count = 0;
	char data_buffer[1024] = "";

    // 新接收的报文
	can_frame new_can;              
	memset(&new_can, 0, sizeof(can_frame));
	new_can.can_dlc = FRAME_DLC;


	int start_attack_pos = (rand()%10 + 1)  * 10000;            // 上一次攻击结束的位置,初始化为第一次攻击的随机位置
    // 获取注入间隔
    int inject_interval = get_injection_interval(injection_strength);

	int64_t last_attack_time = 0;	                // 上一次注入的时间
	int64_t first_attack_time = 0;	                // 第一次注入的时间

	// 构造注入报文
	can_frame targeted_can;              // 新接收的报文
	memset(&targeted_can, 0, sizeof(can_frame));
	targeted_can.can_dlc = FRAME_DLC;
	int targeted_can_id_num = get_rand_can_id(vec_can_id);
	targeted_can.can_id = get_map_num2id(vec_can_id, targeted_can_id_num);
	for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
	{
	    // 存储对应字节位置的所有合法值
	    vector<int> vec_legal_value;
	
		for(int tt = 0; tt < BYTE_VALUE_NUM; tt++)
		{
			int value = byte_profile[targeted_can_id_num][byte_pos].legal_data[tt];
			if(value == VALID)
			{
				vec_legal_value.push_back(tt);
			}
		}
	
		// 随机选取 1个
		int legal_value_size = vec_legal_value.size();
		int legal_value_num = rand()%legal_value_size;
		targeted_can.data[byte_pos] = vec_legal_value[legal_value_num];

//		printf("%08x, byte%d,legal_value: ", new_can.can_id, byte_pos);
//		for(int i = 0; i < legal_value_size; i++)
//		{
//		    printf(" %d", vec_legal_value[i]);
//		}
//		 printf("\n");

	}



	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_file ) != NULL)
	{
		//printf("%s\n", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count ++;
	
        // 当前报文是否篡改的记号
		int is_tamper = 0;

        // 读取了一段报文之后，开始随机注入报文
        if(count >= start_attack_pos)
		{

		    if(last_attack_time == 0)
		    {
		        last_attack_time = current_readtime;
				first_attack_time = current_readtime;
				
		        // 相同时间比较ID
                if(targeted_can.can_id >= new_can.can_id)
                {
					save_read_buffer(tamper_file, &current_readtime, &new_can);
					is_tamper = 0;
					fprintf(label_file, "%d\n", is_tamper); 

					save_read_buffer(tamper_file, &current_readtime, &targeted_can);
					is_tamper = 1;
					fprintf(label_file, "%d\n", is_tamper); 	
                }
				else
				{
					save_read_buffer(tamper_file, &last_attack_time, &targeted_can);
					is_tamper = 1;
					fprintf(label_file, "%d\n", is_tamper); 
					
					
					save_read_buffer(tamper_file, &current_readtime, &new_can);
					is_tamper = 0;
					fprintf(label_file, "%d\n", is_tamper); 
				}

				count += 2;
		    }
			else if(current_readtime > last_attack_time + inject_interval)
			{

			    while(current_readtime > last_attack_time + inject_interval)
			    {
					last_attack_time += inject_interval;
					
					save_read_buffer(tamper_file, &last_attack_time, &targeted_can);
					is_tamper = 1;
					fprintf(label_file, "%d\n", is_tamper); 
					count++;
			    }

				save_read_buffer(tamper_file, &current_readtime, &new_can);
				is_tamper = 0;
				fprintf(label_file, "%d\n", is_tamper); 
				count++;
			}
			else
			{
				save_read_buffer(tamper_file, &current_readtime, &new_can);
				is_tamper = 0;
				fprintf(label_file, "%d\n", is_tamper); 
				count++;
			}			

		}
		else
		{
			save_read_buffer(tamper_file, &current_readtime, &new_can);
			is_tamper = 0;
			fprintf(label_file, "%d\n", is_tamper); 
			count++;
		}		


        // 恶意攻击时间持续10s
        if( (current_readtime > first_attack_time + ATTACK_DURATION) && (first_attack_time > 0) )
        {
            break;
        }


		
	}	

    printf("finish injection, count: %d\n", count);


    return RET_OK;
}


// 重放攻击测试，生成异常报文和标签(读取原始数据并使用重放攻击生成异常数据)(开始攻击后，不断重复注入)
int replay_attack_test(FILE *data_file, FILE *tamper_file, FILE *label_file, vector<UINT> &vec_can_id, int injection_strength)
{
    if(!data_file || !tamper_file || !label_file)
    {
        LOG_DEBUG(RED "invalid input! data_file[%p], tamper_file[%p], label_file[%p]" NONE, data_file, tamper_file, label_file);
		return RET_ERROR;
    }
	srand((unsigned)time(NULL));

	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳) 
    int count = 0;
	char data_buffer[1024] = "";

    // 新接收的报文
	can_frame new_can;              
	memset(&new_can, 0, sizeof(can_frame));
	new_can.can_dlc = FRAME_DLC;

	int start_attack_pos = (rand()%10 + 1) * 10000;            // 上一次攻击结束的位置,初始化为第一次攻击的随机位置
    // 获取注入间隔
    int inject_interval = get_injection_interval(injection_strength);

	int64_t last_attack_time = 0;	                // 上一次注入的时间
	int64_t first_attack_time = 0;	                // 第一次注入的时间

    // 读取训练集中的序列
    int replay_file_num = (rand()%MAX_FILE_NUM + 1);
	FILE *replay_set = NULL;
	char replay_set_path[100] = "";
	snprintf(replay_set_path, sizeof(replay_set_path), "../dataset/train/%03d.csv", replay_file_num);
    replay_set = fopen(replay_set_path, MODE_R_PLUS);
	

	can_frame replay_can;                     // 用于重放的报文
	memset(&replay_can, 0, sizeof(can_frame));
	replay_can.can_dlc = FRAME_DLC;
	int64_t replay_readtime;			// 当前replay序列记录的时间戳(采集的数据记录的时间戳) 

    int replay_count = 0;                                        // replay序列的计数
    int replay_start_position = rand()%10 * 10000;               // replay序列的起始位置

	// 读取用于重放的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), replay_set ) != NULL)
	{
		parse_read_buffer(data_buffer, &replay_readtime, &replay_can);
		replay_count++;
		if(replay_count == replay_start_position)
		{
		    break;
		}
	}

	// printf("replay_path%s: \n replay_position: %d \n attack_position: %d \n", replay_set_path, replay_start_position, last_attack_end);


	
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_file ) != NULL)
	{
		//printf("%s\n", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count ++;

        // 当前报文是否篡改的记号
		int is_tamper = 0;

        // 读取了一段报文之后，开始随机注入报文
        if(count >= start_attack_pos)
		{

		    if(last_attack_time == 0)
		    {
		        last_attack_time = current_readtime;
				first_attack_time = current_readtime;

                if(fgets(data_buffer, sizeof(data_buffer), replay_set ) != NULL)
                {
                	parse_read_buffer(data_buffer, &replay_readtime, &replay_can);
					
					// 相同时间比较ID
					if(replay_can.can_id >= new_can.can_id)
					{
						save_read_buffer(tamper_file, &current_readtime, &new_can);
						is_tamper = 0;
						fprintf(label_file, "%d\n", is_tamper); 
					
						save_read_buffer(tamper_file, &current_readtime, &replay_can);
						is_tamper = 1;
						fprintf(label_file, "%d\n", is_tamper); 	
					}
					else
					{
						save_read_buffer(tamper_file, &last_attack_time, &replay_can);
						is_tamper = 1;
						fprintf(label_file, "%d\n", is_tamper); 
						
						
						save_read_buffer(tamper_file, &current_readtime, &new_can);
						is_tamper = 0;
						fprintf(label_file, "%d\n", is_tamper); 
					}
					
					count += 2;
                }				

		    }
			else if(current_readtime > last_attack_time + inject_interval)
			{

			    while(current_readtime > last_attack_time + inject_interval)
			    {
					last_attack_time += inject_interval;

					if(fgets(data_buffer, sizeof(data_buffer), replay_set ) != NULL)
					{
						parse_read_buffer(data_buffer, &replay_readtime, &replay_can);
						
						save_read_buffer(tamper_file, &last_attack_time, &replay_can);
						is_tamper = 1;
						fprintf(label_file, "%d\n", is_tamper); 
						count++;
					}
			    }

				save_read_buffer(tamper_file, &current_readtime, &new_can);
				is_tamper = 0;
				fprintf(label_file, "%d\n", is_tamper); 
				count++;
			}
			else
			{
				save_read_buffer(tamper_file, &current_readtime, &new_can);
				is_tamper = 0;
				fprintf(label_file, "%d\n", is_tamper); 
				count++;
			}

			

		}
		else
		{
			save_read_buffer(tamper_file, &current_readtime, &new_can);
			is_tamper = 0;
			fprintf(label_file, "%d\n", is_tamper); 
			count++;
		}		


        // 恶意攻击时间持续10s
        if( (current_readtime > first_attack_time + ATTACK_DURATION) && (first_attack_time > 0) )
        {
            break;
        }


	}	

    printf("finish replay, count: %d\n", count);
	

    fclose(replay_set);   //关闭重放的数据集

	return RET_OK;
}

// 随机篡改攻击测试，生成异常报文和标签，[1~8]个字节值改成随机值
int random_tamper_attack_test(FILE *data_file, FILE *tamper_file, FILE *label_file, vector<UINT> &vec_can_id, int tamper_num)
{
    // 读取原始数据并使用重放攻击生成异常数据
    
    if(!data_file || !tamper_file || !label_file)
    {
        LOG_DEBUG(RED "invalid input! data_file[%p], tamper_file[%p], label_file[%p]" NONE, data_file, tamper_file, label_file);
		return RET_ERROR;
    }
	srand((unsigned)time(NULL));

	int can_id_size = (int)vec_can_id.size();

	can_frame new_can;              // 新接收的报文
	memset(&new_can, 0, sizeof(can_frame));
	new_can.can_dlc = FRAME_DLC;
	
	int64_t current_readtime;	        // 当前记录的时间戳(采集的数据记录的时间戳) 
	
    int count = 0;
	
	char data_buffer[1024] = "";

	int start_attack_pos = (rand()%10 + 1) * 10000;            // 开始攻击位置
	int64_t first_attack_time = 0;	                     // 第一次篡改的时间


	int targeted_can_id_num = rand()%can_id_size;
	UINT targeted_can_id = get_map_num2id(vec_can_id, targeted_can_id_num);

    // 确定篡改的字节位置(不重复)
    int mark[FRAME_DLC] = {0};
	mark_pos(mark, tamper_num);
	
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_file ) != NULL)
	{
		//LOG_DEBUG("%s", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count ++;
	
        // 当前报文是否篡改的记号
		int is_tamper = 0;
		

        // 开始攻击报文
        if(count > start_attack_pos && new_can.can_id == targeted_can_id)
		{
		    if(first_attack_time == 0)
		    {
				first_attack_time = current_readtime;
		    }
			
		    //开始篡改报文
		    is_tamper = 1;

			for(int k = 0; k < FRAME_DLC; k++)
			{
				if(mark[k] == VALID)
				{
					int value = rand()%256;
					new_can.data[k] = value;
				}
			}

		}

		
		// 记录报文
		// 存储修改后的报文
		save_read_buffer(tamper_file, &current_readtime, &new_can);

        // 存储标签
        fprintf(label_file, "%d\n", is_tamper);        

        // 恶意攻击时间持续10s
        if( (current_readtime > first_attack_time + ATTACK_DURATION) && (first_attack_time > 0) )
        {
            break;
        }

	}	

    printf("finish tamper, count: %d\n", count);


    return RET_OK;
}





// 特定篡改攻击测试，生成异常报文和标签（重复篡改一种ID的报文，8个字节值均合法）
int targeted_tamper_attack_test(FILE *data_file, FILE *tamper_file, FILE *label_file, vector<UINT> &vec_can_id, vector< vector<Byte_Profile> > &byte_profile, int tamper_num)
{
    // 读取原始数据并使用重放攻击生成异常数据
    
    if(!data_file || !tamper_file || !label_file)
    {
        LOG_DEBUG(RED "invalid input! data_file[%p], tamper_file[%p], label_file[%p]" NONE, data_file, tamper_file, label_file);
		return RET_ERROR;
    }

	srand((unsigned)time(NULL));

	can_frame new_can;                  // 新接收的报文
	memset(&new_can, 0, sizeof(can_frame));
	new_can.can_dlc = FRAME_DLC;
	int64_t current_readtime;	                    // 当前记录的时间戳(采集的数据记录的时间戳) 
    int count = 0;
	int start_attack_pos = (rand()%10 + 1) * 10000;       // 开始攻击位置 
	int64_t first_attack_time = 0;					// 第一次注入的时间

	
	// 确定篡改的报文
	can_frame targeted_can;              // 新接收的报文
	memset(&targeted_can, 0, sizeof(can_frame));
	targeted_can.can_dlc = FRAME_DLC;
	int targeted_can_id_num = get_rand_can_id(vec_can_id);
	targeted_can.can_id = get_map_num2id(vec_can_id, targeted_can_id_num);
	for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
	{
		// 存储对应字节位置的所有合法值
		vector<int> vec_legal_value;
		
		for(int tt = 0; tt < BYTE_VALUE_NUM; tt++)
		{
			int value = byte_profile[targeted_can_id_num][byte_pos].legal_data[tt];
			if(value == VALID)
			{
				vec_legal_value.push_back(tt);
			}
		}
		
		// 随机选取 1个
		int legal_value_size = (int)vec_legal_value.size();
		int legal_value_num = rand()%legal_value_size;
		targeted_can.data[byte_pos] = vec_legal_value[legal_value_num];
	}

    // 确定篡改的字节位置(不重复)
    int mark[FRAME_DLC] = {0};
	mark_pos(mark, tamper_num);

	char data_buffer[1024] = "";
	
	// 读取原始的CAN报文
	while( fgets(data_buffer, sizeof(data_buffer), data_file ) != NULL)
	{
		//printf("%s\n", data_buffer);
		parse_read_buffer(data_buffer, &current_readtime, &new_can);
	    count ++;
	

        // 当前报文是否篡改的记号
		int is_tamper = 0;
		

        // 开始攻击报文
        if(count > start_attack_pos && new_can.can_id == targeted_can.can_id)
		{
		    if(first_attack_time == 0)
		    {
				first_attack_time = current_readtime;
		    }

			is_tamper = 1;
			for(int k = 0; k < FRAME_DLC; k++)
			{
				if(mark[k] == VALID)
				{
					new_can.data[k] = targeted_can.data[k];
				}
			}						
			
		}

		
		// 记录报文
		// 存储修改后的报文
		save_read_buffer(tamper_file, &current_readtime, &new_can);
		
        // 存储标签
        fprintf(label_file, "%d\n", is_tamper);        

        // 恶意攻击时间持续10s
        if( (current_readtime > first_attack_time + ATTACK_DURATION) && (first_attack_time > 0) )
        {
            break;
        }

	}	

    printf("finish tamper, count: %d\n", count);


    return RET_OK;
}

// 不同滑动窗口和比特操作下模型的训练以及无功击情况下的模型验证
int global_model_train_and_model_detect_attack_free()
{
    for(int bit_op = BIT_OP_XOR; bit_op <= BIT_OP_NOR; bit_op++)
    {
	    for(int sliding_window_size = MIN_SLIDING_WINDOW_SIZE; sliding_window_size <= MAX_SLIDING_WINDOW_SIZE; sliding_window_size++)
	    {	                               
	        model_train((BIT_OP)bit_op, sliding_window_size);         // 模型训练
	        LOG_DEBUG("model_train complete! [bit_op: %d, sliding_window_size: %d]", bit_op, sliding_window_size);
			
	        LOG_DEBUG("start model detect... [bit_op: %d, sliding_window_size: %d]", bit_op, sliding_window_size);   
			model_detect_attack_free((BIT_OP)bit_op, sliding_window_size);        // 模型检测
	    }    
    }

    return 0;
}

// 不同位运算和滑动窗口参数的情况下，不同攻击的检测性能（均为最大攻击强度）
int global_model_detect_under_various_attack()
{
    // 用于生成篡改数据的必要变量
	vector<UINT> common_vec_can_id;							// CAN_ID列表
	vector<ID_Profile> common_id_profile;					// id轮廓
	vector< vector<Byte_Profile> > common_byte_profile;		// 字节轮廓
	CSimpleIniA common_configurator;					    // 配置文件
	
	// 打开模型参数文件
	char configuration_path[100] = "";
	snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "0_001.init");	
	SI_Error rc = common_configurator.LoadFile(configuration_path);
	if (rc < 0) 
	{
		LOG_DEBUG(RED "load init file failed !" NONE);
	}
	else
	{
		//LOG_DEBUG(GREEN "load init file OK !" NONE);	
	}
	
	int res = RET_ERROR;
	
	
	// 从配置文件读取ID轮廓
	res = read_id_profile(common_id_profile, common_vec_can_id, common_configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_id_profile failed !" NONE);
		
		return RET_ERROR;
	}
	
	// 从配置文件读取byte轮廓
	res = read_byte_profile(common_byte_profile, common_vec_can_id, common_configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_bit_profile failed !" NONE);
		
		return RET_ERROR;
	}


	for(int attack_type = ATTACK_TYPE_RANDOM_INJECTION; attack_type <= ATTACK_TYPE_TARGETED_TAMPER; attack_type++)
	// for(int attack_type = ATTACK_TYPE_RANDOM_INJECTION; attack_type <= ATTACK_TYPE_RANDOM_INJECTION; attack_type++)
	// for(int attack_type = ATTACK_TYPE_TARGETED_INJECTION; attack_type <= ATTACK_TYPE_TARGETED_INJECTION; attack_type++)
	// for(int attack_type = ATTACK_TYPE_REPLAY; attack_type <= ATTACK_TYPE_REPLAY; attack_type++)
	// for(int attack_type = ATTACK_TYPE_RANDOM_TAMPER; attack_type <= ATTACK_TYPE_RANDOM_TAMPER; attack_type++)	
	// for(int attack_type = ATTACK_TYPE_TARGETED_TAMPER; attack_type <= ATTACK_TYPE_TARGETED_TAMPER; attack_type++)	
	{
		// 获取攻击类型的字符串名称
		char attack_name[ATTACK_NAME_MAX_SIZE] = "";
		get_attack_name(&attack_name[0], (ATTACK_TYPE)attack_type);

		// 各个指标累积计算
		int tp_sum[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0};	  
		int tn_sum[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0}; 
		int fp_sum[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0}; 
		int fn_sum[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0}; 
		int delay_sum[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0}; 
						
		// 执行多次实验,直接从文件读取数据
		//for(int data_num = MIN_FILE_NUM; data_num <= MIN_FILE_NUM; data_num++)
		for(int data_num = MIN_FILE_NUM; data_num <= MAX_FILE_NUM; data_num++)
		{
			// 原始数据文件指针
			FILE *raw_data_file = NULL;
		
			char raw_buffer[100] = "";
			snprintf(raw_buffer, sizeof(raw_buffer), TEST_PATH "%03d.csv", data_num);
			printf(RED "inject data: %s\n" NONE, raw_buffer);
		
			raw_data_file = fopen(raw_buffer, MODE_R_PLUS);
			if(raw_data_file == NULL)
			{
				LOG_DEBUG(RED "open file failed!!  %s " NONE, raw_buffer); 
				break;
			}
		
			// 同一份文件攻击若干次
			//for(int attack_num = 1; attack_num <= 1; attack_num++)
			for(int attack_num = 1; attack_num <= ATTACK_NUM; attack_num++)
			{
		
				// 将文件重新定位到开头
				rewind(raw_data_file);	  
				
				// 篡改数据指针
				FILE *tamper_file = NULL;				
				char tamper_buffer[100] = "";
				snprintf(tamper_buffer, sizeof(tamper_buffer), ATTACK_PATH "%s%03d%06d.csv", attack_name, data_num, attack_num);
				printf(RED "detect data: %s\n" NONE, tamper_buffer);
				tamper_file = fopen(tamper_buffer, MODE_W_PLUS);
				if(tamper_file == NULL)
				{
					LOG_DEBUG(RED "open file failed!!  %s " NONE, tamper_buffer); 
					break;
				}

				// 篡改数据标签
				FILE *label_file = NULL;				
				char label_buffer[100] = "";
				snprintf(label_buffer, sizeof(label_buffer), LABEL_PATH "%s%03d%06d.csv", attack_name, data_num, attack_num);
				label_file = fopen(label_buffer, MODE_W_PLUS);
				if(label_file == NULL)
				{
					LOG_DEBUG(RED "open file failed!!  %s " NONE, label_buffer); 
					break;
				}			
		
				// 制作/获取篡改数据
				switch (attack_type)
					{
					    case ATTACK_TYPE_RANDOM_INJECTION:
					    {
					        random_injection_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, MAX_INJECTION_STRENGTH);
							break;
					    }
					    case ATTACK_TYPE_TARGETED_INJECTION:
					    {
					        targeted_injection_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, common_byte_profile, MAX_INJECTION_STRENGTH);
							break;
					    }
					    case ATTACK_TYPE_REPLAY:
					    {
					        replay_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, MAX_INJECTION_STRENGTH);
							break;
					    }
					    case ATTACK_TYPE_RANDOM_TAMPER:
					    {
					        random_tamper_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, MAX_TAMPER_NUM);
							break;
					    }
					    case ATTACK_TYPE_TARGETED_TAMPER:
					    {
					        targeted_tamper_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, common_byte_profile, MAX_TAMPER_NUM);
							break;
					    }
						default:
							LOG_DEBUG(RED "attack_type error: %d" NONE, attack_type);
					}

				// 依次使用不同的方法进行检测
				for(int bit_op = BIT_OP_XOR; bit_op <= BIT_OP_NOR; bit_op++)
				{
					for(int sliding_window_size = MIN_SLIDING_WINDOW_SIZE; sliding_window_size <= MAX_SLIDING_WINDOW_SIZE; sliding_window_size++)
					{
                        printf("data_num: %d, attack_num: %d, bit_op: %d, sliding_window_size: %d\n", data_num, attack_num, bit_op, sliding_window_size);
					    
						// 将文件重新定位到开头,方便后面执行检测
						rewind(tamper_file);  

						// 篡改数据检测结果
						FILE *result_file = NULL;
						char result_buffer[100] = "";
						snprintf(result_buffer, sizeof(result_buffer), RESULT_PATH "%s%03d%06d_%d_%d.csv", attack_name, data_num, attack_num, (BIT_OP)bit_op, sliding_window_size);
						result_file = fopen(result_buffer, MODE_W_PLUS);
						if(result_file == NULL)
						{
							LOG_DEBUG(RED "open file failed!!  %s " NONE, result_buffer); 
							break;
						}

						// 加载模型
						vector<UINT> vec_can_id;							// CAN_ID列表
						vector<ID_Profile> id_profile;						// id轮廓
						vector< vector<Byte_Profile> > byte_profile;		// 字节轮廓
						vector<vector<Bit_Profile>> bit_profile;			// bit轮廓		
						
						CSimpleIniA configurator;							// 配置文件
						
						// 打开模型参数文件
						char configuration_path[100] = "";
						snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "%d_%03d.init", bit_op, sliding_window_size);	
						SI_Error rc = configurator.LoadFile(configuration_path);
						if (rc < 0) 
						{
							LOG_DEBUG(RED "load init file failed !" NONE);
						}
						else
						{
							// LOG_DEBUG(GREEN "load init file OK !" NONE);	
						}
						
						int res = RET_ERROR;
						
						
						// 从配置文件读取ID轮廓
						res = read_id_profile(id_profile, vec_can_id, configurator);
						if (res == RET_ERROR)
						{
							LOG_DEBUG(RED "read_id_profile failed !" NONE);
							
							return RET_ERROR;
						}
						
						// 从配置文件读取byte轮廓
						res = read_byte_profile(byte_profile, vec_can_id, configurator);
						if (res == RET_ERROR)
						{
							LOG_DEBUG(RED "read_bit_profile failed !" NONE);
							
							return RET_ERROR;
						}
						
						
						// 从配置文件读取bit轮廓
						res = read_bit_profile(bit_profile, vec_can_id, configurator);
						if (res == RET_ERROR)
						{
							LOG_DEBUG(RED "read_bit_profile failed !" NONE);
							
							return RET_ERROR;
						}
						
				        detect_and_record(tamper_file, result_file, id_profile, byte_profile, bit_profile, vec_can_id, (BIT_OP)bit_op, sliding_window_size);
						
						// 对比结果
						rewind(label_file);
						rewind(result_file);
						
		
						int tp_temp[COMPARSION_METHOD_NUM] = {0}; 
						int tn_temp[COMPARSION_METHOD_NUM] = {0}; 
						int fp_temp[COMPARSION_METHOD_NUM] = {0}; 
						int fn_temp[COMPARSION_METHOD_NUM] = {0}; 
						int delay_temp[COMPARSION_METHOD_NUM] = {0};
						
						compare_result_one_by_one(label_file, result_file, tp_temp, tn_temp, fp_temp, fn_temp);
						//compare_result_by_attack_hit(label_file, result_file, tp_temp, tn_temp, fp_temp, fn_temp, delay_temp);	// 从攻击是否被检测的角度比较检测结果
						
						for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
						{
						    
							//printf("tp[%d] tn[%d] fp[%d] fn[%d]\n", tp_temp[i], tn_temp[i], fp_temp[i], fn_temp[i]);
							tp_sum[bit_op][sliding_window_size-1][i] += tp_temp[i];
							tn_sum[bit_op][sliding_window_size-1][i] += tn_temp[i];
							fp_sum[bit_op][sliding_window_size-1][i] += fp_temp[i];
							fn_sum[bit_op][sliding_window_size-1][i] += fn_temp[i];
							delay_sum[bit_op][sliding_window_size-1][i] += delay_temp[i];				
						}
						
						// 关闭文件
						fclose(result_file);
						
						// 删除临时文件
						if(DELETE_TEMP_FILE)
						{
							remove(result_buffer);		 // 检测结果				
						}

					}
				}
				
				// 关闭文件
				fclose(tamper_file);
				fclose(label_file);
				
				// 删除临时文件
				if(DELETE_TEMP_FILE)
				{
					remove(tamper_buffer);		   // 篡改后的文件  remove+文件路径
					remove(label_buffer);		 // 数据标签		
				}

			}
		
			// 关闭文件
			fclose(raw_data_file);
		}


        // 记录检测结果
		char path[100] = "";
		snprintf(path, sizeof(path), ANALYSIS_PATH	"global_%s.csv", attack_name);		
		FILE *final_res = fopen(path, MODE_W_PLUS);
		
		// 准确率
		double accuracy[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0};
		 
		// 精确率
		double precision[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0};
		 
		// 召回率
		double recall[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0};
		 
		// 误报率
		double false_alarm_rate[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {0};
		
		// 平均检测延迟
		double average_delay[BIT_OP_NUM][MAX_SLIDING_WINDOW_SIZE][COMPARSION_METHOD_NUM] = {-1};
		 
		
		for(int bit_op = BIT_OP_XOR; bit_op <= BIT_OP_NOR; bit_op++)
		{
			for(int sliding_window_size = MIN_SLIDING_WINDOW_SIZE; sliding_window_size <= MAX_SLIDING_WINDOW_SIZE; sliding_window_size++)
			{
				for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
				{
					int tp_temp = tp_sum[bit_op][sliding_window_size-1][i]; 
					int tn_temp = tn_sum[bit_op][sliding_window_size-1][i]; 
					int fp_temp = fp_sum[bit_op][sliding_window_size-1][i]; 
					int fn_temp = fn_sum[bit_op][sliding_window_size-1][i]; 
					int delay_temp= delay_sum[bit_op][sliding_window_size-1][i]; 

				
					// 计算准确率
					if((tp_temp + tn_temp + fp_temp + fn_temp)== 0)
					{
						accuracy[bit_op][sliding_window_size-1][i] = 0;
					}
					else
					{
						accuracy[bit_op][sliding_window_size-1][i] = (double)(tp_temp + tn_temp) / (double)(tp_temp + tn_temp + fp_temp + fn_temp);
					}
					
					 
					// 计算精确率
					if( (tp_temp + fp_temp) == 0)
					{
						precision[bit_op][sliding_window_size-1][i] = 0;
					}else
					{
						precision[bit_op][sliding_window_size-1][i] = (double)(tp_temp) / (double)(tp_temp + fp_temp);
					}
					
					 
					// 计算召回率
					if( (tp_temp + fn_temp) == 0 )
					{
						recall[bit_op][sliding_window_size-1][i] = 0;
					}
					else
					{
						recall[bit_op][sliding_window_size-1][i] = (double)(tp_temp) / (double)(tp_temp + fn_temp);
					}
					
					 
					// 计算误报率
					if( (fp_temp + tn_temp)==0 )
					{
						false_alarm_rate[bit_op][sliding_window_size-1][i] = 0;
					}
					else
					{
						false_alarm_rate[bit_op][sliding_window_size-1][i] = (double)(fp_temp) / (double)(fp_temp + tn_temp);
					}
					
					// 计算平均检测延迟
					if(tp_temp == 0)
					{
						average_delay[bit_op][sliding_window_size-1][i] = NOT_EXIST;
					}
					else
					{
						average_delay[bit_op][sliding_window_size-1][i] = (double)(delay_temp) / (double)(tp_temp);
					}	 
				}

//				char final_buffer[1024 * COMPARSION_METHOD_NUM * BIT_OP_NUM * MAX_SLIDING_WINDOW_SIZE] = "";
//				snprintf(final_buffer + strlen(final_buffer), sizeof(final_buffer) - strlen(final_buffer), "bit_op: %d, sliding_window_size: %d\n", bit_op, sliding_window_size); 		
//				for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
//				{
//					snprintf(final_buffer + strlen(final_buffer), sizeof(final_buffer) - strlen(final_buffer), 
//						  "method: %s, tp: %5d, tn: %5d, fp: %5d, fn: %5d, accuracy: %5lf, precision: %5lf, recall: %5lf false_alarm_rate: %5lf delay: %lf\n", 
//							  method_name[i], 
//							  	tp_sum[bit_op][sliding_window_size-1][i], 
//							  		tn_sum[bit_op][sliding_window_size-1][i], 
//							  			fp_sum[bit_op][sliding_window_size-1][i], 
//							  				fn_sum[bit_op][sliding_window_size-1][i], 
//							  					accuracy[bit_op][sliding_window_size-1][i], 
//							  						precision[bit_op][sliding_window_size-1][i], 
//							  							recall[bit_op][sliding_window_size-1][i], 
//							  								false_alarm_rate[bit_op][sliding_window_size-1][i], 
//							  									average_delay[bit_op][sliding_window_size-1][i]); 		
//				}
//				LOG_DEBUG(YELLOW "\n%s" NONE, final_buffer);			  
				
				// 检测结果写入文件
				fprintf(final_res,"%d,%d", bit_op, sliding_window_size);
				for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
				{
					fprintf(final_res,",%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf", 
						tp_sum[bit_op][sliding_window_size-1][i], 
						    tn_sum[bit_op][sliding_window_size-1][i], 
							  	fp_sum[bit_op][sliding_window_size-1][i], 
							  		fn_sum[bit_op][sliding_window_size-1][i], 
							  			accuracy[bit_op][sliding_window_size-1][i], 
							  				precision[bit_op][sliding_window_size-1][i], 
							  					recall[bit_op][sliding_window_size-1][i], 
							  						false_alarm_rate[bit_op][sliding_window_size-1][i], 
							  							average_delay[bit_op][sliding_window_size-1][i]); 		
				}
				fprintf(final_res,"\n");	

			
			}
		}	
	
		// 关闭文件
        fclose(final_res);

	}

    return RET_OK;
}

// 位运算和滑动窗口参数确定的情况下，不同攻击的检测性能（攻击强度变化）
int specific_model_detect_under_various_attack(BIT_OP bit_op, int sliding_window_size)
{
	// 加载模型
	vector<UINT> vec_can_id;							// CAN_ID列表
	vector<ID_Profile> id_profile;						// id轮廓
	vector< vector<Byte_Profile> > byte_profile;		// 字节轮廓
	vector<vector<Bit_Profile>> bit_profile;			// bit轮廓		
	
	CSimpleIniA configurator;							// 配置文件
	
	// 打开模型参数文件
	char configuration_path[100] = "";
	snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "%d_%03d.init", bit_op, sliding_window_size);	
	SI_Error rc = configurator.LoadFile(configuration_path);
	if (rc < 0) 
	{
		LOG_DEBUG(RED "load init file failed !" NONE);
	}
	else
	{
		// LOG_DEBUG(GREEN "load init file OK !" NONE);	
	}
	
	int res = RET_ERROR;
	
	
	// 从配置文件读取ID轮廓
	res = read_id_profile(id_profile, vec_can_id, configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_id_profile failed !" NONE);
		
		return RET_ERROR;
	}
	
	// 从配置文件读取byte轮廓
	res = read_byte_profile(byte_profile, vec_can_id, configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_bit_profile failed !" NONE);
		
		return RET_ERROR;
	}
	
	
	// 从配置文件读取bit轮廓
	res = read_bit_profile(bit_profile, vec_can_id, configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_bit_profile failed !" NONE);
		
		return RET_ERROR;
	}


	//for(int attack_type = ATTACK_TYPE_RANDOM_INJECTION; attack_type <= ATTACK_TYPE_TARGETED_TAMPER; attack_type++)
	//for(int attack_type = ATTACK_TYPE_RANDOM_INJECTION; attack_type <= ATTACK_TYPE_RANDOM_INJECTION; attack_type++)
	for(int attack_type = ATTACK_TYPE_TARGETED_INJECTION; attack_type <= ATTACK_TYPE_TARGETED_INJECTION; attack_type++)
	//for(int attack_type = ATTACK_TYPE_REPLAY; attack_type <= ATTACK_TYPE_REPLAY; attack_type++)
	//for(int attack_type = ATTACK_TYPE_RANDOM_TAMPER; attack_type <= ATTACK_TYPE_RANDOM_TAMPER; attack_type++)
	//for(int attack_type = ATTACK_TYPE_TARGETED_TAMPER; attack_type <= ATTACK_TYPE_TARGETED_TAMPER; attack_type++)
	{
		// 获取攻击类型的字符串名称
		char attack_name[ATTACK_NAME_MAX_SIZE] = "";
		get_attack_name(&attack_name[0], (ATTACK_TYPE)attack_type);


		// 记录检测结果
		char path[100] = "";
		snprintf(path, sizeof(path), ANALYSIS_PATH	"%s_attack_strength_change_%d_%d.csv", attack_name, bit_op, sliding_window_size);	
		FILE *final_res = fopen(path, MODE_W_PLUS);


        // 不同的攻击强度
        for(int attack_strength = MIN_INJECTION_STRENGTH; attack_strength <= MAX_INJECTION_STRENGTH; attack_strength++)
		// for(int attack_strength = MIN_INJECTION_STRENGTH; attack_strength <= MIN_INJECTION_STRENGTH; attack_strength++)
        {
			// 各个指标累积计算
			int tp_sum[COMPARSION_METHOD_NUM] = {0};	  
			int tn_sum[COMPARSION_METHOD_NUM] = {0}; 
			int fp_sum[COMPARSION_METHOD_NUM] = {0}; 
			int fn_sum[COMPARSION_METHOD_NUM] = {0}; 
			int delay_sum[COMPARSION_METHOD_NUM] = {0}; 
			
			// 执行多次实验,直接从文件读取数据
			for(int data_num = MIN_FILE_NUM; data_num <= MAX_FILE_NUM; data_num++)
			//for(int data_num = MIN_FILE_NUM; data_num <= MIN_FILE_NUM; data_num++)
			{
				// 原始数据文件指针
				FILE *raw_data_file = NULL;
			
				char raw_buffer[100] = "";
				snprintf(raw_buffer, sizeof(raw_buffer), TEST_PATH "%03d.csv", data_num);
				printf(RED "inject data: %s\n" NONE, raw_buffer);
			
				raw_data_file = fopen(raw_buffer, MODE_R_PLUS);
				if(raw_data_file == NULL)
				{
					LOG_DEBUG(RED "open file failed!!  %s " NONE, raw_buffer); 
					break;
				}
			
				// 同一份文件攻击若干次
				for(int attack_num = 1; attack_num <= ATTACK_NUM; attack_num++)
				{
			
					// 将文件重新定位到开头
					rewind(raw_data_file);	  
					
					// 篡改数据指针
					FILE *tamper_file = NULL;				
					char tamper_buffer[100] = "";
					snprintf(tamper_buffer, sizeof(tamper_buffer), ATTACK_PATH "%s_%d_%03d%06d.csv", attack_name, attack_strength, data_num, attack_num);
					printf(RED "detect data: %s\n" NONE, tamper_buffer);
					tamper_file = fopen(tamper_buffer, MODE_W_PLUS);
					if(tamper_file == NULL)
					{
						LOG_DEBUG(RED "open file failed!!  %s " NONE, tamper_buffer); 
						break;
					}
			
					// 篡改数据标签
					FILE *label_file = NULL;				
					char label_buffer[100] = "";
					snprintf(label_buffer, sizeof(label_buffer), LABEL_PATH "%s_%d_%03d%06d.csv", attack_name, attack_strength, data_num, attack_num);
					label_file = fopen(label_buffer, MODE_W_PLUS);
					if(label_file == NULL)
					{
						LOG_DEBUG(RED "open file failed!!  %s " NONE, label_buffer); 
						break;
					}			
			
					// 制作/获取篡改数据
					switch (attack_type)
						{
							case ATTACK_TYPE_RANDOM_INJECTION:
							{
								random_injection_attack_test(raw_data_file, tamper_file, label_file, vec_can_id, attack_strength);
								break;
							}
							case ATTACK_TYPE_TARGETED_INJECTION:
							{
								targeted_injection_attack_test(raw_data_file, tamper_file, label_file, vec_can_id, byte_profile, attack_strength);
								break;
							}
							case ATTACK_TYPE_REPLAY:
							{
								replay_attack_test(raw_data_file, tamper_file, label_file, vec_can_id, attack_strength);
								break;
							}
							case ATTACK_TYPE_RANDOM_TAMPER:
							{
								random_tamper_attack_test(raw_data_file, tamper_file, label_file, vec_can_id, attack_strength);
								break;
							}
							case ATTACK_TYPE_TARGETED_TAMPER:
							{
								targeted_tamper_attack_test(raw_data_file, tamper_file, label_file, vec_can_id, byte_profile, attack_strength);
								break;
							}
							default:
								LOG_DEBUG(RED "attack_type error: %d" NONE, attack_type);
						}
			

					// 将文件重新定位到开头,方便后面执行检测
					rewind(tamper_file);  
					
					// 篡改数据检测结果
					FILE *result_file = NULL;
					char result_buffer[100] = "";
					snprintf(result_buffer, sizeof(result_buffer), RESULT_PATH "%s%03d%06d_%d_%d.csv", attack_name, data_num, attack_num, (BIT_OP)bit_op, sliding_window_size);
					result_file = fopen(result_buffer, MODE_W_PLUS);
					if(result_file == NULL)
					{
						LOG_DEBUG(RED "open file failed!!  %s " NONE, result_buffer); 
						break;
					}
					
					
					detect_and_record(tamper_file, result_file, id_profile, byte_profile, bit_profile, vec_can_id, bit_op, sliding_window_size);
					
					// 对比结果
					rewind(label_file);
					rewind(result_file);
							
					int tp_temp[COMPARSION_METHOD_NUM] = {0}; 
					int tn_temp[COMPARSION_METHOD_NUM] = {0}; 
					int fp_temp[COMPARSION_METHOD_NUM] = {0}; 
					int fn_temp[COMPARSION_METHOD_NUM] = {0}; 
					int delay_temp[COMPARSION_METHOD_NUM] = {0};
					
					compare_result_one_by_one(label_file, result_file, tp_temp, tn_temp, fp_temp, fn_temp);
					//compare_result_by_attack_hit(label_file, result_file, tp_temp, tn_temp, fp_temp, fn_temp, delay_temp);	// 从攻击是否被检测的角度比较检测结果
					
					for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
					{
						
						//printf("tp[%d] tn[%d] fp[%d] fn[%d]\n", tp_temp[i], tn_temp[i], fp_temp[i], fn_temp[i]);
						tp_sum[i] += tp_temp[i];
						tn_sum[i] += tn_temp[i];
						fp_sum[i] += fp_temp[i];
						fn_sum[i] += fn_temp[i];
						delay_sum[i] += delay_temp[i];				
					}
					
					
					// 关闭文件
					fclose(tamper_file);
					fclose(label_file);
					fclose(result_file);
					
					// 删除临时文件
					if(DELETE_TEMP_FILE)
					{
						remove(tamper_buffer);		   // 篡改后的文件  remove+文件路径
						remove(label_buffer);		 // 数据标签	
						remove(result_buffer);		 // 检测结果	
					}
			
				}
			
				// 关闭文件
				fclose(raw_data_file);
			}
			
			

			// 准确率
			double accuracy[COMPARSION_METHOD_NUM] = {0};
			 
			// 精确率
			double precision[COMPARSION_METHOD_NUM] = {0};
			 
			// 召回率
			double recall[COMPARSION_METHOD_NUM] = {0};
			 
			// 误报率
			double false_alarm_rate[COMPARSION_METHOD_NUM] = {0};
			
			// 平均检测延迟
			double average_delay[COMPARSION_METHOD_NUM] = {-1};
			 
			
			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
			{
				// 计算准确率
				if((tp_sum[i] + tn_sum[i] + fp_sum[i] + fn_sum[i])== 0)
				{
					accuracy[i] = 0;
				}
				else
				{
					accuracy[i] = (double)(tp_sum[i] + tn_sum[i]) / (double)(tp_sum[i] + tn_sum[i] + fp_sum[i] + fn_sum[i]);
				}
				
				 
				// 计算精确率
				if( (tp_sum[i] + fp_sum[i]) == 0)
				{
					precision[i] = 0;
				}else
				{
					precision[i] = (double)(tp_sum[i]) / (double)(tp_sum[i] + fp_sum[i]);
				}
				
				 
				// 计算召回率
				if( (tp_sum[i] + fn_sum[i]) == 0 )
				{
					recall[i] = 0;
				}
				else
				{
					recall[i] = (double)(tp_sum[i]) / (double)(tp_sum[i] + fn_sum[i]);
				}
				
				 
				// 计算误报率
				if( (fp_sum[i] + tn_sum[i])==0 )
				{
					false_alarm_rate[i] = 0;
				}
				else
				{
					false_alarm_rate[i] = (double)(fp_sum[i]) / (double)(fp_sum[i] + tn_sum[i]);
				}
				
				// 计算平均检测延迟
				if(tp_sum[i] == 0)
				{
					average_delay[i] = NOT_EXIST;
				}
				else
				{
					average_delay[i] = (double)(delay_sum[i]) / (double)(tp_sum[i]);
				}	 
			}
			
//			char final_buffer[1024 * COMPARSION_METHOD_NUM] = "";
//			snprintf(final_buffer + strlen(final_buffer), sizeof(final_buffer) - strlen(final_buffer), "attack_type: %d, attack_strength: %d\n", attack_type, attack_strength);		
//			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
//			{
//				snprintf(final_buffer + strlen(final_buffer), sizeof(final_buffer) - strlen(final_buffer), 
//					  "method: %s, tp: %5d, tn: %5d, fp: %5d, fn: %5d, accuracy: %5lf, precision: %5lf, recall: %5lf false_alarm_rate: %5lf delay: %lf\n", 
//						  method_name[i], tp_sum[i], tn_sum[i], fp_sum[i], fn_sum[i], accuracy[i], precision[i], recall[i], false_alarm_rate[i], average_delay[i]); 		
//			}
//			LOG_DEBUG(YELLOW "\n%s" NONE, final_buffer);			  
			
			// 检测结果写入文件
			fprintf(final_res,"%d", attack_strength);
			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
			{
				fprintf(final_res,",%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf", 
					tp_sum[i], tn_sum[i], fp_sum[i], fn_sum[i], accuracy[i], precision[i], recall[i], false_alarm_rate[i], average_delay[i]);			
			}
			fprintf(final_res,"\n");

        }

		// 关闭文件
		fclose(final_res);		
	}

    return RET_OK;
}


// 测试-训练数据的丰富度对模型误报的影响
int specific_model_generalization_test(BIT_OP bit_op, int sliding_window_size)
{
	// 结果记录
	FILE *res_record = fopen(ANALYSIS_PATH "generalization_anomaly_count.csv", MODE_W_PLUS);

    for(int max_train_file_num = MIN_FILE_NUM; max_train_file_num <= MAX_FILE_NUM; max_train_file_num++)
    {
		vector<UINT> vec_can_id;							// CAN_ID列表
		vector<ID_Profile> id_profile;						// ID轮廓
		vector< vector<Byte_Profile> > byte_profile;		// 字节轮廓
		vector< vector<Bit_Profile> > bit_profile;			// bit轮廓
		
		CSimpleIniA configurator;
		// 设置配置文件编码模式
		configurator.SetUnicode();
		
		for(int train_file_num = MIN_FILE_NUM; train_file_num <= max_train_file_num; train_file_num++)
		{
			char train_data_path[100] = "";
			snprintf(train_data_path, sizeof(train_data_path), TRAIN_PATH "%03d.csv", train_file_num);
			printf(RED "feed %s "  NONE "\n", train_data_path);
			
			// 使用单一文件训练
			train_single_file(train_data_path, id_profile, byte_profile, bit_profile, vec_can_id, bit_op, sliding_window_size);
		}

        // 模型存储过程中有一个剪枝的操作
		int length_of_white_list = (int)vec_can_id.size(); 
		for(int i = 0; i < length_of_white_list; i++)
		{
			for(int j = 0; j < BIT_STREAM_NUM; j++)
			{
				double stable_rate = (double)bit_profile[i][j].stable_num/(double)bit_profile[i][j].total_num;
				if(stable_rate <  DETECTOR_INVALID_THRESHOLD )
				{
				    bit_profile[i][j].invalid_flag = VALID;
				}

			}
		
		}


		// 检测完整的35个文件时异常数目合计
		int frame_count_sum = 0;
		int anomaly_count_sum[COMPARSION_METHOD_NUM] = {0};

        // 仅检测于训练数据同等的文件数目时，异常数目
		int frame_count_with_same_train_file_num = 0;
		int anomaly_count_with_same_train_file_num[COMPARSION_METHOD_NUM] = {0};
		
		// 检测测试集
		for(int test_file_num = MIN_FILE_NUM; test_file_num  <= MAX_FILE_NUM; test_file_num ++)
		{
			char test_data_path[100] = "";
			snprintf(test_data_path, sizeof(test_data_path), TEST_PATH "%03d.csv", test_file_num );
			printf(RED "feed %s "  NONE "\n", test_data_path);
		
			
			// 单一文件检测
			int count = 0;
			int anomaly_count[COMPARSION_METHOD_NUM] = {0};
			detect_single_file_return_result(test_data_path, id_profile, byte_profile, bit_profile, vec_can_id, bit_op, sliding_window_size, count, &anomaly_count[0]);
		
			frame_count_sum += count;
			for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
			{
				anomaly_count_sum[i] += anomaly_count[i];
			}

			if(test_file_num == max_train_file_num)
			{
			    frame_count_with_same_train_file_num = frame_count_sum;
				
				for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
				{
					anomaly_count_with_same_train_file_num[i] = anomaly_count_sum[i];
				}
			}
		}

//		printf(YELLOW "max_train_file_num: %d, test_frame_count: %d," NONE "\n", max_train_file_num, frame_count_sum);
//		for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
//		{
//			printf(YELLOW "%s anomaly_count: %d " NONE "\n", method_name[i], anomaly_count_sum[i]);
//		}
		
		// 记录检测结果至临时文件
		fprintf(res_record, "%d,%d,%d", max_train_file_num, frame_count_sum, frame_count_with_same_train_file_num);
		for(int i = 0; i < COMPARSION_METHOD_NUM; i++)
		{
			fprintf(res_record, ",%d,%d", anomaly_count_with_same_train_file_num[i], anomaly_count_sum[i]);
		}
		fprintf(res_record, "\n");

    }

	fclose(res_record);


    return RET_OK;
}



// 测试检测速度 // xor,and,or,nand,nor效率测试
int test_detection_speed()
{
    int frame_num = 10000;

//    Bit_Profile bit_profile;
//	cout << "temp" << bit_profile.old_data_valid <<endl;
//	bit_profile.old_data_valid<<=1;
//	bit_profile.old_data_valid[0] = 1;
//	cout << "temp" << bit_profile.old_data_valid <<endl;
//	bit_profile.old_data_valid<<=1;
//	cout << "temp" << bit_profile.old_data_valid <<endl;
//
//	int sliding_window_size = 4;
//	
//	bit_profile.old_data.set(); 
//	cout << "1_temp_value," << bit_profile.old_data <<endl;
//	bit_profile.old_data[3] = INVALID; 
//	cout << "2_temp_value," << bit_profile.old_data <<endl;
//	bit_profile.old_data[0] = INVALID; 
//	cout << "3_temp_value," << bit_profile.old_data <<endl;
//	bit_profile.old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
//	bit_profile.old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
//	cout << "4_temp_value," << bit_profile.old_data <<endl;
//	int final_value = bit_profile.old_data.to_ulong();
//	cout << "final_value" << final_value <<endl;


    // 结果记录
	FILE *res_record = fopen(ANALYSIS_PATH "detection_speed_under_bit_op.csv", MODE_W_PLUS);

    for(int bit_op = BIT_OP_XOR; bit_op <= BIT_OP_NOR; bit_op++)
    {
		for(int sliding_window_size = MIN_SLIDING_WINDOW_SIZE; sliding_window_size <= MAX_SLIDING_WINDOW_SIZE; sliding_window_size++)	
		{
			can_frame new_can;				// 新接收的报文
			memset(&new_can, 0, sizeof(can_frame));
			new_can.can_dlc = FRAME_DLC;
		
		
			bitset<65536> tm_data;										   // pow(2, 16)用于存储不同滑动窗口下的whitelist							   
			bitset<MAX_SLIDING_WINDOW_SIZE> old_data;					   // 用于缓存滑动窗口内的 bit segment									
			bitset<MAX_SLIDING_WINDOW_SIZE> old_data_valid; 			   // 缓存的bit位是否有效
			
			int64_t timestamp_start = 0;
			int64_t timestamp_end = 0;
			int64_t timestamp_total = 0;
			double timestamp_average = 0;
		
			for(int i = 0; i < frame_num; i++)
			{
				for(int byte_pos = 0; byte_pos < FRAME_DLC; byte_pos++)
				{
					// 随机选取 0~255 
					int value = rand()%256;
					new_can.data[byte_pos] = value;
				}
		
				timestamp_start = get_system_us_time();
		
				// 时序缓存
				for(int bit_num = 0; bit_num < FRAME_DLC_BIT; bit_num++)
				{
					int byte_pos = bit_num/8;
					int bit_pos = bit_num%8;
					int bit_value = read_bit_value(&new_can.data[byte_pos], bit_pos);
		
					old_data <<= 1;
					old_data[0] = bit_value;
					old_data_valid <<= 1;
					old_data_valid[0] = VALID;
					
					int valid = old_data_valid[sliding_window_size-1];
					if(valid == VALID)
					{
						old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
						old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
						int data = old_data.to_ulong(); 	 // 存储值转化为整型
						
						if(tm_data[data] == INVALID)
						{
							//printf("%d\n", bit_profile[bit_num].tm_data.test(data));
						}
						else
						{
						
						}
					}
				
					
					if( (bit_num+1) < FRAME_DLC_BIT)
					{
						int next_byte_pos = (bit_num+1)/8;
						int next_bit_pos = (bit_num+1)%8;
						int next_bit_value = read_bit_value(&new_can.data[next_byte_pos], next_bit_pos);
						
						// 异或运算^:参加运算的两个对象，如果两个位为“异”（值不同），则该位结果为1，否则为0。
						int temp_value = adjecent_bit_operation(bit_value, next_bit_value, (BIT_OP)bit_op);	
				
						old_data <<= 1;
						old_data[0] = temp_value;
						old_data_valid <<= 1;
						old_data_valid[0] = VALID;
				
						int valid = old_data_valid[sliding_window_size-1];
						if(valid == VALID)
						{
							old_data <<= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
							old_data >>= (MAX_SLIDING_WINDOW_SIZE - sliding_window_size);
							int data = old_data.to_ulong(); 		// 存储值转化为整型 	
							
						
							if(tm_data[data] == INVALID)
							{
		
							}
							else
							{
							
							}
				
							
						}
				
				
					}
					
					
				}
				
				timestamp_end = get_system_us_time();
		
				timestamp_total += timestamp_end - timestamp_start;
		
		
			}
		
			timestamp_average = (double)timestamp_total/(double)frame_num;
			printf("bit: %d, size: %d, total:%ld, ave: %lf\n", bit_op, sliding_window_size, timestamp_total, timestamp_average);

			// 记录检测结果至临时文件
			fprintf(res_record, "%d, %d, %lf\n", bit_op, sliding_window_size, timestamp_average);
		}

        
    }


    fclose(res_record);
   

    return 0;
	
}


// 测试profile读存缓冲区buffer大小
int test_read_profile()
{
    
	CSimpleIniA configurator;                           // 配置文件

    // 打开模型参数文件
    char configuration_path[100] = "";
	snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "%03d.init", 16);    
	SI_Error rc = configurator.LoadFile(configuration_path);
	if (rc < 0) 
	{
	    LOG_DEBUG(RED "load init file failed !" NONE);
	}
	else
	{
	    LOG_DEBUG(GREEN "load init file OK !" NONE);	
	}
			   
	char keys_name[100] = {0}; 										   
	char values[READ_PROFILE_BUFFER_SIZE] = {0};
	
	int sliding_window_size = 16;


	// 读取二进制检测器的合法值
	memset(keys_name, 0, sizeof(keys_name));
	memset(values, 0, sizeof(values));
	//snprintf(keys_name, sizeof(keys_name), "0x%08x.stream%d.legal_value", 0x, j);
	
	
	int num_len = pow(2, sliding_window_size);
	int  is_first_value = 1;
	for(int m = 0; m < num_len; m++)
	{
		int value = VALID;
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


    printf("%s\n", values);

    FILE * test_file = fopen("test_profile.txt", MODE_W_PLUS);
	fprintf(test_file, "%s\n", values);
	fclose(test_file);
	
    
	memset(values, 0, sizeof(values));
	printf("after_memset:\n %s\n", values);
	
	test_file = fopen("test_profile.txt", MODE_R_PLUS);
	while( fgets(values, sizeof(values), test_file ) != NULL)
	{
	    printf(RED "%s\n" NONE, values);
	}
	printf(YELLOW "strlen: %ld\n" NONE, strlen(values));
    fclose(test_file);

    return 0;
	
}

// 根据攻击类型获取字符串名称
int test_get_attack_name()
{
    char attack_name[ATTACK_NAME_MAX_SIZE] = ""; 
    for(int attack_type = ATTACK_TYPE_RANDOM_INJECTION; attack_type <= ATTACK_TYPE_TARGETED_TAMPER; attack_type++)
    {
        get_attack_name(&attack_name[0], (ATTACK_TYPE)attack_type);

		printf("%d: %s, strlen: %d\n", attack_type, attack_name, (int)strlen(attack_name));
    }

    return 0;
}


// 相邻比特位操作
int bit_op_test()
{
    for(int n = 0; n < 3; n++)
    {
		BYTE a, b, c;
		
		a = rand()%256;
		b = rand()%256;
		
		printf("   a: ");
		for(int i = FRAME_DLC-1; i >= 0; i--)
		{
			printf("%d", read_bit_value(&a, i));
		}
		printf("\n");
		
		printf("   b: ");
		for(int i = FRAME_DLC-1; i >= 0; i--)
		{
			printf("%d", read_bit_value(&b, i));
		}
		printf("\n");
		
		
		for(int bit_op = BIT_OP_XOR; bit_op <= BIT_OP_NOR; bit_op++)
		{
		    printf("\n%d: ", bit_op);
			for(int i = FRAME_DLC-1; i >= 0; i--)
			{
			    BIT_OP op = (BIT_OP)bit_op;
				int temp_a = read_bit_value(&a, i);
				int temp_b = read_bit_value(&b, i);
				int temp_c = adjecent_bit_operation(temp_a, temp_b, op);
				write_bit_value(&c, i, temp_c);
				printf("%d", read_bit_value(&c, i));
			}
			printf("\n");
			
		}
    
    }

    BYTE a = 1;
	printf("   a: ");
	for(int i = 0; i < FRAME_DLC; i++)
	{
		printf("%d", read_bit_value(&a, i));
	}
	printf("\n");	


    unsigned char byte_data = 0b11001101;   // 字节数据
    int bit_pos = 3;                        // 要取出的bit位

    int bit_value = read_bit_value(&byte_data, bit_pos);

	printf("Bit value at position %d in byte 0x%02X is %d\n", bit_pos, byte_data, bit_value);

    return 0;
}


int file_attack_test()
{
    int data_num = 1;
	int attack_num = 1;
	//ATTACK_TYPE attack_type = ATTACK_TYPE_RANDOM_INJECTION;
	ATTACK_TYPE attack_type = ATTACK_TYPE_TARGETED_INJECTION;
	// 获取攻击类型的字符串名称
	char attack_name[ATTACK_NAME_MAX_SIZE] = "";
	get_attack_name(&attack_name[0], attack_type);
	
	

    // 用于生成篡改数据的必要变量
	vector<UINT> common_vec_can_id;							// CAN_ID列表
	vector<ID_Profile> common_id_profile;					// id轮廓
	vector< vector<Byte_Profile> > common_byte_profile;		// 字节轮廓
	CSimpleIniA common_configurator;					    // 配置文件
	
	// 打开模型参数文件
	char configuration_path[100] = "";
	snprintf(configuration_path, sizeof(configuration_path), PROFILE_PATH  "0_001.init");	
	SI_Error rc = common_configurator.LoadFile(configuration_path);
	if (rc < 0) 
	{
		LOG_DEBUG(RED "load init file failed !" NONE);
	}
	else
	{
		LOG_DEBUG(GREEN "load init file OK !" NONE);	
	}
	
	int res = RET_ERROR;
	
	
	// 从配置文件读取ID轮廓
	res = read_id_profile(common_id_profile, common_vec_can_id, common_configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_id_profile failed !" NONE);
		
		return RET_ERROR;
	}
	
	// 从配置文件读取byte轮廓
	res = read_byte_profile(common_byte_profile, common_vec_can_id, common_configurator);
	if (res == RET_ERROR)
	{
		LOG_DEBUG(RED "read_bit_profile failed !" NONE);
		
		return RET_ERROR;
	}

	// 原始数据文件指针
	FILE *raw_data_file = NULL;
	
	char raw_buffer[100] = "";
	snprintf(raw_buffer, sizeof(raw_buffer), TEST_PATH "%03d.csv", data_num);
	printf(RED "inject data: %s\n" NONE, raw_buffer);
	
	raw_data_file = fopen(raw_buffer, MODE_R_PLUS);
	if(raw_data_file == NULL)
	{
		LOG_DEBUG(RED "open file failed!!  %s " NONE, raw_buffer); 
	}

	// 篡改数据指针
	FILE *tamper_file = NULL;				
	char tamper_buffer[100] = "";
	snprintf(tamper_buffer, sizeof(tamper_buffer), ATTACK_PATH "%s%03d%06d.csv", attack_name, data_num, attack_num);
	printf(RED "detect data: %s\n" NONE, tamper_buffer);
	tamper_file = fopen(tamper_buffer, MODE_W_PLUS);
	if(tamper_file == NULL)
	{
		LOG_DEBUG(RED "open file failed!!  %s " NONE, tamper_buffer); 
	}
	
	// 篡改数据标签
	FILE *label_file = NULL;				
	char label_buffer[100] = "";
	snprintf(label_buffer, sizeof(label_buffer), LABEL_PATH "%s%03d%06d.csv", attack_name, data_num, attack_num);
	label_file = fopen(label_buffer, MODE_W_PLUS);
	if(label_file == NULL)
	{
		LOG_DEBUG(RED "open file failed!!  %s " NONE, label_buffer); 
	}

	// 制作/获取篡改数据
	switch (attack_type)
		{
			case ATTACK_TYPE_RANDOM_INJECTION:
			{
				random_injection_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, MAX_INJECTION_STRENGTH);
				break;
			}
			case ATTACK_TYPE_TARGETED_INJECTION:
			{
				targeted_injection_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, common_byte_profile, MAX_INJECTION_STRENGTH);
				break;
			}
			case ATTACK_TYPE_REPLAY:
			{
				replay_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, MAX_INJECTION_STRENGTH);
				break;
			}
			case ATTACK_TYPE_RANDOM_TAMPER:
			{
				random_tamper_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, MAX_TAMPER_NUM);
				break;
			}
			case ATTACK_TYPE_TARGETED_TAMPER:
			{
				targeted_tamper_attack_test(raw_data_file, tamper_file, label_file, common_vec_can_id, common_byte_profile, MAX_TAMPER_NUM);
				break;
			}
			default:
				LOG_DEBUG(RED "attack_type error: %d" NONE, attack_type);
		}




	// 关闭文件
	fclose(tamper_file);
	fclose(label_file);
	
	// 删除临时文件
	if(DELETE_TEMP_FILE)
	{
		remove(tamper_buffer);		   // 篡改后的文件  remove+文件路径
		remove(label_buffer);		 // 数据标签		
	}


    return 0;
}




int main(int argc, char *argv[])
{
	// 初始化随机种子
	srand((unsigned)time(NULL));


    // ---------------------------------------------------------------test function begin-------------------------------------------------------//
    // test_bit_read();	                    // 测试bit位读
    // test_bitset();                       // 测试bitset函数库
	// split_data_to_train_and_test();      // 将原始CAN报文数据按一定比例拆分成训练集和测试集
    // split_data_to_set();                 // 将原始CAN报文数据按固定长度拆分成攻击测试样本
    // test_mark_array();                   // 测试标记函数
	// test_hamming();                      // 计算hamming距离测试
	// test_can_data();                     // 测试数据转换
	// test_constant_select();              // 测试根据异或运算选取连续bit位为0的集合
	// turn_file_from_byte_to_bit();	    // byte文件转换成bit文件
	// statistic_data();                    // 数据集统计
    // LOG_DEBUG("split data complete!");
    // test_read_profile();                 // 测试profile读存缓冲区buffer大小
    // bit_op_test();                       // 相邻比特位操作测试
    // test_get_attack_name();              // 根据攻击类型获取字符串名称
    // file_attack_test();                  // 攻击模拟测试
	// ---------------------------------------------------------------test function end---------------------------------------------------------//

	
	// global_model_train_and_model_detect_attack_free();           // 不同滑动窗口和比特操作下模型的训练以及无功击情况下的模型验证
	// global_model_detect_under_various_attack();                  // 不同位运算和滑动窗口参数的情况下，不同攻击的检测性能（均为最大攻击强度）
	// specific_model_detect_under_various_attack(BIT_OP_XOR, 8);      // 位运算和滑动窗口参数确定的情况下，不同攻击的检测性能（攻击强度变化）
	// specific_model_generalization_test(BIT_OP_XOR, 8);           // 测试-训练数据的丰富度对模型误报的影响
	// test_detection_speed();                                      // 测试检测速度 xor,and,or,nand,nor效率测试


	return 0;
	
}


