#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define N 1000000

int n, a[N], b[N];

void readin(char *filename){
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("No such file: %s\n", filename);
		exit(1);
	}
	int t;
	while (~fscanf(fp, "%d", &t))
		a[n++] = t;
	fclose(fp);
}

typedef struct{
	int st, len;
}params;

int cmp(const void *a, const void *b) {
	if (*(int *)a < *(int *) b)	return -1;
	if (*(int *)a > *(int *) b)	return 1;
	return 0;
}

void *Sort(void *pp) {
	params *p = (params *) pp;
	pthread_t pid = pthread_self();
	printf("Thread %lu is sorting %d numbers from position %d...\n", pid, p->len, p->st);
	qsort(a + p->st, p->len, sizeof(int), cmp);
}

void *Merge(void *pp) {
	params *p = (params *) pp;
	pthread_t pid = pthread_self();
	printf("Thread %lu is merging two sorted arrays...\n", pid);
	int *pt1 = a, *pt2 = a + p->len, *ptb = b;
	while (pt1 != a + p->len && pt2 != a + n){
		if (*pt1 <= *pt2)	*ptb = *pt1, pt1++;
		else	*ptb = *pt2, pt2++;
		ptb++;
	}
	while (pt1 != a + p->len) {
		*ptb = *pt1;
		ptb++, pt1++;
	}
	while (pt2 != a + n) {
		*ptb = *pt2;
		ptb++, pt2++;
	}
	for (int i = 0; i < n; i++)
		a[i] = b[i];
	printf("Finish merging!\n");
}

void solve_parallel() {
	pthread_t tid[3];

	params p1;
	p1.st = 0; p1.len = (n + 1) / 2;
	pthread_create(&tid[0], NULL, Sort, &p1);

	params p2;
	p2.st = (n + 1) / 2; p2.len = n / 2;
	pthread_create(&tid[1], NULL, Sort, &p2);
	
	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	pthread_create(&tid[2], NULL, Merge, &p1);
	pthread_join(tid[2], 0);
}

void solve_serial() {
	qsort(a, n, sizeof(int), cmp);
	printf("Finish sorting!\n");
}

int main(int argc, char *argv[]){
	readin(argv[1]);
	if (strcmp(argv[2], "parallel") == 0)
		solve_parallel();
	else if (strcmp(argv[2], "serial") == 0)
		solve_serial();
	FILE *fp = fopen("sorted_array.txt", "w");
	if (fp == NULL) {
		printf("Open file error.\n");
		exit(1);
	}
	for (int i = 0; i < n; i++)
		fprintf(fp, "%d%c", a[i], " \n"[i == n-1]);
	fclose(fp);
	return 0;
}

