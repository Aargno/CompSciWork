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
from utilities import load_data, print_features, print_predictions

from sklearn.neighbors import KNeighborsClassifier #Remove when done testing
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
    return 0

def actual_knn(train_set, train_labels, test_red, k) :
    train_red = train_set[:, [0,6]]
    test_red = test_set[:, [0,6]]
    # train_red = train_set[:, [2,11]]
    # test_red = test_set[:, [2,11]]
    neigh = KNeighborsClassifier(n_neighbors=k)
    neigh.fit(train_red, train_labels)
    return neigh.predict(test_red)

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
            pred_lbls[j] = knn_classifier(train_red, train_labels, test_red[j], k-1)[0]
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
    # fig.savefig('fs.png', dpi=100)
    #7vs1 or 7vs6
    #trainset[:,[0,6]]
    return [0, 6]


def knn(train_set, train_labels, test_set, k, **kwargs):
    train_red = train_set[:, [0,6]]
    test_red = test_set[:, [0,6]]
    # train_red = train_set[:, [2,11]]
    # test_red = test_set[:, [2,11]]
    return knn_classifier(train_red, train_labels, test_red, k)


def alternative_classifier(train_set, train_labels, test_set, **kwargs):
    # write your code here and make sure you return the predictions at the end of 
    # the function
    return []


def knn_three_features(train_set, train_labels, test_set, k, **kwargs):
    #Covariance for finding correlations?
    train_red = train_set[:, [0,6,9]] #THESE FEATURES ARE NOT FINAL, PLOT THE COMPARISONS AND CHOOSE 3 FEATURES
    test_red = test_set[:, [0,6,9]] #THESE FEATURES ARE NOT FINAL
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
        print_features(selected_features)
    elif mode == 'knn':
        # prediction = actual_knn(train_set, train_labels, test_set, args.k)
        predictions = knn(train_set, train_labels, test_set, args.k)
        # accuracy(prediction, predictions)
        # accuracy(prediction, test_labels) #EXTRA FOR REPORT
        accuracy(predictions, test_labels) #EXTRA FOR REPORT
        print_predictions(predictions)
    elif mode == 'alt':
        predictions = alternative_classifier(train_set, train_labels, test_set)
        print_predictions(predictions)
    elif mode == 'knn_3d':
        predictions = knn_three_features(train_set, train_labels, test_set, args.k)
        accuracy(predictions, test_labels) #EXTRA FOR REPORT
        print_predictions(predictions)
    elif mode == 'knn_pca':
        prediction = knn_pca(train_set, train_labels, test_set, args.k)
        accuracy(prediction, test_labels) #EXTRA FOR REPORT
        print_predictions(prediction)
    else:
        raise Exception('Unrecognised mode: {}. Possible modes are: {}'.format(mode, MODES))