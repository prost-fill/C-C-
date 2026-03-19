import math
import numpy as np
from prettytable import PrettyTable 
import matplotlib.pyplot as plt


def Renumber():
    global x_ren, y_ren, x_point, x_help
    x_ren = []
    y_ren = []
    x_help = []
    x_point = 1.9

    for i in range(len(x)):
        x_help.append(math.fabs(x[i] - x_point))

    for i in range(len(x)):
        min_index = x_help.index(min(x_help))
        x_ren.append(x[min_index])
        y_ren.append(y[min_index]) 
        x_help[min_index] = 100

    print("\nxⁱ=1.9. Упорядоченная таблица")
    print("x(k):[", end="")
    for i in range(len(x_ren)):
        print('{:.2f}'.format(x_ren[i]), end="")
        if i != len(x_ren)-1:
            print(', ', end="")
    print(']')
    print("y(k):[", end="")
    for i in range(len(y_ren)):
        print('{:.4f}'.format(y_ren[i]), end="")
        if i != len(y_ren)-1:
            print(', ', end="")
    print(']')

def Aitkens_scheme():
    l1 = []
    l2 = []
    l3 = []
    l4 = []

    l_help = y_ren.copy()
    for j in range(1, 5):
        matrix_help = np.matrix(
            [[l_help[j-1], (x_ren[j-1] - x_point)], [l_help[j], (x_ren[j] - x_point)]])
        l_help[j-1] = np.around(
            (np.linalg.det(matrix_help)/(x_ren[j] - x_ren[j-1])), 4)
        l1.append(np.around((l_help[j-1]), 4))
    
    l_help = l1.copy()
    for i in range(1):
        l2.append(' ')
    for j in range(1, 4):
        matrix_help = np.matrix(
            [[l_help[j-1], (x_ren[j-1] - x_point)], [l_help[j], (x_ren[j+1] - x_point)]])
        l_help[j-1] = np.around(
            (np.linalg.det(matrix_help)/(x_ren[j+1] - x_ren[j-1])), 4)
        l2.append(np.around((l_help[j-1]), 4))

    l_help = l2.copy()
    for i in range(2):
        l3.append(' ')
    for j in range(2, 4):
        matrix_help = np.matrix(
            [[l_help[j-1], (x_ren[j-2] - x_point)], [l_help[j], (x_ren[j+1] - x_point)]])
        l_help[j-1] = np.around(
            (np.linalg.det(matrix_help)/(x_ren[j+1] - x_ren[j-2])), 4)
        l3.append(np.around((l_help[j-1]), 4))

    l_help = l3.copy()
    for i in range(3):
        l4.append(' ')
    for j in range(3, 4):
        matrix_help = np.matrix(
            [[l_help[j-1], (x_ren[j-3] - x_point)], [l_help[j], (x_ren[j+1] - x_point)]])
        l_help[j-1] = np.around(
            (np.linalg.det(matrix_help)/(x_ren[j+1] - x_ren[j-3])), 4)
        l4.append(np.around((l_help[j-1]), 4))

    print("\nСхема Эйткена")

   
    table1 = PrettyTable()
    table1.field_names = ['Lᵢ₋₁ⁱ', 'Lᵢ₋₂ⁱ₋₁ⁱ', 'Lᵢ₋₃ⁱ₋₂ⁱ₋₁ⁱ', 'L₀₁₂₃₄']
    for i in range(len(l1)):
        table1.add_row([l1[i], l2[i], l3[i], l4[i]])
    print(table1)

    
    table2 = PrettyTable()
    table2.field_names = ['Δ', str(np.around(math.fabs(l1[0]-l2[1]),4)), 
                          str(np.around(math.fabs(l2[1]-l3[2]),4)), 
                          str(np.around(math.fabs(l3[2]-l4[3]),4))]
    print(table2)


print("Таблица узлов интерполирования")

x = np.around(list(np.linspace(0.01, 10, 13)), 2)
print("x(k):[", end="")
for i in range(len(x)):
    print('{:.2f}'.format(x[i]), end="")
    if i != len(x)-1:
        print(', ', end="")
print(']')

y = []
for i in range(len(x)):
    y.append(np.around(math.log(x[i])/x[i], 4))

print("y(k):[", end="")
for i in range(len(y)):
    print('{:.4f}'.format(y[i]), end="")
    if i != len(y)-1:
        print(', ', end="")
print(']')

Renumber()
Aitkens_scheme()