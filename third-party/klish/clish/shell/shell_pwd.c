/*
 * shell_pwd.c
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>

#include "target-utils/lub/porting.h"
#include "target-utils/lub/string.h"
#include "private.h"

/*--------------------------------------------------------- */
void clish_shell__init_pwd(clish_shell_pwd_t *pwd)
{
	pwd->line = NULL;
	pwd->view = NULL;
	pwd->pargv = NULL;
	pwd->cmd = NULL;
	pwd->prefix = NULL;
	/* initialise the tree of vars */
	lub_bintree_init(&pwd->viewid,
		clish_var_bt_offset(),
		clish_var_bt_compare, clish_var_bt_getkey);
}

/*--------------------------------------------------------- */
void clish_shell__fini_pwd(clish_shell_pwd_t *pwd)
{
	clish_var_t *var;

	lub_string_free(pwd->line);
	lub_string_free(pwd->cmd);
	if (pwd->prefix)
		lub_string_free(pwd->prefix);
	pwd->view = NULL;
	clish_pargv_delete(pwd->pargv);
	/* delete each VAR held  */
	while ((var = lub_bintree_findfirst(&pwd->viewid))) {
		lub_bintree_remove(&pwd->viewid, var);
		clish_var_delete(var);
	}
}

/*--------------------------------------------------------- */
void clish_shell__set_pwd(clish_shell_t *this,
	const char *line, clish_view_t *view, char *viewid, clish_context_t *context)
{
	clish_shell_pwd_t **tmp;
	size_t new_size = 0;
	unsigned int i;
	unsigned int index = clish_view__get_depth(view);
	clish_shell_pwd_t *newpwd;
	const clish_command_t *full_cmd = clish_context__get_cmd(context);

	/* Create new element */
	newpwd = lub_malloc(sizeof(*newpwd));
	assert(newpwd);
	clish_shell__init_pwd(newpwd);

	/* Resize the pwd vector */
	if (index >= this->pwdc) {
		new_size = (index + 1) * sizeof(clish_shell_pwd_t *);
		tmp = lub_realloc(this->pwdv, new_size);
		assert(tmp);
		this->pwdv = tmp;
		/* Initialize new elements */
		for (i = this->pwdc; i <= index; i++) {
			clish_shell_pwd_t *pwd = lub_malloc(sizeof(*pwd));
			assert(pwd);
			clish_shell__init_pwd(pwd);
			this->pwdv[i] = pwd;
		}
		this->pwdc = index + 1;
	}

	/* Fill the new pwd entry */
	newpwd->line = line ? lub_string_dup(line) : NULL;
	newpwd->view = view;
	newpwd->pargv = clish_pargv_clone(clish_context__get_pargv(context));
	if (full_cmd) {
		const clish_command_t *cmd = clish_command__get_cmd(full_cmd);
		newpwd->cmd = lub_string_dup(clish_command__get_name(cmd));
		if (cmd != full_cmd) {
			const char *full_cmd_name = clish_command__get_name(full_cmd);
			const char *cmd_name = clish_command__get_name(cmd);
			int len = strlen(full_cmd_name) - strlen(cmd_name);
			if (len > 1)
				newpwd->prefix = lub_string_dupn(full_cmd_name, len - 1);
		}
	}
	clish_shell__expand_viewid(viewid, &newpwd->viewid, context);
	clish_shell__fini_pwd(this->pwdv[index]);
	lub_free(this->pwdv[index]);
	this->pwdv[index] = newpwd;
	this->depth = index;
}

/*--------------------------------------------------------- */
char *clish_shell__get_pwd_line(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->pwdc)
		return NULL;

	return this->pwdv[index]->line;
}

/*--------------------------------------------------------- */
clish_pargv_t *clish_shell__get_pwd_pargv(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->pwdc)
		return NULL;

	return this->pwdv[index]->pargv;
}

/*--------------------------------------------------------- */
char *clish_shell__get_pwd_cmd(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->pwdc)
		return NULL;

	return this->pwdv[index]->cmd;
}

/*--------------------------------------------------------- */
char *clish_shell__get_pwd_prefix(const clish_shell_t *this, unsigned int index)
{
	if (index >= this->pwdc)
		return NULL;

	return this->pwdv[index]->prefix;
}

/*--------------------------------------------------------- */
char *clish_shell__get_pwd_full(const clish_shell_t * this, unsigned int depth)
{
	char *pwd = NULL;
	unsigned int i;

	for (i = 1; i <= depth; i++) {
		const char *str =
			clish_shell__get_pwd_line(this, i);
		/* Cannot get full path */
		if (!str) {
			lub_string_free(pwd);
			return NULL;
		}
		if (pwd)
			lub_string_cat(&pwd, " ");
		lub_string_cat(&pwd, "\"");
		lub_string_cat(&pwd, str);
		lub_string_cat(&pwd, "\"");
	}

	return pwd;
}

/*--------------------------------------------------------- */
clish_view_t *clish_shell__get_pwd_view(const clish_shell_t * this, unsigned int index)
{
	if (index >= this->pwdc)
		return NULL;

	return this->pwdv[index]->view;
}

/*--------------------------------------------------------- */
konf_client_t *clish_shell__get_client(const clish_shell_t * this)
{
	return this->client;
}

/*--------------------------------------------------------- */
void clish_shell__set_lockfile(clish_shell_t * this, const char * path)
{
	if (!this)
		return;

	lub_string_free(this->lockfile);
	this->lockfile = NULL;
	if (path)
		this->lockfile = lub_string_dup(path);
}

/*--------------------------------------------------------- */
char * clish_shell__get_lockfile(clish_shell_t * this)
{
	if (!this)
		return NULL;

	return this->lockfile;
}

/*--------------------------------------------------------- */
int clish_shell__set_socket(clish_shell_t * this, const char * path)
{
	if (!this || !path)
		return -1;

	konf_client_free(this->client);
	this->client = konf_client_new(path);

	return 0;
}

/*--------------------------------------------------------- */
void clish_shell__set_facility(clish_shell_t *this, int facility)
{
	if (!this)
		return;
	this->log_facility = facility;
}

/*--------------------------------------------------------- */
int clish_shell__get_facility(clish_shell_t *this)
{
	if (!this)
		return LOG_LOCAL0;

	return this->log_facility;
}
