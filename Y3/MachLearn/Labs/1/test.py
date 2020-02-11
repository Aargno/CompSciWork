import numpy as np
import scipy.stats as scipy
import matplotlib as plt
x = np.array([1, 2, 3]) # create 3 vector
y = np.array([[2], [3], [4]]) # create a 3x1 vector
A = np.array([[1, 2, 4],[2, 6, 8],[3, 3, 3]]) # create a 3x3 matrix
   # print dimensionality
print('x:', x.shape, 'y:', y.shape, 'A:', A.shape)


np.zeros((2,2)) #Zeros
np.ones((2,3)) #Ones
np.eye(5) #Identity
np.empty((3,4)) #Placeholder
np.arange(1,9,2) #Vector vals 1 to 9 inc by 2
np.linspace(0,1,100) #Vector 100 linearly spaced values between 0 and 1

x = np.eye(5) 
x.shape

import numpy as np
x = np.arange(3).reshape(3,1) # 3x1 vector 
y = np.arange(3).reshape(1,3) # 3x1 vector 
A = np.arange(9).reshape(3,3) # 3x3 matrix

print(x-y)
print()
print(y-x) 
print()
print(A-x) 
print()
print(A-y)
print() 
print(x*y) 
print()
print(A*x) 
print()
print(x*A) 
print()
print(A*y)
print()

A.dot(x) # scalar product
A.dot(y.T) # scalar product with transpose of y 
np.outer(x,y.T) # outer product

x = np.ones((32,1)) 
y = np.ones(32)
z = x+y 
print(z.sum())
