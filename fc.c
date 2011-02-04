#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>


long countfiles(char* path) {
	DIR* dh;
	struct dirent *dc;
	long nbfiles = 0;
	char *subdir;

	subdir = (char *)malloc(1024 * sizeof(char));
	if (subdir == NULL) {
		perror("Could not allocate memory");
		return -1;	
	}
	
//	printf("Opening dir %s: ", path);
	dh = opendir(path);
	if (dh == NULL) {
//		printf("failed!\n");
		perror("Could not open directory");
		return -1;
	}
//	printf("done\n");
	while ((dc = readdir(dh)) != NULL) {
		if (dc->d_type == DT_REG) {
			nbfiles++;
//			printf("Found file: %s (%ld)\n", dc->d_name, nbfiles);
		} else if (dc->d_type == DT_DIR) {
			if (strncmp(dc->d_name, ".", 1) && strncmp(dc->d_name, "..", 2)) {

				snprintf(subdir, 1024, "%s/%s", path, dc->d_name);
				nbfiles += countfiles(subdir);
			}

		}
	}
	closedir(dh);
	return nbfiles;
}

int main(int ARGC, char** ARGV) {
	long nbfiles = 0;

	if (ARGC != 2) {
		printf("Usage: count-mails DIR\n");
		return 1;
	}
//	printf("Couting files of dir %s\n", ARGV[1]);

	nbfiles = countfiles(ARGV[1]);
	
	if (nbfiles < 0) {
		return 1;
	}

	printf("%ld\n", nbfiles);
	return 0;
}


