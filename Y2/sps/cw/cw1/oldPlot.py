import utilities as ut
import numpy as np
from skimage import data, io, color, transform, exposure
from pprint import pprint
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import sys

def listToRows(x) :
    X = np.array([x])
    r = X.shape
    return np.reshape(x, (r[1], -r[1]+1))

def addCols(X, n) :
    p = 2
    while n > 0 :
        X = np.c_[X, listToRows((X[:,1])**p)]
        n = n - 1
        p = p + 1
    return X

def leastSquares(X, y) :
    Y = listToRows(y)
    Xt = np.transpose(X)
    return np.dot(np.linalg.inv(np.dot(Xt, X)), np.dot(Xt, Y)) #(Xt.X)^-1.Xt.Y

def genXMatrix(x, n, uf = 0) :
    X = listToRows(x)
    r = X.shape
    X = np.c_[np.ones((r[0],1)), X]
    if uf == 1 : return np.c_[X[:,0], np.sin(X[:,1])]
    return addCols(X, n - 1)

def findError(X, y, A) : #Problem, e not being calculated correctly, X and Y are fine
    Y = listToRows(y)
    r = X.shape
    newY = 0
    i = 0
    while i < r[1] :
        newY = newY + A[i]*X[:,i]
        i = i + 1
    newY = listToRows(newY)
    return np.sum((Y - newY)**2) #Performs (Y[i] - newY[i])^2 for all i 0 -> 19

def plotGraph(x, y, r, A, segs) :
    plt.scatter(x, y)
    i = 0
    j = 0
    lines = np.array([])
    while i < r :
        line = 0
        k = 0
        while k < segs[j].shape[1]:
            line = line + A[j][k]*segs[j][:,k]
            k = k + 1
        lines = np.append(lines, line)
        i = i + 20
        j = j + 1
    lines = lines.flatten()
    plt.plot(x, lines, c ='r')
    plt.show()

# def plotGraph(x, y, r, A, segs) :
#     plt.scatter(x, y)
#     i = 0
#     j = 0
#     while i < r :
#         line = 0
#         k = 0
#         while k < segs[j].shape[1]:
#             line = line + A[j][k]*segs[j][:,k]
#             k = k + 1
#         line = listToRows(line)
#         plt.plot(x[i:i+20], line, c ='r')
#         i = i + 20
#         j = j + 1
#     plt.show()

args = sys.argv[1:]
file = "train/" + args[0]
x, y = ut.load_points_from_file(file)
X = genXMatrix(x, 3) # linear, cubed and sin
r, = x.shape
i = 0
segs = []
As = []
sse = 0
while i < r : #Calculate A matrix for each line segment
    X1 = genXMatrix(x[i:i+20], 1) #Linear
    A1 = leastSquares(X1, y[i:i+20])
    err1 = findError(X1, y[i:i+20], A1)
    X2 = genXMatrix(x[i:i+20], 3) #Cubed
    A2 = leastSquares(X2, y[i:i+20])
    err2 = findError(X2, y[i:i+20], A2)
    X3 = genXMatrix(x[i:i+20], 1, 1) #Sine
    A3 = leastSquares(X3, y[i:i+20])
    err3 = findError(X3, y[i:i+20], A3)
    if err2 < 0.9 * err1 : #If cubed function is significantly better, use that
        if err3 < err2 : #If sine function is better than cubed use that
            As.append(A3)
            sse = sse + err3
            segs.append(X3)
        else :
            As.append(A2)
            sse = sse + err2
            segs.append(X2)
    else :
        As.append(A1)
        sse = sse + err1
        segs.append(X1)
    i = i + 20
print(sse)
if len(args) > 1 :  
    if args[1] == '--plot' : plotGraph(x, y, r, As, segs)
    if args[1] == '--view' : ut.view_data_segments(x, y)