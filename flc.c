/*
 * Copyright 2011 Florian Coulmier
 *
 * This file is part of flc
 *
 * flc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>

/*
 * Defines
 */

#define PROG_NAME "flc"
#define PROG_VERSION "1.1"
#define PROG_DATE "26 January 2012"
#define PROG_AUTHOR "Florian Coulmier"
#define PROG_AUTHORMAIL "florian@coulmier.fr"

/* Maximum number of characters in a directory full path */
#define MAX_PATH_LENGTH 2048

/*
 * Global variables
 */

/* Type of file we are going to count. By default, we count everything, 
   except directories */
struct global_args_t {
	int counttype[15];
} global_args; 

/*
 * function: countfiles
 * Arguments:
 *  - char* path : path where to count files
 *
 * Recursive function that count files in a directories.
 * For each sub-directory found, call itself again with the sub-directory
 * as argument.
 */
long countfiles(char* path, int recursive) {
	DIR* dh; 
	struct dirent *dc; 
	long nbfiles = 0;
	long nbsubfiles = 0;
	char *subdir; /* used to get the full path of the subdir */

	subdir = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
	if (subdir == NULL) {
		perror("Could not allocate memory");
		return -1;	
	}
	
	/* Open directory to count files */
	dh = opendir(path);
	if (dh == NULL) {
		fprintf(stderr, "Could not open directory `%s': ", path );
		perror(NULL);
		return 0;
	}

	/* Count all files in the directory */
	while ((dc = readdir(dh)) != NULL) {
		/* If file is a directory, call this function 
		   recursively and add the result.
		   Do not enter in '.' and '..'  */
		if (dc->d_type == DT_DIR && recursive > 0) {
			if (strncmp(dc->d_name, ".", 1) && strncmp(dc->d_name, "..", 2)) {

				/* If we count directories, increase nbfiles */
				if (global_args.counttype[DT_DIR] == 1) {
					nbfiles++;
				}

				/* Check that subdir full path is not too long */
				if (strlen(path) + dc->d_reclen + 1 >= MAX_PATH_LENGTH) {
					fprintf(stderr, "Too long path: %s/%s\n", path, dc->d_name);
					continue;
				}
				snprintf(subdir, MAX_PATH_LENGTH, "%s/%s", path, dc->d_name);
				nbsubfiles = countfiles(subdir, recursive);
				/* if recursive call returned an error
				   do not continue and return this error */
				if (nbsubfiles < 0) {
					closedir(dh);
					return nbsubfiles;
				}
				nbfiles += nbsubfiles;
			}
		} else if (dc->d_type == DT_UNKNOWN) {
			fprintf(stderr, "%s/%s: Unkown type of file.\n", path, dc->d_name);
		} else if (global_args.counttype[dc->d_type] == 1) {
			/* Increase nbfiles if the file type is one of which we count */
			nbfiles++;
		}
	}
	closedir(dh);
	return nbfiles;
}

/*
 * function: usage
 * No arguments
 *
 * Explain how to use this program
 */
void usage() {
	printf("Usage: %s [OPTIONS] DIR\n", PROG_NAME);
	printf("\n");
	printf("Options:\n");
	printf("  -R,--no-recursive       Do not traverse sub-directories\n");
	printf("  -d,--directories        Also count directories\n");
	printf("  -D,--only-directories   Count only number of directories\n");
	printf("  -L,--only-symlinks      Count only number of symbolic links\n");
	printf("  -F,--only-regular-files Count only number of regular files\n");
	printf("  -h,--help             Show this menu\n");
	printf("\n");
	printf("%s - version %s by %s\n", PROG_NAME, PROG_VERSION, PROG_AUTHOR);
	printf("For any question or comment, write to %s\n", PROG_AUTHORMAIL);
}

/*
 * Entry point of the program
 */
int main(int argc, char** argv) {
	long nbfiles = 0;
	int opt_recursive = 1; /* Count files recursively */
	int option_index = 0;  /* for getopt_long */
	int opt_only_set = 0;  /* Number of --only-* options passed */
	
	int i, c; /* temp variables */
	DIR* dir; /* temp variable, to check if passed dir exists */
	/* Accepted arguments */
	static struct option long_options[] =
	{
		{"help", no_argument, NULL, 'h'},
		{"directories", no_argument, NULL, 'd'},
		{"only-directories", no_argument, NULL, 'D'},
		{"only-symlinks", no_argument, NULL, 'L'},
		{"only-regular-file", no_argument, NULL, 'F'},
		{"no-recursive", no_argument, NULL, 'R'},
		{0, 0, 0, 0}
	};

	/* Initialize default arguments */
	for(i = 0; i < 15; i++) {
		global_args.counttype[i] = -1;
	}
	/* DIR type is not counted by default */
	global_args.counttype[DT_DIR] = 0;
	


	/* Get options passed to the program */
	while ((c = getopt_long(argc, argv, "hdDLFR", long_options, &option_index)) != -1 )
	{
		switch(c) {
		case 'h':
			usage();
			return 0;
			break;
		case 'D':
			opt_only_set++;
			/* volontary fallback */
		case 'd':
			global_args.counttype[DT_DIR] = 1;
			break;
		case 'L':
			global_args.counttype[DT_LNK] = 1;
			opt_only_set++;
			break;
		case 'F':
			global_args.counttype[DT_REG] = 1;
			opt_only_set++;
			break;
		case 'R':
			opt_recursive = 0;
			break;
		case '?':
			/* This is an unkown option. getop_long already
			   print an error message in this case */
			return 1;
			break;
		default:
			/* should not be here... */
			abort();
			break;
		}
	}

	if (opt_only_set > 1) {
		fprintf(stderr, "You cannot set more than one --only-* option\n");
		return 1;
	}

	/* set default unset options */
	for (i = 0; i < 15; i++) {
		if (global_args.counttype[i] == -1) {
			/* if a --only-* option has been set, don't count
			   other type of files */
			global_args.counttype[i] = (opt_only_set > 0 ? 0 : 1);
		}
/*		printf("counttype[%d] = %d\n", i, global_args.counttype[i]); */
	}


	/* If no dir, or more than one dir, has been specified: exit */
	if (optind >= argc) {
		fprintf(stderr, "No directory specified.\n");
		return 1;
	} else if (optind < argc - 1) {
		fprintf(stderr, "Too many directories specified.\n");
		return 1;
	}

	/* Check that given directory exists */
	dir = opendir(argv[optind]);
	if (! dir) {
		fprintf(stderr, "`%s' does not exist or is not a directory.\n", argv[optind]);
		return 1;
	} else {
		closedir(dir);
	}

	/* Call countfiles function with the specified path */
	nbfiles = countfiles(argv[optind], opt_recursive);
	
	/* If nb of files is lether than 0, an error happened.
	   Exit with an error code */
	if (nbfiles < 0) {
		return 1;
	}

	/* Show the number of files count */
	printf("%ld\n", nbfiles);
	return 0;
}


