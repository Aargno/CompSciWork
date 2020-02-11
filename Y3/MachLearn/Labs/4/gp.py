from scipy.spatial.distance import cdist
import numpy as np
import matplotlib.pyplot as plt

def rbf_kernel(x1, x2, varSigma, lengthscale, noise=0.0): 

    if x2 is None:
        d = cdist(x1, x1 + noise) 
    else:
        d = cdist(x1, x2)

    K = varSigma*np.exp(-np.power(d, 2)/lengthscale) 
    
    return K

def lin_kernel(x1, x2, varSigma): 
    if x2 is None:
        return varSigma*x1.dot(x1.T) 
    else:
        return varSigma*x1.dot(x2.T)

def white_kernel(x1, x2, varSigma): 
    if x2 is None:
        return varSigma*np.eye(x1.shape[0]) 
    else:
        return np.zeros(x1.shape[0], x2.shape[0])

def periodic_kernel(x1, x2, varSigma, period, lengthScale, noise=0.0): 
    if x2 is None:
        d = cdist(x1, x1 + noise) 
    else:
        d = cdist(x1, x2)
    return varSigma*np.exp(-(2*np.sin((np.pi/period)*np.sqrt(d))**2)/lengthScale**2)

def gp_prediction(x1, y1, xstar, lengthScale, varSigma, noise):

    k_starX = rbf_kernel(xstar,x1,varSigma,lengthScale)
    k_xx = rbf_kernel(x1, None, varSigma, lengthScale, noise) 
    k_starstar = rbf_kernel(xstar,None,varSigma,lengthScale)

    # k_starX = periodic_kernel(xstar,x1,varSigma,1.0,lengthScale)
    # k_xx = periodic_kernel(x1, None, varSigma, 1.0 ,lengthScale, noise) 
    # k_starstar = periodic_kernel(xstar,None,varSigma,1.0,lengthScale)

    mu = k_starX.dot(np.linalg.inv(k_xx)).dot(y1)
    var = k_starstar - (k_starX).dot(np.linalg.inv(k_xx)).dot(k_starX.T)

    return mu, var, xstar


# # choose index set for the marginal
# x = np.linspace(-6, 6, 200).reshape(-1, 1) # compute covariance matrix

# K1 = rbf_kernel(x, None, 1.0, 2.0)
# K2 = lin_kernel(x, None, 1.0)
# K3 = white_kernel(x, None, 1.0)
# K4 = periodic_kernel(x, None, 1.0, 10.0, 2.0)

# K = K1

# # create mean vector
# mu = np.zeros(x.shape)

# # draw samples 20 from Gaussian distribution
# f = np.random.multivariate_normal(mu.flatten(), K, 20)

# fig = plt.figure()
# ax = fig.add_subplot(111) 
# ax.plot(x, f.T)
# plt.show()

N=5
x = np.linspace(-3.1,3,N)
y = np.sin(2*np.pi/x) + x*0.1 + 0.3*np.random.randn(x.shape[0]) 
x = np.reshape(x,(-1,1))
y = np.reshape(y,(-1,1))
x_star = np.linspace(-6, 6, 500)
x_star = np.reshape(x_star,(-1,1))

Nsamp = 100
mu_star, var_star, x_star = gp_prediction(x, y, x_star, 1.0, 1.0, 0.0) 
fstar = np.random.multivariate_normal(mu_star.flatten(), var_star, Nsamp)
fig = plt.figure()
ax = fig.add_subplot(111)
ax.plot(x_star, fstar.T)
ax.scatter(x, y, 200, 'k', '*', zorder=2)


di_star = var_star[np.diag_indices_from(var_star)]
di_star = np.reshape(di_star,(-1,1))
muP = mu_star+di_star
muM = mu_star-di_star

ax.plot(x_star, muP, '--')
ax.plot(x_star, muM, '--', color='blue')
ax.plot(x_star, mu_star, color='blue')
ax.fill_between(x_star.flatten(),muP.flatten(), muM.flatten(),color='blue',alpha=0.3)


plt.show()