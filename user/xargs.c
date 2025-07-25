#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFFSIZE 512

void
execute(char* args[])
{
    if (fork() == 0) {
        exec(args[0], args);
        fprintf(2, "exec %s failed\n", args[0]);
        exit(1);
    }
    else {
        wait(0);
    }
}


int
main(int argc, char* argv[])
{
    char* args[MAXARG];
    char buf[BUFFSIZE];
    int n, arg_count;

    if (argc < 2) {
        fprintf(2, "Usage: xargs command [initial-argument...]\n");
        exit(1);
    }

    // Copy the command and initial argument
    for (int i = 1; i < argc; ++i) {
        args[i - 1] = argv[i];
    }
    arg_count = argc - 1;

    while ((n = read(0, buf, BUFFSIZE)) > 0) {
        char* p = buf;
        char* end = buf + n;
        while (p < end) {
            char* start = p; // start skip the 'space', always point to ahead of a word
            int end_of_line = 0;
            while (p < end && *p != '\n' && *p != ' ')
                p++;
            if (p > start) {
                if (arg_count >= MAXARG - 1) {
                    fprintf(2, "xargs: too many arguments\n");
                    exit(1);
                }
                if (*p == '\n') {
                    end_of_line = 1;
                }
                *p = '\0';
                args[arg_count] = malloc(p - start + 1);
                if (!args[arg_count]) {
                    fprintf(2, "xargs: malloc failed\n");
                    exit(1);
                }
                strcpy(args[arg_count], start);
                arg_count++;
            }

            if (p < end && end_of_line) {
                args[arg_count] = 0;
                execute(args);
                // free the argument we added
                while (arg_count > argc - 1) {
                    arg_count--;
                    free(args[arg_count]);
                }
            }
            p++;
        }
    }

    exit(0);
}