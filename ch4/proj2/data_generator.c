#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int N = 1000000;
	if(argc > 1) {
		N = atoi(argv[1]);
	}
	char filename[100];
	sprintf(filename, "array%d.txt", N);
	FILE *fp = fopen(filename, "w");
	srand(time(NULL));
	for (int i = 0; i < N; i++)
		fprintf(fp, "%d%c", rand(), " \n"[i==N-1]);
	return 0;
}
