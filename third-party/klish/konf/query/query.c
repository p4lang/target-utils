#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#if WITH_INTERNAL_GETOPT
#include "libc/getopt.h"
#else
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#endif

#include "target-utils/lub/types.h"
#include "target-utils/lub/argv.h"
#include "target-utils/lub/porting.h"
#include "target-utils/lub/string.h"
#include "target-utils/lub/conv.h"
#include "private.h"

/*-------------------------------------------------------- */
konf_query_t *konf_query_new(void)
{
	konf_query_t *this;

	if (!(this = lub_malloc(sizeof(*this))))
		return NULL;

	this->op = KONF_QUERY_OP_NONE;
	this->pattern = NULL;
	this->priority = 0;
	this->seq = BOOL_FALSE;
	this->seq_num = 0;
	this->pwdc = 0;
	this->pwd = NULL;
	this->line = NULL;
	this->path = NULL;
	this->splitter = BOOL_TRUE;
	this->unique = BOOL_TRUE;
	this->depth = -1;

	return this;
}

/*-------------------------------------------------------- */
void konf_query_add_pwd(konf_query_t *this, char *str)
{
	size_t new_size;
	char **tmp;

	if (!this)
		return;

	new_size = ((this->pwdc + 1) * sizeof(char *));

	/* resize the pwd vector */
	tmp = lub_realloc(this->pwd, new_size);
	assert(tmp);
	this->pwd = tmp;
	/* insert reference to the pwd component */
	this->pwd[this->pwdc++] = lub_strdup(str);
}

/*-------------------------------------------------------- */
void konf_query_free(konf_query_t *this)
{
	unsigned i;

	lub_free(this->pattern);
	lub_free(this->line);
	lub_free(this->path);
	if (this->pwdc > 0) {
		for (i = 0; i < this->pwdc; i++)
			lub_free(this->pwd[i]);
		lub_free(this->pwd);
	}

	lub_free(this);
}

/*-------------------------------------------------------- */
/* Parse query */
int konf_query_parse(konf_query_t *this, int argc, char **argv)
{
	int i = 0;
	int pwdc = 0;

	static const char *shortopts = "suoedtp:q:r:l:f:inh:";
#ifdef HAVE_GETOPT_LONG
	static const struct option longopts[] = {
		{"set",		0, NULL, 's'},
		{"unset",	0, NULL, 'u'},
		{"ok",		0, NULL, 'o'},
		{"error",	0, NULL, 'e'},
		{"dump",	0, NULL, 'd'},
		{"stream",	0, NULL, 't'},
		{"priority",	1, NULL, 'p'},
		{"seq",		1, NULL, 'q'},
		{"pattern",	1, NULL, 'r'},
		{"line",	1, NULL, 'l'},
		{"file",	1, NULL, 'f'},
		{"splitter",	0, NULL, 'i'},
		{"non-unique",	0, NULL, 'n'},
		{"depth",	1, NULL, 'h'},
		{NULL,		0, NULL, 0}
	};
#endif

	optind = 0; /* It must be 1 for QNX6. This system has no getopt.h */
	while(1) {
		int opt;
#ifdef HAVE_GETOPT_LONG
		opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
		opt = getopt(argc, argv, shortopts);
#endif
		if (-1 == opt)
			break;
		switch (opt) {
		case 'o':
			this->op = KONF_QUERY_OP_OK;
			break;
		case 'e':
			this->op = KONF_QUERY_OP_ERROR;
			break;
		case 's':
			this->op = KONF_QUERY_OP_SET;
			break;
		case 'u':
			this->op = KONF_QUERY_OP_UNSET;
			break;
		case 'd':
			this->op = KONF_QUERY_OP_DUMP;
			break;
		case 't':
			this->op = KONF_QUERY_OP_STREAM;
			break;
		case 'p':
			{
			unsigned short val = 0;
			if (lub_conv_atous(optarg, &val, 0) < 0)
				break;
			this->priority = val;
			break;
			}
		case 'q':
			{
			unsigned short val = 0;
			this->seq = BOOL_TRUE;
			if (lub_conv_atous(optarg, &val, 0) < 0)
				break;
			this->seq_num = val;
			break;
			}
		case 'r':
			this->pattern = lub_strdup(optarg);
			break;
		case 'l':
			this->line = lub_strdup(optarg);
			break;
		case 'f':
			this->path = lub_strdup(optarg);
			break;
		case 'i':
			this->splitter = BOOL_FALSE;
			break;
		case 'n':
			this->unique = BOOL_FALSE;
			break;
		case 'h':
			{
			unsigned short val = 0;
			if (lub_conv_atous(optarg, &val, 0) < 0)
				break;
			this->depth = val;
			break;
			}
		default:
			break;
		}
	}

	/* Check options */
	if (KONF_QUERY_OP_NONE == this->op)
		return -1;
	if (KONF_QUERY_OP_SET == this->op) {
		if (!this->pattern)
			return -1;
		if (!this->line)
			return -1;
	}

	if ((pwdc = argc - optind) < 0)
		return -1;

	for (i = 0; i < pwdc; i++)
		konf_query_add_pwd(this, argv[optind + i]);

	return 0;
}

/*-------------------------------------------------------- */
/* Parse query string */
int konf_query_parse_str(konf_query_t *this, char *str)
{
	int res;
	lub_argv_t *lub_argv;
	char **str_argv;
	int str_argc;

	/* Make args from string */
	lub_argv = lub_argv_new(str, 0);
	str_argv = lub_argv__get_argv(lub_argv, "");
	str_argc = lub_argv__get_count(lub_argv) + 1;

	/* Parse query */
	res = konf_query_parse(this, str_argc, str_argv);
	lub_argv__free_argv(str_argv);
	lub_argv_delete(lub_argv);

	return res;
}

/*-------------------------------------------------------- */
char * konf_query__get_pwd(konf_query_t *this, unsigned index)
{
	if (!this)
		return NULL;
	if (index >= this->pwdc)
		return NULL;

	return this->pwd[index];
}

/*-------------------------------------------------------- */
int konf_query__get_pwdc(konf_query_t *this)
{
	return this->pwdc;
}

/*-------------------------------------------------------- */
konf_query_op_e konf_query__get_op(konf_query_t *this)
{
	return this->op;
}

/*-------------------------------------------------------- */
char * konf_query__get_path(konf_query_t *this)
{
	return this->path;
}

/*-------------------------------------------------------- */
const char * konf_query__get_pattern(konf_query_t *this)
{
	return this->pattern;
}

/*-------------------------------------------------------- */
const char * konf_query__get_line(konf_query_t *this)
{
	return this->line;
}

/*-------------------------------------------------------- */
unsigned short konf_query__get_priority(konf_query_t *this)
{
	return this->priority;
}

/*-------------------------------------------------------- */
bool_t konf_query__get_splitter(konf_query_t *this)
{
	return this->splitter;
}

/*-------------------------------------------------------- */
bool_t konf_query__get_seq(konf_query_t *this)
{
	return this->seq;
}

/*-------------------------------------------------------- */
unsigned short konf_query__get_seq_num(konf_query_t *this)
{
	return this->seq_num;
}

/*-------------------------------------------------------- */
bool_t konf_query__get_unique(konf_query_t *this)
{
	return this->unique;
}

/*-------------------------------------------------------- */
int konf_query__get_depth(konf_query_t *this)
{
	return this->depth;
}
