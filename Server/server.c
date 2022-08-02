/*****************************************************************
//  //  NAME:        Ujjwal Gautam  
//  //
//  //  DATE:        April 07, 2022
//  //
//  //  FILE:        homework8.c
//  //
//  //  DESCRIPTION:
//  //   server.c a TCP server  that simulates TCP three way handshake
//  //
//  ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
/***************************************************
//  //    Function name: main()
//  //
//  //  DESCRIPTION:   A program main function
//  //                 The main functiom declaris, initalozes variables,
//  //                 and calls other sub functions
//  //               
//  //
//  //  Parameters:    none
//  //                               
//  //
//  //  Return values: int- exit status of the function
//  //                 
//  //
//  **************************************************************/

/*structure of the tcp header*/
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

/*function prototype*/

void printWindow(tcp_header *, char *);
void modifyHeader(tcp_header *, char *);
void convertToHost(tcp_header * header, tcp_header *nheader);
void convertToNetwork(tcp_header *, tcp_header *);

int main(int argc, char * argv[])
{
    int port, cfd, ref;
    struct sockaddr_in serveraddress;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    tcp_header * header;
    tcp_header * nheader;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int counter = 1;
    nheader = malloc(sizeof(tcp_header));
    memset(nheader, 0, sizeof(tcp_header));
    header = malloc(sizeof(tcp_header));
    memset(header, 0, sizeof(tcp_header));
 
     if (argc < 2 || argc > 2)
    {
         printf("Usage: ./server <int> \n");
         return -1;
    }
     port = atoi(argv[1]);
    if (sockfd <= 0)
    {
        perror("socket failed");
        exit(0);
    } 
    
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(port);
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");;

    if((bind(sockfd, (struct sockaddr *)&serveraddress, sizeof(serveraddress))) != 0)
    {
        perror("socket binding failed\n");
        exit(0);
    }
     
    if (listen(sockfd, 20) < 0) 
    {
        perror("listening failed\n");
        exit(0);
    }
        
        cfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size); 
        if (cfd == -1)
        {
            perror("Error accepting \n");
        }
    ref = 0;    
    while (counter == 1) 
    {  
    switch(ref)
    {
      case 0:
         recv(cfd, nheader, sizeof(*nheader), 0);
         convertToHost(header, nheader);
         printWindow(header, "syn");
         ref++;
         break;
      case 1:
         modifyHeader(header, "syn_ack");
         printWindow(header, "syn_ack");
         convertToNetwork(header, nheader);
         send(cfd, nheader, sizeof(*nheader), 0);
         ref++;
         break;
      case 2:
         recv(cfd, nheader, sizeof(*nheader), 0);
         convertToHost(header, nheader);
         printWindow(header, "ack");
         free(header);
         free(nheader);
         close(cfd);
         counter++;
         break;
      default:
         break;
      }
  
    }
    close(sockfd);
     return 1;
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

void printWindow(tcp_header * header, char * str)
{
    int width;

    width = 10;
    strcmp(str, "syn") == 0 ? printf("\n****Received SYN from client****") : printf("");
    strcmp(str, "syn_ack") == 0 ? printf("\n**** Sending SYN_ACK to Client****") : printf("");
    strcmp(str, "ack") == 0 ? printf("\n****Received ACK from Client ****") : printf("");
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
void modifyHeader(tcp_header * header, char * str)
{
     uint16_t sPort = header->src_port;
     uint32_t oldseq = header->seq_num;
     int cmp = strcmp(str, "syn_ack");
     switch (cmp)
     { 
         case 0:
         printf("flags %c  \n", header->flags);
         header->flags = header->flags | (1 << 4);
         printf("flags %c  \n", header->flags);

         break;
         
       default:
          break;   
     }
     header->seq_num = header->ack_num + 1;
     header->ack_num = oldseq + 1;
     header->src_port = header->dest_port;
     header->dest_port = sPort;
     

}

  /* used resourse: https://beej.us/guide/bgnet/html/
 https://github.com/zakgilbert/TCP_handshake_simulator/blob/master/server/server.c
 */
 
