#include	"unp.h"
#define BUFF_SIZE 16192

struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"exe","text/plain" },
    {0,0} };


void sig_chld(int signo) {  
  int status;  
  while(waitpid(-1, &status, WNOHANG) > 0);  
}  


int main(){
	int					listenfd, connfd,ret,filefd,i,len;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
    char buffer[BUFF_SIZE+1],buffer1[BUFF_SIZE+1];
    char index_name[] = "index.html";
    char *file,*ftype,*fstr,*boundary,tmp[1000];

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0) exit(3);
    printf("Socket %d\n",listenfd);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);
    
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr))<0) exit(3);
    printf("bind\n");
    if(listen(listenfd, LISTENQ)<0) exit(3);
    printf("Listen\n");

	signal(SIGCHLD, sig_chld);	/* must call waitpid() */
    
	while(1){
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) continue;
		

		if ( (childpid = fork()) == 0) {	/* child process */
            
			close(listenfd);	/* close listening socket */
            read(connfd,buffer,BUFF_SIZE);
            printf("%s",buffer);
            if (!strncmp(buffer,"GET",3)){ //GET sth...
                if (buffer[5]==' '){
                    file=index_name;
                }else{
                    i=6;
                    while(buffer[i]!=' ') i++;
                    buffer[i]=0;
                    file=buffer+5;
                }
                printf("TRY TO SEND %s\n",file);

                ftype=file;
                while(*ftype!='.') ftype++;
                ftype++;
                fstr = (char *)0;
                for(i=0;extensions[i].ext!=0;i++) {
                    len = strlen(extensions[i].ext);
                    if(!strncmp(ftype, extensions[i].ext, len)) {
                        fstr = extensions[i].filetype;
                        break;
                    }
                }
                if(fstr == 0) fstr = extensions[i-1].filetype;
                filefd=open(file,O_RDONLY);
                printf("HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
                sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
                write(connfd,buffer,strlen(buffer));
                while((ret=read(filefd,buffer,BUFF_SIZE))>0){
                    write(connfd,buffer,ret);
                }
                close(filefd);
            }
            if (!strncmp(buffer,"POST",4)){
                boundary=strstr(buffer,"boundary=");//find boundary
                boundary+=9;
                i=0;
                while(*(boundary+i)!='\r' && *(boundary+i)!='\n') i++;
                *(boundary+i)=0;

                fstr=strstr(boundary+i+1,boundary);//find filename
                file=strstr(fstr,"filename=");
                file+=10;
                i=0;
                while (*(file+i)!=34) i++; // 34 = " 
                *(file+i)=0;
                filefd=open(file,O_WRONLY|O_CREAT);

                fstr=strstr(file+i+1,"\r\n\r\n");//find beginnig of file
                fstr+=4;
                strcpy(tmp,boundary);
                while((file=strstr(fstr,tmp))==NULL){//send file
                    write(filefd,fstr,BUFF_SIZE-((long)fstr-(long)buffer));
                    read(connfd,buffer,BUFF_SIZE);
                    //printf("\n\n%s\n\n",buffer);
                    fstr=buffer;
                }
                write(filefd,fstr,(long)file-(long)fstr-4);
                close(filefd);
                write(connfd,"HTTP/1.0 201 Created\r\n",22);
                
            }
            
            

		}
		close(connfd);			/* parent closes connected socket */
	}
    return 0;
}
