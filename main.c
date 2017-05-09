#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <wordexp.h>


#define MAXSIZE_CMD		20
int help(int, char **);
int quit(int, char **);
int do_ls(int, char **);
int do_zy(int, char **);
int do_cat(int, char **);

char cmd[MAXSIZE_CMD];

struct my_command cmd_set[] = {
	{0, NULL, "help", help}, /* help display info about this shell*/
	{0, NULL, "quit", quit}, /* help display info about this shell*/
	{0, NULL, "ls", do_ls},
	{0, NULL, "zy", do_zy},
	{0, NULL, "cat", do_cat},
	{0, NULL, NULL, NULL}
};

int do_cat(int argc,char **argv) {
    int c;
    while((c=getchar())!=EOF) {
        putchar(c);
    }
    return 0;
}

int do_zy(int argc,char **argv) {
    printf("Hello, zy!!!\n");
    return 0;
}
int do_ls(int argc,char **argv) {
	int fd;	
	DIR *dir;
	struct dirent *stru_dir;
	if ((fd = open(".", O_RDONLY)) < 0) {
		perror("opening a dir ");
	}

	if ( (dir = fdopendir(fd)) ==  NULL) {
		perror("open a dir by fd");
	}


	while((stru_dir = readdir(dir)) != NULL ) {
		if (strcmp(stru_dir->d_name, ".") != 0 
		   && strcmp(stru_dir->d_name, "..") != 0)
		{	
			printf("%s\t", stru_dir->d_name);
		}	
	}
	printf("\n");
    return 0;
}



int quit(int argc,char **argv) {

	exit(-1);

}
int help(int argc,char **argv) {

	printf("This is a program be used for teach \n\
You can type some command example: \n\
cd , ls , quit and so on \n");
	return 0;

}
int findcmd(char *user_cmdline)
{
    struct my_command *p = cmd_set;
    int ret;
    wordexp_t pp;
    wordexp(user_cmdline, &pp, 0);

    while(p->name) {
        if (!strncmp(p->name, pp.we_wordv[0], strlen(pp.we_wordv[0]))){
               ret=p->cmd(pp.we_wordc, pp.we_wordv);
               return ret;
        }
        p++;
    }
    return 127;
}

void main_loop() {
	int count = 0;
	char c;
    int ret;

	printf("kingshell$ ");
	fflush(stdout);
	while ((c=getchar())!='\n' && count < MAXSIZE_CMD) {
		cmd[count++] = c;
	}
	if ( count > MAXSIZE_CMD) {
		perror("command is too long");
		exit(-1);
	}
	cmd[count] = '\0'; //end of string(cmd)
    ret=findcmd(cmd);
    //if (ret==127) {
	//    system(cmd);
    //}
}

int main(int argc,char **argv) {

	while(1) {
		main_loop();
	}
	/*
	 * option 
	 */	
	//	getopt();
}

