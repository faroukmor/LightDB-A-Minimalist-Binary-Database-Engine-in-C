#include <stdio.h>
#include <unistd.h>

int main(){
printf("Hello before calling fork()!\n");
pid_t ppid = getpid();
fork();
pid_t pid = getpid();
if(ppid != pid) {
printf("Hello from the child process!\n");
}else{
printf("Hello from the parent process!\n");
}
return 0;
}