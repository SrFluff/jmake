#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "buildinfo.h"

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
	
	const char *VERSION = "1.5.5";

	if ( argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) ) {
		printf("Usage: jmake [flag] [command] <project>\n");
		printf("Optional flags:\n");
		printf("\t--version, -v Return the current version.\n");
		printf("\t--help, -h    Print this help message.\n");
		printf("Optional commands:\n");
		printf("\tproject <project> Generates a make.json file, with the executable name set to <project>.\n");
		printf("\tinstall           Copies the executable to /usr/local/bin/.\n");
		printf("\tclean             Remove the executable.\n");
		printf("\texport            Print the contents of make.json in Makefile syntax.\n");
		exit(0);
	}

	if ( argc == 2 && ( strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0 ) ) {
		printf("%s (%s - %s)\n",VERSION,PLATFORM,COMPILER);
		exit(0);
	}

	if ( argc == 3 && ( strcmp(argv[1], "project") == 0 ) ) {
		cJSON *root = cJSON_CreateObject();
		cJSON_AddStringToObject(root,"cc","CC");
		cJSON_AddStringToObject(root,"cflags","");
		cJSON_AddStringToObject(root,"src","main.c");
		cJSON_AddStringToObject(root,"exe",argv[2]);
		cJSON_AddStringToObject(root,"install_dir","/usr/local/bin");
		char *root_string = cJSON_Print(root);
		FILE *file_ptr = fopen("make.json","w");

		if ( file_ptr == NULL ) {
			printf("jmake: Error when trying to open make.json.");
			exit(1);
		}

		fprintf(file_ptr,"%s",root_string);
		fclose(file_ptr);
		cJSON_Delete(root);
		free(root_string);
		exit(0);
	}

	if ( argc == 2 && ( strcmp(argv[1], "clean") == 0 ) ) {
		if ( !(access("make.json", F_OK) == 0) ) {
			printf("jmake: No make.json.\n");
			exit(1);
		}
		char *data = readFile("make.json");
		cJSON *root = cJSON_Parse(data);
		free(data);
		cJSON *exe_ptr = cJSON_GetObjectItem(root, "exe");
		char *exe = exe_ptr->valuestring;

		if ( !(access(exe,F_OK) == 0) ) {
			printf("jmake: '%s' no such file or directory.\n",exe);
			exit(1);
		}

		if ( !(remove(exe) == 0) ) {
			printf("jmake: Failed to remove '%s'.\n",exe);
			exit(1);
		}

		exit(0);
	}

	if ( !(access("make.json", F_OK) == 0) ) {
		printf("jmake: No make.json.\n");
		exit(1);
	}

	char *data = readFile("make.json");
	if ( data == NULL ) {
		printf("jmake: Failed to read make.json.\n");
		exit(1);
	}
	cJSON *root = cJSON_Parse(data);
	free(data);

	char *cc;
	char *cflags;
	char *src;
	char *exe;
	char *install_dir;

	if ( cJSON_HasObjectItem(root, "cc") ) {
		cJSON *cc_ptr = cJSON_GetObjectItem(root, "cc");
		cc = cc_ptr->valuestring;
	} else {
		cc = "CC";
		printf("jmake: Warning, no compiler specified in make.json, falling back to '%s'.\n",cc);
	}
	
	if ( cJSON_HasObjectItem(root, "cflags") ) {
		cJSON *cflags_ptr = cJSON_GetObjectItem(root, "cflags");
		cflags = cflags_ptr->valuestring;
	} else {
		cflags = "";
		printf("jmake: Warning, no cflags specified in make.json, falling back to '%s'.\n",cflags);
	}

	if ( cJSON_HasObjectItem(root, "src") ) {
		cJSON *src_ptr = cJSON_GetObjectItem(root, "src");
		src = src_ptr->valuestring;
	} else {
		src = "main.c";
		printf("jmake: Warning, no src specified in make.json, falling back to '%s'.\n",src);
	}

	if ( cJSON_HasObjectItem(root, "exe") ) {
		cJSON *exe_ptr = cJSON_GetObjectItem(root, "exe");
		exe = exe_ptr->valuestring;
	} else {
		exe = "main";
		printf("jmake: Warning, no exe specified in make.json, falling back to '%s'.\n",exe);
	}

	if ( argc == 2 && strcmp(argv[1],"export") == 0 ) {
		if ( cJSON_HasObjectItem(root, "install_dir") ) {
			cJSON *install_dir_ptr = cJSON_GetObjectItem(root, "install_dir");
			install_dir = install_dir_ptr->valuestring;
		} else {
			install_dir = "/usr/local/bin";
			printf("jmake: Warning, no install_dir specified in make.json, falling back to '%s'.\n",install_dir);
		}

		printf("COMPILER = %s\n",cc);
		printf("CFLAGS = %s\n",cflags);
		printf("SOURCE = %s\n",src);
		printf("TARGET = %s\n",exe);
		printf("INSTALLDIR = %s\n\n",install_dir);
		
		printf("all:\n");
		printf("\t$(COMPILER) $(CFLAGS) -o $(TARGET) $(SOURCE)\n");
		printf("install:\n");
		printf("\tinstall -m 755 $(TARGET) $(INSTALLDIR)\n");
		printf("clean:\n");
		printf("\trm $(TARGET)\n");
		cJSON_Delete(root);
		exit(0);
	}

	if ( argc == 2 && strcmp(argv[1],"install") == 0 ) {

		if ( cJSON_HasObjectItem(root, "install_dir") ) {
			cJSON *install_dir_ptr = cJSON_GetObjectItem(root, "install_dir");
			install_dir = install_dir_ptr->valuestring;
		} else {
			printf("jmake: No install_dir specified in make.json.\n");
			exit(1);
		}

		if ( !(access(exe, F_OK) == 0) ) {
			printf("jmake: No executable found.\n");
			exit(1);
		}

		size_t path_size = snprintf(NULL, 0, "%s/%s", install_dir, exe) + 1; // Generates formatted path.
		char *path = malloc(path_size); // Makes a string with the size of the path.
		snprintf(path, path_size, "%s/%s", install_dir, exe); // Puts the formatted path string in the place in memory.

		FILE *file = fopen(exe, "rb"); // Opens the executable for reading.
		if ( file == NULL ) {
			printf("jmake: Failed to open '%s'.\n",exe);
			exit(1);
		}
		
		fseek(file, 0, SEEK_END); // Goes to the end to get file size.
		long exe_size = ftell(file); // Gets the size by ftell.

		rewind(file); // Goes to the start to be able to read properly.
		char *buffer = malloc(exe_size); // Allocates memory to store binary content.
		fread(buffer,exe_size,1,file); // Reads content into buffer.
		fclose(file);

		FILE *install_ptr = fopen(path,"wb"); // Opens install file for writing.
		if ( install_ptr == NULL ) {
			printf("jmake: Failed to open '%s'.\n",path);
			free(buffer);
			free(path);
			exit(1);
		}
		fwrite(buffer, exe_size, 1, install_ptr); // Writes from the buffer to the install stream.
		fclose(install_ptr);
		chmod(path,0755); // Ensures it's executable.
		free(buffer);
		free(path);
		exit(0);
	}

	int len = snprintf(NULL, 0, "%s %s -o %s %s", cc, cflags, exe, src);
	char *command = malloc(len + 1);
	snprintf(command, len + 1, "%s %s -o %s %s", cc, cflags, exe, src);
	system(command);
	free(command);
	cJSON_Delete(root);
	return 0;
}
