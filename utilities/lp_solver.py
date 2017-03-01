# -*- coding: utf-8 -*-

activate_this = 'D:/workspace/plotvenv/Scripts/activate_this.py'
execfile(activate_this, dict(__file__=activate_this))

from gurobipy import *

import math
import random
import networkx as NX
import matplotlib.pyplot as P

def kcenter(m, n, c, k):
    model = Model("k-center")

    #目标变量
    z = model.addVar(obj=1, vtype="C", name="z")

    #决策变量
    y, x = {}, {}
    for j in range(m):
        y[j] = model.addVar(obj=0, vtype="B", name="y[%s]"%j)
        for i in range(n):
            x[i,j] = model.addVar(obj=0, vtype="B", name="x[%s,%s]"%(i,j))
    model.update()

    #约束条件
    for i in range(n):
        coef = [1 for j in range(m)]
        var = [x[i,j] for j in range(m)]
        model.addConstr(LinExpr(coef,var), "=", 1, name="Assign[%s]"%i)
        
    for j in range(m):
        for i in range(n):
            model.addConstr(x[i,j], "<", y[j], name="Strong[%s,%s]"%(i,j))
            
    for i in range(n):
        for j in range(n):
            model.addConstr(LinExpr(c[i,j],x[i,j]), "<", z, name="Max_x[%s,%s]"%(i,j))
            
    coef = [1 for j in range(m)]
    var = [y[j] for j in range(m)]
    model.addConstr(LinExpr(coef,var), "=", rhs=k, name="k_center")
    
    model.update()
    model.__data = x,y
    
    return model
    
def distance(x1, y1, x2, y2):
    return math.sqrt((x2-x1)**2 + (y2-y1)**2)

def make_data(n):
    x = [random.random() for i in range(n)]
    y = [random.random() for i in range(n)]
    c = {}
    for i in range(n):
        for j in range(n):
            c[i,j] = distance(x[i],y[i],x[j],y[j])
    return c, x, y

def floyd(A,v_len):
    for a in range(v_len):  
        for b in range(v_len):  
            for c in range(v_len):  
                if(A[b,a]+A[a,c]<A[b,c]):  
                    A[b,c] = A[b,a]+A[a,c]

    return A

def read_from_f(fp):
    c = {}

    f = open(fp)

    line = f.readline()

    while line:
        pass
    
    f.close()
    
    return c
    
n = 10
c, x_pos, y_pos = make_data(n)
m = n
k = 3
model = kcenter(m, n, c, k)
model.ModelSense = 1
model.optimize()
x,y = model.__data
edges = [(i,j) for (i,j) in x if x[i,j].X == 1]
nodes = [j for j in y if y[j].X == 1]

#print "Optimal value=", model.ObjVal
#print "Selected nodes:", nodes
#print "Edges:", edges


P.ion() # interactive mode on
G = NX.Graph()
other = [j for j in y if j not in nodes]
G.add_nodes_from(nodes)
G.add_nodes_from(other)
for (i,j) in edges:
    G.add_edge(i,j)
    
position = {}
for i in range(n):
    position[i]=(x_pos[i],y_pos[i])

print position
print nodes
print other
    
NX.draw(G, position, node_color='y', nodelist=nodes)
NX.draw(G, position, node_color='g', nodelist=other)
