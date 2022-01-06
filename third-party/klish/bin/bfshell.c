/*
 * --------------------------------------
 * bfshell.c
 * --------------------------------------
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef enum {
	TERMINAL_MODE_RAW,
	TERMINAL_MODE_COOKED,
} terminal_mode_t;

static void terminal_mode(terminal_mode_t mode) {
	static struct termios saved_termios;
	static terminal_mode_t current_mode = TERMINAL_MODE_COOKED;
	struct termios new_termios;

	if (current_mode == mode) {
		return;
	}

	if (mode == TERMINAL_MODE_RAW) {
		tcgetattr(STDIN_FILENO, &saved_termios);
		new_termios = saved_termios;
		new_termios.c_lflag &= ~(ICANON | ECHO | ISIG);
		new_termios.c_iflag &= BRKINT;
		tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
	}
	if (mode == TERMINAL_MODE_COOKED) {
		tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
	}
	current_mode = mode;
}

/*
 * Parses an ipv4 address into an integer in network order (big endian).
 */
static void parse_ipv4(char *data, char *ip_str) {
	char *str_idx = ip_str;
	size_t data_idx = 0;

	while (*str_idx && data_idx < 4) {
		if (isdigit((unsigned char)*str_idx)) {
			data[data_idx] *= 10;
			data[data_idx] += *str_idx - '0';
		} else {
			data_idx++;
		}
		str_idx++;
	}
}

/*--------------------------------------------------------- */
int main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind;
	int port = 9999;
	int ip = htonl(INADDR_LOOPBACK);
	char *ip_str = NULL;
	char *port_str = NULL;
	char *file_str = NULL;
	char *bfrt_file_str = NULL;
	int interactive = 0;
	static char usage[] = "usage: %s [-f command_file] [-b bfrt_python_file] [-i] [-a ipv4_addr] [-p port]\n";

	int c;
	while ((c = getopt(argc, argv, "a:p:f:b:i")) != -1) {
		switch (c) {
		case 'a':
			ip_str = optarg;
			break;
		case 'p':
			port_str = optarg;
			break;
		case 'f':
			file_str = strdup(optarg);
			break;
		case 'b':
			bfrt_file_str = strdup(optarg);
			break;
		case 'i':
			interactive = 1;
			break;
		default:
			printf(usage, argv[0]);
		}
	}

	if (file_str && bfrt_file_str) {
		printf("Error: cannot pass two files to be run");
		free(file_str);
		free(bfrt_file_str);
		return -1;
	}

	if (port_str) {
		port = strtol(port_str, NULL, 0);
	}

	if (ip_str) {
		ip = 0;
		parse_ipv4((char *) &ip, ip_str);
	}

	int sd;
	int on = 1;
	int ret = 0;
	struct sockaddr_in addr;

	int in_fd = STDIN_FILENO;
	if (file_str) {
		in_fd = open(file_str, O_RDONLY);
		if (in_fd < 0) {
			perror("open");
			if (file_str) free(file_str);
			return -1;
		}
	}
	if (file_str) free(file_str);

	int temp_fd = 0;
	if (bfrt_file_str) {
		char *temp_name = strdup("bfshell_tempXXXXXX");
		temp_fd = mkstemp(temp_name);
		remove(temp_name);
		free(temp_name);
		if (temp_fd < 0) {
			perror("open");
			if (bfrt_file_str) free(bfrt_file_str);
			return -1;
		}
		const char *part_a = "bfrt_python ";
		const char *set_interactive = " true\n";
		const char *part_b = "\nexit\n";
		ret = write(temp_fd, part_a, strlen(part_a));
		if (ret < 0) {
			perror("write");
			close(temp_fd);
			free(bfrt_file_str);
			return -1;
		}
		ret = write(temp_fd, bfrt_file_str, strlen(bfrt_file_str));
		if (ret < 0) {
			perror("write");
			close(temp_fd);
			free(bfrt_file_str);
			return -1;
		}
		if (interactive) {
			ret = write(temp_fd, set_interactive, strlen(set_interactive));
			if (ret < 0) {
				perror("write");
				close(temp_fd);
				free(bfrt_file_str);
				return -1;
			}
		} else {
			ret = write(temp_fd, part_b, strlen(part_b));
			if (ret < 0) {
				perror("write");
				close(temp_fd);
				free(bfrt_file_str);
				return -1;
			}
		}
		lseek(temp_fd, SEEK_SET, 0);
		in_fd = temp_fd;
	}
	if (bfrt_file_str) free(bfrt_file_str);

	sd = socket(AF_INET, SOCK_STREAM, 0);
	ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret < 0) {
		perror("socket");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	ret = connect(sd, (const struct sockaddr *)&addr, sizeof(addr));
	if (ret != 0) {
		perror("connect");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return -1;
	}

	int flags = fcntl(sd, F_GETFL, 0);
	if (flags < 0) {
		perror("fcntl");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return -1;
	}
	flags |= O_NONBLOCK;
	if (fcntl(sd, F_SETFL, flags) < 0) {
		perror("fcntl");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return -1;
	}

	terminal_mode(TERMINAL_MODE_RAW);

	fd_set fdset;
	int max_fd = sd > in_fd ? sd : in_fd;
	while (1) {
		FD_ZERO(&fdset);
		FD_SET(sd, &fdset);
		FD_SET(in_fd, &fdset);
		ret = select(max_fd + 1, &fdset, NULL, NULL, NULL);
		if (ret == -1) {
			break;
		} else {
			char input;
			int nbytes;
			if (FD_ISSET(sd, &fdset)) {
				nbytes = read(sd, &input, 1);
				if (!nbytes || ((nbytes == -1) && (errno != EAGAIN))) {
					break;
				}
				if (write(STDOUT_FILENO, &input, nbytes) != nbytes) {
					break;
				}
			}
			if (FD_ISSET(in_fd, &fdset)) {
				nbytes = read(in_fd, &input, 1);
				if (!nbytes || ((nbytes == -1) && (errno != EAGAIN))) {
					if (in_fd == STDIN_FILENO) {
						break;
					}
					close(in_fd);
					in_fd = STDIN_FILENO;
					continue;
				}
				if (input == 3) {
					printf("\nPlease type Ctrl+\\ for quit from bfshell cli\n");
				}
				if (input == 28) {
					printf("\nQuiting bfshell.\n");
					break;
				}
				if (write(sd, &input, nbytes) != nbytes) {
					break;
				}
			}
		}
	}

	close(sd);
	terminal_mode(TERMINAL_MODE_COOKED);
	return 0;
}
/*--------------------------------------------------------- */
