import numpy as np
import pandas as pd
import math

###############################################################################

#DECISION TREE
#tree takes data set and prints a result
class DecisionTree:
    
    #initialize empty tree
    def __init__(self, D):
        self.head = self.GrowTree(0,D)
        
    #traverse takes a row and pushes it through the tree until a result is found
    def traverse(self, row):
        hold = self.head
        while (hold.branches != []):
            hold = self.hold.compare(row.iloc[self.hold.name])
        return hold.data[0]

    #classify takes a data set and attempts to find the best results
    def classify(self, D):
        results
        for row in D[:]:
            results += self.traverse(row)
        print(results) 
        
    #GROWTREE
    #takes a data set and a node, then creates a decision tree that fits the data
    def GrowTree(self, height, D):
        max = 5
        #if homogenous, store result in Node and return
        
        #elem, counts = np.unique(D["y"], return_counts=True)
        #if (len(elem) == 1):
        #    return Node(data=D.loc[D.shape[1]-1][0])
        
        D1,D2,colname,split = self.BestSplit(D)
        
        hold = Node(data=split, name=colname)
        
        if ((D1.shape[1] > 0) and not (height == max)):
            branch[0] = self.GrowTree(height+1,D1)
        else:
            branch[0] = Node(data=majority(D1))
        
        if ((D2.shape[1] > 0) and not (height == max)):
            branch[1] = self.GrowTree(height+1,D2)
        else:
            branch[1] = Node(data=majority(D2))
            
        return hold

    def ent(self,column):
        column = list(column)
        entropy = 0
        total = len(column)
        elements, counts = np.unique(column, return_counts=True)
        numElems = len(elements)

        for elem in range(len(counts)):
            pi = counts[elem]/total
            if(numElems < 2):
                entropy = entropy - pi*(math.log(pi,2))
            else:
                entropy = entropy - pi*(math.log(pi,  numElems))

        return entropy

    #("String", dataframe with last column being the output column, int)
    def infoGain(self, colName, data, splitOn):
        marker = 0
        baseEntropy = self.ent(data[colName])

        elements, counts = np.unique(data[colName], return_counts=True)
        lowCount = 0
        highCount = 0

        for i in range(len(elements)):
            if(elements[i] < splitOn):
                lowCount = lowCount + counts[i]
            else:
                highCount = highCount + counts[i]
        
        df1, df2 = [x for _, x in data.groupby(data[colName] < splitOn)]
        #Returns Entropy of data - Scaled Entropy of #<splitOn - Scaled Entropy #>=splitOn  
        return (baseEntropy - ((lowCount/(highCount+lowCount))*(self.ent(df1.iloc[:,-1]))) - ((highCount/(highCount+lowCount))*(self.ent(data.iloc[:,-1])))), df1, df2 

    def BestSplit(self, data):
        mostInfoGain = 0
        labels = list(data.columns.values)
        for i in range(data.shape[1]-1):
            elements, counts = np.unique(data[labels[i]], return_counts=True)
            print("Label:", labels[i])
            for j in range(1,len(elements)):
                gain, leftPart, rightPart = self.infoGain(labels[i], data, elements[j])
                if(gain > mostInfoGain):
                    mostInfoGain = gain
                    colToSplitOn = labels[i]
                    digitToSplitOn = elements[j]
                    leftPartition = leftPart
                    rightPartition = rightPart
        return leftPartition, rightPartition, colToSplitOn, digitToSplitOn
                       
    
    def majority(D):
        elem, counts = np.unique(D["y"], return_counts=True)
        return elem[list(counts).index(max(counts))]
        
    
###############################################################################
#NODE
#takes a row of data and categorizes it
class Node:
    
        #initialize node
        def __init__(self, data=None, branch=None, name=None):
                self.column = name
                self.branches = [branch]
                #if no branches, data is the return value
                #else each data point is a comparison value
                #     where if num < data[i], then branch[i]
                self.data = [data]
                
        #function to compare a num with stored comparison values and return a node
        def compare(self, num):
                for i in range(len(branches)):
                        if (num < self.data[i]):
                            return self.branches[i]
                #if num out of all ranges, return highest branch
                return self.branches[len(branches)]
                
                
###############################################################################
