
/*------------------------------------
 * cfg_grp.h
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

#ifndef NO_CFG_CFG_GRP_H_
#define NO_CFG_CFG_GRP_H_

/*================================================================*/
extern int add_config_key(char *group, char *key, char *val);
extern char *get_config_key_string(char *group, char *key, char *default_val);
extern int get_config_key_bool(char *group, char *key, int default_val);
/*================================================================*/
extern int cfg_add_grp_key(char *grp_n, char *key_n, char *value);
extern int cfg_del_grp_key(void);
extern int cfg_dump_grp_key(void);

extern char *cfg_get_key_value(char *grp_n, char *key_n, char *val);
extern int cfg_get_key_bool(char *grp_n, char *key_n, int val);

/*================================================================*/
#endif 
/* end of cfg_grp.h */
