#10_sec_heavy_breathing_non_exagerated

#with open('testread.txt') as f:
#4    polyShape = []
#    for line in f:
#        line = line.split() # to deal with blank 
#        if line:            # lines (ie skip them)
#            line = [int(i) for i in line]
#            polyShape.append(line)

file = open('testread.txt', "r")
#file = open('10_sec_heavy_breathing_non_exagerated.txt', "r")
strnumbers = file.read().split()
polyShape= map(int, strnumbers)
print polyShape
