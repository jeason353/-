#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define BUFSIZE 1514
 
struct ether_header
{
  unsigned char ether_dhost[6]; //destination mac
  unsigned char ether_shost[6]; //source mac
  unsigned short ether_type;    //type of ethernet
  unsigned short ip_type;       //type of ip protocol
  unsigned char ip[10];         
  unsigned char source[4];
  unsigned char destination[4];
  unsigned short source_port;
  unsigned short destination_port;
};

void ethernet_protocol_callback(unsigned char *argument,const struct pcap_pkthdr *packet_heaher,const unsigned char *packet_content)
{
  // freopen("log.txt", "a+", stdout);   // write to specific file
  unsigned char *source_mac, *des_mac, *ip_string;
  struct ether_header *ethernet_protocol;
  unsigned short ethernet_type, ip_type, source_port, destination_port;
  unsigned char *source, *destination;
  
  ethernet_protocol = (struct ether_header *)packet_content;      // header of data package
  
  ethernet_type = ntohs(ethernet_protocol->ether_type);
  if(ethernet_type == 0x0800){ // judge if is IP protocol or not
    
    source_mac = (unsigned char *)ethernet_protocol->ether_shost;
    des_mac = (unsigned char *)ethernet_protocol->ether_dhost;
    ip_type = ntohs(ethernet_protocol->ip_type);
    source = (unsigned char*)ethernet_protocol->source;
    destination = (unsigned char*)ethernet_protocol->destination;
    source_port = ntohs(ethernet_protocol->source_port);
    destination_port = ntohs(ethernet_protocol->destination_port);
    if(ip_type/4096 == 4 && (destination_port == 8080 || destination_port == 9090)){
      printf("----------------------------------------------------\n");
      printf("Mac Source Address:%02x:%02x:%02x:%02x:%02x:%02x\n",*(source_mac+0),*(source_mac+1),*(source_mac+2),*(source_mac+3),*(source_mac+4),*(source_mac+5));
      printf("Mac Destination Address:%02x:%02x:%02x:%02x:%02x:%02x\n",*(des_mac+0),*(des_mac+1),*(des_mac+2),*(des_mac+3),*(des_mac+4),*(des_mac+5));

      source = (unsigned char*)ethernet_protocol->source;
      destination = (unsigned char*)ethernet_protocol->destination;
      source_port = ntohs(ethernet_protocol->source_port);
      destination_port = ntohs(ethernet_protocol->destination_port);
      printf("Source ip :%d.%d.%d.%d\tport:%d\n", *(source+0), *(source+1), *(source+2), *(source+3), source_port);
      printf("Destination ip :%d.%d.%d.%d\tport:%d\n", *(destination+0), *(destination+1), *(destination+2), *(destination+3), destination_port);
    }
  }
  // usleep(800*1000);
}

int main(int argc, char *argv[])
{
  // freopen("log.txt", "w", stdout);
  char error_content[100];
  pcap_t * pcap_handle;
  unsigned char *mac_string;        
  // unsigned short ethernet_type;    
  char *device = NULL;
  
  // printf("开始抓包\n");

  device = pcap_lookupdev(error_content);
  if(device == NULL)
  {
    perror("pcap_lookupdev");
    exit(-1);
  }
 
  pcap_handle = pcap_open_live(device,BUFSIZE,1,0,error_content);//打开网络接口
    
  if(pcap_loop(pcap_handle,-1,ethernet_protocol_callback,NULL) < 0)
  {
    perror("pcap_loop");
  }
  
  pcap_close(pcap_handle);
  return 0;
}