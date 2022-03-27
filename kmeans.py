import sys

from sklearn import cluster


EPSILON = 0.001

def invalid_input():
    print("Invalid Input!")
    sys.exit()


class Vector:
    def __init__(self, cordinates):
        self.cordinates = cordinates
        self.cluster = None
    
    @property
    def size(self):
        return len(self.cordinates)
    
    @property
    def norm(self):
        _norm = 0
        for cordinate in self.cordinates:
            _norm += cordinate ** 2
        return _norm ** 0.5

    def __getitem__(self, i):
        return self.cordinates[i]
    
    def __setitem__(self, i, value):
        self.cordinates[i] = value

    def __str__(self):
        return "Vector: " + str(['%.4f' % cord for cord in self.cordinates])

    def copy(self):
        return Vector([cord for cord in self.cordinates])
        
    
    def add_or_sub(self, other, to_add: bool = True):
        for i in range(self.size):
            if to_add:
                self.cordinates[i] += other.cordinates[i]
            else:
                self.cordinates[i] -= other.cordinates[i]

    def distance(self, other):
        sum = 0
        for i in range(self.size):
            sum += (self.cordinates[i] - other.cordinates[i]) ** 2
        return sum


    def update_cluster(self, new_cluster):
            if self.cluster:
                self.cluster.remove_vector(self)
            self.cluster = new_cluster
            new_cluster.add_vector(self)


class Cluster:
    def __init__(self, centroid: Vector, vecotr_size: int):
        self.centroid = centroid
        self.vectors = []
        self.sum_vector = Vector([0] * vecotr_size)

    @property
    def size(self):
        return len(self.vectors)

    def remove_vector(self, vector):
        self.vectors.remove(vector)
        self._updte_sum_vector(vector, to_add=False)

    def add_vector(self, vector):
        self.vectors.append(vector)
        self._updte_sum_vector(vector, to_add=True)

    def _updte_sum_vector(self, vector, to_add: bool):
        self.sum_vector.add_or_sub(vector, to_add)

    def updates_centroid(self):
        for i in range(self.sum_vector.size):
            self.centroid[i] = self.sum_vector[i] / self.size


class KMeans:
    def __init__(self, k:  int, max_iter: int, input_file: str, output_file: str):
        self.k = k
        self.max_iter = max_iter
        self.input_file = input_file
        self.output_file = output_file
        self.n = -1
        self.vectors = []
        self.clusters = []
        self.vectors_size = 0 

    def create_vecotrs(self):
        with open(self.input_file, 'r') as f:
            for line in f:
                raw_vector = [float(cor) for cor in line.strip().split(',')]
                if raw_vector:
                    self.vectors.append(Vector(raw_vector))
        self.n = len(self.vectors)

        if self.n > 0:
            self.vectors_size = self.vectors[0].size

    def initialize_clusters(self):
        for i in range(self.k):
            self.clusters.append(Cluster(self.vectors[i], self.vectors_size))

    def k_validation(self):
        if not (1 < self.k < self.n):
            invalid_input()

    def get_closet_cluster(self, vector: Vector):
        min = [self.clusters[0], vector.distance(self.clusters[0].centroid)]

        for i in range(1, len(self.clusters)):
            distance = vector.distance(self.clusters[i].centroid)
            if distance < min[1]:
                min[0] = self.clusters[i]
                min[1] = distance
        return min[0]

    def updates_centroids(self):
        valid_norms_counter = 0
        for i in range(len(self.clusters)):
            cluster = self.clusters[i]
            old_centroid = cluster.centroid.copy()
            cluster.updates_centroid()

            diff_vector = old_centroid.copy()
            diff_vector.add_or_sub(cluster.centroid, to_add=False)
            if diff_vector.norm < EPSILON:
                valid_norms_counter += 1

        return valid_norms_counter

    def assign_to_cluster(self):
        for i in range(self.n):
            vector = self.vectors[i]
            closest = self.get_closet_cluster(vector)
            if vector.cluster != closest:
                vector.update_cluster(closest)

    def run(self):
        self.create_vecotrs()
        self.k_validation()
        self.initialize_clusters()

        iteration_number = 0
        num_of_valid_norms = 0 # number of Clusters that thier centroid norm is lower than EPSILON
        while (iteration_number < self.max_iter ):
            iteration_number += 1
            print(iteration_number)
            self.assign_to_cluster()
            num_of_valid_norms = self.updates_centroids()
        
        for cluster in self.clusters:
            print(cluster.centroid)

# def get_vector_size():


def args_parsing():
    max_iter = 200
    input_file_index = 2
    output_file_index = input_file_index + 1
    
    args = sys.argv
    if not(4 <= len(args) <= 5):
        invalid_input()

    k = args[1] # TODO: Check that 1<k<N
    if not k.isnumeric():
        invalid_input()

    k = int(k)

    second_arg = args[2]
    if second_arg.isnumeric():
        max_iter = int(second_arg)
        if max_iter <= 0:
            invalid_input()
        input_file_index += 1
    
    input_file = args[input_file_index]
    output_file = args[output_file_index]    

    return KMeans(k, max_iter, input_file, output_file)
    

def main():
    kmenas = args_parsing()
    kmenas.run()


if __name__ == "__main__":
    main()