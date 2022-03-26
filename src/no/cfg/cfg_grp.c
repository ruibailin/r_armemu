
/*------------------------------------
 * cfg_grp.c
 * Create:  2021-02-01
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#include "0ctr.h"
/*================================================================*/
#include "../1imp.h"

struct grp_key {
    struct grp_key 	*next;
    char 			*name;
    char 			*value;
};

struct cfg_grp {
    struct cfg_grp 	*next;
    char 			*name;
    struct grp_key  *list;
};

static struct cfg_grp *all_cfg=NULL;
/*================================================================*/
static struct cfg_grp *cfg_find_grp(char *grp_n);
static struct cfg_grp *cfg_find_grp(char *grp_n)
{
    struct cfg_grp *grp;

    for (grp = all_cfg; grp != NULL; grp = grp->next) {
        if (os_strcasecmp(grp->name, grp_n) == 0)
            return grp;
    }

    return NULL;
}

static struct cfg_grp *cfg_create_grp(char *grp_n);
static struct cfg_grp *cfg_create_grp(char *grp_n)
{
    struct cfg_grp *grp;

    grp = os_malloc(sizeof(struct cfg_grp));
    grp->name = os_strdup(grp_n);
    grp->list = NULL;

    os_print_addr(grp->name);
    os_print_addr(grp_n);

    /* add it to the grp_n list */

    grp->next = all_cfg;
    all_cfg = grp;

    return grp;
}

static struct grp_key *cfg_grp_find_key(struct cfg_grp *grp, char *key_n);
static struct grp_key *cfg_grp_find_key(struct cfg_grp *grp, char *key_n)
{
    struct grp_key *key;

    for (key = grp->list; key != NULL; key = key->next) {
        if (os_strcasecmp(key->name, key_n) == 0)
            return key;
    }

    return NULL;
}
static struct grp_key *cfg_grp_create_key(struct cfg_grp *grp,char *key_n,char *value);
static struct grp_key *cfg_grp_create_key(struct cfg_grp *grp,char *key_n,char *value)
{
    struct grp_key *key;

    /* create a new one */
    key = os_malloc(sizeof(struct grp_key));
    key->name = os_strdup(key_n);
    key->value = os_strdup(value);

    /* add it to the grp_n */
    key->next = grp->list;
    grp->list = key;

    return key;
}

/*================================================================*/
int cfg_add_grp_key(char *grp_n, char *key_n, char *value);
int cfg_add_grp_key(char *grp_n, char *key_n, char *value)
{
    struct cfg_grp *grp;
    struct grp_key *key;

    os_printf("cfg_add_grp_key: %s.%s = %s\n", grp_n, key_n, value);

    /* find or create grp_n */
    grp = cfg_find_grp(grp_n);
    if (grp == NULL)
    {
        os_printf("cfg_add_grp_key: create new grp: %s\n",grp_n);
    	grp=cfg_create_grp(grp_n);
    }

    /* find or create key_n */
    key = cfg_grp_find_key(grp, key_n);
    if (key == NULL)
    {
        os_printf("cfg_add_grp_key: create new key: %s\n",key_n);
        key=cfg_grp_create_key(grp,key_n,value);
    }
    else
    {
        os_printf("cfg_add_grp_key: replace old key: %s\n",key_n);
    	/* the key_n already exists, override it */
        if (key->value != NULL)
            os_free(key->value);
        key->value = os_strdup(value);
    }
    return 0;
}

/*================================================================*/
static struct grp_key *cfg_find_key(char *grp_n, char *key_n);
static struct grp_key *cfg_find_key(char *grp_n, char *key_n)
{
    struct cfg_grp *grp;
    struct grp_key *key;

    grp = cfg_find_grp(grp_n);
    if (grp == NULL)
        return NULL;
    key = cfg_grp_find_key(grp, key_n);
    return key;
}

/*================================================================*/

char *cfg_get_key_value(char *grp_n, char *key_n, char *val);
char *cfg_get_key_value(char *grp_n, char *key_n, char *val)
{
    struct grp_key *key;

    key = cfg_find_key(grp_n, key_n);
    if (key == NULL)
        return val;

    return key->value;
}


int get_cfg_key_bool(char *grp_n, char *key_n, int val);
int get_cfg_key_bool(char *grp_n, char *key_n, int val)
{
    struct grp_key *key;

    key = cfg_find_key(grp_n, key_n);
    if (key == NULL)
        return val;

    if (!os_strcasecmp(key->value, "true"))
        return 1;
    if (!os_strcasecmp(key->value, "yes"))
        return 1;
    if (os_atoi(key->value) != 0)
        return 1;

    return 0;
}
/*================================================================*/
static void grp_del_key(struct cfg_grp *grp);
static void grp_del_key(struct cfg_grp *grp)
{
	int k;
    struct grp_key *key;

    for(;;)
    {
        key = grp->list;
    	if(key == NULL)
    		return;
    	k++;
    	grp->list = key->next;
    	os_printf("del key %d: name=%s value=%s \n",k,key->name,key->value);
    	os_free(key->name);
    	os_free(key->value);
    	os_free(key);			//only can free those is alloced by you
    }
    return;
}

int cfg_del_grp_key(void);
int cfg_del_grp_key(void)
{
	int g;
    struct cfg_grp *grp;

    os_printf("Begin delete:\n");
    g=0;
    for(;;)
    {
        grp=all_cfg;			//delete from head to tail
        if(grp == NULL)
        	return g;
        os_printf("del Group %d: %s \n", g, grp->name);
        grp_del_key(grp);

        all_cfg = grp->next;
        os_free(grp->name);
        os_free(grp);			//only can free those is alloced by you
        g++;
    }
    return g;
}

/*================================================================*/
static void grp_dump_key(struct cfg_grp *grp);
static void grp_dump_key(struct cfg_grp *grp)
{
	int k;
    struct grp_key *key;

    k=0;
    key = grp->list;
    for(;;)
    {
    	if(key == NULL)
    		return;
    	k++;
    	os_printf("key %d: name=%s value=%s \n",k,key->name,key->value);
    	key = key->next;

    }
    return;
}

int cfg_dump_grp_key();
int cfg_dump_grp_key()
{
	int g;
    struct cfg_grp *grp;

    os_printf("Begin dump:\n");
    g=0;
	grp=all_cfg;
    for(;;)
    {
        if(grp == NULL)
        	return g;
        os_printf("Group %d: %s \n", g, grp->name);
        grp_dump_key(grp);
        grp = grp->next;
        g++;
    }
    return g;
}
/*================================================================*/
/* end of cfg_grp.c */
