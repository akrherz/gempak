#include "df.h"

void df_mfrq ( int *iret )
/************************************************************************
 * df_mfrq								*
 *									*
 * This subroutine computes modified values of the second input so that	*
 * the modified values match the frequency distribution of values in	*
 * the first input array.						*
 *									*
 *  [ Modified S2 with same freq. dist. as S1 ]  = PD_MFRQ ( S1, S2 )	*
 *									*
 * df_mfrq ( iret )							*
 *									*
 * Output parameters:							*
 *	*iret		int		Return code			*
 *					As for DG_GETS			*
 **									*
 * Log:									*
 * K. Brill/IMSG   	 6/15	Created					*
 ************************************************************************/
{
    int num1, num2, num, kxd, kyd, kxyd, ksub1, ksub2, zero, ier, ier2;
    float *gnum1, *gnum2, *gnum;
/*----------------------------------------------------------------------*/
    *iret = 0;
    zero = 0;

    dg_ssub ( iret );

    /*
     * Get the two input grid numbers.
     */
    dg_gets ( &num1, iret );
    if ( *iret != 0 ) return;
    dg_gets ( &num2, iret );
    if ( *iret != 0 ) return;

    /*
     * Get a new grid number and call the frequency matching subroutine.
     */
    dg_nxts ( &num, iret );
    if ( *iret != 0 ) return;

    /*
     * Grid number to grid.
     */
    dg_getg ( &num1, &gnum1, &kxd, &kyd, &ksub1, &ksub2, iret );
    dg_getg ( &num2, &gnum2, &kxd, &kyd, &ksub1, &ksub2, iret );
    dg_getg ( &num,  &gnum,  &kxd, &kyd, &ksub1, &ksub2, iret );

    kxyd = kxd * kyd;
    pd_mfrq ( gnum1, gnum2, &kxyd, gnum, &ier );
    if ( ier == 1 ) {
	er_wmsg ( "PD", &ier, " ", &ier2, strlen("PD"), strlen(" ") );
	ier = 0;
    }

    /*    
     * Make a name of the form 'MFRQ'//S1//S2 and update header;
     * update stack.
     */
    dg_updh ( "MFRQ", &num, &num1, &num2, iret );
    dg_puts ( &num, iret );
    dg_esub ( &num, &zero, &zero, &zero, &ier );
    if ( ier != 0 ) *iret = ier;

    return;
}
