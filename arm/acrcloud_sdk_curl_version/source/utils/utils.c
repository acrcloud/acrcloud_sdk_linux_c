#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <net/if_dl.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "../log/log.h"

int get_mac(char *mac_buffer) {
/*	struct ifaddrs *ifaphead = NULL;
	struct ifaddrs *ifap = NULL;
	struct sockaddr_dl *sdl = NULL;
	int result_len = 0, i = 0;

	if (getifaddrs(&ifaphead) != 0) {
		return 0;
	}

	for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
		if ((ifap->ifa_addr->sa_family == AF_LINK)) {
			//if (strcmp(ifap->ifa_name, "lo0") == 0) {
			if (strstr(ifap->ifa_name, "lo") != NULL) {
				continue;
			}
			sdl = (struct sockaddr_dl *) ifap->ifa_addr;
			if (sdl) {
				unsigned char *mac_addr_tmp = LLADDR(sdl);
				for (i=0; i<sdl->sdl_alen; i++) {
					result_len += sprintf(mac_buffer+result_len, "%02X", mac_addr_tmp[i]);
				}
				break;
			}
		}
	}
	//printf("%s  %d %d\n", mac_buffer, result_len, strlen(mac_buffer));
	if (ifaphead) {
		freeifaddrs(ifaphead);
	}
	return result_len;
*/

	 /*struct ifreq ifreq;
	 int sock;

	 if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	 return -1;
	 }
	 strcpy(ifreq.ifr_name, "eth0");
	 if(ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
	 strcpy(ifreq.ifr_name, "em1");
	 if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
	 return -1;
	 }
	 }

	 return sprintf(mac_buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
	 (unsigned char)ifreq.ifr_hwaddr.sa_data[0],
	 (unsigned char)ifreq.ifr_hwaddr.sa_data[1],
	 (unsigned char)ifreq.ifr_hwaddr.sa_data[2],
	 (unsigned char)ifreq.ifr_hwaddr.sa_data[3],
	 (unsigned char)ifreq.ifr_hwaddr.sa_data[4],
	 (unsigned char)ifreq.ifr_hwaddr.sa_data[5]); */
return 0;
}
