#include "my_sock.h"
void sig_child(int signo){
    pid_t pid;
    int stat;
    while((pid=waitpid(-1,&stat,WNOHANG))>0)
       printf("child %d terminated\n",pid);
    return;
}
int main(int argc,char **argv){
    int listenfd, connfd,len=0,flag_exist;
    pid_t childpid;
    socklen_t clilen;
    struct stat st;
    struct sockaddr_in cliaddr,servaddr;
    listenfd = Socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
    Listen(listenfd,LISTENQ);
    Signal(SIGCHLD,sig_child);
    for(;;){
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd,(SA *)&cliaddr,&clilen);
        if((childpid=Fork())==0){
            Close(listenfd);
            char reply[3]="OK",filename[6],recvline[3],recv_over[5],fail[]="fail ",exist[]="exist",buff[20];int fq;
        again:
            Read(connfd,filename,6);
            printf("%s\n",filename);
            flag_exist=access(filename, 0);
            printf("%d\n",flag_exist);
            if(flag_exist<0){
                Write(connfd,fail,sizeof(fail));
                printf("open error");
                goto again;
            }
            else{
            fq = open(filename , O_RDONLY);
            }
            Write(connfd,exist,sizeof(exist));
            if(Read(connfd,recvline,sizeof(recvline))==0)
            {  printf("client terminated prematurely");
               exit(1);
            }
            stat(filename,&st);
            len = st.st_size;
            printf("%d\n",len);
            snprintf(buff,sizeof(buff),"%d",len);
            printf("%s\n",buff);
            Write(connfd, buff, strlen(buff));
            Read(connfd,recvline,sizeof(recvline));
            printf("%s\n",recvline);
            if(strcmp(recvline,"OK")==0){
            if(sendfile(connfd,fq,0,len) < 0)
            {   
                err_sys("send error\n");
                goto again;
                        }
            }
        if(Read(connfd,recv_over,sizeof(recv_over))>0){
            printf("%s\n",recv_over);
        }
        else{ 
            printf("client terminated prematurely");
            exit(1);}
        if(strcmp(recv_over,"over")==0){
           exit(0);
        }
        else if(strcmp(recv_over,"not")==0){
            Write(connfd, reply, sizeof(reply));
            goto again;
        }
        else{
           printf("client error");
           exit(1);
        }
    }
     Close(connfd);
}
}
