
/*------------------------------------
 * parse_line.c
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

/*================================================================*/
#define	MAX_CHAR	256
static char *line_buf;
static char *group_buf;

char *cfg_init_buf(void);
char *cfg_init_buf()
{
	line_buf = os_malloc(MAX_CHAR);
	group_buf = os_malloc(MAX_CHAR);
    os_print_addr(group_buf);
	return line_buf;
}

void cfg_free_buf(void);
void cfg_free_buf()
{
	os_free(line_buf);
	os_free(group_buf);
}

/*================================================================*/
static char *cur_group = NULL;
static int cfg_parse_group(char *s);
static int cfg_parse_group(char *s)
{
	char *g;

    /* not in a grp_n yet */
    cur_group = group_buf;
    cur_group[0] = '\0';
    s++;		//jump [
    g = group_buf;
    for(;;)
    {
    	if(*s == '\0')
    		return 0;		//no pair of [ ]
    	if(*s == ']')
    		break;
     	*g = *s;
    	g++;
    	s++;
    }
    *g = 0;    /* ]->0, trim the group buffer */
    return	1;
}

/*================================================================*/
static char *cur_key_name;
static char *cur_key_val;
static int cfg_parse_key(char *s);
static int cfg_parse_key(char *s)
{
    /* assume it's a name value pair, in 'name = value' format */
    /* pick out the name */
    cur_key_name = s;

    for(;;)
    {
    	if(*s == '\0')
    		return 0;
    	if(*s == '=')
    		break;
    	s++;
    }

    *s = '\0';			//change '=' to '\0', add an end for name;

    /* value should be the rest of the line */
    s++;
    cur_key_val = s;
    return 1;
}

/*================================================================*/
#include "parse_line.h"

int cfg_parse_line(char *s);
int cfg_parse_line(char *s)
{
	int i;
    if (*s == '\0')
        return CFG_EMPTY_LINE;
    if (*s == '#')
        return CFG_REMARK_LINE;
    /* see if it's a grp_n, in '[group]' format */
    if (*s == '[')
    {
    	i = cfg_parse_group(s);
    	if(i == 0)
    		return CFG_BAD_GROUP;
    	else
    	{
            cur_group = os_trim(group_buf);
            if(cur_group[0] == '\0')
            	return CFG_EMPTY_GROUP;
            else
            	return CFG_GROUP_LINE;
    	}
    }
    else
    {
    	i=cfg_parse_key(s);
    	cur_key_name = os_trim(cur_key_name);
    	cur_key_val = os_trim(cur_key_val);
    	if(i)
    		return CFG_KEY_LINE;
    	else
    		return CFG_OTHER_LINE;
    }
}

/*================================================================*/
#include "cfg_grp.h"
int cfg_add_parse(void);
int cfg_add_parse(void)
{
    if(cur_group == NULL)
    	return -1;				//no group is ahead of key
	if(cur_key_name[0] == '\0')
		return 0;				//empty key
	cfg_add_grp_key(cur_group,cur_key_name,cur_key_val);
	return 1;
}

/*================================================================*/
/* end of parse_line.c */
