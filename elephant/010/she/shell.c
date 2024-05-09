#include "shell.h"
#include "fs.h"
#include "stdio.h"
#include "syscall.h"
#include "debug.h"
#include "buildin_cmd.h"

static char cmdline[cmd_len];
char cwd_cache[MAX_PATH_LEN];
char* argv[MAX_ARG_NR];

void print_prompt(void) {
    printf("[piu@localhost %s] $ ",cwd_cache);
}

Bool readline(char* buf,uint_32 count)
{
    char new_char;
    char* bufp = buf;
    while (count--)
    {
        if ((read(stdin,&new_char,1) == -1)) {
            return -1;
        }
        switch(new_char) {
            case 'l'-'a':
                *bufp = 0;
                clear();
                print_prompt();
                printf("%s",buf);
                break;
            case 'u'-'a':
                while (bufp > buf) {
                    putchar('\b');
                    *(--bufp) = 0;
                }
                break;
            case '\n':
            case '\r':
                *bufp++ = 0;
                putchar('\n');
                return true;
            case '\b':
                (bufp == buf) ? bufp : bufp--;
                *bufp = 0;
                putchar('\b');
                break;
            default:
                putchar(new_char);
                *bufp++ = new_char;
                break;
        }
    }

    return true;
}

static int_32 cmd_parse(char* cmd_str,char** argv,char tocken)
{
    char* next = cmd_str;
    char* cmd  = NULL;
    int_32 argc = 0;
    while (*next)
    {
        while (*next == tocken) next++;
        if (*next == NULL) {
            return argc;
        }

        if (argc >= MAX_ARG_NR) {
            printf("exceed max arg number!\n");
            return -1;
        }
        cmd = next;
        while (*next && *next != tocken) next++;
        if (*next != NULL) {
            *next++ = 0;
        }
        argv[argc++] = cmd;
    }
    return argc;
}

void my_shell(void)
{
    cwd_cache[0] = '/';
    while (1) {
        print_prompt();
        memset(cmdline,0,cmd_len);
        readline(cmdline,cmd_len);
        if (cmdline[0] == 0) {
            continue;
        }
        char new_path[MAX_PATH_LEN];
        uint_32 argc = cmd_parse(cmdline,argv,' ');
        if (argc == -1) {
            continue;
        }
        if (!strcmp(argv[0],"cd")) {
            buildin_cd(argc,argv);
        } 
        else if (!strcmp(argv[0],"ls")) {
            buildin_ls(argc,argv);
        }
        else if (!strcmp(argv[0],"rm")) {
            buildin_rm(argc,argv);
        }
        else if (!strcmp(argv[0],"rmdir")) {
            buildin_rmdir(argc,argv);
        }
        else if (!strcmp(argv[0],"mkdir")) {
            buildin_mkdir(argc,argv);
        }
        else if (!strcmp(argv[0],"clear")) {
            buildin_clear(argc,argv);
        }
        else if (!strcmp(argv[0],"ps")) {
            buildin_ps(argc,argv);
        }
        else if (!strcmp(argv[0],"pwd")) {
            buildin_pwd(argc,argv);
        }
        else
        {
            int_32 pid = fork();
            if (pid == -1) {
                PANIC("in shell: fork failed\n");
            }
            if (pid){
                while(1);
            }
            else
            {
                char final_path[MAX_PATH_LEN];
                memset(final_path,0,MAX_PATH_LEN);
                make_abs_path(argv[0],final_path);
                argv[0] = final_path;

                struct stat filestat;
                if (stat(argv[0],&filestat) == -1) {
                    printf("my_shell can't access %s\n",argv[0]);
                } else {
                    if (execv(argv[0],argv) == -1) {
                        printf("execv failed\n");
                        return ;
                    }
                }
                while(1);
            }

        }
    }
    PANIC("my_shell: should not be here");
}

