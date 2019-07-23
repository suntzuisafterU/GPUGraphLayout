import sys
import numpy as np
import math

file_name = sys.argv[1]

# Original filename was:
# ("/Users/his795/GraphSAGE/Stress/OnlyCluster4/Cluster4-Edge-List.txt")
with open(file_name) as f:
    content = f.readlines()
content = [x.split() for x in content]


n = 1000
max_val = 0
x = np.zeros(shape=(n, n))
for val in content:
    val[0] = int(val[0])
    val[1] = int(val[1])
    if max_val < val[0]:
        max_val = val[0]
    if max_val < val[1]:
        max_val = val[1]
    x[val[0]][val[1]] = 1
print(max_val)
MAXIMUM = 83456934890
# MAXIMUM = int("+inf") # Does this work?
for i in range(n):
    for j in range(n):
        if x[i][j] == 0 and i != j:
            x[i][j] = MAXIMUM
            
for k in range(n):
    for i in range(n):
        for j in range(n):
            if x[i][j] > x[i][k] + x[k][j]:
                x[i][j] = x[i][k] + x[k][j]
                
with open("/Users/his795/GraphSAGE/Stress/OnlyCluster4/Actual-X-Y-Values.txt") as f:
    euc_actual = f.readlines()
euc_actual = [x.split() for x in euc_actual]

node_actual = np.zeros(shape=(n, 2))
i = 0
for val in euc_actual:
    node_actual[i][0] = (val[1])
    node_actual[i][1] = (val[2])
    i = i+1
    
with open("/Users/his795/GraphSAGE/Stress/OnlyCluster4/Predicted-X-Y-Values.txt") as f:
    euc_predicted = f.readlines()
euc_predicted = [x.split() for x in euc_predicted]

node_predicted=np.zeros(shape=(n, 2))
i = 0
for val in euc_predicted:
    a = ""
    b = ""
    for k in range(1, len(val[1])-1):
        a += val[1][k]
        
    print(a)
    for k in range(1, len(val[2])-1):
        b += val[2][k]
    node_predicted[i][0] = (float(a))
    node_predicted[i][1] = (float(b))
    i = i+1

###################################################################################
# STRESS FOR ACTUAL VALUES

Actual_Stress = 0
for i in range(n):
    for j in range(i+1, n):
        if x[i][j] != MAXIMUM:
            euclidian_dist = math.sqrt((node_actual[i][0]-node_actual[j][0])**2 +
                                       (node_actual[i][1]-node_actual[j][1])**2)
            shortest_dist = x[i][j]
            Actual_Stress = Actual_Stress + ((euclidian_dist-shortest_dist)**2)/(shortest_dist**2)
            

Predicted_Stress=0
for i in range(n):
    for j in range(i+1, n):
        if x[i][j] != MAXIMUM:
            euclidian_dist = math.sqrt((node_predicted[i][0]-node_predicted[j][0])**2 +
                                       (node_predicted[i][1]-node_predicted[j][1])**2)
            shortest_dist = x[i][j]
            Predicted_Stress = Predicted_Stress + ((euclidian_dist-shortest_dist)**2)/(shortest_dist**2)    

print("The Actual Stree values is ->" + str(Actual_Stress))

print("The Predicted Stree values is ->" + str(Predicted_Stress))



