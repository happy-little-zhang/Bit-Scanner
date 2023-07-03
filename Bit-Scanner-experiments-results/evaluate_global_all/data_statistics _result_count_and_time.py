# 用python绘制工件的图像
import numpy as np
import matplotlib.pyplot as plt
import csv
from matplotlib.pyplot import MultipleLocator   #从pyplot导入MultipleLocator类，这个类用于设置刻度间隔

import matplotlib
matplotlib.rcParams.update({'font.size': 14})

if __name__ == "__main__":
    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题
    #x, y = statistic_data[:, 0], statistic_data[:, 1]
    csv_file = open('../dataset_global_all/data_statistics _result_count_and_time.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件

    total_count_sum = 0
    total_frame_time = 0
    total_file_num = 0

    file_index = []
    file_frame_num = []
    file_frame_time = []
    count = 0


    for one_line in csv_reader_lines:
        count = count + 1
        # print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        print(temp_line)   # 打印截取的8字节16进制值

        if count <= 2:
            if count == 1:
                total_count_sum = np.float64(temp_line[0])
                total_frame_time = np.float64(temp_line[1])
            else:
                total_file_num = np.float64(temp_line[0])
        else:
            file_index.append(np.float64(temp_line[0]))
            file_frame_num.append(np.float64(temp_line[1]))
            file_frame_time.append(np.float64(temp_line[2]))

    x = np.array(file_index)  # 将python列表转化为array
    y_num = np.array(file_frame_num)  # 将python列表转化为array
    y_time = np.array(file_frame_time)  # 将python列表转化为array

    #x_major_locator = MultipleLocator(1)
    # 把x轴的刻度间隔设置为1，并存在变量里
    #y_major_locator = MultipleLocator(50)
    # 把y轴的刻度间隔设置为10，并存在变量里
    # ax = plt.gca()
    # ax为两条坐标轴的实例
    # ax.xaxis.set_major_locator(x_major_locator)
    # 把x轴的主刻度设置为1的倍数
    #ax.yaxis.set_major_locator(y_major_locator)
    # 把y轴的主刻度设置为10的倍数

    #plt.xlim(-0.1, 48.5)
    # plt.xticks(x,)
    # 把x轴的刻度范围设置为-0.5到11，因为0.5不满一个刻度间隔，所以数字不会显示出来，但是能看到一点空白
    #plt.ylim(-5, 260)
    # 把y轴的刻度范围设置为-5到110，同理，-5不会标出来，但是能看到一点空白



    #for i in range(len(y)):
    #    y[i] = y[i]/total_count_sum

    print(y_time)
    for i in range(len(y_num)):
        y_num[i] = y_num[i]/1000
        y_time[i] = y_time[i] / 60


    plt.grid()           #添加网格
    plt.xlabel('File Index')
    plt.ylabel('Traffic time (Minutes)')
    plt.bar(x, y_time, label='Frame count', color='b')
    plt.grid(linestyle='--')  # 添加网格
    #plt.legend()   # 用来标示不同图形的文本标签图例
    #plt.legend( loc = "best", edgecolor = "black")  # 用来标示不同图形的文本标签图例
    plt.show()


    # 默认情况下，Matplotlib中的图形大小是 6.4英寸 x 4.8英寸
    # 默认情况下，条形的宽度是0.8
    bar_width = 0.3
    #fig, ax1 = plt.subplots()
    #fig, ax1 = plt.subplots(figsize=(12.8, 4.8))  # 两倍宽度，高度不变
    fig, ax1 = plt.subplots(figsize=(12.8, 4.8))  # 两倍宽度，高度不变

    # ax1.bar(x, y_num, bar_width, label='Frame count', color='b')
    #ax1.bar(x, y_num, label='Frame count', color='b')
    # ax1.set_ylabel('Frame count (Thousand)')

    #ax1.bar(x, y_time, label='Frame count', color='b')
    ax1.bar(x, y_time, label='Frame count', color='b')
    ax1.set_ylabel('Traffic time (Minutes)')
    ax1.set_xlabel('File Index')

    ax1.tick_params(axis='y', labelcolor='b')
    ax1.grid(linestyle='--')  # 添加网格
    #ax1.set_ylim([0, 3000])  # 设置第二个坐标轴的范围
    #ax1.set_ylim([0, 3000])  # 设置第二个坐标轴的范围

    #ax2 = ax1.twinx()
    #ax2.bar(x + bar_width, y_time, bar_width, label='Frame time', color='r')
    #ax2.bar(x, y_time, label='Frame time', color='b')
    #ax2.set_ylabel('Frame time (Minutes)')
    #ax2.tick_params(axis='y', labelcolor='b')
    #ax2.grid(linestyle='--')  # 添加网格
    #ax2.set_ylim([0, 35])  # 设置第二个坐标轴的范围

    #plt.legend()
    plt.xticks(x)
    plt.show()