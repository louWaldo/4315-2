#this testcase is with recursive function calls
        
def inc(val):
    val = val+1
    return val

def sumOfHead(list1,list2):
    tmp = list1[0] + list2[0]
    return tmp

def max(a, b):
    if a >= b:
        return a
    else:
        return b

def maxElement():
    listA = [1]
    listB = [2]
    listC = [3]
    listD = [4]

    
    sum1 = sumOfHead(listA,listB)
    sum2 = sumOfHead(listC,listD)

    maxSum = max(inc(inc(inc(sum1))),inc(sum2))
    print(maxSum)



maxElement()