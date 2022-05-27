
import sys
import pandas as pd
import numpy as np
import mykmeanssp

np.random.seed(0)

class KMeans:
    def __init__(self, k:  int, max_iter: int, input_file1: str, input_file2: str, epsilon: float):
        self.k = k
        self.max_iter = max_iter
        self.input_file1 = input_file1
        self.input_file2 = input_file2
        self.epsilon = epsilon
        self.vector_data_frame = self._merge_input_files()
        self.centroids = []    
        self.vectors_list = self.vector_data_frame.values.tolist()


    def _read_input_file(self, filename):
        dataframe = pd.read_csv(filename, sep=",", header=None, index_col=0)
        dataframe.index = dataframe.index.astype('int')
        dataframe = dataframe.sort_index()
        return dataframe
    
    def set_columns_name(self, df, offset=0):
        df.columns = [f"column {i + offset}" for i in range(len(df.columns))]

    def _merge_input_files(self):
        file1 = self._read_input_file(self.input_file1)
        self.set_columns_name(file1)
        file2 = self._read_input_file(self.input_file2)
        self.set_columns_name(file2, len(file1.columns))
        merged = file1.join(file2)
        return merged

    def get_random_vector(self, probability=None):
        index =  np.random.choice(len(self.vector_data_frame), p=probability)
        return index

    def norm(self, v1, v2):
        sum = 0
        for i in range(len(v1)):
            sum += (v1[i] - v2[i]) ** 2
        return sum

    def algorithm1(self):
        self.centroids.append(self.get_random_vector())
        distances = pd.DataFrame()
         
        for i in range(self.k-1):
            distances[f"centroid {i}"] = self.vector_data_frame.apply(lambda x: self.norm(x, self.vectors_list[self.centroids[i]]), axis=1)
            columns = [column for column in distances.columns if column.startswith("centroid")]
            distances["min"] = distances[columns].apply(min, axis=1)
            distance_sum = distances["min"].sum(axis=0, skipna=True)
            distances["p"] = distances.apply(lambda x: x['min'] / distance_sum, axis=1)
            self.centroids.append(self.get_random_vector(distances["p"].tolist()))

def invalid_input():
    print("Invalid Input!")
    sys.exit()


def args_parsing():
    args = sys.argv

    if not(5 <= len(args) <= 6):
        invalid_input()

    k = args[1] # TODO: Check that 1<k<N
    if not k.isnumeric():
        invalid_input()

    k = int(k)

    if len(args) == 5:
        max_iter = 300
        epsilon = float(args[2])
        input_file1 = args[3]
        input_file2 = args[4]
    else:
        max_iter = int(args[2])
        epsilon = float(args[3])
        input_file1 = args[4]
        input_file2 = args[5]

    return KMeans(k, max_iter, input_file1, input_file2, epsilon)

def print_float_list(lst):
    print("".join(f"%.4f," %s for s in lst)[:-1])

def print_list(lst):
    print("".join(f"{s}," for s in lst)[:-1])


def main():
    try:
        kmeans = args_parsing()
        kmeans.algorithm1()
        centroids = [kmeans.vectors_list[centroid_index] for centroid_index in kmeans.centroids]

        centroids_for_c = []
        for centroid in centroids:
                for c in centroid:
                    centroids_for_c.append(c)

        vectors_for_c = []
        for vector in kmeans.vectors_list:
            for v in vector:
                vectors_for_c.append(v)

        final_centroids = mykmeanssp.fit(vectors_for_c, centroids_for_c, kmeans.max_iter, kmeans.epsilon, kmeans.k, len(kmeans.vectors_list[0]), len(kmeans.vectors_list))
        print_list(kmeans.centroids)
        for centroid in final_centroids:
            print_float_list(centroid) 
    except:
        print("An Error Has Occurred")

if __name__ == '__main__':
    main()