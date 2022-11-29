#This test case tests if/while
#first define a function
def Increment(counter):
    counter = counter + 1
    return counter


#the following is if/else, and with a while loop inside
counter = 1
var = 1
#BEGIN
if(var < 50):
    while(counter < 10):
        var = var + 5;
        counter = Increment(counter)
else:
    var = 50

print(var)