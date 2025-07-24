#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc, char* argv[])
{
    int parent_to_child[2];   // zero for read, one for write
    int child_to_parent[2];
    char buf[1];
    int pid;
    
    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        printf("pipe failed\n");
        exit(1);
    }
    pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }
    if (pid == 0) {     // child
        close(parent_to_child[1]);  // close write
        close(child_to_parent[0]);  // close read
        read(parent_to_child[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(child_to_parent[1], buf, 1);
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } 
      // parent
    buf[0] = 'p';
    close(parent_to_child[0]);
    close(child_to_parent[1]);
    write(parent_to_child[1], buf, 1);
    read(child_to_parent[0], buf, 1);
    printf("%d: received pong\n", getpid());
    close(parent_to_child[1]);
    close(child_to_parent[0]);
    wait(0);
    exit(0);    
    
}