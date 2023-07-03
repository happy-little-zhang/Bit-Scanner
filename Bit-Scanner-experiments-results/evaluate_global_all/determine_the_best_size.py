# 用python绘制工件的图像
import numpy as np
import matplotlib.pyplot as plt
import csv
import math
from matplotlib.pyplot import MultipleLocator   #从pyplot导入MultipleLocator类，这个类用于设置刻度间隔

if __name__ == "__main__":
    #statistic_data = np.loadtxt(open('dataset/data_static_result.csv',"rb"),delimiter=",",skiprows=2)   #skiprows 跳过前n行
    # np.argsort(statistic_data, axis=0)#排序
    # a,b = statistic_data.shape    #a为行数 b为数据维度
    # print(a,b)

    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题
    #x, y = statistic_data[:, 0], statistic_data[:, 1]
    csv_file = open('dataset_new/occupation_record.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件

    size = []
    total_stream_num = []
    valid_stream_num = []
    invalid_stream_num = []
    full_occupy_stream_num = []
    not_full_occupy_stream_num = []
    constant_stream_num = []
    not_constant_stream_num = []
    no_anomaly_num = []



    for one_line in csv_reader_lines:
        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        #print(temp_line)   # 打印截取的8字节16进制值
        size.append(np.float64(temp_line[0]))
        total_stream_num.append(np.float64(temp_line[1]))
        valid_stream_num.append(np.float64(temp_line[2]))
        invalid_stream_num.append(np.float64(temp_line[3]))
        full_occupy_stream_num.append(np.float64(temp_line[4]))
        not_full_occupy_stream_num.append(np.float64(temp_line[5]))
        constant_stream_num.append(np.float64(temp_line[6]))
        no_anomaly_num.append(np.float64(temp_line[7]))


    memory_cost = np.copy(size)
    effect_rate = np.copy(size)

    #正则化时使用的临时变量
    max_memory_cost = 0
    max_effect_rate = 0
    # 计算内存开销,确定各变量的最大值
    for i in range(len(size)):
        memory_cost[i] = math.pow(2, size[i])
        if((total_stream_num[i] - constant_stream_num[i]) > 0):
            effect_rate[i] = (no_anomaly_num[i] - constant_stream_num[i])/ total_stream_num[i]
        #    effect_rate[i] = (not_full_occupy_stream_num[i] - no_anomaly_num[i]) / (total_stream_num[i] )
        else:
            effect_rate[i] = 0
        if(max_memory_cost < memory_cost[i]):
            max_memory_cost = memory_cost[i]
        if(max_effect_rate < effect_rate[i]):
            max_effect_rate = effect_rate[i]


    # 各特征正则化
    for i in range(len(size)):
        memory_cost[i] = memory_cost[i] / max_memory_cost
        #effect_rate[i] = effect_rate[i] / max_effect_rate



    x = np.array(size)                          # 将python列表转化为array
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


    #plt.bar(x, y, width=0.4, color='b', label=None)
    bar_width = 0.2
    plt.bar(x - 3*(bar_width/2), y_invalid + y_valid, width=bar_width, color='r', label="Invalid_detector")
    plt.bar(x - 3*(bar_width/2), y_valid, width=bar_width, color='g', label="Valid_detector")
    plt.bar(x - bar_width/2, y_full + y_not_full, width=bar_width, color='b', label="Full_detector")
    plt.bar(x - bar_width/2, y_not_full, width=bar_width, color='y', label="Not_full_detector")
    plt.bar(x + (bar_width/2), y_constant, color='k', width=bar_width, label="Only_one_occupy")
    plt.bar(x + 3*(bar_width/2), y_no_anomaly, color='pink', width=bar_width, label="No_misreport_detector")

    #plt.grid()           #添加网格
    #plt.legend(loc="best", edgecolor="k")
    plt.legend(loc="lower right", edgecolor="k")      # upper center lower, left right
    #plt.title("XXXXXX")
    plt.xlabel('Parameter K')
    plt.ylabel('Proportion [%]')
    # plt.legend(loc="best")  # 图例
    # plt.tick_params(axis='both',which='major',labelsize=1)
    # plt.savefig('res.fng')
    plt.grid(linestyle='--')  # 添加网格
    #plt.legend()   # 用来标示不同图形的文本标签图例
    #plt.legend( loc = "best", edgecolor = "black")  # 用来标示不同图形的文本标签图例
    plt.show()


    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    plt.plot(x, y_valid, linestyle='-', color='r', label="y_valid")
    plt.plot(x, y_invalid, linestyle='-', color='g', label="y_invalid")
    plt.plot(x, y_full, linestyle='-', color='b', label="y_full")
    plt.plot(x, y_not_full, linestyle='-', color='y', label="y_not_full")
    plt.plot(x, y_constant, linestyle='-', color='k', label="y_constant")
    plt.plot(x, y_no_anomaly, linestyle='-', color='pink', label="y_no_anomaly")
    plt.xlabel('Parameter K')
    plt.ylabel('Proportion [%]')
    plt.legend(loc="best")  # 图例
    # plt.tick_params(axis='both',which='major',labelsize=1)
    # plt.savefig('res.fng')
    plt.grid(linestyle='--')  # 添加网格
    plt.show()


    y_memory_cost = np.array(memory_cost)    # 将python列表转化为array
    y_effect_rate = np.array(effect_rate)  # 将python列表转化为array
    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    #plt.plot(x, y_memory_cost, linestyle='-', color='r', label="memory_cost")
    plt.plot(x, y_effect_rate, linestyle='-', color='b', label="effect_rate")
    plt.xlabel("Parameter K")
    plt.ylim(-0.005, 1.0)
    #plt.ylim(-0.5, 100.0)
    plt.ylabel("memory_cost, effect_rate")
    plt.grid(linestyle='--')  # 添加网格
    plt.legend(loc="best", edgecolor="k")
    #plt.legend(loc="lower right", edgecolor="k")      # upper center lower, left right
    #plt.title("XXXXXX")


    plt.show()



