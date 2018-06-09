// Program used to test fragmentation suffered by each algorithm, both best-fit and first-fit.

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define claimedMemory syscall(359)
#define freeMemory syscall(360)

int main() {
	float fragmentation;

	printf("Running 3 tests:\n");

    int i;
	for (i = 0; i < 3; i++) {
		fragmentation = (float)freeMemory / (float)claimedMemory;
		printf("Claimed Memory: %lu\n", claimedMemory);
		printf("Free Memory: %lu\n", freeMemory);
		printf("Fragmentation: %f\n", fragmentation);
		printf("-----\n");
		sleep(1);
	}
}