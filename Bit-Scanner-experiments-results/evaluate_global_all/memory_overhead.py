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

    #size = [1, 2, 3, 4, 5, 6, 7, 8]
    size = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
    proposed_memory_overhead = []
    sim_proposed_memory_overhead = []

    bit_flow_num = 127
    sim_bit_flow_num = 64
    can_id_num = 48

    # 计算内存开销,确定各变量的最大值
    for i in range(len(size)):
        proposed_memory_overhead.append(math.pow(2, size[i]) * bit_flow_num * can_id_num/8/1024)
        sim_proposed_memory_overhead.append(math.pow(2, size[i]) * sim_bit_flow_num * can_id_num/8/1024)


    x = np.array(size)                          # 将python列表转化为array
    y_proposed_memory_overhead = np.array(proposed_memory_overhead)  # 将python列表转化为array
    y_sim_proposed_memory_overhead = np.array(sim_proposed_memory_overhead)  # 将python列表转化为array

    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    #plt.plot(x, y_memory_cost, linestyle='-', color='r', label="memory_cost")
    plt.xlabel('Sliding Window Size')
    plt.xticks(x, )
    plt.ylabel('Memory Overhead (KB)')
    plt.plot(x, y_proposed_memory_overhead, linestyle='-', color='r', label="Bit Scanner")
    plt.plot(x, y_sim_proposed_memory_overhead, linestyle='-', color='g', label="Simplified Bit Scanner")
    #plt.ylim(-0.005, 1.0)
    #plt.ylim(-0.5, 100.0)
    plt.grid(linestyle='--')  # 添加网格
    plt.legend(loc="best", edgecolor="k")
    #plt.legend(loc="lower right", edgecolor="k")      # upper center lower, left right
    #plt.title("XXXXXX")


    plt.show()



