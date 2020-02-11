import numpy as np
from scipy.stats import multivariate_normal
from scipy.stats import norm
from numpy.linalg import inv
import matplotlib.pyplot as plt

def posterior(m0, S0, beta, X, y):
    mN = inv(inv(S0) + beta*X.T.dot(X)).dot(inv(S0).dot(m0) + beta*X.T.dot(y))
    SN = inv(inv(S0) + beta*X.T.dot(X))
    return mN, SN

def predictiveposterior(m0, S0, beta, x_star, X, y): 
    mN, SN = posterior(m0, S0, beta, X, y)
    
    m_star = mN.T.dot(x_star)
    S_star = 1.0/beta + x_star.T.dot(SN).dot(x_star)
    
    return m_star, S_star


def plot_line(ax, w): # input data
    X = np.zeros((2,2)) 
    X[0,0] = -5.0 
    X[1,0] = 5.0
    X[:,1] = 1.0
        # because of the concatenation we have to flip the transpose
    y = w.dot(X.T) 
    ax.plot(X[:,0], y)


"""
Create a contour plot of a two-dimensional normal distribution
Parameters
----------
ax : axis handle to plot
mu : mean vector 2x1
Sigma : covariance matrix 2x2
"""
def plotdistribution(ax,mu,Sigma):
    x = np.linspace(-1.5,1.5,100)
    x1p, x2p = np.meshgrid(x,x)
    pos = np.vstack((x1p.flatten(), x2p.flatten())).T

    pdf = multivariate_normal(mu.flatten(), Sigma) 
    Z = pdf.pdf(pos)
    Z = Z.reshape(100,100)

    ax.contour(x1p,x2p,Z, 5, colors='r', lw=5, alpha=0.7) 
    ax.set_xlabel('w_0')
    ax.set_ylabel('w_1')
    
    return

# create prior distribution
tau = 1.0*np.eye(2) 
w_0 = np.zeros((2,1))

w = np.zeros((2,1))
w[0] = -1.3
w[1] = 0.5

X = np.ones((201,2))
X = X.T
X[0] = np.linspace(-1,1,201)
X = X.T

beta = 0.3
noise = norm(loc=0, scale=beta)
samps = noise.rvs(201)

y = np.empty((201,1))

for i in range(0, X.shape[0]):
    y[i] = w.T.dot(X[i,:]) + samps[i]

index = np.random.permutation(X.shape[0]) 
for i in range(0, index.shape[0]):
    # X_i = X[index,:]
    # y_i = y[index]
    mN, SN = posterior(w_0, tau, beta, X, y)

print(w)
print(mN)

fig = plt.figure(figsize=(10,5)) 
ax = fig.add_subplot(121)
ax2 = fig.add_subplot(122)

w_samp = np.random.multivariate_normal(mN.flatten(), SN, size=10)

for i in range(0, w_samp.shape[0]): plot_line(ax2, w_samp[i,:])
ax2.scatter(X[:,0], y)

plotdistribution(ax, mN, SN)

plt.tight_layout()
plt.show()

#         # compute posterior
#         # visualise posterior
#         # visualise samples from posterior with the data
#         # print out the mean of the posterior