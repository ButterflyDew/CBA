import matplotlib.pyplot as plt
import numpy as np

# 读取文件并提取数据
with open('data.txt', 'r') as file:
    lines = file.readlines()

data = []
for i in range(0, len(lines), 2):
    title = lines[i].strip().split()[0]
    values1 = list(map(float, lines[i].strip().split()[2:]))
    values2 = list(map(float, lines[i + 1].strip().split()[2:]))
    print(values1, values2)
    values1[1], values1[2], values1[3], values1[4] = values1[4], values1[1], values1[2], values1[3]
    values2[1], values2[2], values2[3], values2[4] = values2[4], values2[1], values2[2], values2[3]
    data.append((title, values1, values2))

# 横轴标签
categories = ['UW', 'IW']
# 绘制柱状图
fig, axs = plt.subplots(2, 4, figsize=(16, 8))

colo = ['skyblue', 'lightcoral']

styles = [
'',
'//',    
'||',        
'--',   
'\\\\'
]

labelname = ["all","PLL", "D=2", "D=4", "D=6"]

for i, (title, values1, values2) in enumerate(data):
    row = i // 4
    col = i % 4
    flg = row == 0 and col == 0
    x1 = np.arange(len(values1)) + 0.5
    x2 = np.arange(len(values1)) + len(values1) + 1.5  # 调整后5个柱子的x轴坐标
    cur = 0
    #print(x1, x2)
    #print(row, col)
    for v1 in values1:
        #print(v1)
        axs[row, col].bar(x1[cur], v1, color='skyblue', width=1, label=labelname[cur] if flg else "", edgecolor='black', hatch = styles[cur])
        cur += 1

    cur = 0
    for v2 in values2:
        axs[row, col].bar(x2[cur], v2, color='skyblue', width=1, edgecolor='black', hatch = styles[cur])
        cur += 1
    axs[row, col].set_title(title)

    axs[row, col].set_xticks([2.5,8.5])  # 设置标签的位置
    axs[row, col].set_xticklabels(categories)  # 设置x轴标签
    #axs[row, col].set_xlabel('Category')
    axs[row, col].set_ylabel('Size')
    axs[row, col].legend()


#plt.legend(bbox_to_anchor=(1.3, 1.3), loc='upper center')
# 调整布局
plt.tight_layout()
plt.show()