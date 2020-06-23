
#BINARY DECISION TREE
#tree takes data set
#sends each row into first node
#if not the bottom of tree
#	compares value to node
#	if less
#		left branch
#	else
#		right branch
#else
#	return majority vote
class DecisionTree:
    
    def __init__(self, head=None):
        self.head = head
        
    def classify(self, row, names):
        hold = self.head
        while (hold.branches != []):
            hold = self.head.compare(row[self.hold.name])
        return hold.data[0]

