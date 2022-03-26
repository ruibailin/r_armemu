
/*------------------------------------
 * cop_ops.h
 * Create:  2021-02-13
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


#ifndef NO_ARM_UOP_COP_OPS_H_
#define NO_ARM_UOP_COP_OPS_H_
/*================================================================*/
#include "../1imp.h"
#include "uop_dec.h"

/*================================================================*/
extern void uop_cop_reg_transfer(struct uop *op);
extern void uop_cop_double_reg_transfer(struct uop *op);
extern void uop_cop_data_processing(struct uop *op);
extern void uop_cop_load_store(struct uop *op);
/*================================================================*/
#endif
/* end of cop_ops.h */
