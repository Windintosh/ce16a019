#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>   
#include<string.h>    
#include<netinet/udp.h>   			// udp header를 제공받기위한 헤더
#include<netinet/tcp.h>   			// tcp header를 제공받기위한 헤더
#include<netinet/ip.h>    			// ip header를 제공받기한 헤더
#include<netinet/if_ether.h>  			// ETH_P_ALL 을 제공받기위한 헤더
#include<net/ethernet.h>
#include<netinet/ip_icmp.h>   // icmp header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

//소켓관련 헤더
void ProcessPacket(unsigned char* , int); 	// protocol 고유번호를 받고 실행하는 함수
void print_ip_header(unsigned char* , int);
void print_ip_header_two(unsigned char* , int); 	// ip header 출력 함수
void print_tcp_packet(unsigned char * , int ); 	// tcp header 출력 함수
void print_udp_packet(unsigned char * , int ); 	// udp header 출력 함수
void print_icmp_packet(const u_char * , int );
void PrintData (unsigned char* , int);
void PrintDataTwo (unsigned char* , int); 		// Appdata 출력 함수

FILE *logfile, *logfiletwo;

struct sockaddr_in source,dest; 		// sockaddr_in 구조체를 선언(목적지와 출발지 저장)

int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j; 
int fsel, psel;
 

int main() {

    int saddr_size , data_size; 

    struct sockaddr saddr; 					// 소켓의 주소를 표현하는 구조체 생성

         							// IP 포트, IP 주소 저장

    unsigned char *buffer = (unsigned char *) malloc(65536); 	// buffer변수 동적할당

    printf("select filter: 1.pre-filter 2. post-filter \n");
    printf(">");
    scanf("%d", &fsel);
    switch(fsel) {
    	case 1:
    		printf("select protocol: 1. HTTP(TCP) 2. IP(UDP) 3. ICMP \n");
    		printf(">");
    		scanf("%d", &psel);
    		switch(psel){
				case 1:
					logfile=fopen("http tcp result.txt","a");
					break;
				case 2:
					logfile=fopen("ip udp result.txt","a");
					break;
				case 3:
					logfile=fopen("icmp result.txt","a");
					break;
				default:
				printf("invalid value");
			}
			break;
    	case 2:
    		psel=4;
    		logfile=fopen("combined result.txt","a");     			//txt 파일에 정보 넣기 위함
    		break;
    	default:
    		printf("invalid value \n");
	}
	
    if(logfile==NULL) 

    {
        printf("Unable to create txt file.");
		return 0;
    }


    printf("Starting Program\n"); 					// 프로그램 실행시 출력

     
    // 데이터 링크 계층으로부터 패킷 수신
    // socket() 함수로 소켓을 생성하여 반환
    // 매개변수 int domain, int type, int protocol (통신 방식, 데이터 전송 형태, 프로토콜 사용 지정)

    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));	

     

    if(sock_raw < 0)

    {

        perror("Socket Error");

        return 1;

    }


    while(1)

    {

        saddr_size = sizeof saddr;

       
        //recvfrom 함수 호출로 sock_raw 라는 소켓을 통해 패킷을 수신

        data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);

      

        if(data_size < 0 )

        {

            printf("Recvfrom error , failed to get packets\n");

            return 1;

        }

        ProcessPacket(buffer , data_size); 			//buffer에 저장된 값들과 데이터수를 인자로 전달 

    }


    close(sock_raw);

    printf("Finished");

    return 0;

}


// protocol 고유번호를 받고 실행하는 함수

void ProcessPacket(unsigned char* buffer, int size)
{

    //ethernet header 를 제외하고ip header 부분의 패킷을 얻음
    struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ++total;
    //프로토콜 고유번호 체크
    switch (iph->protocol) 
    {         
        case 1:  //ICMP Protocol
        	++icmp;
	    	print_icmp_packet( buffer , size);
        	break;
         
        case 2:  //IGMP Protocol
            ++igmp;
            break;
            
        case 6:  //TCP Protocol
            ++tcp;
            print_tcp_packet(buffer , size);
            break;

        case 17: //UDP Protocol
            ++udp;
            print_udp_packet(buffer , size);
            break;

        default: //다른 Protocol
            ++others;
            break;
    }
    //어떤 프로토콜이 들어왔는지 출력
	if(psel ==1)
		printf("Processing TCP Packets : %d \r", tcp);
	else if(psel==2)
		printf("Processing UDP Packets : %d \r", udp);
	else if(psel==3)
		printf("Processing ICMP Packets : %d \r", icmp);
	else if(psel==4) {
		printf("Processing Packets... \n");
		printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);
		}
	else {
		printf("Invalid Protocol \n");
		return 0;
		}
}

void print_ethernet_header(const u_char *Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
     
    fprintf(logfile , "\n");
    fprintf(logfile , "Ethernet Header\n");
    fprintf(logfile , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    fprintf(logfile , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    fprintf(logfile , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
    

}

void print_ethernet_header_two(const u_char *Buffer, int Size)
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
     
    fprintf(logfiletwo , "\n");
    fprintf(logfiletwo , "Ethernet Header\n");
    fprintf(logfiletwo , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    fprintf(logfiletwo , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    fprintf(logfiletwo , "   |-Protocol            : %u \n",(unsigned short)eth->h_proto);
    

}

//ip header를 출력하기 위한 함수

void print_ip_header(unsigned char* Buffer, int Size) 

{	
	print_ethernet_header(Buffer , Size);

    unsigned short iphdrlen;  					//ip헤더 길이를 저장할 변수선언

    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) ); 


    iphdrlen =iph->ihl*4; 					//ip_header_len 안의 값들과 4를 곱해서 원래 헤더길이를 구한다(32bit)   

    memset(&source, 0, sizeof(source));  			//소스에서 소스의 크기만큼 0으로 변환

    source.sin_addr.s_addr = iph->saddr; 

     

    memset(&dest, 0, sizeof(dest));  				//목적지에서 목적지의 크기만큼 0으로 변환

    dest.sin_addr.s_addr = iph->daddr;

     

    fprintf(logfile , "\n");

    fprintf(logfile , "< IP Header >\n");

    fprintf(logfile , "   IP Version        : %d\n",(unsigned int)iph->version);

    fprintf(logfile , "   IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);

    fprintf(logfile , "   Type Of Service   : %d\n",(unsigned int)iph->tos);

    fprintf(logfile , "   IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));

    fprintf(logfile , "   Identification    : %d\n",ntohs(iph->id));

    fprintf(logfile , "   TTL      : %d\n",(unsigned int)iph->ttl);

    fprintf(logfile , "   Protocol : %d\n",(unsigned int)iph->protocol);

    fprintf(logfile , "   Checksum : %d\n",ntohs(iph->check));

    fprintf(logfile , "   Source IP        : %s\n",inet_ntoa(source.sin_addr));

    fprintf(logfile , "   Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
	
}

void print_ip_header_two(unsigned char* Buffer, int Size) 

{	
	print_ethernet_header_two(Buffer , Size);

    unsigned short iphdrlen;  					//ip헤더 길이를 저장할 변수선언

    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr) ); 


    iphdrlen =iph->ihl*4; 					//ip_header_len 안의 값들과 4를 곱해서 원래 헤더길이를 구한다(32bit)

     

    memset(&source, 0, sizeof(source));  			//소스에서 소스의 크기만큼 0으로 변환

    source.sin_addr.s_addr = iph->saddr; 

     

    memset(&dest, 0, sizeof(dest));  				//목적지에서 목적지의 크기만큼 0으로 변환

    dest.sin_addr.s_addr = iph->daddr;

     

    fprintf(logfiletwo , "\n");

    fprintf(logfiletwo , "< IP Header >\n");

    fprintf(logfiletwo , "   IP Version        : %d\n",(unsigned int)iph->version);

    fprintf(logfiletwo , "   IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);

    fprintf(logfiletwo , "   Type Of Service   : %d\n",(unsigned int)iph->tos);

    fprintf(logfiletwo , "   IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));

    fprintf(logfiletwo , "   Identification    : %d\n",ntohs(iph->id));

    fprintf(logfiletwo , "   TTL      : %d\n",(unsigned int)iph->ttl);

    fprintf(logfiletwo , "   Protocol : %d\n",(unsigned int)iph->protocol);

    fprintf(logfiletwo , "   Checksum : %d\n",ntohs(iph->check));

    fprintf(logfiletwo , "   Source IP        : %s\n",inet_ntoa(source.sin_addr));

    fprintf(logfiletwo , "   Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
	
}
 

//tcp header를 출력하기 위한 함수

void print_tcp_packet(unsigned char* Buffer, int Size)

{
	switch(psel){
		case 1:
			break;
		case 2:
			return;
		case 3:
			return;
		case 4:
			break;
		default:
		printf("invalid value");
		return;
	}
	
    unsigned short iphdrlen;  					//ip프로토콜의 길이를 정의하기 위한 필드

    struct iphdr *iph = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );

    iphdrlen = iph->ihl*4;

     

    struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen + sizeof(struct ethhdr));

             

    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

     

    fprintf(logfile , "\n\n----------------------- TCP Packet ----------------------- \n");  

         
    //ip프로토콜의 정보를 log파일에 적는 함수를 선언하면서 하위 프로토콜의 정보를 정의

    print_ip_header(Buffer,Size);  

         

    fprintf(logfile , "\n");

    fprintf(logfile , "< TCP Header >\n");

    fprintf(logfile , "   Source Port      : %u\n",ntohs(tcph->source));

    fprintf(logfile , "   Destination Port : %u\n",ntohs(tcph->dest));

    fprintf(logfile , "   Sequence Number    : %u\n",ntohl(tcph->seq));

    fprintf(logfile , "   Acknowledge Number : %u\n",ntohl(tcph->ack_seq));

    fprintf(logfile , "   Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);

    fprintf(logfile , "   Urgent Flag          : %d\n",(unsigned int)tcph->urg);

    fprintf(logfile , "   Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);

    fprintf(logfile , "   Push Flag            : %d\n",(unsigned int)tcph->psh);

    fprintf(logfile , "   Reset Flag           : %d\n",(unsigned int)tcph->rst);

    fprintf(logfile , "   Synchronise Flag     : %d\n",(unsigned int)tcph->syn);

    fprintf(logfile , "   Finish Flag          : %d\n",(unsigned int)tcph->fin);

    fprintf(logfile , "   Window         : %d\n",ntohs(tcph->window));

    fprintf(logfile , "   Checksum       : %d\n",ntohs(tcph->check));

    fprintf(logfile , "   Urgent Pointer : %d\n",tcph->urg_ptr);

 	fprintf(logfile , "                        DATA Dump                         ");

	PrintData(Buffer,iphdrlen);

	fprintf(logfile,"TCP Header\n");

	PrintData(Buffer+iphdrlen,tcph->doff*4);

	fprintf(logfile,"Data Payload\n");

    PrintData(Buffer + header_size , Size - header_size );

    fprintf(logfile , "\n");                         

    fprintf(logfile , "\n-----------------------------------------------------------\n");
    
	if(psel==4){
	logfiletwo=fopen("http tcp result.txt","a");
	fprintf(logfiletwo , "\n\n----------------------- TCP Packet ----------------------- \n");  

         
    //ip프로토콜의 정보를 log파일에 적는 함수를 선언하면서 하위 프로토콜의 정보를 정의

    print_ip_header_two(Buffer,Size);  
        
    fprintf(logfiletwo , "\n");

    fprintf(logfiletwo , "< TCP Header >\n");

    fprintf(logfiletwo , "   Source Port      : %u\n",ntohs(tcph->source));

    fprintf(logfiletwo , "   Destination Port : %u\n",ntohs(tcph->dest));

    fprintf(logfiletwo , "   Sequence Number    : %u\n",ntohl(tcph->seq));

    fprintf(logfiletwo , "   Acknowledge Number : %u\n",ntohl(tcph->ack_seq));

    fprintf(logfiletwo , "   Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);

    fprintf(logfiletwo , "   Urgent Flag          : %d\n",(unsigned int)tcph->urg);

    fprintf(logfiletwo , "   Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);

    fprintf(logfiletwo , "   Push Flag            : %d\n",(unsigned int)tcph->psh);

    fprintf(logfiletwo , "   Reset Flag           : %d\n",(unsigned int)tcph->rst);

    fprintf(logfiletwo , "   Synchronise Flag     : %d\n",(unsigned int)tcph->syn);

    fprintf(logfiletwo , "   Finish Flag          : %d\n",(unsigned int)tcph->fin);

    fprintf(logfiletwo , "   Window         : %d\n",ntohs(tcph->window));

    fprintf(logfiletwo , "   Checksum       : %d\n",ntohs(tcph->check));

    fprintf(logfiletwo , "   Urgent Pointer : %d\n",tcph->urg_ptr);

  	fprintf(logfiletwo , "                        DATA Dump                         ");

	PrintDataTwo(Buffer,iphdrlen);

	fprintf(logfiletwo,"TCP Header\n");

	PrintDataTwo(Buffer+iphdrlen,tcph->doff*4);

	fprintf(logfiletwo,"Data Payload\n");

    PrintDataTwo(Buffer + header_size , Size - header_size );

    fprintf(logfiletwo , "\n");                         

    fprintf(logfiletwo , "\n-----------------------------------------------------------\n");
    fclose(logfiletwo);
	}
}

 

//udp header를 출력하기 위한 함수

void print_udp_packet(unsigned char *Buffer , int Size)

{
	switch(psel){
		case 1:
			return;
		case 2:
			break;
		case 3:
			return;
		case 4:
			break;
		default:
			printf("invalid value");
			return;
	}
     
    unsigned short iphdrlen; 					// ip프로토콜의 헤더 길이를 나타내기 위한 변수

     

    struct iphdr *iph = (struct iphdr *)(Buffer +  sizeof(struct ethhdr));

    iphdrlen = iph->ihl*4; 					// ip_header_len 안의 값들과 4를 곱해서 ip헤더길이를 구함

     

    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen  + sizeof(struct ethhdr));

     

    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;

     

    fprintf(logfile , "\n\n----------------------- UDP Packet ----------------------- \n");

    print_ip_header(Buffer,Size);           

    fprintf(logfile , "\n< UDP Header >\n");

    fprintf(logfile , "Source Port      : %d\n" , ntohs(udph->source));

    fprintf(logfile , "Destination Port : %d\n" , ntohs(udph->dest));

    fprintf(logfile , "UDP Length       : %d\n" , ntohs(udph->len));

    fprintf(logfile , "\n");

    fprintf(logfile , "\n-----------------------------------------------------------\n");
	
	if(psel==4){
	logfiletwo=fopen("ip udp result.txt","a");
	
	fprintf(logfiletwo , "\n\n----------------------- UDP Packet ----------------------- \n");
	
    print_ip_header_two(Buffer,Size);           

    fprintf(logfiletwo , "\n< UDP Header >\n");

    fprintf(logfiletwo , "Source Port      : %d\n" , ntohs(udph->source));

    fprintf(logfiletwo , "Destination Port : %d\n" , ntohs(udph->dest));

    fprintf(logfiletwo , "UDP Length       : %d\n" , ntohs(udph->len));

    fprintf(logfiletwo , "\n");

    fprintf(logfiletwo , "\n-----------------------------------------------------------\n");
    
    fclose(logfiletwo);
	}
}

 void print_icmp_packet(const u_char * Buffer , int Size)
{
	switch(psel){
		case 1:
			return;
		case 2:
			return;
		case 3:
			break;
		case 4:
			break;
		default:
			printf("invalid value");
			return;
			}
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)(Buffer  + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;
     
    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen  + sizeof(struct ethhdr));
     
    int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;
     
    fprintf(logfile , "\n\n***********************ICMP Packet*************************\n"); 
     
    print_ip_header(Buffer , Size);
             
    fprintf(logfile , "\n");
         
    fprintf(logfile , "ICMP Header\n");
    fprintf(logfile , "   |-Type : %d",(unsigned int)(icmph->type));
             
    if((unsigned int)(icmph->type) == 11)
    {
        fprintf(logfile , "  (TTL Expired)\n");
    }
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    {
        fprintf(logfile , "  (ICMP Echo Reply)\n");
    }
     
    fprintf(logfile , "   |-Code : %d\n",(unsigned int)(icmph->code));
    fprintf(logfile , "   |-Checksum : %d\n",ntohs(icmph->checksum));
    fprintf(logfile , "\n");
 
    fprintf(logfile , "IP Header\n");
    PrintData(Buffer,iphdrlen);
         
    fprintf(logfile , "UDP Header\n");
    PrintData(Buffer + iphdrlen , sizeof icmph);
         
    fprintf(logfile , "Data Payload\n");    
     
    //Move the pointer ahead and reduce the size of string
    PrintData(Buffer + header_size , (Size - header_size) );
     
    fprintf(logfile , "\n###########################################################");
    
    if(psel==4){
		logfiletwo=fopen("icmp result.txt","a");
	
		fprintf(logfiletwo , "\n\n***********************ICMP Packet*************************\n"); 
     
	    print_ip_header_two(Buffer , Size);
             
    	fprintf(logfiletwo , "\n");
         
    	fprintf(logfiletwo , "ICMP Header\n");
    	fprintf(logfiletwo , "   |-Type : %d",(unsigned int)(icmph->type));
             
   		if((unsigned int)(icmph->type) == 11)
    		{
        		fprintf(logfiletwo , "  (TTL Expired)\n");
    		}
    	else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
    		{
        fprintf(logfiletwo , "  (ICMP Echo Reply)\n");
    		}
     
    fprintf(logfiletwo , "   |-Code : %d\n",(unsigned int)(icmph->code));
    fprintf(logfiletwo , "   |-Checksum : %d\n",ntohs(icmph->checksum));
    fprintf(logfiletwo , "\n");
 
    fprintf(logfiletwo , "IP Header\n");
    PrintDataTwo(Buffer,iphdrlen);
         
    fprintf(logfiletwo , "UDP Header\n");
    PrintDataTwo(Buffer + iphdrlen , sizeof icmph);
         
    fprintf(logfiletwo , "Data Payload\n");    
     
    //Move the pointer ahead and reduce the size of string
    PrintDataTwo(Buffer + header_size , (Size - header_size) );
     
    fprintf(logfiletwo , "\n###########################################################");
	
	fclose(logfiletwo);
	}
}

// Appdata 출력 함수

void PrintData (unsigned char* data , int Size)

{
    int i , j;
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   					// 만약 한 행이 16개가 되면 다음 줄로 넘어감
        {
            fprintf(logfile , "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) 		// 숫자 또는 알파벳일 때 출력
                    fprintf(logfile , "%c",(unsigned char)data[j]); 
                else fprintf(logfile , ".");  			// 그렇지 않으면 .(점) 을 출력
            }
            fprintf(logfile , "\n");
        } 
        if(i%16==0) fprintf(logfile , "   ");
            fprintf(logfile , " %02X",(unsigned int)data[i]);
                 
        if( i==Size-1)  
        {
            for(j=0;j<15-i%16;j++) 
            {
              fprintf(logfile , "   ");
            }
            fprintf(logfile , "         ");
            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) 
                {
                  fprintf(logfile , "%c",(unsigned char)data[j]);
                }
                else
                {
                  fprintf(logfile , ".");
                }
            }
            fprintf(logfile ,  "\n" );
        }
    }
}

void PrintDataTwo (unsigned char* data , int Size)

{
    int i , j;
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   					// 만약 한 행이 16개가 되면 다음 줄로 넘어감
        {
            fprintf(logfiletwo , "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) 		// 숫자 또는 알파벳일 때 출력
                    fprintf(logfiletwo , "%c",(unsigned char)data[j]); 
                
                else fprintf(logfiletwo , ".");  			// 그렇지 않으면 .(점) 을 출력
            }
            fprintf(logfiletwo , "\n");
        } 

        if(i%16==0) fprintf(logfile , "   ");

            fprintf(logfiletwo , " %02X",(unsigned int)data[i]);
        if( i==Size-1)  

        {
            for(j=0;j<15-i%16;j++) 
            {
              fprintf(logfiletwo , "   ");
            }
            fprintf(logfiletwo , "         ");
            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) 
                {
                  fprintf(logfiletwo , "%c",(unsigned char)data[j]);
                }
                else
                {
                  fprintf(logfiletwo , ".");
                }
            }
            fprintf(logfiletwo ,  "\n" );
        }
    }
}
