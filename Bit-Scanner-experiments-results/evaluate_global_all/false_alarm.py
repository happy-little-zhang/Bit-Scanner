import numpy as np
import matplotlib.pyplot as plt
import csv
import matplotlib
matplotlib.rcParams.update({'font.size': 15})

if __name__ == "__main__":
    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题

    csv_file = open('../dataset_global_all/alarm_record.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件

    sliding_window_size = []
    bit_scanner_false_alarm_rate = []
    sim_bit_scanner_false_alarm_rate = []

    false_alarm_rate = []

    # min-max, hamming, id-matrix, bitscanner(xor), sim-bitscanner,
    compare_method_num = 5

    #bitscanner( and), bitscanner( or), bitscanner(nand), bitscanner(nor)
    extra_method_num = 4

    # 这里通过插入"[]"来生成2维list
    for i in range(compare_method_num + extra_method_num):
        false_alarm_rate.append([])

    for one_line in csv_reader_lines:
        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        print(temp_line)   # 打印截取的8字节16进制值

        red_bit_op = np.int_(temp_line[0])

        if red_bit_op == 0:
            sliding_window_size.append(np.float64(temp_line[1]))

            for i in range(compare_method_num):
                false_alarm_rate[i].append(np.float64(temp_line[4 + i*2]))
        else:
            false_alarm_rate[red_bit_op + compare_method_num - 1].append(np.float64(temp_line[4 + 2 * 4]))

    x_sliding_window_size = np.array(sliding_window_size)        # 将python列表转化为array
    false_alarm_rate = np.array(false_alarm_rate)  # 将python列表转化为array

    print(false_alarm_rate)


    enable = np.array([0, 0, 0, 1, 1, 0, 0, 0, 0])
    plot_color = ['c', 'm', 'b', 'r', 'g', 'y', 'k', 'purple', 'orange']
    plot_label = ['Min max', 'Hamming', 'ID matrix', 'Bit Scanner', 'Simplified Bit Scanner', 'Bit Scanner(AND)', 'Bit Scanner(OR)', 'Bit Scanner(NAND)', 'Bit Scanner(NOR)']
    line_style = ['-', '-', '-', '-', '-', '-', '-', '-', '-']


    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    for i in range(compare_method_num + extra_method_num):
        if(enable[i]>0):
            plt.plot(x_sliding_window_size, false_alarm_rate[i]*100, linestyle=line_style[i], color=plot_color[i], label = plot_label[i])

    plt.xlabel("Sliding Window Size")
    plt.xticks(x_sliding_window_size, )
    #plt.ylim(-0.05, 20)
    plt.ylabel("False Alarm Rate (%)")
    plt.ticklabel_format(style='sci', axis='y', scilimits=(0, 0))  # 使用科学计数法显示 y 轴标签
    plt.tick_params(axis='y', which='major', pad=-2)  # 设置刻度标签与轴线的距离
    plt.tick_params(axis='x', which='major', pad=0)  # 设置刻度标签与轴线的距离
    plt.legend(frameon="false", loc="best", edgecolor="k")  # 图例
    # plt.legend(loc="best", edgecolor="k")  # 图例
    plt.show()



    # 图中图
    fig = plt.figure()
    #新建区域ax1
    #figure的百分比,从figure 10%的位置开始绘制, 宽高是figure的80%
    left, bottom, width, height = 0.1, 0.1, 0.8, 0.8
    #获得绘制的句柄
    ax1 = fig.add_axes([left, bottom, width, height])
    for i in range(compare_method_num + extra_method_num):
        if(enable[i]>0):
           ax1.plot(x_sliding_window_size, false_alarm_rate[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
    ax1.grid(linestyle='--')  # 添加网格
    ax1.set_xlabel("Sliding Window Size")
    ax1.set_xticks(x_sliding_window_size, )
    #ax1.set_ylim(-0.5, 105.0)
    ax1.set_ylabel("Detection Rate (%)")
    ax1.legend(loc="best", edgecolor="k")  # 图例
    #ax1.set_title('area1')
    #新增区域ax2,嵌套在ax1内，看一看图中图是什么样，这就是与subplot的区别
    left, bottom, width, height = 0.6, 0.3, 0.25, 0.25
    #获得绘制的句柄
    ax2 = fig.add_axes([left, bottom, width, height])
    for i in range(compare_method_num + extra_method_num):
        if(enable[i]>0):
           ax2.plot(x_sliding_window_size, false_alarm_rate[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
    #ax2.grid(linestyle='--')  # 添加网格
    #ax2.set_xlabel("Sliding Window Size")
    #ax2.set_xticks(x_injection_num, )
    ax2.set_xlim(0,8 )
    #ax2.set_ylim(99.0, 100)
    #ax2.set_ylabel("Detection Rate (%)")
    #ax2.legend(loc="best", edgecolor="k")  # 图例
    #ax2.set_title('area2')
    plt.show()
