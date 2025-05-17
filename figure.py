import matplotlib.pyplot as plt

# # 在文件开头添加字体配置

# 新增中文字体配置
plt.rcParams['font.sans-serif'] = ['SimHei']  # 使用黑体显示中文
plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示问题

# # 数据
# labels = ['创业团队', '风险投资', '银行贷款', '员工持股']
# sizes = [31.25, 37.5, 12.5, 18.75]
# colors = ['#ff9999', '#66b3ff', '#99ff99', '#ffcc99']

# # 创建饼图
# plt.figure(figsize=(8, 8))
# plt.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90, shadow=True)

# # 添加标题
# plt.title('公司股权结构', fontsize=16)

# # 显示图表
# plt.axis('equal')
# plt.show()

import matplotlib.pyplot as plt
import numpy as np

# 设置年份
years = np.arange(2025, 2030)
# 设置用户数量（示例数据）
users = [5000, 9650, 18634.5, 35859.585, 68000]
# 设置营业收入（单位：万元）
revenue = [250, 480, 922, 1774, 3400]
# 设置可变成本（单位：万元）
variable_cost = [175, 225, 280, 500, 784.59]
# 设置固定成本比例（示例数据）
fixed_cost_ratio = [0.05, 0.055, 0.06, 0.065, 0.027]

# 创建图表
plt.figure(figsize=(12, 8))
plt.plot(years, revenue, label='营业收入', marker='o', linestyle='-', color='#66b3ff', linewidth=2)
plt.plot(years, variable_cost, label='可变成本', marker='o', linestyle='--', color='#ff9999', linewidth=2)
plt.plot(years, np.array(revenue) * np.array(fixed_cost_ratio), label='固定成本', marker='o', linestyle='-.', color='#99ff99', linewidth=2)

# 添加图表标题和坐标轴标签
plt.title('企业收入与成本预测趋势', fontsize=16)
plt.xlabel('年份', fontsize=12)
plt.ylabel('金额（万元）', fontsize=12)

# 添加图例
plt.legend()

# 添加数据标签
for i, year in enumerate(years):
    plt.text(year, revenue[i], f'{revenue[i]}', ha='center', va='bottom')
    plt.text(year, variable_cost[i], f'{variable_cost[i]}', ha='center', va='top')
    plt.text(year, np.array(revenue[i]) * np.array(fixed_cost_ratio[i]), f'{np.round(np.array(revenue[i]) * np.array(fixed_cost_ratio[i]), 2)}', ha='center', va='bottom')

# 显示图表
plt.grid(linestyle='--', alpha=0.7)
plt.show()