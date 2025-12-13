#define _GNU_SOURCE	// para el WSL

#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

int print_text_file(char *path)
{
	/* To be completed (part A) */
	FILE* file = fopen(path, "r");
	char* delimite = ":";
	char line[MAXLEN_LINE_FILE];
	int num_students = 0;

	if(file == NULL){
		perror("Error al abrir al archivo");
		return EXIT_FAILURE;
	}

	if(fgets(line, sizeof(line), file) == NULL){
		perror("Error al leer el archivo");
		fclose(file);
		return EXIT_FAILURE;
	}

	num_students = atoi(line);
	
	
	for(int i = 0; i < num_students; ++i){
		if(fgets(line, sizeof(line), file) != NULL){

		student_t student;
		char* token;
		char* temp_line = line;

		token = strsep(&temp_line, delimite);
		if(token != NULL){
			student.student_id = atoi(token);
		}

		token = strsep(&temp_line, delimite);
        if (token != NULL) {
            strncpy(student.NIF, token, MAX_CHARS_NIF);
            student.NIF[MAX_CHARS_NIF] = '\0';
        }

        token = strsep(&temp_line, delimite);
        if (token != NULL) {
            student.first_name = token;
        }

        token = strsep(&temp_line, "\n"); 
        if (token != NULL) {
            student.last_name = token; 
        }

		printf("[Entry #%d]\n", i);
        printf("\tstudent_id=%d\n", student.student_id);
        printf("\tNIF=%s\n", student.NIF);
        printf("\tfirst_name=%s\n", student.first_name);
        printf("\tlast_name=%s\n", student.last_name);

		}

		else{
			perror("Error al leer líneas");
			fclose(file);
			return EXIT_FAILURE;
		}

	}
	


	fclose(file);
	return 0;
}

char *loadstr(FILE *file)

{	/* To be completed */
	int lenght = 0;
	int space;

	long start = ftell(file);

	while((space = getc(file)) != '\0' && space != EOF){
		lenght++;
	}

	if (space == EOF || lenght == 0){
		return NULL;
	}

	fseek(file, start, SEEK_SET);

	char* str = (char *) malloc((lenght + 1) * sizeof(char));
	    if (str == NULL) {
        err(3, "Error al reservar memoria");
		}

	fread(str, sizeof(char), (lenght + 1), file);

	return str;	
}

int print_binary_file(char *path)
{
	char* token = NULL;
	FILE* file = fopen(path, "rb");
	int num_students;

	if(file == NULL){
		perror("Error al abrir al archivo");
		return EXIT_FAILURE;
	}

	if(fread(&num_students, sizeof(int), 1, file) != 1){
		perror("Error al leer el número de estudiantes");
        fclose(file);
        return EXIT_FAILURE;
	}

	for(int i = 0; i < num_students; ++i){

		student_t student;
		fread(&student.student_id, sizeof(int), 1, file);
		token = loadstr(file);
		strcpy(student.NIF, token);
		free(token);
		token = loadstr(file);
		student.first_name = token;
		token = loadstr(file);
		student.last_name = token;

		printf("[Entry #%d]\n", i);
        printf("\tstudent_id=%d\n", student.student_id);
        printf("\tNIF=%s\n", student.NIF);
        printf("\tfirst_name=%s\n", student.first_name);
        printf("\tlast_name=%s\n", student.last_name);

// ----
// 		int a; 
// 		int b;

// 		fscanf(file,"%d:%d",&a,&b)<2 --> FALLO; --> 2
	}


	
	fclose(file);
	return EXIT_SUCCESS;
	
}


int write_binary_file(char *input_file, char *output_file)
{
    FILE *infile = fopen(input_file, "r");
    FILE *outfile = fopen(output_file, "wb");
    char line[MAXLEN_LINE_FILE];
    int num_students = 0;

    if (infile == NULL) {
        perror("Error al abrir el archivo de entrada");
        return EXIT_FAILURE;
    }
    if (outfile == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(infile);
        return EXIT_FAILURE;
    }

    if (fgets(line, sizeof(line), infile) == NULL) {
        perror("Error al leer el archivo de entrada");
        fclose(infile);
        fclose(outfile);
        return EXIT_FAILURE;
    }
    num_students = atoi(line);

    fwrite(&num_students, sizeof(int), 1, outfile);

    for (int i = 0; i < num_students; ++i) {
        if (fgets(line, sizeof(line), infile) != NULL) {
            student_t student;
            char *temp_line = line;
            char *token;

            token = strsep(&temp_line, ":");
            student.student_id = atoi(token);

            token = strsep(&temp_line, ":");
            strcpy(student.NIF, token);

            token = strsep(&temp_line, ":");
            student.first_name = token;

            token = strsep(&temp_line, "\0");
            student.last_name = token;
			student.last_name[strlen(student.last_name)-1]='\0';

            fwrite(&student.student_id, sizeof(int), 1, outfile);
            fwrite(student.NIF, sizeof(char), strlen(student.NIF) + 1, outfile);
            fwrite(student.first_name, sizeof(char), strlen(student.first_name) + 1, outfile);
            fwrite(student.last_name, sizeof(char), strlen(student.last_name) + 1, outfile);
        } else {
            perror("Error al leer una línea del archivo de entrada");
            fclose(infile);
            fclose(outfile);
            return EXIT_FAILURE;
        }
    }
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:pob")) != -1)
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
		case 'o':
			options.action = WRITE_BINARY_ACT;
            options.output_file = argv[optind];
            if (!options.output_file) {
                fprintf(stderr, "Debe especificar un archivo de salida con -o.\n");
                exit(EXIT_FAILURE);
            }
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
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
