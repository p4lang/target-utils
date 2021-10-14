/*
 * shell/private.h - private interface to the shell class
 */
#include "target_utils/lub/bintree.h"
#include "target_utils/lub/list.h"
#include "target_utils/tinyrl/tinyrl.h"
#include "target_utils/clish/shell.h"
#include "target_utils/clish/pargv.h"
#include "target_utils/clish/var.h"
#include "target_utils/clish/action.h"
#include "target_utils/clish/plugin.h"
#include "target_utils/clish/udata.h"

/*-------------------------------------
 * PRIVATE TYPES
 *------------------------------------- */

/*-------------------------------------------------------- */

/*
 * iterate around commands
 */
typedef struct {
	const char *last_cmd;
	clish_nspace_visibility_e field;
} clish_shell_iterator_t;

/* this is used to maintain a stack of file handles */
typedef struct clish_shell_file_s clish_shell_file_t;
struct clish_shell_file_s {
	clish_shell_file_t *next;
	FILE *file;
	char *fname;
	unsigned int line;
	bool_t stop_on_error; /* stop on error for file input  */
};

typedef struct {
	char *line;
	clish_view_t *view;
	lub_bintree_t viewid;
	clish_pargv_t *pargv; /* Saved pargv structure */
	char *cmd; /* Command name without prefix */
	char *prefix; /* Prefix string if exists */
} clish_shell_pwd_t;

/* Context structure */
struct clish_context_s {
	clish_shell_t *shell;
	const clish_command_t *cmd;
	clish_pargv_t *pargv;
	const clish_action_t *action;
};

/* Shell structure */
struct clish_shell_s {
	lub_bintree_t view_tree; /* Tree of views */
	lub_bintree_t ptype_tree; /* Tree of ptypes */
	lub_bintree_t var_tree; /* Tree of global variables */

	/* Hooks */
	clish_sym_t *hooks[CLISH_SYM_TYPE_MAX]; /* Callback hooks */
	bool_t hooks_use[CLISH_SYM_TYPE_MAX]; /* Is hook defined */

	clish_view_t *global; /* Reference to the global view. */
	clish_command_t *startup; /* This is the startup command */
	unsigned int idle_timeout; /* This is the idle timeout */

	/* Watchdog */
	clish_command_t *wdog; /* Watchdog command */
	unsigned int wdog_timeout; /* Watchdog timeout */
	bool_t wdog_active; /* If watchdog is active now */

	clish_shell_state_e state; /* The current state */
	char *overview; /* Overview text for this shell */
	tinyrl_t *tinyrl; /* Tiny readline instance */
	clish_shell_file_t *current_file; /* file currently in use for input */
	clish_shell_pwd_t **pwdv; /* Levels for the config file structure */
	unsigned int pwdc;
	int depth;
	konf_client_t *client;
	char *lockfile;
	char *default_shebang;
	char *fifo_temp; /* The template of temporary FIFO file name */
	struct passwd *user; /* Current user information */
	const char *install_dir;

	/* Boolean flags */
	bool_t interactive; /* Is shell interactive. */
	bool_t log; /* If command logging is enabled */
	int log_facility; /* Syslog facility */
	bool_t dryrun; /* Is this a dry-running */
	bool_t default_plugin; /* Use or not default plugin */

	/* Plugins and symbols */
	lub_list_t *plugins; /* List of plugins */
	lub_list_t *syms; /* List of all used symbols. Must be resolved. */

	/* Userdata list holder */
	lub_list_t *udata;

	/* strmap for shell varaibles in plugins */
	lub_bintree_t *strmap;
};

/**
 * Initialise a command iterator structure
 */
void
clish_shell_iterator_init(clish_shell_iterator_t * iter,
	clish_nspace_visibility_e field);

/**
 * get the next command which is an extension of the specified line
 */
const clish_command_t *clish_shell_find_next_completion(const clish_shell_t *
	instance, const char *line, clish_shell_iterator_t * iter);
/**
 * Pop the current file handle from the stack of file handles, shutting
 * the file down and freeing any associated memory. The next file handle
 * in the stack becomes associated with the input stream for this shell.
 *
 * \return
 * BOOL_TRUE - the current file handle has been replaced.
 * BOOL_FALSE - there is only one handle on the stack which cannot be replaced.
 */
int clish_shell_pop_file(clish_shell_t * instance);

clish_view_t *clish_shell_find_view(clish_shell_t * instance, const char *name);
void clish_shell_insert_view(clish_shell_t * instance, clish_view_t * view);
clish_pargv_status_e clish_shell_parse(clish_shell_t * instance,
	const char *line, const clish_command_t ** cmd, clish_pargv_t ** pargv);
clish_pargv_status_e clish_shell_parse_pargv(clish_pargv_t *pargv,
	const clish_command_t *cmd,
	void *context,
	clish_paramv_t *paramv,
	const lub_argv_t *argv,
	unsigned *idx, clish_pargv_t *last, unsigned need_index);
char *clish_shell_word_generator(clish_shell_t * instance,
	const char *line, unsigned offset, unsigned state);
const clish_command_t *clish_shell_resolve_command(const clish_shell_t *
	instance, const char *line);
const clish_command_t *clish_shell_resolve_prefix(const clish_shell_t *
	instance, const char *line);
void clish_shell_insert_ptype(clish_shell_t * instance, clish_ptype_t * ptype);
void clish_shell_tinyrl_history(clish_shell_t * instance, unsigned int *limit);
tinyrl_t *clish_shell_tinyrl_new(FILE * instream,
	FILE * outstream, unsigned stifle);
void clish_shell_tinyrl_delete(tinyrl_t * instance);
void clish_shell_param_generator(clish_shell_t * instance, lub_argv_t *matches,
	const clish_command_t * cmd, const char *line, unsigned offset);
char **clish_shell_tinyrl_completion(tinyrl_t * tinyrl,
	const char *line, unsigned start, unsigned end);
void clish_shell__expand_viewid(const char *viewid, lub_bintree_t *tree,
	clish_context_t *context);
void clish_shell__init_pwd(clish_shell_pwd_t *pwd);
void clish_shell__fini_pwd(clish_shell_pwd_t *pwd);
int clish_shell_timeout_fn(tinyrl_t *tinyrl);
int clish_shell_keypress_fn(tinyrl_t *tinyrl, int key);
int clish_thread_trylock();
int clish_thread_unlock();
