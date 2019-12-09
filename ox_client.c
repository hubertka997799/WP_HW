#include	"unp.h"
#define BUFF_SIZE 1024

char mes[BUFF_SIZE];

char master_name[20];
int gaming=0,master=0,asked=0;

void *get_input(void* sfd){
    int fd=*(int *)sfd;
    char input[BUFF_SIZE],temp[BUFF_SIZE];

    while(1){
        fgets(input,BUFF_SIZE-1,stdin);
        if (!strncmp(input,"!exit",5)) break;
        if (!strncmp(input,"!help",5)){
            printf("!exit --- exit\n");
            printf("!list --- list online players\n");
            printf("!logout --- logout\n");
            printf("!duel [someone] --- ask for a duel\n");
            printf("!join --- to join someone's room\n");
            continue;
        }
        if (!strncmp(input,"!join",5) && asked==1){
            sprintf(input,"!transfer %s !join",master_name);
            send(fd,input,strlen(input)+1,0);
            gaming=1;
            master=0;
            printf("input [row][col](0-2) to play. Ex.11 (center point)\n");
            asked=0;
            continue;
        }
        if (gaming==1){//game mode
            if (master==1){
                sprintf(input,"!master %C%C\n",input[0],input[1]);
            }else{
                sprintf(input,"!transfer %s !player %C%C\n",master_name,input[0],input[1]);
            }
        }
        send(fd,input,strlen(input)+1,0);
    }
    close(fd);
}

int main(){
	int	fd,i,recv_len;
    char *ptr;
	struct sockaddr_in servaddr;
    pthread_t tid;

    if((fd = socket(AF_INET, SOCK_STREAM, 0))<0) exit(3);
	bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(SERV_PORT); 
    inet_aton("127.0.0.1", &servaddr.sin_addr);
    if(connect(fd, (struct sockaddr *)&servaddr,sizeof(struct sockaddr))==-1){  
    printf("connect() error\n"); 
    exit(1); 
    } 
    printf("Connected!\n");
    pthread_create(&tid,NULL,(void*)get_input,&fd);
	while((recv_len=recv(fd,mes,BUFF_SIZE,0))>0){
        if (!strncmp(mes,"!duel",5)){
            printf("%s",mes+6);
            ptr=mes+6;
            while(*ptr!=' ') ptr++;
            *ptr=0;
            strcpy(master_name,mes+6);
            asked=1;
            printf("Type !join to join the game\n");
            continue;
        }
        if (!strncmp(mes,"!start",6)){//only room master receive !start ,player use !join
            gaming=1;
            master=1;
            printf("game start\n");
            printf("input [row][col](0-2) to play. Ex.11 (center point)\n");
        }
        if (!strncmp(mes,"!endgame",8)){
            gaming=0;
        }
        if (!strncmp(mes,"!trans",6)){
            send(fd,mes+7,strlen(mes+7)+1,0);
            continue;
        }
        printf("%s",mes);
		
	}
    close(fd);
    return 0;
}
