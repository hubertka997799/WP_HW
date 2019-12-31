#include "unp.h"
#include <pcap/pcap.h>
typedef unsigned char u_char;
int main(int argc, char const *argv[]){
    pcap_t *handle; 
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *header = NULL;
    const u_char *content = NULL;
    int ret,pacamount=0;
    errbuf[0]=0;
    int flag=0;
    struct tm *t;
    char filename[100]="dump.pcap";

    if (argc>1)
        strcpy(filename,argv[2]);
    handle=pcap_open_offline(filename,errbuf);

    if (strlen(errbuf)>0)
        printf("error=%s\n",errbuf);
    while (1){
        ret=pcap_next_ex(handle, &header, &content);
        //printf("%d: ",ret);
        if(ret == 1){
            printf("packet[%d]\t",pacamount);
            t=gmtime(&header->ts.tv_sec);
            printf("[%d-%d-%d %d:%d:%d] ",1900+t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
            printf("mac: Dst=%02x:%02x:%02x:%02x:%02x:%02x",content[0],content[1],content[2],content[3],content[4],content[5]);
            printf(" Src=%02x:%02x:%02x:%02x:%02x:%02x\n",content[6],content[7],content[8],content[9],content[10],content[11]);
            if (content[12]==8 && content[13]==0){
                printf("IPv4: Src=%d.%d.%d.%d",content[26],content[27],content[28],content[29]);
                printf("  Dst=%d.%d.%d.%d\n",content[30],content[31],content[32],content[33]);
                if (content[23]==0x11){
                    printf("UDP Port: Src=%d",(content[34]<<8)+content[35]);
                    printf(" Dst=%d\n",(content[36]<<8)+content[37]);
                }else if(content[23]==6){
                    printf("TCP Port: Src=%d",(content[34]<<8)+content[35]);
                    printf(" Dst=%d\n",(content[36]<<8)+content[37]);
                }
            }else if (content[12]==0x86 && content[13]==0xdd){
                printf("IPv6: Src=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",content[22],content[23],content[24],content[25],content[26],content[27],content[28],content[29],content[30],content[31],content[32],content[33],content[34],content[35],content[36],content[37]);
                printf("  Dst=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",content[38],content[39],content[40],content[41],content[42],content[43],content[44],content[45],content[46],content[47],content[48],content[49],content[50],content[51],content[52],content[53]);
                if (content[20]==0x11){
                    printf("UDP Port: Src=%d",(content[54]<<8)+content[55]);
                    printf(" Dst=%d\n",(content[56]<<8)+content[57]);
                }else if(content[20]==6){
                    printf("TCP Port: Src=%d",(content[54]<<8)+content[55]);
                    printf(" Dst=%d\n",(content[56]<<8)+content[57]);
                }
            }else pacamount--;
            
            printf("\n");
            pacamount++;
        }
        if(ret == -2)  
            break;
    }
    printf("tot : %d IP pack\n",pacamount);
    
    return 0;
}

