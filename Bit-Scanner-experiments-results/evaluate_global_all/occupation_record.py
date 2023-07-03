import numpy as np
import matplotlib.pyplot as plt
import csv


if __name__ == "__main__":
    #skiprows跳过前n行
    #statistic_data = np.loadtxt(open('dataset/data_static_result.csv',"rb"),delimiter=",",skiprows=2)
    #np.argsort(statistic_data, axis=0)#排序
    #a,b = statistic_data.shape    #a为行数 b为数据维度
    #print(a,b)

    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题

    csv_file = open('../dataset_global_all/occupation_record.csv')  # 打开文件
    #csv_file = open('dataset_new/occupation_record.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件

    sliding_window_size = []
    total_stream_num = []
    valid_stream_num = []
    invalid_stream_num = []
    full_occupy_stream_num = []
    not_full_occupy_stream_num = []
    constant_stream_num = []
    not_constant_stream_num = []
    no_anomaly_num = []

    draw_bit_op = 0


    for one_line in csv_reader_lines:
        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        #print(temp_line)   # 打印截取的8字节16进制值

        red_bit_op = np.float64(temp_line[0])

        if red_bit_op == draw_bit_op:
            sliding_window_size.append(np.float64(temp_line[1]))
            total_stream_num.append(np.float64(temp_line[2]))
            valid_stream_num.append(np.float64(temp_line[3]))
            invalid_stream_num.append(np.float64(temp_line[4]))
            full_occupy_stream_num.append(np.float64(temp_line[5]))
            not_full_occupy_stream_num.append(np.float64(temp_line[6]))
            constant_stream_num.append(np.float64(temp_line[7]))
            no_anomaly_num.append(np.float64(temp_line[8]))
        else:
            continue

    x = np.array(sliding_window_size)                          # 将python列表转化为array
    y_total = np.array(total_stream_num)  # 将python列表转化为array
    y_valid = np.array(valid_stream_num)  # 将python列表转化为array
    y_invalid = np.array(invalid_stream_num)    # 将python列表转化为array
    y_full = np.array(full_occupy_stream_num)  # 将python列表转化为array
    y_not_full = np.array(not_full_occupy_stream_num)  # 将python列表转化为array
    y_constant = np.array(constant_stream_num)  # 将python列表转化为array
    y_no_anomaly = np.array(no_anomaly_num)     # 将python列表转化为array


    for i in range(len(y_invalid)):
        y_valid[i] = y_valid[i] / total_stream_num[i] * 100
        y_invalid[i] = y_invalid[i] /total_stream_num[i] * 100
        y_full[i] = y_full[i] / total_stream_num[i] * 100
        y_not_full[i] = y_not_full[i] /total_stream_num[i] * 100
        y_constant[i] = y_constant[i] / total_stream_num[i] * 100
        y_no_anomaly[i] = y_no_anomaly[i] / total_stream_num[i] * 100


    bar_width = 0.3
    plt.bar(x - 2*(bar_width/2), y_full + y_not_full, width=bar_width, color='r', label="Full_detector")
    plt.bar(x - 2*(bar_width/2), y_not_full, width=bar_width, color='g', label="Not_full_detector")
    plt.bar(x, y_invalid + y_valid, width=bar_width, color='pink', label="Inactive_detector")
    plt.bar(x, y_valid, width=bar_width, color='b', label="Active_detector")
    #plt.bar(x + (bar_width/2), y_constant, color='m', width=bar_width, label="Only_one_occupy")
    plt.bar(x + 2*(bar_width/2), y_no_anomaly, color='yellow', width=bar_width, label="No_alarm_detector")

    #plt.grid()           #添加网格
    #plt.legend(loc="best", edgecolor="k")
    plt.legend(loc="lower right", edgecolor="k")      # upper center lower, left right
    #plt.title("XXXXXX")
    plt.xlabel('Sliding Window Size')
    plt.xticks(x, )
    plt.yticks([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])
    plt.ylabel('Proportion [%]')
    # plt.legend(loc="best")  # 图例
    # plt.tick_params(axis='both',which='major',labelsize=1)
    # plt.savefig('res.fng')
    plt.grid(linestyle='--')  # 添加网格
    #plt.legend()   # 用来标示不同图形的文本标签图例
    #plt.legend( loc = "best", edgecolor = "black")  # 用来标示不同图形的文本标签图例
    plt.show()




