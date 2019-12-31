#include "unp.h"
#include <pcap/pcap.h>
typedef unsigned char u_char;
int main(int argc, char const *argv[]){
    pcap_t *handle; 
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *header = NULL;
    const u_char *content = NULL;
    int ret,ippacamount=0,totpac=0;
    errbuf[0]=0;
    int flag=0,i;
    struct tm *t;
    char filename[100]="dump.pcap";
    unsigned char pair[3000][32];
    int pairtot[3000];
    int pairv4[3000];
    for ( i = 0; i < 3000; i++){
        pairtot[i]=1;
        pairv4[i]=0;
    }
    

    if (argc>1)
        strcpy(filename,argv[2]);
    handle=pcap_open_offline(filename,errbuf);

    if (strlen(errbuf)>0)
        printf("error=%s\n",errbuf);
    while (1){
        ret=pcap_next_ex(handle, &header, &content);
        //printf("%d: ",ret);
        if(ret == 1){
            flag=0;
            printf("packet[%d]\t",totpac);
            totpac++;
            t=gmtime(&header->ts.tv_sec);
            printf("[%d-%d-%d %d:%d:%d] ",1900+t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
            printf("mac: Dst=%02x:%02x:%02x:%02x:%02x:%02x",content[0],content[1],content[2],content[3],content[4],content[5]);
            printf(" Src=%02x:%02x:%02x:%02x:%02x:%02x\n",content[6],content[7],content[8],content[9],content[10],content[11]);
            if (content[12]==8 && content[13]==0){
                flag=1;
                printf("IPv4: Src=%d.%d.%d.%d",content[26],content[27],content[28],content[29]);
                printf("  Dst=%d.%d.%d.%d\n",content[30],content[31],content[32],content[33]);
                if (content[23]==0x11){
                    printf("UDP Port: Src=%d",(content[34]<<8)+content[35]);
                    printf(" Dst=%d\n",(content[36]<<8)+content[37]);
                }else if(content[23]==6){
                    printf("TCP Port: Src=%d",(content[34]<<8)+content[35]);
                    printf(" Dst=%d\n",(content[36]<<8)+content[37]);
                }
                flag=1;
                for ( i = 0; i < ippacamount ; i++){
                    if (memcmp(content+26,pair[i],8)==0){
                        pairtot[i]++;
                        flag=0;    
                        break;
                    }    
                }
                if (flag==1){
                    memcpy(pair[ippacamount],content+26,8);
                    pairv4[ippacamount]=1;
                    ippacamount++;
                }
                
            }else if (content[12]==0x86 && content[13]==0xdd){
                flag=1;
                printf("IPv6: Src=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",content[22],content[23],content[24],content[25],content[26],content[27],content[28],content[29],content[30],content[31],content[32],content[33],content[34],content[35],content[36],content[37]);
                printf("  Dst=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",content[38],content[39],content[40],content[41],content[42],content[43],content[44],content[45],content[46],content[47],content[48],content[49],content[50],content[51],content[52],content[53]);
                if (content[20]==0x11){
                    printf("UDP Port: Src=%d",(content[54]<<8)+content[55]);
                    printf(" Dst=%d\n",(content[56]<<8)+content[57]);
                }else if(content[20]==6){
                    printf("TCP Port: Src=%d",(content[54]<<8)+content[55]);
                    printf(" Dst=%d\n",(content[56]<<8)+content[57]);
                }
                flag=1;
                for ( i = 0; i < ippacamount ; i++){
                    if (memcmp(content+22,pair[i],32)==0){
                        pairtot[i]++;
                        flag=0;    
                        break;
                    }    
                }
                if (flag==1){
                    memcpy(pair[ippacamount],content+22,32);
                    pairv4[ippacamount]=0;
                    ippacamount++;
                }
                
            }
            
            printf("\n");
        }
        if(ret == -2)  
            break;
    }
    for ( i = 0; i < ippacamount; i++){
        if(pairv4[i]==1)
            printf("Pair: %d.%d.%d.%d %d.%d.%d.%d %d times\n",pair[i][0],pair[i][1],pair[i][2],pair[i][3],pair[i][4],pair[i][5],pair[i][6],pair[i][7],pairtot[i]);
        else
            printf("Pair: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x %d times\n",pair[i][0],pair[i][1],pair[i][2],pair[i][3],pair[i][4],pair[i][5],pair[i][6],pair[i][7],pair[i][8],pair[i][9],pair[i][10],pair[i][11],pair[i][12],pair[i][13],pair[i][14],pair[i][15],pair[i][16],pair[i][17],pair[i][18],pair[i][19],pair[i][20],pair[i][21],pair[i][22],pair[i][23],pair[i][24],pair[i][25],pair[i][26],pair[i][27],pair[i][28],pair[i][29],pair[i][30],pair[i][31],pairtot[i]);
    }
    
    printf("tot : %d IP pair\n",ippacamount);
    
    return 0;
}

