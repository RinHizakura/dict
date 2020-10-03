import random
import string
import pandas as pd

def get_random_string(length):
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str

ct = pd.read_table("cities.txt",sep=",",usecols=[0,1])
x, y = ct.shape

opt = ['a','d']
f = open('input_rand.txt', 'w')
for i in range(0,100000):
     act = random.choice(opt)
     f.write(act)
     if(act == 'd'):
         s = ct.iloc[random.randint(0, x-1), random.randint(0,y-1)]
         if(s[0] != " "): f.write(" ")
         f.write(s)
     elif(act == 's'):
         s = ct.iloc[random.randint(0, x-1), random.randint(0,y-1)]
         if(s[0] != " "): f.write(" ")
         f.write(s[0:3])
     else:    
         f.write(" ")
         s = get_random_string(random.randint(4, 32))
         l = list(s)
         l[0] = s.upper()[0]
         s = ''.join(l)
         f.write(s)
     f.write("\n")
f.close()
