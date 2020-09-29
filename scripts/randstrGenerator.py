import random
import string

def get_random_string(length):
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str

opt = ['a', 'd']
f = open('input_rand.txt', 'w')
for i in range(0,500000):
     f.write(random.choice(opt))
     f.write(" ")
     s = get_random_string(random.randint(4, 32))
     f.write(s)
     f.write("\n")
f.close()
