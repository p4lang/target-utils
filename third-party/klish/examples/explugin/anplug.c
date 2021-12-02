#include <stdlib.h>
#include <stdio.h>

#include "target-utils/clish/plugin.h"
#include "target-utils/lub/ini.h"

CLISH_PLUGIN_SYM(anplug_fn)
{
	printf("anplug: Another plugin\n");
	return 0;
}

CLISH_PLUGIN_FINI(clish_plugin_anplug_fini)
{
	printf("anplug: FINI this = %p\n", clish_shell);

	return 0;
}

CLISH_PLUGIN_INIT(anplug)
{
	char *conf;
	lub_ini_t *ini;
	lub_ini_node_t *iter;
	lub_pair_t *pair;

	printf("anplug: INIT shell = %p\n", clish_shell);
	/* Set a fini function */
	clish_plugin_add_fini(plugin, clish_plugin_anplug_fini);
	/* Add symbols */
	clish_plugin_add_sym(plugin, anplug_fn, "an_fn");
	/* Show plugin config from <PLUGIN>...</PLUGIN> */
	conf = clish_plugin__get_conf(plugin);
	ini = lub_ini_new();
	lub_ini_parse_str(ini, conf);
	/* Iterate INI elements */
	for(iter = lub_ini__get_head(ini);
		iter; iter = lub_ini__get_next(iter)) {
		pair = lub_ini__iter_data(iter);
		printf("anplug iter: [%s] = [%s]\n",
			lub_pair__get_name(pair),
			lub_pair__get_value(pair));
	}

	/* Get specified config element */
	printf("anplug conf: vvv = %s\n", lub_ini_find(ini, "vvv"));

	lub_ini_free(ini);

	return 0;
}



