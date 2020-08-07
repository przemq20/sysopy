#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct matrix {
	int** values;
	int width;
	int height;
} matrix;

matrix* read(char* file_name) {
	FILE* f = fopen(file_name, "r");
	char* row = (char*)calloc(1000, sizeof(char));
	matrix* mat = (matrix*)malloc(sizeof(matrix));
	mat->values = (int**)calloc(1000, sizeof(int*));
	int i = 0, j;
	while (fgets(row, 1000, f) != NULL) {
		mat->values[i] = (int*)calloc(1000, sizeof(int));
		j = 0;
		char* elem = strtok(row, " \n");
		while (elem != NULL) {
			mat->values[i][j++] = atoi(elem);
			elem = strtok(NULL, " \n");
		}
		i++;
	}
	mat->height = i;
	mat->width = j;
	free(row);
	fclose(f);
	return mat;
}

matrix* multiply(matrix* m_A, matrix* m_B) {
	matrix* m_C = (matrix*)malloc(sizeof(matrix));
	m_C->width = m_B->width;
	m_C->height = m_A->height;
	m_C->values = (int**)calloc(m_C->height, sizeof(int*));
	for (int i = 0; i < m_C->height; i++) {
		m_C->values[i] = (int*)calloc(m_C->width, sizeof(int));
	}
	for (int i = 0; i < m_C->height; i++) {
		for (int j = 0; j < m_C->width; j++) {
			m_C->values[i][j] = 0;
			for (int k = 0; k < m_A->width; k++) {
				m_C->values[i][j] += m_A->values[i][k] * m_B->values[k][j];
			}
		}
	}
	return m_C;
}

bool equal(matrix* m_A, matrix* m_B) {
	if (m_A->width != m_B->width) {
		return false;
	}
	if (m_A->height != m_B->height){
		return false;
	}
	for (int i = 0; i < m_A->height; i++) {
		for (int j = 0; j < m_A->width; j++) {
			if (m_A->values[i][j] != m_B->values[i][j]) {
				return false;
			}
		}
	}
	return true;
}

int main(int argc, char** argv) {
	char* list = argv[1];
	FILE* f = fopen(list, "r");
	char file_name[1000];
	int i = 0;

	matrix* matrixA, * matrixB, * matrixC;
	while (fscanf(f, "%s", file_name) != EOF) {
		if (i % 3 == 0) {
			matrixA = read(file_name);
		}
		else if (i % 3 == 1) {
			matrixB = read(file_name);
		}
		else if (i % 3 == 2)
		{
			matrixC = read(file_name);
			matrix *result = multiply(matrixA, matrixB);
			bool matrices_equal = equal(result, matrixC);
			if (matrices_equal) {
				printf("Passed test %d\n", i / 3);
			}
			else {
				printf("Failed test %d\n", i / 3);
			}
		}
		i++;
	}
	return 0;
}