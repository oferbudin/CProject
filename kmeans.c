#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define EPSILON  0.01

int nunberOfVectors = 0;
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

int isNum(char* str){
    int i=0;
    while(str[i] != '\0'){
        if(!(((int) str[i] >= 48) || ((int) str[i] <= 57))){
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
    a = calloc(r, sizeof(double*));
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
        if(nunberOfVectors == 0){
            if(c == ','){
                vectorSize++;
            }
        }
        if(c == '\n'){
            nunberOfVectors++;
        }
    }

    fseek(file, 0,0);

    vectors = getMatrix(nunberOfVectors, vectorSize);
    
     while(fscanf(file, "%lf%c", &cordinate, &c) == 2){
        if(c == '\n'){
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

struct cluster** initClusters(double** vectors, int k) {
    int i, j;
    struct cluster** clusters = (struct cluster **) calloc(k, sizeof(struct cluster*));
    for (i = 0; i < k; i++) {
        struct cluster *clust = (struct cluster *) calloc(1, sizeof(struct cluster));
         clust -> centroid = (double*) calloc(vectorSize, sizeof(double));
        for (j = 0; j < vectorSize; j++){
            clust -> centroid[j] = vectors[i][j];
        }
        clust -> vectors = getMatrix(nunberOfVectors, vectorSize);
        clust -> size = 0;
        clust -> sumVector = (double*) calloc(vectorSize, sizeof(double));
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

void updateClusterCentroid(struct cluster* cluster) {
    int i;
    for (i = 0; i < vectorSize; i++) {
        cluster -> centroid[i] = cluster -> sumVector[i] / cluster -> size;
    }
}

int main(int argc, char* argv[]){
    int i;
    int iterIndex = 0;
    int minClusterIndex;
    int maxIter = 200;
    int k;
    int inputFileIndex = 2;
    char* inputFile;
    char* outputFile;
    double** vectors;
    double* vector;
    struct cluster** clusters;

    if( (argc < 4) || (argc > 5)){
        printInvalidInput();
        return 1;
    }
    k = strToInt(argv[1]);
    if(isNum(argv[2]) == 1){
        maxIter = strToInt(argv[2]);
        inputFileIndex++;
    } 
    inputFile = argv[inputFileIndex++];
    outputFile = argv[inputFileIndex];
    printf("%d %d %s %s \n", k, maxIter, inputFile, outputFile);
    
    vectors = readVector(inputFile);
    clusters = initClusters(vectors, k);

    while (iterIndex < maxIter){
        for(i = 0; i < nunberOfVectors; i++) {
            vector = vectors[i];
            minClusterIndex = getClosestCluster(vector, clusters, k);
            removeVectorFromOtherClusters(clusters, k, i);
            addVectorToCluster(vector, clusters[minClusterIndex], i);
        }
        for (i = 0; i < k; i++) {
            updateClusterCentroid(clusters[i]);
        }
        iterIndex++;
    }

    for (i = 0; i < k; i++) {
        printf("Cluster %d centroid: ", i);
        printVector( clusters[i] -> centroid, vectorSize);
    }


    return 0;
}


