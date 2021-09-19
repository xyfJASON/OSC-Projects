#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 11

int matrix[9][9];
int ok[NUM_THREADS];

void readin(char *argv) {
	FILE *fp = fopen(argv, "r");
	if (fp == NULL) {
		printf("No such file.\n");
		exit(0);
	}
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			fscanf(fp, "%d", &matrix[i][j]);
	fclose(fp);
}

void *check(void *id) {
	int k = *(int *)id;
	ok[k] = 1;
	if(k == 9) {
		// check each column
		for (int j = 0; j < 9; j++) {
			int o[10] = {0};
			for (int i = 0; i < 9; i++) {
				if (o[matrix[i][j]])
					ok[k] = 0;
				o[matrix[i][j]] = 1;
			}
		}
	}
	else if(k == 10) {
		// check each row
		for (int i = 0; i < 9; i++) {
			int o[10] = {0};
			for (int j = 0; j < 9; j++) {
				if (o[matrix[i][j]])
					ok[k] = 0;
				o[matrix[i][j]] = 1;
			}
		}
	}
	else {
		int sti = 3 * (k / 3);
		int stj = 3 * (k % 3);
		int o[10] = {0};
		for (int i = sti; i < sti + 3; i++) {
			for (int j = stj; j < stj + 3; j++) {
				if (o[matrix[i][j]])
					ok[k] = 0;
				o[matrix[i][j]] = 1;
			}
		}
	}
	printf("Result from thread %2d is %s\n", k, ok[k] ? "pass" : "fail");
	pthread_exit(0);
}

int main(int argc, char *argv[]){
	if (argc < 2) {
		printf("Missing an argument: input file name\n");
		exit(1);
	}
	readin(argv[1]);
	int nth[NUM_THREADS];
	pthread_t tids[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		nth[i] = i;
		pthread_create(&tids[i], NULL, check, nth + i);
	}
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(tids[i], NULL);
	int result = 1;
	for (int i = 0; i < NUM_THREADS; i++)
		result &= ok[i];
	printf("Final result: %s\n", result ? "pass": "fail");
	return 0;
}
