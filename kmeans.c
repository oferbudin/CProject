#include <stdio.h>
#define EPSILON  0.01

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

double** readVector(char* filename){
    
}

int main(int argc, char* argv[]){
    int maxIter = 200;
    int k;
    int inputFileIndex = 2;
    char* inputFile;
    char* outputFile;

    if( (argc < 4) || (argc > 5)){
        printInvalidInput();
        return 1;
    }
    k = strToInt(argv[1]);
    if(isNum(argv[2]) == 0){
        maxIter = strToInt(argv[2]);
        inputFileIndex++;
    } 
    inputFile = argv[inputFileIndex++];
    outputFile = argv[inputFileIndex];
    return 0;
}


