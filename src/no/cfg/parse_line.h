
/*------------------------------------
 * parse_line.h
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
#ifndef NO_CFG_PARSE_LINE_H_
#define NO_CFG_PARSE_LINE_H_
/*================================================================*/
#define CFG_BAD_GROUP	-1			// only "[xxxx
#define CFG_EMPTY_GROUP	-2			//only "[]"

#define CFG_EMPTY_LINE  1
#define CFG_REMARK_LINE 2
#define	CFG_GROUP_LINE	3
#define CFG_KEY_LINE	5
#define	CFG_OTHER_LINE	6
extern char *cfg_init_buf(void);
extern void cfg_free_buf(void);
extern int cfg_parse_line(char *s);
extern int cfg_add_parse(void);

/*================================================================*/
#endif 
/* end of parse_line.h */
