#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"
#include <bits/getopt_core.h>


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

int print_text_file(char *path)
{
	FILE *file = fopen(path, "r");
	if (!file) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	char line[MAXLEN_LINE_FILE];
	int n, i = 0;

	// Leer la primera línea: número de estudiantes
	if (!fgets(line, sizeof(line), file)) {
		fprintf(stderr, "Error reading student count\n");
		fclose(file);
		return EXIT_FAILURE;
	}
	n = atoi(line);

	while (fgets(line, sizeof(line), file)) {
		student_t student;
		char *fields[NR_FIELDS_STUDENT];
		char *saveptr, *token;
		int j = 0;

		// Quitar salto de línea si existe
		line[strcspn(line, "\n")] = 0;

		// Parsear campos usando strsep
		char *ptr = line;
		while ((token = strsep(&ptr, ":")) != NULL && j < NR_FIELDS_STUDENT) {
			fields[j++] = token;
		}
		if (j != NR_FIELDS_STUDENT) continue; // línea malformada

		// Asignar campos al struct
		student.student_id = atoi(fields[STUDENT_ID_IDX]);
		strncpy(student.NIF, fields[NIF_IDX], MAX_CHARS_NIF);
		student.NIF[MAX_CHARS_NIF] = '\0';
		student.first_name = strdup(fields[FIRST_NAME_IDX]);
		student.last_name  = strdup(fields[LAST_NAME_IDX]);

		printf("[Entry #%d]\n", i);
		printf("\tstudent_id=%d\n", student.student_id);
		printf("\tNIF=%s\n", student.NIF);
		printf("\tfirst_name=%s\n", student.first_name);
		printf("\tlast_name=%s\n", student.last_name);

		free(student.first_name);
		free(student.last_name);
		i++;
	}

	fclose(file);
	return EXIT_SUCCESS;
}

int print_binary_file(char *path)
{
	/* To be completed  (part B) */
	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part C) */
	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:p")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		/**
		 **  To be completed ...
		 **/

		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
