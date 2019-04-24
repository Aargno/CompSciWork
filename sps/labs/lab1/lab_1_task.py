import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

A = np.matrix('2 3; 4 -1; 5 6')
B = np.matrix([[5,2], [8, 9], [2, 1]])

print("A\n{}\n\n B\n{}\n\n".format(A, B))

C = 3 * A
print("{}\n\n".format(C))
C = A + B
print("{}\n\n".format(C))
C = B.transpose().dot(A)
print("{}\n\n".format(C))

C = A.sum()
print("{}\n".format(C))
C = B.sum()
print("{}\n".format(C))
C = A.mean()
print("{}\n".format(C))
C = B.mean()
print("{}\n".format(C))
C = A.var()
print("{}\n".format(C))
C = B.var()
print("{}\n".format(C))

np.set_printoptions(threshold=np.nan)

D = np.loadtxt('data.dat', delimiter=',')
print("{}\n\n".format(D))

C = D.shape
print("{}\n\n".format(C))

plt.scatter(D[:,0], D[:,1], s=None, c=None, marker='v', cmap=None,
 norm=None, vmin=None, vmax=None, alpha=None, linewidths=None, 
 verts=None, edgecolors=None)
plt.xlabel("Col 1")
plt.ylabel("Col 2")
plt.show()