// In order to use the command "TCP PORT" you need to run "server" first
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/wait.h>

#define INPUT_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3490
#define STD 0
#define TCP 1

int static flag = STD, sock = 0;

int main(){
    char commend[INPUT_SIZE], buff[INPUT_SIZE], src[INPUT_SIZE], dest[INPUT_SIZE];
    int rval;
    char* cwd;
    int org_stdout = dup(STDOUT_FILENO);
    while(1){
        dup2(org_stdout, 1);
        memset(commend, 0, INPUT_SIZE);
        memset(buff, 0, INPUT_SIZE);
        cwd = getcwd(buff, INPUT_SIZE);
        printf("%s$ ", cwd);
        printf("What?\n");
        scanf("%s", commend);
        if(flag == TCP){
            dup2(sock, 1);
        }

        if(strcmp(commend,"ECHO") == 0){
            scanf("%[^\n]", commend);
            printf("%s\n",commend);
            continue;
        }

        else if(strcmp(commend, "TCP") == 0){
            if(scanf("%s", commend) <= 0){
                continue;
            }
            if(strcmp(commend, "PORT")!=0){
                continue;
            }
            flag = TCP;
            struct sockaddr_in serverAddress;
            sock = socket(AF_INET, SOCK_STREAM, 0); //open socket
            if (sock == -1) {
                perror("socket");
                return -1;
            } 
            memset(&serverAddress, 0, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(SERVER_PORT);
            rval = inet_pton(AF_INET, (const char*)SERVER_IP, &serverAddress.sin_addr);//convert to binary 
            if (rval <= 0)
            {
                printf("inet_pton() failed");
                return -1;
            }
            if(connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){ //connect to server
	            printf("connect() failed with error code : %d",errno);
            }
            continue;
        }

        else if(strcmp(commend, "LOCAL") == 0){
            if(flag == TCP){
                printf("EXIT\n");
                close(sock);
                flag = STD;
            }
            continue;
        }

        else if(strcmp(commend, "DIR") == 0){ //credit to https://c-for-dummies.com/blog/?p=3246
            DIR *folder;
            struct dirent *entry;
            int files = 0;
            if((folder = opendir(".")) == NULL){
                printf("unable to open directory");
                continue;
            }
            while((entry = readdir(folder))){
                files++;
                strcat(buff, entry->d_name);
                strcat(buff, "\n");
            }
            printf("%s\n",buff);
            closedir(folder);
        }

        else if(strcmp(commend, "CD") == 0){  // chdir is system call.
            scanf("%[^\n]", commend);
            const char* path = commend +1;
            chdir(path);
        }

        else if(strcmp(commend, "COPY") == 0){ //all the functions are library functions
            memset(src, 0, INPUT_SIZE);
            memset(dest, 0, INPUT_SIZE);
            FILE *srcptr, *destptr;
            scanf("%s", src);
            scanf("%s", dest);
            srcptr = fopen(src, "r");
            destptr = fopen(dest, "w");
            while(fread(buff, 1, 1, srcptr) == 1){
                fwrite(buff, 1, 1, destptr);
            }
            fclose(srcptr);
            fclose(destptr);
            continue;
        }

        else if(strcmp(commend, "DELETE") == 0){ //unlink is a system call
            scanf("%s", buff);
            if(unlink(buff) == -1){
                printf("an error accured");
            }
        }

        else if(strcmp(commend, "EXIT") == 0){
            if(flag == TCP){
                send(sock, commend, strlen(commend), 0);
                close(sock);
            }
            return 0;
        }

        else{  // system is library function.
            // scanf("%[^\n]", buff);
            // strcat(commend, buff);
            // system(commend);
            scanf("%[^\n]", buff);
            int i=1;
            char* args[10]; // assuming that there is no more than 8 flags
            memset(args, '\0', 10*sizeof(char*));
            args[0] = commend;
            char* arg = strtok(buff, " ");
            while(arg != NULL && i<9){
                args[i] = arg;
                printf("%s",arg);
                arg = NULL;
                char* arg = strtok(NULL, " ");
                i++;
            }
            args[i] = NULL;
            pid_t pid = fork();
            if(pid == 0){
                execvp(commend, args);
            }else{
                wait(NULL);
            }
        }
    }
    return 0;
}
