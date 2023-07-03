# 用python绘制工件的图像
import numpy as np
import matplotlib.pyplot as plt
import csv
from matplotlib.pyplot import MultipleLocator   #从pyplot导入MultipleLocator类，这个类用于设置刻度间隔

import matplotlib
matplotlib.rcParams.update({'font.size': 15})

if __name__ == "__main__":
    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题

    csv_file = open('../dataset_under_window_size_8_xor_bit_op/random_injection_attack_strength_change_0_8.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件


    injection_strength = []
    accuracy = []
    precision = []
    recall = []
    delay = []


    count = 0
    compare_method_num = 5

    # 这里通过插入"[]"来生成2维list
    for i in range(compare_method_num):
        accuracy.append([])
        precision.append([])
        recall.append([])
        delay.append([])

    #遍历csv数据
    for one_line in csv_reader_lines:
        count = count + 1
        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        #print(temp_line)   # 打印截取的8字节16进制值
        injection_strength.append(np.float64(temp_line[0]))
        # tp, tn, fp, fn, accuracy, precision, recall, false_alarm_rate, delay
        for i in range(compare_method_num):
            accuracy[i].append(np.float64(temp_line[1 + 4 + i * 9]))
            precision[i].append(np.float64(temp_line[1 + 5 + i * 9]))
            recall[i].append(np.float64(temp_line[1 + 6 + i * 9]))
            delay[i].append(np.float64(temp_line[1 + 8 + i * 9]))

    #将list数据转化为array
    x_injection_strength = np.array(injection_strength)  # 将python列表转化为array
    y_accuracy = np.array(accuracy)            # 将python列表转化为array
    y_precision = np.array(precision)          # 将python列表转化为array
    y_recall = np.array(recall)                # 将python列表转化为array
    y_delay = np.array(delay)                  # 将python列表转化为array

    # 打印读取的数据
    # print(x_injection_num)
    # print(y_accuracy)

    enable = np.array([0, 1, 1, 1, 0])
    plot_color = ['c', 'r', 'g', 'b', 'y']
    plot_label = ['Min max', 'Hamming', 'ID matrix', 'Bit Scanner', 'sim_bit_scanner']
    line_style = ['-', '-', '-', '-', '-']

    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    #plt.plot(x, y0[0], '--1', color='r', label='method0')
    for i in range(compare_method_num):
        if(enable[i]>0):
            #plt.plot(x_injection_num, y_accuracy[i]*100, linestyle=line_style[i], color=plot_color[i], label = plot_label[i])
            plt.plot(x_injection_strength, y_recall[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
            #plt.plot(x_injection_num, y_precision[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])

    plt.xlabel("Attack Strength")
    plt.xticks(x_injection_strength, )
    plt.ylim(-0.5, 105.0)
    plt.ylabel("Detection Rate (%)")
    plt.gca().yaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    plt.gca().xaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    plt.tick_params(axis='y', which='major', pad=0)  # 设置刻度标签与轴线的距离
    plt.tick_params(axis='x', which='major', pad=0)  # 设置刻度标签与轴线的距离
    # plt.legend(frameon="false", loc="best", edgecolor="k")  # 图例
    plt.legend(loc="best", edgecolor="k")  # 图例
    plt.show()




