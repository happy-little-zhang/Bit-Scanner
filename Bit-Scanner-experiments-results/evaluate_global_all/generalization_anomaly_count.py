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
    # csv_file = open('../dataset_global_all/generalization_anomaly_count.csv')  # 打开文件
    csv_file = open('../dataset_global_all_1/generalization_anomaly_count.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件


    count = 0
    max_file_num = []
    anomaly_count_total = []
    anomaly_count = []
    false_alarm_rate = []
    compare_method_num = 5

    anomaly_count_total_with_same = []
    anomaly_count_with_same = []
    false_alarm_rate_with_same = []

    # 这里通过插入"[]"来生成2维list
    for i in range(compare_method_num):
        anomaly_count.append([])
        false_alarm_rate.append([])

        anomaly_count_with_same.append([])
        false_alarm_rate_with_same.append([])

    #遍历csv数据
    for one_line in csv_reader_lines:
        count = count + 1
        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        #print(temp_line)   # 打印截取的8字节16进制值

        max_file_num.append(np.float64(temp_line[0]))
        anomaly_count_total.append(np.float64(temp_line[1]))
        anomaly_count_total_with_same.append(np.float64(temp_line[2]))

        # tp, tn, fp, fn, accuracy, precision, recall, false_alarm_rate, delay
        for i in range(compare_method_num):

            anomaly_count[i].append(np.float64(temp_line[i*2 + 4]))
            false_alarm_rate[i].append(np.float64(temp_line[i*2 + 4])/np.float64(temp_line[1]))

            anomaly_count_with_same[i].append(np.float64(temp_line[i*2 + 3]))
            false_alarm_rate_with_same[i].append(np.float64(temp_line[i*2 + 3])/np.float64(temp_line[2]))

    #将list数据转化为array
    x_max_file_num = np.array(max_file_num)  # 将python列表转化为array
    y_anomaly_count = np.array(anomaly_count)            # 将python列表转化为array
    y_false_alarm_rate = np.array(false_alarm_rate)          # 将python列表转化为array

    y_anomaly_count_with_same = np.array(anomaly_count_with_same)            # 将python列表转化为array
    y_false_alarm_rate_with_same = np.array(false_alarm_rate_with_same)          # 将python列表转化为array

    print(y_anomaly_count)
    print(y_false_alarm_rate)
    print(y_anomaly_count_with_same)
    print(y_false_alarm_rate_with_same)

    enable = np.array([0, 0, 0, 1, 0])
    plot_color = ['c', 'm', 'b', 'r', 'g']
    plot_label = ['Min max', 'Hamming', 'ID matrix', 'Bit Scanner', 'Simplified Bit Scanner']
    line_style = ['-', '-', '-', '-', '-']

    plot_color2 = ['c', 'm', 'b', 'g', 'r']
    plot_label2 = ['Min max', 'Hamming', 'ID matrix', 'Bit Scanner', 'Simplified Bit Scanner']


    #plt.clf()  # 清空画布
    #plt.grid(linestyle='--')  # 添加网格
    # 图中图
    #fig = plt.figure(figsize=(12.8, 4.8))
    fig = plt.figure()
    #fig, ax1 = plt.subplots(figsize=(12.8, 4.8))  # 两倍宽度，高度不变
    #新建区域ax1
    #figure的百分比,从figure 10%的位置开始绘制, 宽高是figure的80%
    left, bottom, width, height = 0.1, 0.1, 0.8, 0.8
    #获得绘制的句柄
    ax1 = fig.add_axes([left, bottom, width, height])
    for i in range(compare_method_num):
        if(enable[i]>0):
            ax1.plot(x_max_file_num, y_false_alarm_rate[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
            ax1.plot(x_max_file_num, y_false_alarm_rate_with_same[i] * 100, linestyle=line_style[i], color=plot_color2[i], label=plot_label2[i])

    ax1.grid(linestyle='--')  # 添加网格
    ax1.set_xlabel("Max training file num")
    #ax1.set_xticks(x_max_file_num, )
    # ax1.set_ylim(-0.5, 105.0)
    ax1.set_ylabel("False Alarm Rate (%)")
    ax1.yaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    ax1.xaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    ax1.tick_params(axis='y', which='major', pad=-2)  # 设置刻度标签与轴线的距离
    ax1.tick_params(axis='x', which='major', pad=0)  # 设置刻度标签与轴线的距离
    #ax1.legend(loc="best", edgecolor="k")  # 图例
    #ax1.set_title('area1')
    #新增区域ax2,嵌套在ax1内，看一看图中图是什么样，这就是与subplot的区别
    left, bottom, width, height = 0.6, 0.3, 0.25, 0.25
    #获得绘制的句柄
    ax2 = fig.add_axes([left, bottom, width, height])
    for i in range(compare_method_num):
        if(enable[i]>0):
           ax2.plot(x_max_file_num, y_false_alarm_rate[i] * 100, linestyle='--', color=plot_color[i], label=plot_label[i])
           ax2.plot(x_max_file_num, y_false_alarm_rate_with_same[i] * 100, linestyle='--', color=plot_color2[i], label=plot_label2[i])
    #ax2.grid(linestyle='--')  # 添加网格
    #ax2.set_xlabel("Sliding Window Size")
    #ax2.set_xticks(x_injection_num, )
    ax2.set_ylim(-0.001, 0.2)
    ax2.set_xlim(25, 35)
    #ax2.set_ylim(99.7, 100.05)
    #ax2.set_ylabel("Detection Rate (%)")
    #ax2.legend(loc="best", edgecolor="k")  # 图例
    #ax2.set_title('area2')
    plt.show()



