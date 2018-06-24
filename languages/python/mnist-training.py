#!/usr/bin/python
#
# Example from book "Deep Learning with Python" by François Chollet
#

#
# Loading the MNIST dataset in Keras
#
from keras.datasets import mnist

(train_images, train_labels), (test_images, test_labels) = mnist.load_data()

#
# The network architecture
#
from keras import models
from keras import layers

network = models.Sequential()
network.add(layers.Dense(512, activation = 'relu', input_shape = (28 * 28,)))
network.add(layers.Dense(10, activation = 'softmax'))

#
# The compilation step
#
network.compile(optimizer = 'rmsprop',
                loss = 'categorical_crossentropy',
                metrics = ['accuracy'])

#
# Preparing the image data
#
train_images = train_images.reshape((60000, 28 * 28))
train_images = train_images.astype('float32') / 255
test_images = test_images.reshape((10000, 28 * 28))
test_images = test_images.astype('float32') / 255

#
# Preparing the labels
#
from keras.utils import to_categorical

train_labels = to_categorical(train_labels)
test_labels = to_categorical(test_labels)

#
# Train the network: fit the model to its training data
#
network.fit(train_images, train_labels, epochs = 5, batch_size = 128)

#
# Check that the model performs well on the test set
#
test_loss, test_acc = network.evaluate(test_images, test_labels)
print "Test set accuracy:", test_acc
