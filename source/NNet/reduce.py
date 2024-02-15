import argparse
import numpy as np

from keras.models import load_model
from NeuralNetwork import ConvolutionalAutoencoder
    
def load_dataset(file_path):
    with open(file_path, 'rb') as file:
        header = file.read(16)
        num_images = int.from_bytes(header[4:8], byteorder = 'big')
        image_data = np.frombuffer(file.read(), dtype = np.uint8)
        images = image_data.reshape((num_images, 28, 28, 1))

    return images

def save_to_file(file_path, total_data):
    magic_number = 33
    num_images = len(total_data)
    num_col = len(total_data[0]) 
    num_rows = 1

    with open(file_path, 'wb') as file:
        file.write(magic_number.to_bytes(4, byteorder = 'big')) # Magic number
        file.write(num_images.to_bytes(4, byteorder = 'big'))   # Number of images
        file.write(num_rows.to_bytes(4, byteorder = 'big'))     # Number of rows
        file.write(num_col.to_bytes(4, byteorder = 'big'))      # Number of columns

        for image in total_data:
            file.write(image.astype(np.uint8).tobytes())

def normalize(data):
    normalized_data = (data - np.min(data)) / (np.max(data) - np.min(data)) # Normalize the data to the range [0, 1]
    new_data = (normalized_data * 255).astype(np.uint8)                     # and scale to the range [0, 255]
    return new_data

def preprocess(array):
    array = array.astype('float32') / 255.0
    array = np.reshape(array, (len(array), 28, 28, 1))
    return array

def main():
    parser = argparse.ArgumentParser(description = "Image size reduction using convolutional autoencoder")

    parser.add_argument("-d", "--dataset", required = True, help = "Path to the dataset file")
    parser.add_argument("-q", "--queryset", required = True, help = "Path to the queryset file")
    parser.add_argument("-od", "--output_dataset", required = True, help = "Path to the output dataset file")
    parser.add_argument("-oq", "--output_queryset", required = True, help = "Path to the output queryset file")

    args = parser.parse_args()

    dataset = args.dataset
    queryset = args.queryset
    new_dataset = args.output_dataset
    new_queryset = args.output_queryset

    input_images = preprocess(load_dataset(dataset))
    query_images = preprocess(load_dataset(queryset))

    autoencoder = ConvolutionalAutoencoder(input_shape = (28, 28, 1), latent_dimension = 70)
    model = load_model('../../data/autoencoder.h5')
    autoencoder.autoencoder = model
    
    reduced_dataset = normalize(autoencoder.compress(input_images))
    reduced_queryset = normalize(autoencoder.compress(query_images))

    save_to_file(new_dataset, reduced_dataset)
    save_to_file(new_queryset, reduced_queryset)    

if __name__ == "__main__":
    main()