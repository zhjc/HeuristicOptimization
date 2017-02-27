# -*- coding: utf-8 -*-

import os

def listdir_(dir_):
    f = open("D:\\workspace\\HeuristicOptimization\\p center\\instances\\results\\instances3.txt","w")
    
    l = []
    for item in os.listdir(targetdir):
        curfile = targetdir+item
        #print(curfile)
        if os.path.isfile(curfile):
            print(curfile)
            l.append(curfile)
            #f.write(curfile)
            #f.write('\n')

    for elem in l:
        f.write(elem)
        f.write('\n')
        
    f.close()

def listdir_2(dir_2):
    f = open("D:\\workspace\\HeuristicOptimization\\p center\\instances\\results\\instances3.txt","w")

    for i in range(15):
        f.write(dir_2+"u1060_"+str((i+1)*10)+".tsp")
        f.write("\n")

    for i in range(15):
        f.write(dir_2+"u1817_"+str((i+1)*10)+".tsp")
        f.write("\n")
    
    f.close()
work_path = 'D:\\workspace\\HeuristicOptimization\\p center\\instances\\tspg\\'
if __name__ == '__main__':
    targetdir = work_path+''
    #listdir_(targetdir)
    listdir_2(targetdir)
    
            
