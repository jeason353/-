#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libnet.h>
 
int main(int argc, char *argv[])
{
	char send_msg[1000] = "";
	char err_buf[100] = "";
	libnet_t *lib_net = NULL;
	int lens = 0;
	libnet_ptag_t lib_t = 0;
	unsigned char src_mac[6] = {0x00, 0x0c, 0x29, 0x85, 0xec, 0xd7};//发送者网卡地址
	unsigned char dst_mac[6] = {0x00, 0x0c, 0x29, 0x85, 0xec, 0xd7};//接收者网卡地址‎
    char *src_ip_str = "192.168.12.130"; //源主机IP地址
    char *dst_ip_str = "192.168.12.130"; //目的主机IP地址
	unsigned long src_ip,dst_ip;
	int res = 0;
 
	lens = sprintf(send_msg, "%s", "This is a upd package.");
 
 	lib_net = libnet_init(LIBNET_LINK, NULL, err_buf);	//初始化
	if(NULL == lib_net)
	{
		perror("libnet_init");
		exit(-1);
	}
 
	src_ip = libnet_name2addr4(lib_net,src_ip_str,LIBNET_RESOLVE);	//将字符串类型的ip转换为顺序网络字节流
	dst_ip = libnet_name2addr4(lib_net,dst_ip_str,LIBNET_RESOLVE);
 
	lib_t = libnet_build_udp(8080, 8080, 8+lens, 0, send_msg, lens, lib_net, 0);
 
	lib_t = libnet_build_ipv4( 20+8+lens, 0, 500, 0, 10, 17, 0, src_ip, dst_ip, NULL, 0, lib_net, 0);
 
	lib_t = libnet_build_ethernet((u_int8_t *)dst_mac, (u_int8_t *)src_mac, 0x800, NULL, 0, lib_net, 0);
	res = libnet_write(lib_net);	//发送数据包	
	if(res == -1)
	{
		perror("libnet_write");
		exit(-1);
	}
 
 	usleep(1000*1000);
	libnet_destroy(lib_net);	//销毁资源

 	lib_net = NULL;
	lens = sprintf(send_msg, "%s", "This is a tcp package.");
	lib_net = libnet_init(LIBNET_LINK, NULL, err_buf);
	if(lib_net == NULL){
		perror("libnet_init");
		exit(-1);
	}

	lib_t = libnet_build_tcp(9090, 9090, 1, 0, 0, 2, 0, 0, lens + 20, send_msg, lens, lib_net, 0);
	lib_t = libnet_build_ipv4(lens + 20 + 20, 0, 500, 0, 10, 17, 0, src_ip, dst_ip, NULL, 0, lib_net, 0);
	lib_t = libnet_build_ethernet((u_int8_t *)dst_mac, (u_int8_t *)src_mac, 0x800, NULL, 0, lib_net, 0);

	res = libnet_write(lib_net);
	if(res == -1)
	{
		perror("libnet_write");
		exit(-1);
	}
 
	libnet_destroy(lib_net);	//销毁资源
	
	printf("----ok-----\n");
	return 0;
 }