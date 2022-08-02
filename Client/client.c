#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <string.h>
#include <netdb.h>

typedef struct TCP_HEADER {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t offset;
    unsigned char flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
}tcp_header;

void printWindow(tcp_header *, char *);
void modifyHeader(tcp_header *, char *);
void convertToHost(tcp_header * header, tcp_header *nheader);
void convertToNetwork(tcp_header *, tcp_header *);
tcp_header * createHeader(int port, int src_port);
int main(int argc, char const* argv[])
{
    int connectStatus;    
    int sockd;
    char strData[50];
    tcp_header * header;
    tcp_header * nheader;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct sockaddr_in my_addr;
    socklen_t len;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    len = sizeof(my_addr);
    nheader = malloc(sizeof(tcp_header));
    memset(nheader, 0, sizeof(tcp_header));
    int port;
    int ref = 0;
    if (argc < 2 || argc > 2)
    {
         printf("Usage: ./client <port> \n");
         return -1;
    }
    sockd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockd == -1)
    {
        perror("Error creating socket \n");
        exit(0);
    }
    if (getaddrinfo(NULL, argv[1], &hints, &servinfo) != 0)
    {
        perror("getaddrinfo error \n");
        exit(0);
    }
    port = atoi(argv[1]);
    connectStatus = connect(sockd, servinfo->ai_addr, servinfo->ai_addrlen);
    freeaddrinfo(servinfo);
    if (connectStatus == -1)
    {
        perror("Connect error \n");
        exit(0);
    }
    	
   getsockname(sockd, (struct sockaddr *) &my_addr,&len); 
   header = createHeader(port, ntohs(my_addr.sin_port));
   printWindow(header,"syn\0");   
   convertToNetwork(header, nheader);
   while(1)
 {
    switch(ref)
    {    
      case 0:
         send(sockd, nheader, sizeof(*nheader), 0);
         ref++;
         break;
      case 1: 
         recv(sockd, nheader, sizeof(*nheader), 0);
	 convertToHost(header, nheader);
         printWindow(header, "syn_ack");
         ref++;
         break;
      case 2:
         modifyHeader(header, "ack");
         printWindow(header, "ack");
         convertToNetwork(header, nheader);
         send(sockd, nheader, sizeof(*nheader), 0);
         free(header);
         free(nheader);
         exit(0);
      default:
         break;
      } 
}   
 return 0;
}

void modifyHeader(tcp_header * header, char * str)
{
     uint16_t sPort = header->dest_port;
     uint32_t oldseq = header->seq_num;
     int cmp = strcmp(str, "ack");
     switch (cmp)
     {
         case 0:
         header->flags = header->flags & (1 << 4);
         break;

       default:
          break;
     }
     header->seq_num = header->ack_num + 1;
     header->ack_num = oldseq + 1;
     header->dest_port = header->src_port;
     header->src_port = sPort;   
}

void printWindow(tcp_header * header, char * str)
{
    int width;
    width = 10;
     strcmp(str, "syn") == 0 ? printf("\n****Sending SYN to Server****") : printf("");
    strcmp(str, "syn_ack") == 0 ? printf("\n**** Received SYN_ACK from Server****") : printf("");
    strcmp(str, "ack") == 0 ? printf("\n****Sending ACK to Server ****") : printf("");
  
    printf("\n ***************************************"); 
    printf("\n %04X %04X %08X %08X ",header->src_port,  header->dest_port,  header->seq_num,  header->ack_num);
    printf("\n %02X %02X %02X %02X %02X %02X %02X %2X",header->offset, header->offset,((header->flags) & (1 << 5)) ? '1' : '0',((header->flags) & (1 << 4)) ? '1' : '0', ((header->flags) & (1 << 3)) ? '1' : '0',((header->flags) & (1 << 2)) ? '1' : '0',((header->flags) & (1 << 1)) ? '1' : '0',((header->flags) & (1 << 0)) ? '1' : '0');
    printf("\n %02X %02X %02X",header->window,header->checksum,header->urgent);
    printf("\n****************************************");
    printf("\n TCP HEADER:");
    printf("\n  Source Port:               %*u", width, header->src_port);
    printf("\n  Destination Port:          %*u", width, header->dest_port);
    printf("\n  Sequence Number:           %*u", width, header->seq_num);
    printf("\n  Acknowledgement Number:    %*u", width, header->ack_num); 
    printf("\n  Data Offset:    %*d", width+10, header->offset);
    printf("\n  Reserved:    %*d", width+13, header->offset);
    printf("\n  Flags:                     ");
    printf("\n     URG is %*c", width, ((header->flags) & (1 << 5)) ? '1' : '0');
    printf("\n     ACK is %*c", width, ((header->flags) & (1 << 4)) ? '1' : '0');
    printf("\n     PSH is %*c", width, ((header->flags) & (1 << 3)) ? '1' : '0');
    printf("\n     RST is %*c", width, ((header->flags) & (1 << 2)) ? '1' : '0');
    printf("\n     SYN is %*c", width, ((header->flags) & (1 << 1)) ? '1' : '0');
    printf("\n     FIN is %*c", width, ((header->flags) & (1 << 0)) ? '1' : '0');
    printf("\n  Window  :                  %*u", width, header->window);
    printf("\n  Checksum:                  %*x", width, header->checksum);
    printf("\n  Urgent:                    %*u", width, header->urgent);
    printf("\n ***************************************\n"); 
} 
tcp_header * createHeader(int port, int src_port)
{
    tcp_header * rtn = malloc(sizeof(tcp_header));
    memset(rtn, 0, sizeof(tcp_header));
    rtn->seq_num = 100;
    rtn->ack_num = 1000;
    rtn->dest_port = port;
    rtn->src_port = src_port;
   /* printf("%d    %d   \n", port, src_port);*/
    rtn->offset = 0;
    rtn->checksum = -1;
    rtn->window = 17520;
    rtn->flags = (rtn->flags) ^ (1 << 1); 
    return rtn;
}

void convertToNetwork(tcp_header * header, tcp_header* nheader)
{
    nheader->seq_num = htonl(header->seq_num);
    nheader->ack_num = htonl(header->ack_num);
    nheader->src_port = htons(header->src_port);
    nheader->dest_port = htons(header->dest_port);
    nheader->checksum = htons(header->checksum);
    nheader->window = htons(header->window);
    nheader->flags = header->flags;
    nheader->offset = header->offset;

}
void convertToHost(tcp_header * header, tcp_header *nheader)
{
    header->seq_num = ntohl(nheader->seq_num);
    header->ack_num = ntohl(nheader->ack_num);
    header->src_port = ntohs(nheader->src_port);
    header->dest_port = ntohs(nheader->dest_port);
    header->checksum = ntohs(nheader->checksum);
    header->window = ntohs(nheader->window);
    header->flags = nheader->flags;
    header->offset = nheader->offset;
}
/* Reference: https://beej.us/guide/bgnet/html/#socket
https://github.com/zakgilbert/TCP_handshake_simulator/blob/master/client/client.c
 * */
