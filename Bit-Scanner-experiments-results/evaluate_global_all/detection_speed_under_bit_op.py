import numpy as np
import matplotlib.pyplot as plt
import csv
import matplotlib
matplotlib.rcParams.update({'font.size': 15})

if __name__ == "__main__":
    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题
    csv_file = open('../dataset_global_all/detection_speed_under_bit_op.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件
    #next(csv_reader_lines)   # 跳过首行

    sliding_window_size = []
    processing_time = []


    # bitscanner(xor), bitscanner(and), bitscanner(or), bitscanner(nand), bitscanner(nor)
    compare_method_num = 5

    # 这里通过插入"[]"来生成2维list
    for i in range(compare_method_num):
        processing_time.append([])



    #遍历csv数据
    for one_line in csv_reader_lines:
        print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        print(temp_line)   # 打印截取的8字节16进制值

        red_bit_op = np.int_(temp_line[0])

        if red_bit_op == 0:
            sliding_window_size.append(np.float64(temp_line[1]))

        processing_time[red_bit_op].append(np.float64(temp_line[2]))


    #将list数据转化为array
    x_sliding_window_size = np.array(sliding_window_size)  # 将python列表转化为array
    y_processing_time = np.array(processing_time)            # 将python列表转化为array



    enable = np.array([1, 1, 1, 1, 1])
    plot_color = ['r', 'g', 'y', 'k', 'purple', 'orange']
    plot_label = ['Bit Scanner (XOR)', 'Bit Scanner (AND)', 'Bit Scanner (OR)', 'Bit Scanner (NAND)', 'Bit Scanner (NOR)']
    line_style = ['-', '-', '-', '-', '-']


    plt.clf()  # 清空画布
    plt.grid(linestyle='--')  # 添加网格
    for i in range(compare_method_num):
        if(enable[i]>0):
            #plt.plot(x_injection_num, y_accuracy[i]*100, linestyle=line_style[i], color=plot_color[i], label = plot_label[i])
           plt.plot(sliding_window_size, y_processing_time[i], linestyle=line_style[i], color=plot_color[i], label=plot_label[i])
            #plt.plot(x_injection_num, y_precision[i] * 100, linestyle=line_style[i], color=plot_color[i], label=plot_label[i])

    plt.xlabel("Sliding Window Size")
    plt.xticks(sliding_window_size, )
    plt.ylim(-0.5, 105.0)
    plt.ylabel("Detection speed (us)")
    plt.gca().yaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    plt.gca().xaxis.labelpad = 0  # 设置 y 轴标签与 y 轴线之间的距离
    plt.tick_params(axis='y', which='major', pad=0)  # 设置刻度标签与轴线的距离
    plt.tick_params(axis='x', which='major', pad=0)  # 设置刻度标签与轴线的距离
    # plt.legend(frameon="false", loc="best", edgecolor="k")  # 图例
    plt.legend(loc="best", edgecolor="k")  # 图例
    plt.show()




