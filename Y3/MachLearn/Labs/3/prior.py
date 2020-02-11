import numpy as np
import matplotlib.pyplot as plt
def plot_line(ax, w): # input data
    X = np.zeros((2,2)) 
    X[0,0] = -5.0 
    X[1,0] = 5.0
    X[:,1] = 1.0
        # because of the concatenation we have to flip the transpose
    y = w.dot(X.T)
    ax.plot(X[:,0], y)

   # create prior distribution
tau = 1.0*np.eye(2) 
w_0 = np.zeros((2,1))
   # sample from prior
n_samples = 100
w_samp = np.random.multivariate_normal(w_0.flatten(), tau, size=n_samples)
# create plot
fig = plt.figure(figsize=(10,5)) 
ax = fig.add_subplot(111)
for i in range(0, w_samp.shape[0]): plot_line(ax, w_samp[i,:])
# save fig
plt.tight_layout()
plt.show()