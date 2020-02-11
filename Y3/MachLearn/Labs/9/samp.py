import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def add_gaussian_noise(im,prop,varSigma):
    N = int(np.round(np.prod(im.shape)*prop))

    index = np.unravel_index(np.random.permutation(np.prod(im.shape))[1:N],im.shape) 
    e = varSigma*np.random.randn(np.prod(im.shape)).reshape(im.shape)
    im2 = np.copy(im).astype('float')
    im2[index] += e[index]

    return im2

def add_saltnpeppar_noise(im,prop):
    N = int(np.round(np.prod(im.shape)*prop))
    index = np.unravel_index(np.random.permutation(np.prod(im.shape))[1:N],im.shape) 
    im2 = np.copy(im)
    im2[index] = 1-im2[index]

    return im2

   # proportion of pixels to alter
prop = 0.7 
varSigma = 0.1

im = Image.open('stellaGray.png') 
im = im/255
fig = plt.figure()
ax = fig.add_subplot(131) 
ax.imshow(im,cmap='gray')

im2 = add_gaussian_noise(im,prop,varSigma) 
ax2 = fig.add_subplot(132) 
ax2.imshow(im2,cmap='gray')
im2 = add_saltnpeppar_noise(im,prop)
ax3 = fig.add_subplot(133) 
ax3.imshow(im2,cmap='gray')

plt.show()