#include <cwcore.h>
#include <cwhost.h>

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>

int cwhost_cansocket_create() {
	return socket(PF_CAN, SOCK_RAW, CAN_RAW);
}

int cwhost_cansocket_bind(int fd, const char* interface) {
    int res;

    struct ifreq interface_request;
    strcpy(interface_request.ifr_name, interface);
    res = ioctl(fd, SIOCGIFINDEX, &interface_request);
    if (res == -1) return errno;

    int index = interface_request.ifr_ifindex;
    struct sockaddr_can can_address;
    can_address.can_family = AF_CAN;
    can_address.can_ifindex = index;

	// need to cast the address to the more generic sockaddr type 
	// before passing it to bind
    struct sockaddr* address = (struct sockaddr*)(&can_address);
    res = bind(fd, address, sizeof(struct sockaddr_can));
    if (res == -1) return errno;

	return 0;
}


int cwhost_cansocket_set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;

	int res = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (res) return errno;

	return 0;
}

int cwhost_cansocket_transmit_ready(int fd) {
	struct pollfd query;
	query.fd = fd;
	query.events = POLLOUT;

	int number_ready = poll(&query, 1, 0);
	if (number_ready == 0) return 0;

	if (query.revents & POLLERR) return -1 * errno;
	else if (query.revents & POLLOUT) return 1;
	else return 0;
}

int cwhost_cansocket_receive_ready(int fd) {
	struct pollfd query;
	query.fd = fd;
	query.events = POLLIN;

	int number_ready = poll(&query, 1, 0);
	if (number_ready == 0) return 0;

	if (query.revents & POLLERR) return -1 * errno;
	else if (query.revents & POLLIN) return 1;
	else return 0;
}
