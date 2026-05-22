#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <unistd.h>
#include <stdbool.h>

char* readFile( const char *filePath ) {
	FILE *file = fopen(filePath,"rb");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *buffer = malloc(length + 1);
	if ( buffer ) {
		fread(buffer, 1, length, file);
		buffer[length] = '\0';
	}
	fclose(file);
	return buffer;
}

int main(int argc, char **argv) {
	
	const char *VERSION = "1.3.1";
	bool VERBOSE = false;

	if ( argc == 2 && strcmp(argv[1], "--help") == 0 ) {
		printf("Usage: jmake [optional flags] [optional command] [optional string]\n");
		printf("Optional flags:\n");
		printf("\t--verbose, -V Run with increased verbosity\n");
		printf("\t--version, -v Return the current version\n");
		printf("\t--help        Print this help message\n");
		printf("Optional commands:\n");
		printf("\tproject        Takes a string as a project name for the executable\n");
		printf("\tinstall        Copies the executable to /usr/local/bin/\n");
		exit(0);
	}

	if ( argc == 2 && ( strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--verbose") == 0 ) ) {
		VERBOSE = true;
	}

	if ( argc == 2 && ( strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0 ) ) {
		printf("%s\n",VERSION);
		exit(0);
	}

	if ( argc == 3 && ( strcmp(argv[1], "project") == 0 ) ) {
		int len = snprintf(
				NULL, 
				0, 
				"echo '{\\n\\t\"cc\":\"clang\",\\n\\t\"cflags\":\"-O3 -march=native\",\\n\\t\"src\":\"main.c\",\\n\\t\"out\":\"%s\"\\n}' > make.json", 
				argv[2]
				);
		char *command = malloc(len + 1);
		snprintf(command, 
				len + 1, 
				"echo '{\\n\\t\"cc\":\"clang\",\\n\\t\"cflags\":\"-O3 -march=native\",\\n\\t\"src\":\"main.c\",\\n\\t\"out\":\"%s\"\\n}' > make.json",
			   	argv[2]);
		system(command);
		free(command);
		exit(0);
	}

	if ( VERBOSE) {
		printf("Checking for make.json\n");
	}

	if ( !(access("make.json", F_OK) == 0) ) {
		printf("No make.json, stop.\n");
		exit(1);
	}

	if ( VERBOSE ) {
		printf("Reading make.json\n");
	}

	char *data = readFile("make.json");
	cJSON *root = cJSON_Parse(data);
	free(data);
	
	if ( VERBOSE ) {
		printf("Checking cc\n");
	}
	
	cJSON *cc_ptr = cJSON_GetObjectItem(root, "cc");
	char *cc = cc_ptr->valuestring;
	
	if ( VERBOSE ) {
		printf("Checking cflags\n");
	}
	
	cJSON *cflags_ptr = cJSON_GetObjectItem(root, "cflags");
	char *cflags = cflags_ptr->valuestring;

	if ( VERBOSE ) {
		printf("Checking src\n");
	}

	cJSON *src_ptr = cJSON_GetObjectItem(root, "src");
	char *src = src_ptr->valuestring;

	if ( VERBOSE ) {
		printf("Checking exe\n");
	}

	cJSON *exe_ptr = cJSON_GetObjectItem(root, "exe");
	char *exe = exe_ptr->valuestring;

	if ( argc == 2 && strcmp(argv[1],"install") == 0 ) {
		if ( !(access(exe, F_OK) == 0) ) {
			printf("No executable found.\n");
			exit(1);
		}
		int install_len = snprintf(NULL, 0, "cp %s /usr/local/bin/", exe);
		char *install = malloc(install_len + 1);
		snprintf(install, install_len + 1, "cp %s /usr/local/bin/", exe);
		system(install);
		free(install);
		exit(0);
	}

	if ( VERBOSE ) {
		printf("Setting command length\n");
	}

	int len = snprintf(NULL, 0, "%s %s -o %s %s", cc, cflags, exe, src);

	if ( VERBOSE ) {
		printf("Allocating space for command\n");
	}

	char *command = malloc(len + 1);

	if ( VERBOSE ) {
		printf("Writing command to buffer\n");
	}

	snprintf(command, len + 1, "%s %s -o %s %s", cc, cflags, exe, src);
	
	if ( VERBOSE ) {
		printf("Running command\n");
	}

	system(command);

	if ( VERBOSE ) {
		printf("Freeing command\n");
	}

	free(command);

	if ( VERBOSE ) {
		printf("Freeing JSON object\n");
	}
	
	cJSON_Delete(root);

	return 0;
}
