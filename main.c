#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <wordexp.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>



#define MAXSIZE_CMD		256
int help(int, char **);
int quit(int, char **);
int do_ls(int, char **);
int do_zy(int, char **);
int do_cat(int, char **);
int do_passwd(int, char **);
void read_rcfile();
int do_cd(int, char **);

void read_cmdline(void);

char cmd[MAXSIZE_CMD];
struct termios save;

struct my_command cmd_set[] = {
	{0, NULL, "help", help}, /* help display info about this shell*/
	{0, NULL, "quit", quit}, /* help display info about this shell*/
	{0, NULL, "ls", do_ls},
	{0, NULL, "zy", do_zy},
	{0, NULL, "cat", do_cat},
	{0, NULL, "passwd", do_passwd},
	{0, NULL, "cd", do_cd},
	{0, NULL, NULL, NULL}
};
int do_cd(int argc,char **argv) {
    int result=0;
    if (argc == 1) {
        result = chdir("/home/jnwang");
    }
    if (argc > 1) {
        result = chdir(argv[1]);
    }
    if (result != 0) {
        perror(argv[1]);
    }
    return result;
}

static int terminal_echo_close(struct termios *save)
{
    struct termios tp;
    if (tcgetattr(STDIN_FILENO, &tp) == -1) {
        printf("tcgetattr failed\n");
        goto out;
    }
    *save = tp;
    tp.c_lflag &= ~(ECHO|ECHOE);
    tp.c_lflag &= ~ICANON;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1) {
        printf("tcsetattr failed\n");
        goto out;
    }
out:
    return 0;
}
static int terminal_echo_open(struct termios *save)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, save) == -1) {
        printf("tcsetattr failed\n");
        goto out;
    }
out:
    return 0;
}

int do_passwd(int argc,char **argv) {
    struct termios save;
    char *buffer;
    buffer = (char *)malloc(100);
    terminal_echo_close(&save);
    printf("Enter text:");
    fflush(stdout);
    if (fgets(buffer, 100, stdin) == NULL) {
        printf("Got end-of-file/error on fgets()\n");
        goto out;
    }else {
        printf("%s\n", buffer);
    }
    terminal_echo_open(&save);
out:
    free(buffer);
    return 0;
        
}
int do_cat(int argc,char **argv) {
    int c;
    int i=0;
    int saved_stdin;
    saved_stdin = dup(STDIN_FILENO);
    if (argc > 1) {
         i = argc;//number of arguments.
    }
    i--;//transfer to index of array.
    while(argv[i] && i >= 0) {
        if (i > 0) {
            freopen(argv[i], "r", stdin);
        }
        while((c=getchar())!=EOF) {
            putchar(c);
        }
        i--;
    }
    dup2(saved_stdin, STDIN_FILENO);
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

void read_cmdline(void)
{
	int count = 0;
	int c;
    int pos = 0;
	while ((c=getchar()) != '\n' && count < MAXSIZE_CMD) {
        if (c == 0x1b) {//move cursor
            c = getchar();
            switch (c) {
                case 0x5b:
                    c = getchar();
                    switch (c) {
                    case 'A':
                        break;
                    case 'B':
                        break;
                    case 'C':
                        if (count > pos) { //you could go forward
                            printf("\033[C");
                            pos++;
                        }
                        break;
                    case 'D':
                        if (pos > 0) { //you could go back
                            printf("\033[D");
                            pos--;
                        }
                        break;
                    }
                    break;
            }
            continue;
        }
        if (c == 0x7f) {//delete char
            if (pos > 0) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                cmd[pos]='\0';
                pos--;
                count--;
            }
            continue;
        }
        if (count == pos) {
            cmd[pos++] = c;
            count++;
            putchar(c);
        }
        if (pos < count) {
            int i = count;
            while (i>pos) {
                cmd[i] = cmd[i-1];
                i--;
            }
            cmd[pos++] = (char)c;
            count++;
            for (i=pos-1;i<count;i++) {
                putchar(cmd[i]);
            }
            for (i=count;i>pos;i--) {
                putchar('\b');
            }
        }
	}
    cmd[count]='\0';
    return;
}

void main_loop() {
	int count = 0;
    int ret;

    terminal_echo_close(&save);
	printf("kingshell$ ");
	fflush(stdout);
    read_cmdline();
    terminal_echo_open(&save);
    printf("\n");
	if ( count > MAXSIZE_CMD) {
		perror("command is too long");
		exit(-1);
	}
    ret=findcmd(cmd);//find command as built-in
    if (ret==127) {//this is a external command
	    system(cmd);
        printf("external command is executed\n");
    }
    cmd[0] = '\0';
}
void read_rcfile()
{
    FILE *fp;
    char *buffer;
    char passwd[10];
    fp = fopen("/home/jnwang/.ksrc", "r");
    if (!fp) {
        if (errno == ENOENT) {
            return; 
        }
        perror("fopen");
        return;
    }
    buffer = (char *)malloc(100);
    while(1){
        if (fgets(buffer, 100, fp) == NULL) {
            if (strlen(buffer) == 0) {
                goto out;
            }else {
                perror("fgets");
                goto out;
            }
        }else {
            sscanf(buffer,"passwd=%s",passwd);
            printf("%s\n", passwd);
            buffer[0]='\0';
        }
    }
out:
    free(buffer);
    return;
    
}
int main(int argc,char **argv) {

    read_rcfile();


	while(1) {
		main_loop();
	}
	/*
	 * option 
	 */	
	//	getopt();
}

