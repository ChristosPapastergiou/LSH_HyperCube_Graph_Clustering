import numpy as np
import matplotlib.pyplot as plt

from sklearn.model_selection import train_test_split

from keras.datasets import mnist
from keras.models import Model, save_model
from keras.layers import Input, Conv2D, MaxPooling2D, UpSampling2D, BatchNormalization, Flatten, Dense, Reshape 

class ConvolutionalAutoencoder:
    def __init__(self, input_shape = (28, 28, 1), latent_dimension = 10):
        self.input_shape = input_shape
        self.latent_dimension = latent_dimension
        self.encoder, self.autoencoder = self.__autoencoder__()

    def __autoencoder__(self):
        image = Input(shape = self.input_shape)                                     # 28 x 28 x 1

        layer_1, layer_2 = 8, 16

        x = Conv2D(layer_1, (3, 3), activation = 'relu', padding = 'same')(image)   # 28 x 28 x layer_1
        x = BatchNormalization()(x)
        x = MaxPooling2D((2, 2), padding = 'same')(x)                               # 14 x 14 x layer_1
        x = Conv2D(layer_2, (3, 3), activation = 'relu', padding = 'same')(x)       # 14 x 14 x layer_2
        x = BatchNormalization()(x)

        flat = Flatten()(x)                                                         # Flattening the image and make 
        encoded = Dense(self.latent_dimension, activation = 'relu')(flat)           # it to the latent dimension 
        
        x = Dense(14 * 14 * layer_2, activation = 'relu')(encoded)                  # 7  x 7  x layer_3 was flattened
        x = Reshape((14, 14, layer_2))(x)                                           # Must reshape it to the shape before flattening

        x = Conv2D(layer_2, (3, 3), activation = 'relu', padding = 'same')(x)       # 14 x 14 x layer_2
        x = BatchNormalization()(x)
        x = UpSampling2D((2, 2))(x)                                                 # 28 x 28 x layer_2
        
        decoded = Conv2D(1, (3, 3), activation = 'sigmoid', padding = 'same')(x)    # 28 x 28 x 1

        encoder = Model(image, encoded)
        autoencoder = Model(image, decoded)
        autoencoder.compile(optimizer = 'adam', loss = 'mean_squared_error')

        return encoder, autoencoder

    def train(self, x_train, x_test, epochs = 10, batch_size = 64):
        return self.autoencoder.fit(x_train, x_train, epochs = epochs, batch_size = batch_size, shuffle = True, validation_data = (x_test, x_test))

    def predict(self, x):
        return self.autoencoder.predict(x)

    def compress(self, x):
        return self.encoder.predict(x)
    
    def plot_learning_curve(self, history):
        plt.figure()
        plt.plot(history.history['loss'], label = 'Training Loss')
        plt.plot(history.history['val_loss'], label = 'Validation Loss')
        plt.xlabel('Epochs')
        plt.ylabel('Loss')
        plt.legend()

    def plot_results(self, x_test, num_images = 10):
        decoded_imgs = self.predict(x_test)

        plt.figure(figsize = (20, 4))
        for i in range(num_images):
            ax = plt.subplot(2, num_images, i + 1)
            plt.imshow(x_test[i].reshape(self.input_shape[:-1]))
            plt.gray()
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)

            ax = plt.subplot(2, num_images, i + 1 + num_images)
            plt.imshow(decoded_imgs[i].reshape(self.input_shape[:-1]))
            plt.gray()
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)

def preprocess(array):
    array = array.astype('float32') / 255.0
    array = np.reshape(array, (len(array), 28, 28, 1))
    return array

def load_data():
    (x_train, y_train), _ = mnist.load_data()

    x_train = preprocess(x_train)
    x_train, x_valid, y_train, y_valid = train_test_split(x_train, y_train, test_size = 0.2, random_state = 42)

    return (x_train, y_train), (x_valid, y_valid)

def main():
    (x_train, _), (x_valid, _) = load_data()

    autoencoder = ConvolutionalAutoencoder(input_shape = (28, 28, 1), latent_dimension = 70)    # Model creation
    history = autoencoder.train(x_train, x_valid, epochs = 10, batch_size = 64)                 # Model training 
    save_model(autoencoder.autoencoder, '../../data/autoencoder.h5')                            # Model save

    autoencoder.plot_learning_curve(history)
    autoencoder.plot_results(x_valid, num_images = 10)
    plt.show()

if __name__ == "__main__":
    main()