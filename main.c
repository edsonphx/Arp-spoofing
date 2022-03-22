#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ARPHRD_ETHER 1
#define TRUE 1

int main()
{
	printf("Creating socket...\n");

	int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));

	if (socket_fd <= 0)
	{
		perror("socket()\n");
		return 1;
	}

	printf("Socket created!\n");

	char ethernet_frame[60] = 
	{
		// Ethernet Header - 14 bytes
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest Mac Address
		0xb8, 0x27, 0xed, 0xbf, 0x0e, 0xf4, // Source Mac Address
		0x08, 0x06,							// Ether type | Arp - 0x0806

		// ARP Frame - 28 bytes
		0x00, 0x01,							// Hardware Type | Ethernet - 0x0001
		0x08, 0x00,							// Protocol Type | Ipv4 - 0x0800 
		0x06,								// Mac Address Len
		0x04,								// Ipv4 Len
		0x00, 0x02,							// Opcode | 01 - Request | 02 - Reply
		0xb8, 0x27, 0xed, 0xbf, 0x0e, 0xf4, // Source Mac Address
		0xc0, 0xa8, 0x0f, 0x01,				// Source Ip Address
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest Mac Address
		0xff, 0xff, 0xff, 0xff,				// Dest Ip Address

		// Padding - 18 bytes
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	struct sockaddr_ll device;
	memset(&device, 0, sizeof (device));
	
	device.sll_family = AF_PACKET;
	device.sll_protocol = htons(ETH_P_ALL);
  	device.sll_halen = 6;
	device.sll_pkttype = PACKET_BROADCAST;
	device.sll_hatype = htons(ARPHRD_ETHER);
	device.sll_ifindex = if_nametoindex("eth0");
	
	while (TRUE)
	{
		printf("Sending data...\n");
		int bytes_read = sendto(socket_fd, ethernet_frame, 60, 0, (const struct sockaddr*)&device, sizeof(device));
		if (bytes_read <= 0)
		{
			perror("sendto()\n");
			return 1;
		}

		printf("Data sended!\n");
	}

	return 0;
}