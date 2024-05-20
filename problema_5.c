#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4

void multiplicar_matriz_vector(int rango, int size, int *sub_matriz, int *vector, int *sub_resultado, int filas_por_proceso) {
    int i, j;
    for (i = 0; i < filas_por_proceso; i++) {
        sub_resultado[i] = 0;
        for (j = 0; j < N; j++) {
            sub_resultado[i] += sub_matriz[i * N + j] * vector[j];
        }
    }
}

int main(int argc, char *argv[]) {
    int rango, size, i, j;
    int matriz[N*N], vector[N], resultado[N];
    int filas_por_proceso;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    filas_por_proceso = N / size;

    if (rango == 0) {
        for (i = 0; i < N; i++) {
            vector[i] = i + 1;
            for (j = 0; j < N; j++) {
                matriz[i * N + j] = i * N + j + 1;
            }
        }
    }

    MPI_Bcast(vector, N, MPI_INT, 0, MPI_COMM_WORLD);

    int *sub_matriz = (int *)malloc(filas_por_proceso * N * sizeof(int));
    int *sub_resultado = (int *)malloc(filas_por_proceso * sizeof(int));

    MPI_Scatter(matriz, filas_por_proceso * N, MPI_INT, sub_matriz, filas_por_proceso * N, MPI_INT, 0, MPI_COMM_WORLD);

    multiplicar_matriz_vector(rango, size, sub_matriz, vector, sub_resultado, filas_por_proceso);

    MPI_Gather(sub_resultado, filas_por_proceso, MPI_INT, resultado, filas_por_proceso, MPI_INT, 0, MPI_COMM_WORLD);

    if (rango == 0) {
        printf("Resultado de la multiplicación:\n");
        for (i = 0; i < N; i++) {
            printf("%d\n", resultado[i]);
        }
    }

    free(sub_matriz);
    free(sub_resultado);
    MPI_Finalize();
    return 0;
}
