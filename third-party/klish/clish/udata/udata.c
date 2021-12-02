/*
 * udata.c
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "target-utils/lub/porting.h"
#include "target-utils/lub/list.h"
#include "target-utils/lub/string.h"

#include "private.h"

/*--------------------------------------------------------- */
int clish_udata_compare(const void *first, const void *second)
{
	const clish_udata_t *f = (const clish_udata_t *)first;
	const clish_udata_t *s = (const clish_udata_t *)second;

	return strcmp(f->name, s->name);
}

/*--------------------------------------------------------- */
clish_udata_t *clish_udata_new(const char *name, void *data)
{
	clish_udata_t *pdata =
		(clish_udata_t *)lub_malloc(sizeof(clish_udata_t));

	if (!name) {
		lub_free(pdata);
		return NULL;
	}
	memset(pdata, 0, sizeof(*pdata));
	pdata->name = lub_string_dup(name);
	pdata->data = data;

	return pdata;
}

/*--------------------------------------------------------- */
void *clish_udata_free(clish_udata_t *this)
{
	void *data;

	if (!this)
		return NULL;
	if (this->name)
		lub_string_free(this->name);
	data = this->data;
	lub_free(this);

	return data;
}

/*--------------------------------------------------------- */
void *clish_udata__get_data(const clish_udata_t *this)
{
	if (!this)
		return NULL;
	return this->data;
}

/*--------------------------------------------------------- */
int clish_udata__set_data(clish_udata_t *this, void *data)
{
	if (!this)
		return -1;
	this->data = data;
	return 0;
}

/*--------------------------------------------------------- */
char *clish_udata__get_name(const clish_udata_t *this)
{
	if (!this)
		return NULL;
	return this->name;
}


/*--------------------------------------------------------- */
