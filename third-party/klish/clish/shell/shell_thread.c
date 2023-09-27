/*
 * --------------------------------------
 * shell_thread.c
 * --------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#define _XOPEN_SOURCE
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/select.h>
#include <ctype.h>
#include <sys/ioctl.h>

#include <signal.h>
#if HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <target-sys/bf_sal/bf_sys_sem.h>

#include "target-utils/lub/porting.h"
#include "target-utils/lub/list.h"
#include "target-utils/lub/system.h"
#include "target-utils/lub/log.h"
#include "target-utils/lub/conv.h"
#include "target-utils/clish/shell.h"

typedef struct cli_thread_args {
	FILE *in;
	FILE *out;
	char *install_dir_path;
	char **p4_names;
} cli_thread_args_t;

typedef struct cli_socket_thread_args {
	int nd;
	char *install_dir_path;
	char **p4_names;
} cli_socket_thread_args_t;

typedef struct cli_socket_main_args {
	char *install_dir_path;
	char **p4_names;
	bool local_only;
} cli_socket_main_args_t;

static bf_sys_mutex_t bfshell_thread_mutex;
static bf_sys_sem_t connect_semaphore;
static bool default_running;
static int sd;
static int out_fd;

static void update_terminal_size()
{
	struct winsize ws = {0};

	const bool valid_out_fd = fcntl(out_fd, F_GETFD) != -1 || errno != EBADF;
	if (!valid_out_fd)
		return;

	/* Get stdout's winsize */
	ioctl(fileno(stdout), TIOCGWINSZ, &ws);

	if (ws.ws_col == 0)
		ws.ws_col = 80;

	if (ws.ws_row == 0)
		ws.ws_row = 25;

	/* Set pty's winsize */
	ioctl(out_fd, TIOCSWINSZ, &ws);
}

static void SIGWINCHhandler(int sig)
{
	if (sig == SIGWINCH)
		update_terminal_size();
}

/*--------------------------------------------------------- */
static void *cli_thread_start(void *args)
{
	int running;
	int result = -1;
	clish_shell_t *shell = NULL;

	/* Command line options */
	bool_t lockless = BOOL_FALSE;
	bool_t stop_on_error = BOOL_FALSE;
	bool_t interactive = BOOL_TRUE;
	bool_t quiet = BOOL_FALSE;
	bool_t utf8 = BOOL_FALSE;
	bool_t bit8 = BOOL_FALSE;
	bool_t log = BOOL_FALSE;
	int log_facility = LOG_LOCAL0;
	bool_t dryrun = BOOL_FALSE;
	bool_t dryrun_config = BOOL_FALSE;
	const char *view = getenv("CLISH_VIEW");
	const char *viewid = getenv("CLISH_VIEWID");
	const char *xslt_file = NULL;

	bool_t istimeout = BOOL_FALSE;
	unsigned int timeout = 0;
	const char *histfile = "~/.clish_history";
	char *histfile_expanded = NULL;
	unsigned int histsize = 50;
	clish_sym_t *sym = NULL;

	/* Signal Handling */
	/* SIG_PIPE will be ignored and write(3) will return
	 * EPIPE error. The whole program must not write to
	 * the same pipe and act accordingly, or the signal will
	 * be delivered repeatly.
	 */
	struct sigaction sigpipe_act = {0};
	sigpipe_act.sa_flags = SA_RESTART;
	sigpipe_act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sigpipe_act, NULL);

	cli_thread_args_t *cargs = (cli_thread_args_t *)args;
	FILE *infd = cargs->in;
	FILE *outfd = cargs->out;
	char *install_dir_path = cargs->install_dir_path;
	char **p4_names = cargs->p4_names;
	bf_sys_free(args);

	/* Create shell instance */
	shell = clish_shell_new(infd, outfd, stop_on_error, install_dir_path);
	if (!shell) {
		fprintf(stderr, "Error: Can't run clish.\n");
		goto end;
	}

	/* Store output fd and update terminal size */
	out_fd = fileno(outfd);
	update_terminal_size();

	/* Load the XML files */
	clish_xmldoc_start();
	if (clish_shell_load_scheme(shell, install_dir_path, p4_names, xslt_file))
		goto end;
	/* Set lockless mode */
	if (lockless)
		clish_shell__set_lockfile(shell, NULL);
	/* Set interactive mode */
	if (!interactive)
		clish_shell__set_interactive(shell, interactive);
	/* Set startup view */
	if (view)
		clish_shell__set_startup_view(shell, view);
	/* Set startup viewid */
	if (viewid)
		clish_shell__set_startup_viewid(shell, viewid);
	/* Set UTF-8 or 8-bit mode */
	if (utf8 || bit8)
		clish_shell__set_utf8(shell, utf8);
	else {
#if HAVE_LANGINFO_CODESET
		/* Autodetect encoding */
		if (!strcmp(nl_langinfo(CODESET), "UTF-8"))
			clish_shell__set_utf8(shell, BOOL_TRUE);
#else
		/* The default is 8-bit if locale is not supported */
		clish_shell__set_utf8(shell, BOOL_FALSE);
#endif
	}
	/* Set logging */
	if (log) {
		clish_shell__set_log(shell, log);
		clish_shell__set_facility(shell, log_facility);
	}
	/* Set dry-run */
	if (dryrun)
		clish_shell__set_dryrun(shell, dryrun);
	/* Set idle timeout */
	if (istimeout)
		clish_shell__set_timeout(shell, timeout);
	/* Set history settings */
	clish_shell__stifle_history(shell, histsize);
	if (histfile)
		histfile_expanded = lub_system_tilde_expand(histfile);
	if (histfile_expanded)
		clish_shell__restore_history(shell, histfile_expanded);

	/* Load plugins, link aliases and check access rights */
	if (clish_shell_prepare(shell) < 0)
		goto end;
	/* Dryrun config and log hooks */
	if (dryrun_config) {
		if ((sym = clish_shell_get_hook(shell, CLISH_SYM_TYPE_CONFIG)))
			clish_sym__set_permanent(sym, BOOL_FALSE);
		if ((sym = clish_shell_get_hook(shell, CLISH_SYM_TYPE_LOG)))
			clish_sym__set_permanent(sym, BOOL_FALSE);
	}
#ifdef DEBUG
	clish_shell_dump(shell);
#endif

	/* Execute startup */
	running = clish_shell_startup(shell);
	if (running) {
		fprintf(stderr, "Error: Can't startup clish.\n");
		goto end;
	}

	/* Main loop */
	result = clish_shell_loop(shell);

end:
	/* Cleanup */
	if (shell) {
		clish_shell_strmap_delete(shell);
		if (histfile_expanded) {
			clish_shell__save_history(shell, histfile_expanded);
			lub_free(histfile_expanded);
		}
		clish_shell_delete(shell); // will close infd FILE*
	} else {
		fclose(infd);
	}

	fclose(outfd); // also close initial fds

	/* Stop XML engine */
	clish_xmldoc_stop();

	return NULL;
}

/*--------------------------------------------------------- */
static bool open_pty(int *fdm, int *fds)
{
	int ret;

	*fdm = *fds = -1;
	int master_fd = -1;
	int slave_fd = -1;

	if ((master_fd = posix_openpt(O_RDWR)) < 0) {
		return false;
	}

	if ((ret = grantpt(master_fd)) < 0) {
		close(master_fd);
		return false;
	}

	if ((ret = unlockpt(master_fd)) < 0) {
		close(master_fd);
		return false;
	}

	if ((slave_fd = open(ptsname(master_fd), O_RDWR)) < 0) {
		close(master_fd);
		return false;
	}

	*fdm = master_fd;
	*fds = slave_fd;
	return true;
}

/*--------------------------------------------------------- */
static void close_pty(int fdm, int fds)
{
	close(fdm);
	close(fds);
}

/*--------------------------------------------------------- */
static void *cli_socket_thread_start(void *args)
{
	int nd;
	char *install_dir_path;
	char **p4_names;

	cli_socket_thread_args_t *cargs = (cli_socket_thread_args_t *) args;
	nd = cargs->nd;
	install_dir_path = cargs->install_dir_path;
	p4_names = cargs->p4_names;
	bf_sys_free(args);

	// make the socket non-blocking
	int flags = fcntl(nd, F_GETFL, 0);
	if (flags < 0) {
		close(nd);
		return NULL;
	}
	flags |= O_NONBLOCK;
	if (fcntl(nd, F_SETFL, flags) < 0) {
		close(nd);
		return NULL;
	}

	int fdm, fds;
	if (open_pty(&fdm, &fds) == false) {
		fprintf(stderr, "Unable to open pty\n");
		close(nd);
		return NULL;
	}

	// start the CLI thread with master pty
	cli_thread_args_t *targs;
	targs = bf_sys_malloc(sizeof(cli_thread_args_t));
	targs->in = fdopen(dup(fds), "r");
	if (targs->in == NULL) {
		perror("Unable to initialize input stream");
		close(nd);
		bf_sys_free(targs);
		return NULL;
	}
	targs->out = fdopen(fds, "w");
	if (targs->out == NULL) {
		perror("Unable to initialize output stream");
		close(nd);
		bf_sys_free(targs);
		return NULL;
	}
	targs->install_dir_path = install_dir_path;
	targs->p4_names = p4_names;
	bf_sys_thread_t tid;
	bf_sys_thread_create(&tid, cli_thread_start, targs, 0);
	bf_sys_thread_set_name(tid, "bf_cli_smain");
	bf_sys_thread_detach(tid);

	// stitch the incoming connection with slave pty
	fd_set fdset_r, fdset_w;
	char input, old_input;
	int nbytes, old_nbytes;
	int max_fd = fdm > nd ? fdm : nd;
	int bytes_w = 0;
	bool prev_read_valid = false;
	while (true) {
		int ret;
		FD_ZERO(&fdset_r);
		FD_SET(nd, &fdset_r);
		FD_SET(fdm, &fdset_r);

		FD_ZERO(&fdset_w);
		if (prev_read_valid) {
			FD_SET(nd, &fdset_w);
		}
		// Set client-fd on fdset_write because we want to
		// be notified when it is write-ready again
		ret = select(max_fd + 1, &fdset_r, &fdset_w, NULL, NULL);
		if (ret == -1) {
			perror("select");
			break;
		} else {
			// Check if client-fd is writable again. prev_read_valid
			// is logically equivalent to write_pending
			if (FD_ISSET(nd, &fdset_w) && prev_read_valid) {
				// We only use this fd_isset section to write when a prev
				// write failed in fd_isset(fdb, fdset_r)
				prev_read_valid = false;
				nbytes = old_nbytes;
				input = old_input;
				if (nbytes == 1) {
					if (input == '\n') {
						if (write(nd, "\r\n", 2) != 2) {
							// the following 3 steps are needed wherever
							// we want to wait for nd to be writable again
							prev_read_valid = true;
							old_nbytes = nbytes;
							old_input = input;
						}
					} else {
						if (write(nd, &input, nbytes) != nbytes) {
							prev_read_valid = true;
							old_nbytes = nbytes;
							old_input = input;
						}
					}
				} else {
					if (!nbytes || ((nbytes == -1) && (errno != EAGAIN))) {
						break;
					}
				}
			}
			// Check if client-fd has anything to read.
			if (FD_ISSET(nd, &fdset_r)) {
				nbytes = read(nd, &input, 1);
				if (nbytes == 1) {
					if (write(fdm, &input, nbytes) != nbytes) {
						break;
					}
				} else {
					if (!nbytes || ((nbytes == -1) && (errno != EAGAIN))) {
						break;
					}
				}
			}
			// read only if there is no write pending.
			if (FD_ISSET(fdm, &fdset_r) && !prev_read_valid) {
				nbytes = read(fdm, &input, 1);
				if (nbytes == 1) {
					if (input == '\n') {
						if (write(nd, "\r\n", 2) != 2) {
							prev_read_valid = true;
							old_nbytes = nbytes;
							old_input = input;
						}
					} else {
						if (write(nd, &input, nbytes) != nbytes) {
							prev_read_valid = true;
							old_nbytes = nbytes;
							old_input = input;
						}
					}
				} else {
					if (!nbytes || ((nbytes == -1) && (errno != EAGAIN))) {
						break;
					}
				}
			}
		}
	}

	close(fdm);
	close(nd);
	// fds is closed in clish_shell_pop_file
  return NULL;
}

/*--------------------------------------------------------- */
static void *cli_socket_main(void *args)
{
	int sd, nd;
	int on = 1;
	int ret;
	struct sockaddr_in addr;
	char *install_dir_path;
	char **p4_names;
	bool local_only;
	int last_status = 0;

	cli_socket_main_args_t *cargs = (cli_socket_main_args_t *)args;
	install_dir_path = cargs->install_dir_path;
	p4_names = cargs->p4_names;
	local_only = cargs->local_only;
	bf_sys_free(args);

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error: Unable to open socket.\n");
		bf_sys_sem_post(&connect_semaphore);
		return NULL;
	}

	ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret < 0) {
		fprintf(stderr, "Error: setsockopy failed.\n");
		bf_sys_sem_post(&connect_semaphore);
		return NULL;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (local_only) {
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	} else {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	addr.sin_port = htons(9999);
	ret = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		fprintf(stderr, "Error: bind failed.\n");
		bf_sys_sem_post(&connect_semaphore);
		return NULL;
	}

	ret = listen(sd, 5);
	bf_sys_sem_post(&connect_semaphore);
	if (ret < 0) {
		fprintf(stderr, "listen returned %d with error %d\n", ret, errno);
		return NULL;
	}
	while (nd = accept(sd, NULL, 0)) {
		if (nd < 0) {
			// The condition needs for prevent spam of logs
			// in case reaching the descriptor limit
			if (last_status != errno) {
				fprintf(stderr, "accept %d errored with error %d\n", nd, errno);
			}
			// If error is EINTR, then retry again
			if (errno == EINTR) {
				continue;
			// If errors is EMFILE or ENFILE, means that the limit of fd
			// has been reached max limit, then retry again
			} else if (errno == EMFILE || errno == ENFILE) {
				last_status = errno;
				continue;
			// else something went horribly wrong and quit this thread
			} else {
				perror("UnHandled Error on accept in shell_thread");
				return NULL;
			}
		}
		last_status = 0;
		cli_socket_thread_args_t *targs;
		targs = bf_sys_malloc(sizeof(cli_socket_thread_args_t));
		targs->nd = nd;
		targs->install_dir_path = install_dir_path;
		targs->p4_names = p4_names;
		bf_sys_thread_t tid;
		bf_sys_thread_create(&tid, cli_socket_thread_start, targs, 0);
		bf_sys_thread_set_name(tid, "bf_cli_cln_wrk");
		bf_sys_thread_detach(tid);
	}

	bf_sys_free(install_dir_path);
	int cur_name = 0;
	while (p4_names[cur_name]) {
		if (strcmp(p4_names[cur_name], "switchapi~") == 0) {
			// this string is in data segment, not in heap
			cur_name++;
			continue;
		}
		bf_sys_free(p4_names[cur_name]);
		cur_name++;
	}
	bf_sys_free(p4_names);
	close(sd);
	return NULL;
}

/*--------------------------------------------------------- */
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

void *default_shell(void *args)
{
	(void) args;

	int port = 9999;
	int ip = htonl(INADDR_LOOPBACK);

	int on = 1;
	int ret;
	struct sockaddr_in addr;
	int in_fd = STDIN_FILENO;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret < 0) {
		perror("socket");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return 0;
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
		return 0;
	}

	int flags = fcntl(sd, F_GETFL, 0);
	if (flags < 0) {
		perror("fcntl");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return 0;
	}
	flags |= O_NONBLOCK;
	if (fcntl(sd, F_SETFL, flags) < 0) {
		perror("fcntl");
		if (in_fd != STDIN_FILENO) {
			close(in_fd);
		}
		return 0;
	}

	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	struct sigaction old_winch = {0}, new_winch = {0};
	new_winch.sa_handler = SIGWINCHhandler;
	sigaction(SIGWINCH, &new_winch, &old_winch);

	terminal_mode(TERMINAL_MODE_RAW);

	fd_set fdset;
	int max_fd = sd > in_fd ? sd : in_fd;
	default_running = true;
	while (default_running) {
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
					write(STDOUT_FILENO, "\n", 1);
					kill(getpid(), SIGINT);
				} else if (input == 28) {
					write(STDOUT_FILENO, "\n", 1);
					kill(getpid(), SIGQUIT);
				} else if (input == 26) {
					write(STDOUT_FILENO, "\n", 1);
					raise(SIGTSTP);
				}
				if (write(sd, &input, nbytes) != nbytes) {
					break;
				}
			}
		}
	}

	sigaction(SIGWINCH, &old_winch, NULL);
	close(sd);
	terminal_mode(TERMINAL_MODE_COOKED);
	return 0;
}

/*--------------------------------------------------------- */
void cli_thread_main(char *install_dir_path, char **p4_names, bool local_only)
{
	if (bf_sys_mutex_init(&bfshell_thread_mutex) != 0) {
		return;
	}
	if (bf_sys_sem_init(&connect_semaphore, 0, 0) != 0) {
		return;
	}

	cli_socket_main_args_t *args;
	args = bf_sys_malloc(sizeof(cli_socket_main_args_t));
	args->install_dir_path = install_dir_path;
	args->p4_names = p4_names;
	args->local_only = local_only;

	bf_sys_thread_t tid;
	bf_sys_thread_create(&tid, cli_socket_main, args, 0);
	bf_sys_thread_set_name(tid, "bf_cli_main");
	bf_sys_thread_detach(tid);

	//wait til the server is running before returning
	bf_sys_sem_wait(&connect_semaphore);
	bf_sys_sem_destroy(&connect_semaphore);
}

/*--------------------------------------------------------- */
void cli_run_bfshell()
{
	bf_sys_thread_t tid;
	bf_sys_thread_create(&tid, default_shell, NULL, 0);
	bf_sys_thread_set_name(tid, "default_shell");
	bf_sys_thread_detach(tid);
}

/*--------------------------------------------------------- */
void clish_print(clish_context_t *clish_context, char *str)
{
	clish_shell_t *shell = clish_context__get_shell(clish_context);
	tinyrl_t *tinyrl = clish_shell__get_tinyrl(shell);
	tinyrl_printf(tinyrl, "%s", str);
	if (!tinyrl__get_isatty(tinyrl)) {
		fflush(tinyrl__get_ostream(tinyrl));
	}
}

/*--------------------------------------------------------- */
int clish_thread_trylock() {
	return bf_sys_mutex_trylock(&bfshell_thread_mutex);
}

/*--------------------------------------------------------- */
int clish_thread_unlock() {
	return bf_sys_mutex_unlock(&bfshell_thread_mutex);
}
