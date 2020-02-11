from scipy.stats import norm

def rbf_kernel(x1, x2, varSigma, lengthscale, noise=0.0): 

    if x2 is None:
        d = cdist(x1, x1 + noise) 
    else:
        d = cdist(x1, x2)

    K = varSigma*np.exp(-np.power(d, 2)/lengthscale) 
    
    return K

def gp_prediction(x1, y1, xstar, lengthScale, varSigma, noise):

    k_starX = rbf_kernel(xstar,x1,varSigma,lengthScale)
    k_xx = rbf_kernel(x1, None, varSigma, lengthScale, noise) 
    k_starstar = rbf_kernel(xstar,None,varSigma,lengthScale)

    mu = k_starX.dot(np.linalg.inv(k_xx)).dot(y1)
    var = k_starstar - (k_starX).dot(np.linalg.inv(k_xx)).dot(k_starX.T)

    return mu, var, xstar

def surrogate_belief(x,f,x_star,theta): 
    #What is f? Sub for y? A function list?
    mu_star, varSigma_star, x_star = gp_prediction(x, f, x_star, 1.0, 1.0, 0.0)
    return mu_star, varSigma_star

def expected_improvement(f_*, mu, varSigma, x):

    # norm.cdf(x, loc, scale) evaluates the cdf of the normal distribution

    

    return alpha

def fT(x, beta=0, alpha1=1.0, alpha2=1.0):
    return np.sin(3.0*x) - alpha1*x + alpha2*x**2 + beta*np.random.randn(x.shape[0])

it = 100
N=5
x = np.linspace(-3,3,N)
y = fT(x)

for i in range(0 ,it):


# # remove points from an array
# x_2 = np.arange(10)
# index = np.random.permutation(10) x_1 = x_2[index[0:3]]
# x_2 = np.delete(x_2, index[0:3])
#    # remove largest element
# ind = np.argmax(x_2)
# x_1 = np.append(x_1, x_2[ind]) x_2 = np.delete(x_2, ind)