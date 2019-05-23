#include "my_sock.h"
void recv_cli(char *filename,int sockfd,int size){
  char filebuff[4096]; struct stat st;int len;
  int fd = Open(filename, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);  
  ssize_t length;         
  while(1){
      length = Recv(sockfd, filebuff, sizeof(filebuff), 0);
      if(length<=0){break;}
      Write(fd, filebuff, length);
      stat(filename,&st);
      len = st.st_size;
      if(len==size){break;}
  }
    Close(fd);
}
int main(int argc,char **argv){
    int sockfd,size;struct sockaddr_in servaddr;char	fin[]="over",remain[]="not";char buff[20],exist[6];
    if(argc!=2){
      printf( "Not enough parameter!\n ");
      exit(1);}
    sockfd =Socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    for(int i=1;i<=9;i++){
     char FileName[50],reply[3]="OK",recvline[3];
     char fileName[6]="file_";
     fileName[4]=i+48;        
     if(i==2){fileName[4]=i+60;}      
     sprintf(FileName, "/root/Documents/%s", fileName);
     printf("%s\n",fileName);
     Write(sockfd, fileName, 6);
     bzero(buff,sizeof(buff));
     if(Read(sockfd,exist,sizeof(exist))==0){
       printf("server terminated prematurely");
       exit(1);
     }
     if(strcmp(exist,"exist")!=0){
       printf("%s\n",exist);
       continue;}
     Write(sockfd,reply,sizeof(reply));
     if(Read(sockfd,buff,sizeof(buff))>0){
       printf("%s\n",buff);
       size=atoi(buff);
       printf("%d\n",size);
       Write(sockfd,reply,sizeof(reply));
     }
     else{
        printf("server terminated prematurely");
        exit(1);
     }
     recv_cli(FileName,sockfd,size);
     if(i!=9){
        Write(sockfd,remain,sizeof(remain));
        if(Read(sockfd,recvline,sizeof(recvline))==0){
            printf("server terminated prematurely");
            exit(1);
        }
     }
     else{
        Write(sockfd,fin,sizeof(fin));
     }}
    exit(0);
}