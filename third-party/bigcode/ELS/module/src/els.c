/************************************************************
 * <bsn.cl fy=2013 v=epl-1.0>
 *
 *        Copyright 2013, 2014 Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/

#include <target-utils/ELS/els_config.h>
#include <target-utils/ELS/els.h>
#include "els_log.h"
#include <memory.h>
#include <stdlib.h>
#include <target-sys/bf_sal/bf_sys_sem.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <target-sys/bf_sal/bf_sys_thread.h>

#if ELS_CONFIG_INCLUDE_EVENTFD == 1
#include <sys/eventfd.h>
#endif

typedef struct els_ctl_s {
    /* Client structure */
    els_t els;

    /* Private implementation variables */
    els_get_prompt_f get_prompt;
    void* get_prompt_cookie;
    char prompt[ELS_CONFIG_MAX_PROMPT];

    els_complete_f complete;
    void* complete_cookie;
    void* complete_pvs_cookie;

    volatile const char* line;
    bf_sys_thread_t thread;
    bf_sys_sem_t prompt_lock;
    bf_sys_sem_t get_lock;

} els_ctl_t;


static char*
els_get_prompt__(EditLine* el)
{
    els_ctl_t* ctl;
    el_get(el, EL_CLIENTDATA, &ctl);
    ctl->get_prompt(ctl->prompt, sizeof(ctl->prompt), ctl->get_prompt_cookie);
    return ctl->prompt;
}

static unsigned char
els_keyhandler__(EditLine* el, int ch)
{
    els_ctl_t* ctl;
    const LineInfo* li;
    el_get(el, EL_CLIENTDATA, &ctl);
    li = el_line(el);
    *((char*)li->lastchar) = 0;

    switch(ch)
        {
        case '\t':
            {
                char* s;
                if(ctl->complete == NULL) {
                    return CC_NORM;
                }
                s = ctl->complete(ctl->complete_cookie, li->buffer, ctl->complete_pvs_cookie);
                if(s) {
                    el_insertstr(ctl->els.el, s);
                    aim_free(s);
                }
                return CC_REDISPLAY;
            }
        case 4: /* Ctrl-D */
            {
                /*
                 * If the input line is empty, return "^D";
                 * Otherwise, ignore it.
                 */
                if(li->buffer[0] == 0) {
                    el_insertstr(ctl->els.el, "^D\n");
                    return CC_NEWLINE;
                }
                return CC_NORM;
            }
        }

    return CC_NORM;
}

els_t*
els_create(char* name, els_get_prompt_f get_prompt, void* cookie, FILE *fin, FILE *fout, FILE *ferr)
{
    els_ctl_t* ctl;

    if(get_prompt == NULL) {
        AIM_LOG_INTERNAL("get_prompt is NULL.");
        return NULL;
    }

    if( (ctl = aim_zmalloc(sizeof(*ctl))) == NULL) {
        AIM_LOG_INTERNAL("out of memory.");
        return NULL;
    }

    ctl->els.el = el_init(name, fin, fout, ferr);
    if ((fin == stdin) && (fout == stdout)) {
        ctl->els.std_streams = true;
    } else {
        ctl->els.std_streams = false;
    }
    el_set(ctl->els.el, EL_EDITOR, "emacs");
    el_set(ctl->els.el, EL_CLIENTDATA, ctl);
    ctl->els.history = history_init();
    history(ctl->els.history, &ctl->els.ev, H_SETSIZE, 1000);
    el_set(ctl->els.el, EL_HIST, history, ctl->els.history);

    bf_sys_sem_init(&ctl->prompt_lock, 0, 0);
    bf_sys_sem_init(&ctl->get_lock, 0, 0);
    ctl->line = NULL;
    ctl->get_prompt = get_prompt;
    ctl->get_prompt_cookie = cookie;
    el_set(ctl->els.el, EL_PROMPT, els_get_prompt__);

    el_set(ctl->els.el, EL_ADDFN, "completion", "command completion", els_keyhandler__);
    el_set(ctl->els.el, EL_ADDFN, "exit", "exit", els_keyhandler__);
    el_set(ctl->els.el, EL_BIND, "\t", "completion", NULL);
    el_set(ctl->els.el, EL_BIND, "^D", "exit", NULL);

#if ELS_CONFIG_INCLUDE_EVENTFD == 1
    ctl->els.eventfd = eventfd(0, 0);
#endif
    return &ctl->els;
}

int
els_complete_set(els_t* els, els_complete_f cf, void* cookie, aim_pvs_t *pvs)
{
    els_ctl_t* ctl = (els_ctl_t*)els;
    ctl->complete = cf;
    ctl->complete_cookie = cookie;
    ctl->complete_pvs_cookie = pvs;
    return 0;
}


static void*
els_thread__(void* arg)
{
    els_ctl_t* ctl = (els_ctl_t*) arg;
    int count;

    /*
     * We depend upon cancellation points to terminate,
     * either in sem_wait() or blocking on IO in el_gets().
     *
     * When we exit will not be under our control, so
     * there is no reason for an explicit "exit" sentinal.
     */
    for(;;) {
        if(bf_sys_sem_wait(&ctl->prompt_lock) == EINTR) {
            continue;
        }
        ctl->line = NULL;
        count = 0;
        do {
            ctl->line = el_gets(ctl->els.el, &count);

            /* If we received a read error on non-standard stream session exit
               so it can be processed by session
             */
            if ((ctl->line == NULL) && (!ctl->els.std_streams)) {
                break;
            }
        } while(ctl->line == NULL || count == 0);
        /** Don't add blank lines to the history. */
        if(ctl->line && ctl->line[0] && ctl->line[0] != '\n') {
            history(ctl->els.history, &ctl->els.ev, H_ENTER, ctl->line);
        }
        bf_sys_sem_post(&ctl->get_lock);
#if ELS_CONFIG_INCLUDE_EVENTFD == 1
        {
            uint64_t one = 1;
            if (write(ctl->els.eventfd, &one, sizeof(one)) < 0) {
                /* silence warn_unused_result */
            }
        }
#endif
    }
    return NULL;
}

int
els_start(els_t* el)
{
    els_ctl_t* ctl = (els_ctl_t*)el;
    if(ctl->thread) {
        return 0;
    }

    if(bf_sys_thread_create(&ctl->thread, els_thread__, ctl, 0) < 0) {
        return -1;
    }
    bf_sys_thread_set_name(ctl->thread, "bf_els_main");

    return 0;
}

int
els_stop(els_t* el)
{
    void* rv;
    els_ctl_t* ctl = (els_ctl_t*)el;

    if(ctl == NULL || ctl->thread == 0) {
        return -1;
    }

    bf_sys_thread_cancel(ctl->thread);
    bf_sys_thread_join(ctl->thread, &rv);
    ctl->thread = 0;
    el_reset(ctl->els.el);
    return 0;
}

void
els_destroy(els_t* el)
{
    els_ctl_t* ctl = (els_ctl_t*)el;
    if(el) {
        els_stop(el);
#if ELS_CONFIG_INCLUDE_EVENTFD == 1
        close(ctl->els.eventfd);
#endif
        history_end(ctl->els.history);
        el_end(ctl->els.el);
        bf_sys_sem_destroy(&ctl->prompt_lock);
        bf_sys_sem_destroy(&ctl->get_lock);
        aim_free(ctl);
    }
}

int
els_prompt(els_t* el)
{
    els_ctl_t* ctl = (els_ctl_t*)el;
    if(ctl->thread) {
        bf_sys_sem_destroy(&ctl->get_lock);
        bf_sys_sem_init(&ctl->get_lock, 0, 0);
        bf_sys_sem_post(&ctl->prompt_lock);
    }
    return 0;
}

char*
els_getline(els_t* el)
{
    els_ctl_t* ctl = (els_ctl_t*)el;
    char* rv = (char*)ctl->line;
    ctl->line = NULL;
    return rv;
}

void
els_waitline(els_t* els)
{
    els_ctl_t* ctl = (els_ctl_t*)els;

    for(;;) {
        if(bf_sys_sem_wait(&ctl->get_lock) == EINTR) {
            continue;
        }
        if(ctl->line == NULL) {
            if (ctl->els.std_streams) {
                AIM_LOG_INTERNAL("waitline(): semaphore returned without line.");
            }
        }
        return;
    }
}
