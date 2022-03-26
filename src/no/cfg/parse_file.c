
/*------------------------------------
 * parse_file.c
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
#include "parse_line.h"
static	int	line_num;
/*================================================================*/
int cfg_parse_file(char *filenmae);
int cfg_parse_file(char *filename)
{
    void *fp;
    char *buf,*s;
    int i;

    /* open the config file */
    fp = os_fopen(filename, "r");
    if (fp == NULL)
    {
        os_printf("Open parse file failed: %s, The reason is %s !\n", filename,os_ferror_name());
        return -1;
    }

    os_printf("parse file: %s\n", filename);
    buf = cfg_init_buf();
    line_num = 0;
     while (!os_feof(fp))
     {
         line_num++;
         /* read in a line */
         s = os_fgets(fp,buf,1024);
         if (s == NULL)
             break;

//         os_printf("parse file: line %d: %s",line_num, s);
         s = os_trim(s);
         i=cfg_parse_line(s);

         switch(i)
         {
         case CFG_BAD_GROUP:
//             os_printf("parse file: bad group\n");
        	 break;
         case CFG_EMPTY_GROUP:
//             os_printf("parse file: empty group\n");
        	 break;
         case CFG_EMPTY_LINE:
//             os_printf("parse file: emtpy line\n");
        	 break;
         case CFG_REMARK_LINE:
 //            os_printf("parse file: remark line\n");
        	 break;
         case CFG_GROUP_LINE:
 //       	 os_printf("parse file: group line\n");
        	 break;
         case CFG_KEY_LINE:
//        	 os_printf("parse file: key line\n");
        	 cfg_add_parse();
        	 break;
         case CFG_OTHER_LINE:
//        	 os_printf("parse file: other line\n");
        	 break;
         default:
        	 break;
          }
     }
     os_fclose(fp);
     cfg_free_buf();
     return line_num;
}

/*================================================================*/

/* end of parse_file.c */
