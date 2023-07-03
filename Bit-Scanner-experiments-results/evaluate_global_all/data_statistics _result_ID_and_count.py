import numpy as np
import matplotlib.pyplot as plt
import csv
# import matplotlib
# matplotlib.rcParams.update({'font.size': 15})

if __name__ == "__main__":
    #statistic_data = np.loadtxt(open('dataset/data_static_result.csv',"rb"),delimiter=",",skiprows=2)   #skiprows 跳过前n行
    # np.argsort(statistic_data, axis=0)#排序
    # a,b = statistic_data.shape    #a为行数 b为数据维度
    # print(a,b)

    # plt.rcParams['font.sans-serif'] = ['STSong']  # 解决中文显示问题
    # plt.rcParams['axes.unicode_minus'] = False  # 解决中文显示问题
    #x, y = statistic_data[:, 0], statistic_data[:, 1]
    csv_file = open('../dataset_global_all/data_statistics _result_ID_and_count.csv')  # 打开文件
    #csv_file = open('dataset/data_static_result.csv')  # 打开文件
    csv_reader_lines = csv.reader(csv_file)  # 用csv.reader读文件
    next(csv_reader_lines)   # 跳过

    num = []
    id = []
    count_num = []
    count = 0
    total_count_sum = 0

    for one_line in csv_reader_lines:
        count = count + 1

        #print(one_line)    # 打印读取一行的内容
        temp_line = one_line[:]
        #print(temp_line)   # 打印截取的8字节16进制值
        num.append(np.float64(count))
        id.append(np.str_(temp_line[0]))
        count_num.append(np.float64(temp_line[1]))

        total_count_sum = total_count_sum + np.float64(temp_line[1])

    # 将id和count_num打包成元素为元组的列表
    id_count_pairs = list(zip(id, count_num))

    # 对列表进行排序，按照count_num值的降序排列
    sorted_pairs = sorted(id_count_pairs, key=lambda x: x[1], reverse=True)

    # 解包排序后的列表为新的id和count_num列表
    new_id, new_count_num = zip(*sorted_pairs)

    # 打印结果
    for i in range(len(new_count_num)):
        print(new_id[i],new_count_num[i])

    x = np.array(num)  # 将python列表转化为array
    y = np.array(new_count_num)  # 将python列表转化为array

    #x_major_locator = MultipleLocator(1)
    # 把x轴的刻度间隔设置为1，并存在变量里
    #y_major_locator = MultipleLocator(50)
    # 把y轴的刻度间隔设置为10，并存在变量里
    ax = plt.gca()
    # ax为两条坐标轴的实例
    # ax.xaxis.set_major_locator(x_major_locator)
    # 把x轴的主刻度设置为1的倍数
    #ax.yaxis.set_major_locator(y_major_locator)
    # 把y轴的主刻度设置为10的倍数

    plt.xlim(-0.1, 48.5)
    #plt.xticks(x,)
    # 把x轴的刻度范围设置为-0.5到11，因为0.5不满一个刻度间隔，所以数字不会显示出来，但是能看到一点空白
    #plt.ylim(-5, 260)
    # 把y轴的刻度范围设置为-5到110，同理，-5不会标出来，但是能看到一点空白



    #for i in range(len(y)):
    #    y[i] = y[i]/total_count_sum

    for i in range(len(y)):
        y[i] = y[i]/1000

    #plt.bar(x, y, width=0.4, color='b', label=None)
    plt.bar(x, y, color='b', label=None)
    #yy = y.sort()
    #plt.bar(x, yy, color='b', label=None)

    #plt.grid()           #添加网格
    #plt.legend()  # 显示图例 plt.legend(fontsize=6)
    #plt.title("XXXXXX")
    plt.xlabel('ID Index')
    #plt.ylabel('Quantity Proportion (%)')
    plt.ylabel('Frame Quantity (Thousand)')
    # plt.legend(loc="best")  # 图例
    # plt.tick_params(axis='both',which='major',labelsize=1)
    # plt.savefig('res.fng')
    plt.grid(linestyle='--')  # 添加网格
    #plt.legend()   # 用来标示不同图形的文本标签图例
    #plt.legend( loc = "best", edgecolor = "black")  # 用来标示不同图形的文本标签图例

    plt.show()


