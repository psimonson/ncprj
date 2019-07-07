/* main.c - Small program designed to create a C project with a
 *          Makefile.
 ****************************************************************
 * Author: Philip R. Simonson
 * Date  : 2019/06/05
 ****************************************************************
 */

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/* Custom Includes */
#include "file.h"
#include "clogger.h"

/* Some defines */
#define MAXDIRS 10
#define MAXLINE 260

/* Message defines */
#define ERRMSG "[ERROR]: "
#define DBGMSG "[DEBUG]: "
#define WRNMSG "[WARNG]: "

/* Gets user input filling a string buffer.
 */
int read_line(char *s, int size)
{
	int c,i;
	for(i=0; i < size && (c = getchar()) != EOF && c != '\n'; i++)
		*s++ = c;
	*s = 0;
	return i;
}
/* String to lowercase.
 */
void str_to_lower(char *s, const char *t)
{
	char *p;
	for(p = (char*)t; *p != 0; p++)
		*s++ = *p;
	*s = 0;
}
/* Create new directory.
 */
void make_dir(const char *name)
{
	if(mkdir(name, 0755) != 0) {
		write_log(CLOG0, ERRMSG "Cannot create directory.\n");
		return;
	}
	write_log(CLOG0, DBGMSG "Directory %s created.\n", name);
	chdir(name);
}
/* Create files in current directory.
 */
void gen_file(const char *filename)
{
	char buf[MAXLINE];
	file_t file;

	init_file(&file);
	open_file(&file, filename, "wt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY) {
		write_log(CLOG0, ERRMSG "Cannot create file: %s\n", filename);
		return;
	}
	str_to_lower(buf, filename);
	if(!strcmp(buf, "main.c")) {
		writef_file(&file, "#include <stdio.h>\n\n"
			"/* Execution begins here for program.\n */\n"
			"int main(int argc, char **argv)\n{\n\treturn 0;\n"
			"}\n");
	}
	close_file(&file);
	write_log(CLOG0, DBGMSG "File %s created.\n", filename);
}
/* Print message for main menu.
 */
void print_menu()
{
	printf(
		" *** Main Menu ***\n"
		" *****************\n"
		" 0. Exit program.\n"
		" 1. Create new directory.\n"
		" 2. Set working directory.\n"
		" 3. Add source files.\n"
		" 4. Finish (write changes).\n\n"
		"What do you want to do? ");
}
/* Loop getting a character at a time.
 */
void main_loop()
{
	char buf[MAXLINE];
	int c,nfiles;
	do {
		print_menu();
		c = getchar();
		if(c == EOF) { putchar('\n'); return; }
		getchar();
		switch(c) {
		case '0':
			/* exit main loop */
			return;
		case '1':
			/* create new directory */
			printf("Please enter directory name: ");
			if(read_line(buf, sizeof(buf)) > 0) {
				printf("Directory %s created.\n", buf);
				make_dir(buf);
				break;
			}
			printf("Nothing was entered!\n");
		break;
		case '2':
			/* set working directory */
			printf("Please set current directory: ");
			if(read_line(buf, sizeof(buf)) > 0) {
				printf("Changing to directory: %s\n", buf);
				chdir(buf);
				break;
			}
			printf("Nothing was entered!\n");
		break;
		case '3':
			/* add files */
			printf("Enter file count: ");
			if(read_line(buf, sizeof(buf)) <= 0)
				break;
			nfiles = atoi(buf);
			if(!nfiles) break;
			do {
				printf("Enter filename: ");
				if(read_line(buf, sizeof(buf)) <= 0)
					break;
				printf("Creating file: %s\n", buf);
				gen_file(buf);
			} while(--nfiles > 0);
		break;
		case '4':
			/* finish project */
			printf("Running makefile generation.\n");
			if(system("confproj") != 0) {
				write_log(CLOG0, ERRMSG "Generating Makefile!");
				printf("Error generating Makefile...\n");
				break;
			}
			write_log(CLOG0, DBGMSG "Generating done.\n");
			printf("Finished generating makefile.\n");
		break;
		default:
			/* unknown selection */
			printf("Unknown selection: '%c'\n", c);
		break;
		}
	} while(c != '0');
}
/* Program main execution.
 */
int main()
{
	init_logger();
	open_log(CLOG0, "creatproj.log");
	main_loop();
	close_log(CLOG0);
	return 0;
}
