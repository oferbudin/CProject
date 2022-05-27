#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double epsilon;
int numberOfVectors = 0;
int vectorSize = 1;

struct cluster {
    double *centroid;
    double **vectors;
    double *sumVector;
    int size;
    int index;
};


void printVector(double* v, int size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%f ", v[i]);
    }
    printf("\n");
}


void printInvalidInput(){
    printf("Invalid Input!\n");
}

void printError(){
    printf("An Error Has Occurred\n");
}

int isNum(char* str){
    int i=0;
    while(str[i] != '\0'){
        if(str[i] == '.'){
            return 0;
        }
        i++;
    }
    return 1;
}

int strToInt(char* str){
    char* s = str;
    int num = 0;
    int i=0;
    while(s[i] != '\0'){
        num = num * 10 + (s[i] - '0');
        i++;
    }
    return num;
}

double** getMatrix(int r, int c){
    double *p;
    double **a;
    int i;
    p = calloc(r*c, sizeof(double));
    if (!p){
        return NULL;
    }
    a = calloc(r, sizeof(double*));
    if (!a){
        return NULL;
    }
    for(i=0; i<r; i++){
        a[i] = p + i * c;
    }
    return a;
}

void printMatrix(double** matrix, int rows, int columns) {
    int x = 0;
    int y = 0;

    for(x = 0 ; x < rows ; x++) {
        printf(" (");
        for(y = 0 ; y < columns ; y++){
            printf("%f     ", matrix[x][y]);
        }
        printf(")\n");
    }
}

double** readVector(char* filename){
    double** vectors;
    double cordinate;
    int row = 0;
    int column = 0;
    char c;

    FILE *file = fopen(filename, "r");
    while(fscanf(file, "%lf%c", &cordinate, &c) == 2){
        if(numberOfVectors == 0){
            if(c == ','){
                vectorSize++;
            }
        }
        if((int)c == 13){
            numberOfVectors++;
        }
    }

    fseek(file, 0,0);

    vectors = getMatrix(numberOfVectors, vectorSize);
    if (!vectors){
        return NULL;
    }
    
     while(fscanf(file, "%lf%c", &cordinate, &c) == 2){
        if((int) c == 13){
            vectors[row][column] = cordinate;
            column = 0;
            row++;
        }
        else if (c == ',') {
             vectors[row][column] = cordinate;
             column++;
        }
    }
    fclose(file);
    return vectors;
}

void createOutput(char* filename, struct cluster** clusters, int k){
    int i, j;
    FILE *file = fopen(filename, "w");
    for(i = 0; i < k; i++){
        for(j = 0; j < vectorSize - 1; j++){
            fprintf(file, "%.4f," , clusters[i] -> centroid[j]);
        }
        fprintf(file, "%.4f\n" , clusters[i] -> centroid[vectorSize-1]);
    }
    fclose(file);
}

struct cluster** initClusters(double** vectors, int k, double** centroids) {
    int i, j;
    struct cluster** clusters = (struct cluster **) calloc(k, sizeof(struct cluster*));
    if(!clusters){
        return NULL;
    }
    for (i = 0; i < k; i++) {
        struct cluster *clust = (struct cluster *) calloc(1, sizeof(struct cluster));
        if(!clust){
            return NULL;
        }
         clust -> centroid = (double*) calloc(vectorSize, sizeof(double));
        if(! clust -> centroid){
            return NULL;
        }
        for (j = 0; j < vectorSize; j++){
            clust -> centroid[j] = centroids[i][j];
        }
        clust -> vectors = getMatrix(numberOfVectors, vectorSize);
        if (! clust -> vectors) {
            return NULL;
        }
        clust -> size = 0;
        clust -> sumVector = (double*) calloc(vectorSize, sizeof(double));
        if (!clust -> sumVector) {
            return NULL;
        }
        clust -> index = i;
        clusters[i] = clust;
    }
    return clusters;

}

double vectorsDistance(double* v1, double* v2) {
    int i;
    double distance = 0;
    for (i = 0; i < vectorSize; i++) {
        distance += pow( v1[i] - v2[i], 2);
    }
    return distance;
}

int getClosestCluster(double* vector, struct cluster** clusters, int k) {
    int i;
    double distance;
    int minIndex = 0;
    double min = vectorsDistance(vector, clusters[minIndex] -> centroid);

    for (i = 0; i < k; i++) {
        distance = vectorsDistance(vector, clusters[i] -> centroid);
        if (distance < min) {
            min = distance;
            minIndex = i;
        }
    }
    return minIndex;
}

int isVectorZero(double* vector) {
    int i;
    for (i = 0; i < vectorSize; i++) {
        if (vector[i] != 0) {
            return 0;
        }
    }
    return 1;
}

void removeVectorFromOtherClusters(struct cluster** clusters, int k, int vectorIndex) {
    int i, j;
    double* clusterVector;
    for (i = 0; i < k; i++) {
        clusterVector = clusters[i] -> vectors[vectorIndex];
        if (isVectorZero(clusterVector) == 0) {
            clusters[i] -> size--;
            for (j = 0; j < vectorSize; j++) {
                clusters[i] -> sumVector[j] -= clusterVector[j];
                clusterVector[j] = 0.00000000;
            }
            break;
        }
    }
}

void addVectorToCluster(double* vector, struct cluster* cluster, int vectorIndex) {
    int i;
    cluster -> size++;
    for (i = 0; i < vectorSize; i++) {
        cluster -> vectors[vectorIndex][i] = vector[i];
        cluster -> sumVector[i] += vector[i];
    }
}

double* copyVector(double* v){
    int i;
    double* copy = (double*) calloc(vectorSize, sizeof(double));
    if(!copy) {
        return NULL;
    }
    for(i = 0; i < vectorSize; i++){
        copy[i] = v[i];
    }
    return copy;
}

int normSmallerThanEpsilon(double* c1, double* c2){
    double delta = 0;
    int i=0;
    
    for(i=0; i < vectorSize; i++){
        delta += pow(c1[i] - c2[i],2);
    }
    if(pow(delta, 0.5) < epsilon){
        return 1;
    }
    return 0;
}

int updateClusterCentroid(struct cluster* cluster) {
    int i;
    int res;
    double* oldCentroid = copyVector(cluster -> centroid);
    if(!oldCentroid){
        return -1;
    }

    for (i = 0; i < vectorSize; i++) {
        cluster -> centroid[i] = cluster -> sumVector[i] / cluster -> size;
    }
    res = normSmallerThanEpsilon(oldCentroid, cluster -> centroid);
    free(oldCentroid);
    return res;
}

double** convertPyObjectToMatrix(PyObject *po, int lines, int columns) {
    int i, j;
    PyObject *c;
    double **matrix;
    matrix = getMatrix(lines, columns);
    if (!matrix) {
        return NULL;
    }
     for (i = 0; i < lines; ++i) {
        for (j = 0; j < columns; ++j) {
            c = PyList_GetItem(po, i * columns + j);
            matrix[i][j] = PyFloat_AsDouble(c);
        }
    }
    return matrix;
}

static PyObject *fit(PyObject *self, PyObject *args){
    int i , k, j;
    int iterIndex = 0;
    int minClusterIndex;
    int maxIter = 200;
    double **vectors;
    PyObject *pyVectors;
    PyObject *pyCentroids;
    double* vector;
    double **centroids;
    struct cluster** clusters;
    int numOfValidCentroids = 0;
    PyObject *res;

    if( !PyArg_ParseTuple(args, "OOidiii", &pyVectors, &pyCentroids, &maxIter, &epsilon, &k, &vectorSize, &numberOfVectors)) {
        return NULL;
    }

    vectors = convertPyObjectToMatrix(pyVectors, numberOfVectors, vectorSize);
    if(!vectors){
        printError();
        return NULL;
    }

    centroids = convertPyObjectToMatrix(pyCentroids, k, vectorSize);
    if(!centroids){
        printError();
        return NULL;
    }

    clusters = initClusters(vectors, k, centroids);
    if (!clusters) {
        printError();
        return NULL;
    }

    while ((iterIndex < maxIter) && (numOfValidCentroids < k)){
        numOfValidCentroids = 0;
        for(i = 0; i < numberOfVectors; i++) {
            vector = vectors[i];
            minClusterIndex = getClosestCluster(vector, clusters, k);
            removeVectorFromOtherClusters(clusters, k, i);
            addVectorToCluster(vector, clusters[minClusterIndex], i);
        }
        for (i = 0; i < k; i++) {
            numOfValidCentroids = numOfValidCentroids + updateClusterCentroid(clusters[i]);
        }
        iterIndex++;
    }
    // preping the result
    res = PyList_New(0);
    PyObject *pylist;
    for (i = 0; i < k; ++i) {
        pylist = PyList_New(0);
        for (j = 0; j < vectorSize; ++j) {
            if (PyList_Append(pylist, PyFloat_FromDouble(clusters[i]->centroid[j])) != 0) return NULL;
        }
        if (PyList_Append(res, pylist) != 0) return NULL;
    }	

    for(i = 0; i < k; i++){
        free(clusters[i] -> sumVector);
        free(clusters[i] -> centroid);
	free(clusters[i] -> vectors[0]);
        free(clusters[i] -> vectors);
        free(clusters[i]);
    }
    free(clusters);
    free(vectors[0]);
    free(vectors);

    return res;
}

static PyMethodDef capiMethods[] = {
    {"fit",
     (PyCFunction) fit, 
     METH_VARARGS,
     PyDoc_STR("Calculates k menas on a provided data set and centroids")},
     {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    capiMethods
};

PyMODINIT_FUNC
PyInit_mykmeanssp(void) {
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}
