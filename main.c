#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <math.h>

struct timespec beginSeq, endSeq, beginParallel, endParallel;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <vetNum> <threadsNum> <seed>\n", argv[0]);
        return 1;
    }

    int vetNum = atoi(argv[1]);
    int threadsNum = atoi(argv[2]);
    int seed = atoi(argv[3]);

    double execTimeSeq = 0.0;
    double execTimeParallel = 0.0;

    srand(seed);

    double *vetA, *vetB, *vetCSeq, *vetCParallel;

    vetA = malloc(vetNum * vetNum * sizeof(double));
    vetB = malloc(vetNum * vetNum * sizeof(double));
    vetCSeq = malloc(vetNum * vetNum * sizeof(double));
    vetCParallel = malloc(vetNum * vetNum * sizeof(double));

    for (int i = 0; i < vetNum; i++) {
        for (int j = 0; j < vetNum; j++) {
            vetA[i * vetNum + j] = rand();
            vetB[i * vetNum + j] = rand();
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &beginSeq);

    for (int i = 0; i < vetNum; i++) {
        for (int j = 0; j < vetNum; j++) {
            double soma = 0;
            for (int k = 0; k < vetNum; k++) {
                soma += vetA[i * vetNum + k] * vetB[k * vetNum + j];
            }
            vetCSeq[i * vetNum + j] = soma;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &endSeq);

    clock_gettime(CLOCK_MONOTONIC, &beginParallel);

#pragma omp parallel for num_threads(threadsNum)
    for (int i = 0; i < vetNum; i++) {
        for (int j = 0; j < vetNum; j++) {
            double soma = 0;
            for (int k = 0; k < vetNum; k++) {
                soma += vetA[i * vetNum + k] * vetB[k * vetNum + j];
            }
            vetCParallel[i * vetNum + j] = soma;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &endParallel);

    execTimeSeq = (endSeq.tv_sec - beginSeq.tv_sec) + (endSeq.tv_nsec - beginSeq.tv_nsec) / 1e9;
    execTimeParallel = (endParallel.tv_sec - beginParallel.tv_sec) + (endParallel.tv_nsec - beginParallel.tv_nsec) /
                       1e9;


    printf("Tempo sequencial: %lfs\n", execTimeSeq);
    printf("Tempo paralelo com %d threads: %lfs\n", threadsNum, execTimeParallel);

    for (int i = 0; i < vetNum * vetNum; i++) {
        if (fabs(vetCSeq[i] - vetCParallel[i]) > 1e-9) {
            printf("Divergência na posição %d!\n", i);
            break;
        }
    }

    free(vetA);
    free(vetB);
    free(vetCSeq);
    free(vetCParallel);

    return 0;
}
