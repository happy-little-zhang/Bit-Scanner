import numpy as np
import matplotlib.pyplot as plt
import csv
import matplotlib
matplotlib.rcParams.update({'font.size': 15})

if __name__ == "__main__":
    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题
    #csv_file = open('../dataset_global_all/global_targeted_injection.csv')  # 打开文件
    csv_file = open('../dataset_global_all_1/global_targeted_injection.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件
    #next(csv_reader_lines)   # 跳过首行

    sliding_window_size = []
    accuracy = []
    precision = []
    recall = []
    false_alarm_rate = []
    delay = []

    # min-max, hamming, id-matrix, bitscanner(xor), sim-bitscanner,
    compare_method_num = 5

    #bitscanner( and), bitscanner( or), bitscanner(nand), bitscanner(nor)
    extra_method_num = 4

    # 这里通过插入"[]"来生成2维list
    for i in range(compare_method_num + extra_method_num):
        accuracy.append([])
        precision.append([])
        recall.append([])
        false_alarm_rate.append([])
        delay.append([])


    #遍历csv数据
    for one_line in csv_reader_lines:
        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        # print(temp_line)   # 打印截取的8字节16进制值

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


    #将list数据转化为array
    x_sliding_window_size = np.array(sliding_window_size)  # 将python列表转化为array
    y_accuracy = np.array(accuracy)            # 将python列表转化为array
    y_precision = np.array(precision)          # 将python列表转化为array
    y_recall = np.array(recall)                # 将python列表转化为array
    y_delay = np.array(delay)                  # 将python列表转化为array


    enable = np.array([0, 0, 0, 1, 1, 0, 0, 0, 0])
    plot_color = ['c', 'm', 'b', 'r', 'g', 'y', 'k', 'purple', 'orange']
    plot_label = ['Min max', 'Hamming', 'ID matrix', 'Bit Scanner', 'Simplified Bit Scanner', 'Bit Scanner(AND)', 'Bit Scanner(OR)', 'Bit Scanner(NAND)', 'Bit Scanner(NOR)']
    line_style = ['-', '-', '-', '-', '-', '-', '-', '-', '-']


    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    for i in range(compare_method_num + extra_method_num):
        if(enable[i]>0):
            #plt.plot(x_injection_num, y_accuracy[i]*100, linestyle=line_style[i], color=plot_color[i], label = plot_label[i])
           plt.plot(sliding_window_size, y_recall[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
            #plt.plot(x_injection_num, y_precision[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])

    plt.xlabel("Sliding Window Size")
    plt.xticks(sliding_window_size, )
    plt.ylim(-0.5, 105.0)
    plt.ylabel("Detection Rate (%)")
    plt.gca().yaxis.labelpad = -4  # 设置 y 轴标签与 y 轴线之间的距离
    plt.gca().xaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    plt.tick_params(axis='y', which='major', pad=0)  # 设置刻度标签与轴线的距离
    plt.tick_params(axis='x', which='major', pad=0)  # 设置刻度标签与轴线的距离
    # plt.legend(frameon="false", loc="best", edgecolor="k")  # 图例
    plt.legend(loc="best", edgecolor="k")  # 图例
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
    print(np.round(xor_increment*100, 1))
    print(np.round(and_increment * 100, 1))
    print(np.round(or_increment * 100, 1))
    print(np.round(nand_increment * 100, 1))
    print(np.round(nor_increment * 100, 1))

