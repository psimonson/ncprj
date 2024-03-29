/* main.c - Main source file for this program.
 ******************************************************************
 * Description: Program to auto generate makefiles for C source
 *              code.
 * Author: Philip R. Simonson
 * Date:   2019/06/05
 ******************************************************************
 */

#ifdef __linux
extern int getcwd(char *name, long size);
#endif

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "file.h"
#include "clogger.h"
/* Some defines */
#define MAXDIRS 30	/* Max directories to recurse through */
#define MAXLINE 260	/* Max size of directory name */
/* Some message defines */
#define ERRMSG  "[ERROR]: "
#define DBGMSG  "[DEBUG]: "
#define WRNMSG  "[WARNG]: "
/* Custom directory structure */
struct dir {
	char name[MAXLINE];
	DIR *p;
};
/* Initialize the custom directory structure.
 * Returns: 1 on error, 0 on success.
 */
int init_dir(struct dir *dir)
{
	memset(dir->name, 0, sizeof(dir->name));
	getcwd(dir->name, sizeof(dir->name));
	if(!(dir->p = opendir("."))) {
		write_log(CLOG0, ERRMSG "Cannot open directory.\n");
		return 1;
	}
	write_log(CLOG0, DBGMSG "Opened directory: %s\n",
		dir->name);
	return 0;
}
/* Gets the next directory in the structure.
 * Returns: Full path to directory.
 */
char *get_dir(struct dir *dir)
{
	static char name[MAXLINE*2];
	struct dirent *p;

	if(!(p = readdir(dir->p)))
		return NULL;
	memset(name, 0, sizeof(name));
	strncpy(name, dir->name, strlen(dir->name)+1);
	strncat(name, "/", 2);
	strncat(name, p->d_name, strlen(p->d_name)+1);
	return name;
}
/* Close directory and clear directory name variable
 */
void close_dir(struct dir *dir)
{
	write_log(CLOG0, DBGMSG "Closing directory: %s\n", dir->name);
	memset(dir->name, 0, sizeof(dir->name));
	closedir(dir->p);
}
/* Get all C source files in directory
 */
void get_sources(struct dir *dir, file_t *file)
{
	struct dirent *p;
	if(get_errori_file(file) != FILE_ERROR_OKAY)
		return;
	printf("Sources in %s:\n", dir->name);
	while((p = readdir(dir->p))) {
		if(!strstr(p->d_name, ".c.") && strstr(p->d_name, ".c")) {
			writef_file(file, "%s ", p->d_name);
			printf("source:%s\n", p->d_name);
			write_log(CLOG0, DBGMSG "Source file [ADDED]: %s\n",
				p->d_name);
		}
	}
	rewinddir(dir->p);
}
/* Gets all files inside of directory
 */
void get_files(struct dir *dir)
{
	const char *p;
	printf("Files in %s:\n", dir->name);
	while((p = get_dir(dir))) {
		if(strstr(p, "/.") || strstr(p, "/.."))
			continue;
		printf("file:%s\n", p);
	}
	rewinddir(dir->p);
}
/* Generate makefile for directory
 */
int gen_make(struct dir *dir)
{
	file_t file;
	init_file(&file);
	open_file(&file, "Makefile", "wt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY)
		goto error;
	writef_file(&file,
		"# Makefile generated by Philip Simonson's NCPRJ project.\n\n"
		"SRCDIR=$(shell pwd)\n"
		"BINDIR=.\n\n"
		"BASENAME=$(shell echo $$(basename $(SRCDIR)))\n"
		"VERSION=0.1\n\nDESTDIR?=\nPREFIX?=usr/local\n\n"
		"CFLAGS+=-std=c89 -Wall -Wextra -Werror "
		"-Wno-unused-parameter\n"
		"LDFLAGS+=\n\nSOURCES=");
	get_sources(dir, &file);
	writef_file(&file, "\nOBJECTS=$(SOURCES:%%.c=%%.c.o)\n"
		"TARGET=$(BASENAME)\n\n.PHONY: all clean\n"
		"all: $(TARGET)\n\n"
		"%%.c.o: %%.c\n\t$(CC) $(CFLAGS) -c -o $@ $<\n\n"
		"$(TARGET): $(OBJECTS)\n\t"
		"$(CC) $(CFLAGS) -o $(BINDIR)/$(TARGET) "
		"$(OBJECTS) $(LDFLAGS)\n\n"
		"clean:\n\t@echo Cleaning directory...\n\t"
		"@rm -f *~ *.log $(OBJECTS) $(TARGET)\n");
	write_log(CLOG0, DBGMSG "Created file: %s\n", get_name_file(&file));
	close_file(&file);
	return 0;

error:
	write_log(CLOG0, ERRMSG "Cannot create makefile for directory.\n");
	close_file(&file);
	return 1;
}
/* Where program execution begins
 */
int main(int argc, char **argv)
{
	struct dir dir;
	init_logger();
	open_log(CLOG0, "configure.log");
	init_dir(&dir);
	gen_make(&dir);
	close_dir(&dir);
	close_log(CLOG0);
	return 0;
}
