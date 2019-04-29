#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Skeleton code for CW2 submission. 
We suggest you implement your code in the provided functions
Make sure to use our print_features() and print_predictions() functions
to print your results
"""

from __future__ import print_function

import argparse
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from utilities import load_data, print_features, print_predictions
import math

# from sklearn.neighbors import KNeighborsClassifier #Remove when done testing
from sklearn.decomposition import PCA

# you may use these colours to produce the scatter plots
CLASS_1_C = r'#3366ff'
CLASS_2_C = r'#cc3300'
CLASS_3_C = r'#ffc34d'

MODES = ['feature_sel', 'knn', 'alt', 'knn_3d', 'knn_pca']    

def accuracy(pred_labels, test_labels) :
    #Code to show accuracy of guess
    j = 0
    matches = 0.0
    while j < len(test_labels) :
        print("Predicted: {} GT: {}".format(pred_labels[j], test_labels[j]))
        if (pred_labels[j] == test_labels[j]) :
            matches += 1.0
        j += 1
    print("Accuracy: {}%".format(round((matches/float(len(test_labels)))*100, 2)))
    return 0

def confusion_matrix(pred_labels, test_labels, name) :
    #Save a confusion matrix
    mismatch = 0
    classes = len(set(test_labels))
    cm = np.zeros((classes, classes))
    unique, counts = np.unique(test_labels, return_counts=True)
    print(dict(zip(unique, counts)))
    for i in range(1, classes+1):
        for j in range(1, classes+1):
            mismatch = 0
            for n in range(0, len(pred_labels)):
                if (test_labels[n] == i and pred_labels[n] == j): mismatch += 1
            cm[i-1][j-1] = mismatch/counts[i-1]            
    
    ax = plt.gca()
    fig = plt.gcf()
    im = ax.imshow(X=cm, cmap=plt.get_cmap('summer'))
    for i, j in enumerate(cm):
        for k, l in enumerate(j):
            test_l = str(l)
            if len(test_l) > 5:
                test_l = test_l[:5]
            plt.text(k, i, test_l, fontsize=8)
    fig.colorbar(im)    
    fig.savefig(name)
    
    return cm

# def actual_knn(train_set, train_labels, test_red, k) :
#     train_red = train_set[:, [0,6]]
#     test_red = test_set[:, [0,6]]
#     # train_red = train_set[:, [2,11]]
#     # test_red = test_set[:, [2,11]]
#     neigh = KNeighborsClassifier(n_neighbors=k)
#     neigh.fit(train_red, train_labels)
#     return neigh.predict(test_red)

def knn_classifier(train_red, train_labels, test_red, k) : #If something coes wrong jsut copy this code back into knn funcs
    dist = lambda x, y: np.sqrt(np.sum((x-y)**2))
    train_dist = lambda x : [dist(x, train) for train in train_red] #Returns distances between x and every element in train set
    idx = np.argpartition([train_dist(test) for test in test_red], k) #first k elements are indexes of k min
    pred_lbls = [0] * test_set.shape[0]
    j = 0
    while j < test_red.shape[0] :
        lbls = [0] * 3
        i = 0
        while i < k :
            lbls[train_labels[idx[j][i]] - 1] += 1 #Maybe some room to implement distance weighting?
            i += 1
        pred_lbls[j] = np.argmax(lbls) + 1
        if (lbls.count(lbls[np.argmax(lbls)]) > 1) : #If no single modal value, run knn again with k-1
            if (test_red.ndim > 1) : 
                pred_lbls[j] = knn_classifier(train_red, train_labels, test_red[j], k-1)[0]
            else : 
                pred_lbls[j] = knn_classifier(train_red, train_labels, test_red, k-1)[0]
        j += 1
    return pred_lbls

def feature_selection(train_set, train_labels, **kwargs):
    # write your code here and make sure you return the features at the end of 
    # the function

    #Taken from lab_5 answer, if we want to put pur code in here, we can
    n_features = train_set.shape[1] 
    fig, ax = plt.subplots(n_features, n_features)
    fig = plt.gcf()
    fig.set_size_inches(50, 50, forward=True)
    plt.subplots_adjust(left=0.01, right=0.99, top=0.99, bottom=0.01, wspace=0.2, hspace=0.4)

    colours = np.zeros_like(train_labels, dtype=np.object)
    colours[train_labels == 1] = CLASS_1_C
    colours[train_labels == 2] = CLASS_2_C
    colours[train_labels == 3] = CLASS_3_C

    for row in range(n_features):
        for col in range(n_features):
            ax[row][col].scatter(train_set[:, row], train_set[:, col], c=colours)
            ax[row][col].set_title('Features {} vs {}'.format(row+1, col+1))
            
    # plt.show()
    fig.savefig('fs.png', dpi=100)
    #7vs1 or 7vs6
    #trainset[:,[0,6]]
    return [0, 6]

def three_feature_selection(train_set, train_labels, **kwargs):
    n_features = train_set.shape[1] 
    fig = plt.figure()
    fig.set_size_inches(50, 50, forward=True)
    plt.subplots_adjust(left=0.01, right=0.99, top=0.99, bottom=0.01, wspace=0.2, hspace=0.4)


    colours = np.zeros_like(train_labels, dtype=np.object)
    colours[train_labels == 1] = CLASS_1_C
    colours[train_labels == 2] = CLASS_2_C
    colours[train_labels == 3] = CLASS_3_C

    for z in range(n_features):
        ax = fig.add_subplot(3, 5, z+1, projection='3d')
        ax.scatter(train_set[:, 0], train_set[:, 6], train_set[:, z], c=colours)
        ax.set_title('Feature {}'.format(z+1))
            
    # plt.show()
    fig.savefig('tfs.png', dpi=100)
    #7vs1 or 7vs6
    #trainset[:,[0,6]]
    return [0, 6]


def knn(train_set, train_labels, test_set, k, **kwargs):
    train_red = train_set[:, [0,6]]
    test_red = test_set[:, [0,6]]
    # train_red = train_set[:, [2,11]]
    # test_red = test_set[:, [2,11]]
    return knn_classifier(train_red, train_labels, test_red, k)

def separateClasses(data, labels):
    classes = {}
    for i in range(len(data)):
        instance = data[i]
        if (labels[i] not in classes):
            classes[labels[i]] = []
        classes[labels[i]].append(instance)
    return classes

def getMean(data):
    return sum(data)/float(len(data))

def getStandardDeviation(data):
    mean = getMean(data)
    s = 0;
    for x in data:
        s+= pow(x - mean, 2)
    s = s/float(len(data) - 1)
    return math.sqrt(s)

def summarize(data):
    summaries = [(getMean(feature), getStandardDeviation(feature)) for feature in zip(*data)]
    return summaries
    
def summarizeByClass(data, labels):
    separatedData = separateClasses(data, labels)
    summaries = {}
    for className, data in separatedData.items():
        summaries[className] = summarize(data)
    return summaries

def getGPDF(x, mean, standardDeviation):
    exp = -(pow((x - mean), 2))/(2 * pow(standardDeviation, 2))
    return 1 * math.exp(exp) / (standardDeviation * math.sqrt(2 * math.pi))

def getProbabilities(summaries, instance):
    probabilities = {}
    for className, classSummaries in summaries.items():
        for i in range(len(classSummaries)):
            x = instance[i]
            mean, standardDeviation = classSummaries[i]
            probabilities[className] = getGPDF(x, mean, standardDeviation)
    return probabilities

def classify(summaries, instance):
    probabilities = getProbabilities(summaries, instance)
    currentProbability = 0
    currentClass = None
    for className, probability in probabilities.items():
        if probability >= currentProbability:
            currentClass = className
            currentProbability = probability
    return currentClass
            

def alternative_classifier(train_set, train_labels, test_set, **kwargs):
    # write your code here and make sure you return the predictions at the end of 
    # the function    
    pred = []
    train_red = train_set[:, [0,6]]
    test_red = test_set[:, [0,6]]
    summaries = summarizeByClass(train_red, train_labels)
    for i in range(len(test_red)):
        pred.append(classify(summaries, test_red[i]))
    return pred


def knn_three_features(train_set, train_labels, test_set, k, **kwargs):
    #Covariance for finding correlations?
    train_red = train_set[:, [0, 6, 9]] #THESE FEATURES ARE NOT FINAL, PLOT THE COMPARISONS AND CHOOSE 3 FEATURES
    test_red = test_set[:, [0, 6, 9]] #THESE FEATURES ARE NOT FINAL
    return knn_classifier(train_red, train_labels, test_red, k)


def knn_pca(train_set, train_labels, test_set, k, n_components=2, **kwargs):
    # write your code here and make sure you return the predictions at the end of 
    # the function
    pca = PCA(n_components)
    pca.fit(train_set)
    train_red = pca.transform(train_set)
    test_red = pca.transform(test_set)
    return knn_classifier(train_red, train_labels, test_red, k)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('mode', nargs=1, type=str, help='Running mode. Must be one of the following modes: {}'.format(MODES))
    parser.add_argument('--k', nargs='?', type=int, default=1, help='Number of neighbours for knn')
    parser.add_argument('--train_set_path', nargs='?', type=str, default='data/wine_train.csv', help='Path to the training set csv')
    parser.add_argument('--train_labels_path', nargs='?', type=str, default='data/wine_train_labels.csv', help='Path to training labels')
    parser.add_argument('--test_set_path', nargs='?', type=str, default='data/wine_test.csv', help='Path to the test set csv')
    parser.add_argument('--test_labels_path', nargs='?', type=str, default='data/wine_test_labels.csv', help='Path to the test labels csv')
    
    args = parser.parse_args()
    mode = args.mode[0]
    
    return args, mode


if __name__ == '__main__':
    args, mode = parse_args() # get argument from the command line
    
    # load the data
    train_set, train_labels, test_set, test_labels = load_data(train_set_path=args.train_set_path, 
                                                                       train_labels_path=args.train_labels_path,
                                                                       test_set_path=args.test_set_path,
                                                                       test_labels_path=args.test_labels_path)
    if mode == 'feature_sel':
        selected_features = feature_selection(train_set, train_labels)
        #three_feature_selection(train_set, train_labels)
        print_features(selected_features)
    elif mode == 'knn':
        predictions = knn(train_set, train_labels, test_set, args.k)
        #accuracy(predictions, test_labels) #EXTRA FOR REPORT REMOVE AFTER REPORT
        print_predictions(predictions)
        # confusion_matrix(predictions, test_labels, 'knn_cm.png')
    elif mode == 'alt':
        predictions = alternative_classifier(train_set, train_labels, test_set)
        #accuracy(predictions, test_labels) #REMOVE AFTER REPORT
        #confusion_matrix(predictions, test_labels, 'alt_cm.png')
        print_predictions(predictions)    
    elif mode == 'knn_3d':
        predictions = knn_three_features(train_set, train_labels, test_set, args.k)
        # accuracy(predictions, test_labels) #EXTRA FOR REPORT REMOVE AFTER REPORT
        # confusion_matrix(predictions, test_labels, '3d_knn_cm.png')
        print_predictions(predictions)
    elif mode == 'knn_pca':
        prediction = knn_pca(train_set, train_labels, test_set, args.k)
        # accuracy(prediction, test_labels) #EXTRA FOR REPORT REMOVE AFTER REPORT
        print_predictions(prediction)
        # confusion_matrix(prediction, test_labels, 'knnPCA_cm.png')
    else:
        raise Exception('Unrecognised mode: {}. Possible modes are: {}'.format(mode, MODES))