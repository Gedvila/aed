#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <math.h>

struct timespec beginSeq, endSeq, beginParallel, endParallel;

void parallel(int, int, double *, double *, double *);

void seq(int, double *, double *, double *);

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <vetNum> <threadsNum> <seed>\n", argv[0]);
        return 1;
    }

    int vetNum = atoi(argv[1]);
    int threadsNum = atoi(argv[2]);
    int seed = atoi(argv[3]);
    int doSeq = (argc >= 5) ? atoi(argv[4]) : 1;


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


    switch (doSeq) {
        case 2:
        case 0:
            clock_gettime(CLOCK_MONOTONIC, &beginParallel);
            parallel(vetNum, threadsNum, vetA, vetB, vetCParallel);
            clock_gettime(CLOCK_MONOTONIC, &endParallel);
            execTimeParallel = (endParallel.tv_sec - beginParallel.tv_sec) + (
                                   endParallel.tv_nsec - beginParallel.tv_nsec) /
                               1e9;
            printf("\nTempo paralelo com %d threads: %.3lfs\n", threadsNum, execTimeParallel);
            if (doSeq != 2) break;

        case 1:
            clock_gettime(CLOCK_MONOTONIC, &beginSeq);
            seq(vetNum, vetA, vetB, vetCSeq);
            clock_gettime(CLOCK_MONOTONIC, &endSeq);
            execTimeSeq = (endSeq.tv_sec - beginSeq.tv_sec) + (endSeq.tv_nsec - beginSeq.tv_nsec) / 1e9;
            printf("\nTempo sequencial: %.3lfs\n", execTimeSeq);
            break;
        default:
            fprintf(stderr, "Modo invalido. Use 0 (paralelo), 1 (sequencial) ou 2 (ambos).\n");
            free(vetA);
            free(vetB);
            free(vetCSeq);
            free(vetCParallel);
            return 1;
    }

    if (doSeq == 2) {
        for (int i = 0; i < vetNum * vetNum; i++) {
            if (fabs(vetCSeq[i] - vetCParallel[i]) / (fabs(vetCSeq[i]) + 1e-10) > 1e-6) {
                printf("Divergencia na posicao: %d!\n", i);
                break;
            }
        }
    }

    free(vetA);
    free(vetB);
    free(vetCSeq);
    free(vetCParallel);

    return 0;
}

void parallel(int vetNum, int threadsNum, double *vetA, double *vetB, double *vetCParallel) {
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
}

void seq(int vetNum, double *vetA, double *vetB, double *vetCSeq) {
    for (int i = 0; i < vetNum; i++) {
        for (int j = 0; j < vetNum; j++) {
            double soma = 0;
            for (int k = 0; k < vetNum; k++) {
                soma += vetA[i * vetNum + k] * vetB[k * vetNum + j];
            }
            vetCSeq[i * vetNum + j] = soma;
        }
    }
}
