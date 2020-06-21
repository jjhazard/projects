import sys
import math
import numpy as np
import pandas as pd

# First select the threshold of the attribute to split set of test data on
# The threshold chosen splits the test data such that information gain is maximized
def select_threshold(df, attribute, predict_attr):
    
    # Remove duplicate values by converting the list to a set, then sort the set
    values = list(df[attribute].unique())
    values.sort()
    max_ig = -10000
    thres_val = 0
    
    # try all threshold values that are half-way between successive values in this sorted list
    for i in range(1, len(values)-2):
        thres = (values[i] + values[i+1])/2
        ig, part1, part2 = info_gain(df, attribute, predict_attr, thres)
        if ig > max_ig:
            max_ig = ig
            thres_val = thres
            topPart = part1
            bottomPart = part2
    # Return the threshold value that maximizes information gained
    return thres_val, topPart, bottomPart

# Calculate info content (entropy) of the test data
def info_entropy(df, predict_attr):
    # Dataframe and number of positive/negatives examples in the data
    heights = []
    total = 0
    for i in range(1, 6):
        subframe = df[df[predict_attr] == i]
        heights += [float(subframe.shape[0])]
        total += heights[i-1]
    # Calculate entropy
    I = 0
    for i in range(5):
        if (heights[i] == 0):
            I += 0
        else:
            I += ((-1*heights[i])/(total))*math.log(heights[i]/(total), 5)
    return I

# Calculates the weighted average of the entropy after an attribute test
def remainder(df, df_subsets, predict_attr):
        # number of test data
        num_data = df.shape[0]
        remainder = float(0)
        for df_sub in df_subsets:
        	if df_sub.shape[0] > 1:
        		remainder += float(df_sub.shape[0]/num_data)*info_entropy(df_sub, predict_attr)
        return remainder

# Calculates the information gain from the attribute test based on a given threshold
# Note: thresholds can change for the same attribute over time
def info_gain(df, attribute, predict_attr, threshold):
        sub_1 = df[df[attribute] < threshold]
        sub_2 = df[df[attribute] > threshold]
        if (sub_1.shape[0] < 5) or (sub_2.shape[0] < 5):
            ig = 0
        else:
            # Determine information content, and subract remainder of attributes from it
            ig = info_entropy(df, predict_attr) - remainder(df, [sub_1, sub_2], predict_attr)
        return ig, sub_1, sub_2

# Chooses the attribute and its threshold with the highest info gain
# from the set of attributes
def choose_attr(df, attributes, predict_attr):
        #establish variables
        max_info_gain = -100000
        best_attr = None
        threshold = 0
        # Test each attribute
        for attr in attributes[:(len(attributes)-1)]:
                print(attr,",",end=" ")
                thres, topPart, bottomPart = select_threshold(df, attr, predict_attr)
                ig, temp, temp2 = info_gain(df, attr, predict_attr, thres)
                if ig > max_info_gain:
                        max_info_gain = ig
                        best_attr = attr
                        threshold = thres
        print("chose ",best_attr)
        return max_info_gain, best_attr, threshold, topPart, bottomPart
    
    
    
    
###############################################################################

#DECISION TREE
#tree takes data set and prints a result
class DecisionTree:
    
    #initialize empty tree
    def __init__(self, D):
        attributes =  list(D.columns.values)
        self.head = self.GrowTree(0,D,attributes)
        
    #traverse takes a row and pushes it through the tree until a result is found
    def traverse(self, row):
        hold = self.head
        while (hold.branches != []):
            hold = hold.compare(row[hold.name])
        return hold.data[0]

    #classify takes a data set and attempts to find the best results
    def classify(self, D):
        results = []
        for i in range(D.shape[0]):
            results += [self.traverse(D.loc[i])]
        print(results) 
        
    #GROWTREE
    #takes a data set and a node, then creates a decision tree that fits the data
    def GrowTree(self, height, D, attributes):
        #max height is 20 to avoid overfitting
        max = 20
        
        #if homogenous, create Leaf
        elem = np.unique(D["y"])
        if (len(elem) == 1):
            return Node(data=self.majority(D),name="Leaf")
        
        #choose column and value to split data on
        gain,colname,split,D1,D2 = choose_attr(D, attributes, 'y')
        hold = Node(data=split, name=colname)
        attributes.remove(colname)
        print(gain)
                        
        #establish left branch
        if ((D1.shape[0] > 19) and not (height == max)):
            hold.branches += [self.GrowTree(height+1,D1,attributes)]
            12
        #create leaf if very little data or max height reached
        else:
            hold.branches += [Node(data=self.majority(D1), name="Leaf")]
        
        #establish right branch
        if ((D2.shape[0] > 19) and not (height == max)):
            hold.branches += [self.GrowTree(height+1,D2,attributes)]
            
        #create leaf if very little data or max height reached
        else:
            hold.branches += [Node(data=self.majority(D2), name="Leaf")]
            
        #return node
        return hold

                        
    def majority(self, D):
        elem, counts = np.unique(D["y"], return_counts=True)
        return elem[list(counts).index(max(counts))]
        
    
###############################################################################
#NODE
#takes a row of data and categorizes it
class Node:
    
        #initialize node
        def __init__(self, data=None, branch=None, name=None):
                self.name = name
                self.branches = []
                #if no branches, data is the return value
                #else each data point is a comparison value
                #     where if num < data[i], then branch[i]
                self.data = [data]
                
        #function to compare a num with stored comparison values and return a node
        def compare(self, num):
                if (num < self.data[0]):
                    return self.branches[0]
                #if num out of all ranges, return highest branch
                return self.branches[1]
                
                
###############################################################################
