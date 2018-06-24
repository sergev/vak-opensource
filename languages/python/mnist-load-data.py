#!/usr/bin/python
#
# Example from book "Deep Learning with Python" by François Chollet
#

#
# Loading the MNIST dataset in Keras
#
from keras.datasets import mnist

(train_images, train_labels), (test_images, test_labels) = mnist.load_data()

print "train_images.shape =", train_images.shape
# (60000, 28, 28)

print "len(train_labels) =", len(train_labels)
# 60000

print "train_labels =", train_labels
# [5, 0, 4, ..., 5, 6, 8]

print "test_images.shape =", test_images.shape
# (10000, 28, 28)

print "len(test_labels) =", len(test_labels)
# 10000

print "test_labels =", test_labels
# [7, 2, 1, ..., 4, 5, 6]
