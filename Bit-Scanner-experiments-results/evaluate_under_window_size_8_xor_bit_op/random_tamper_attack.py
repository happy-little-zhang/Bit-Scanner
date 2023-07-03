import numpy as np
import matplotlib.pyplot as plt
import csv
import matplotlib
matplotlib.rcParams.update({'font.size': 15})

if __name__ == "__main__":
    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题

    csv_file = open('../dataset_global_all/global_random_tamper.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件

    sliding_window_size = []
    accuracy = []
    precision = []
    recall = []
    false_alarm_rate = []
    delay = []

    # min-max, hamming, id-matrix, bitscanner(xor), sim-bitscanner,
    compare_method_num = 5

    # bitscanner( and), bitscanner( or), bitscanner(nand), bitscanner(nor)
    extra_method_num = 4

    # 这里通过插入"[]"来生成2维list
    for i in range(compare_method_num + extra_method_num):
        accuracy.append([])
        precision.append([])
        recall.append([])
        false_alarm_rate.append([])
        delay.append([])

    # 遍历csv数据
    for one_line in csv_reader_lines:
        # print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        print(temp_line)  # 打印截取的8字节16进制值

        red_bit_op = np.int_(temp_line[0])

        if red_bit_op == 0:

            sliding_window_size.append(np.float64(temp_line[1]))

            # tp, tn, fp, fn, accuracy, precision, recall, false_alarm_rate, delay
            for i in range(compare_method_num):
                accuracy[i].append(np.float64(temp_line[i * 9 + 4 + 2]))
                precision[i].append(np.float64(temp_line[i * 9 + 5 + 2]))
                recall[i].append(np.float64(temp_line[i * 9 + 6 + 2]))
                false_alarm_rate[i].append(np.float64(temp_line[i * 9 + 7 + 2]))
                delay[i].append(np.float64(temp_line[i * 9 + 8 + 2]))
        else:
            accuracy[red_bit_op + compare_method_num - 1].append(np.float64(temp_line[3 * 9 + 4 + 2]))
            precision[red_bit_op + compare_method_num - 1].append(np.float64(temp_line[3 * 9 + 5 + 2]))
            recall[red_bit_op + compare_method_num - 1].append(np.float64(temp_line[3 * 9 + 6 + 2]))
            false_alarm_rate[red_bit_op + compare_method_num - 1].append(np.float64(temp_line[3 * 9 + 7 + 2]))
            delay[red_bit_op + compare_method_num - 1].append(np.float64(temp_line[3 * 9 + 8 + 2]))

    # 将list数据转化为array
    x_sliding_window_size = np.array(sliding_window_size)  # 将python列表转化为array
    y_accuracy = np.array(accuracy)  # 将python列表转化为array
    y_precision = np.array(precision)  # 将python列表转化为array
    y_recall = np.array(recall)  # 将python列表转化为array
    y_delay = np.array(delay)  # 将python列表转化为array

    enable = np.array([0, 0, 0, 1, 1, 1, 1, 1, 1])
    plot_color = ['c', 'm', 'b', 'r', 'g', 'y', 'k', 'purple', 'orange']
    plot_label = ['Min max', 'Hamming', 'ID matrix', 'Bit Scanner(XOR)', 'Simplified Bit Scanner', 'Bit Scanner(AND)', 'Bit Scanner(OR)', 'Bit Scanner(NAND)', 'Bit Scanner(NOR)']
    line_style = ['-', '-', '-', '-', '-', '-', '-', '-', '-']



    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    for i in range(compare_method_num + extra_method_num):
        if(enable[i]>0):
            #plt.plot(x_injection_num, y_accuracy[i]*100, linestyle=line_style[i], color=plot_color[i], label = plot_label[i])
           plt.plot(x_sliding_window_size, y_recall[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
            #plt.plot(x_injection_num, y_precision[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])


    plt.xlabel("Sliding Window Size")
    plt.xticks(x_sliding_window_size, )
    plt.ylim(-0.5, 105.0)
    plt.ylabel("Detection Rate (%)")

    # plt.legend(frameon="false", loc="best", edgecolor="k")  # 图例
    plt.legend(loc="best", edgecolor="k")  # 图例
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
           ax1.plot(x_sliding_window_size, y_recall[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
    ax1.grid(linestyle='--')  # 添加网格
    ax1.set_xlabel("Sliding Window Size")
    ax1.set_xticks(x_sliding_window_size, )
    ax1.set_ylim(-0.5, 105.0)
    ax1.set_ylabel("Detection Rate (%)")
    ax1.legend(loc="best", edgecolor="k")  # 图例
    #ax1.set_title('area1')
    #新增区域ax2,嵌套在ax1内，看一看图中图是什么样，这就是与subplot的区别
    left, bottom, width, height = 0.6, 0.3, 0.25, 0.25
    #获得绘制的句柄
    ax2 = fig.add_axes([left, bottom, width, height])
    for i in range(compare_method_num + extra_method_num):
        if(enable[i]>0):
           ax2.plot(x_sliding_window_size, y_recall[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
    #ax2.grid(linestyle='--')  # 添加网格
    #ax2.set_xlabel("Sliding Window Size")
    #ax2.set_xticks(x_injection_num, )
    #ax2.set_xlim(0,16 )
    ax2.set_ylim(99.0, 100)
    #ax2.set_ylabel("Detection Rate (%)")
    #ax2.legend(loc="best", edgecolor="k")  # 图例
    #ax2.set_title('area2')
    plt.show()



    # 数据截取,计算增量
    # min-max, hamming, id-matrix, bitscanner(xor), sim-bitscanner,
    #bitscanner(and), bitscanner(or), bitscanner(nand), bitscanner(nor)
    sim_detection_rate = y_recall[4]
    xor_detection_rate = y_recall[3]
    and_detection_rate = y_recall[5]
    or_detection_rate = y_recall[6]
    nand_detection_rate = y_recall[7]
    nor_detection_rate = y_recall[8]
    xor_increment = xor_detection_rate-sim_detection_rate
    and_increment = and_detection_rate - sim_detection_rate
    or_increment = or_detection_rate - sim_detection_rate
    nand_increment = nand_detection_rate - sim_detection_rate
    nor_increment = nor_detection_rate - sim_detection_rate
    print(xor_increment*100)
    print(and_increment * 100)
    print(or_increment * 100)
    print(nand_increment * 100)
    print(nor_increment * 100)

