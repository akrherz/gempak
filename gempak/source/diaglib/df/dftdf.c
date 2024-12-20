#include "df.h"

void df_tdf  ( int *iret )
/************************************************************************
 * df_tdf								*
 *									*
 * This subroutine computes the time difference of a scalar:		*
 *									*
 *     TDF (S) = S (time1) - S (time2)					*
 *									*
 * df_tdf ( iret )							*
 *									*
 * Output parameters:							*
 *	*iret		int		Return code			*
 *					  0 = normal return		*
 *					 -7 = Grid ... cannot be found	*
 *					 -8 = Grid ... is the wrong size*
 *					-10 = Internal grid list is full*
 *					-12 = ... must be a scalar	*
 *					-13 = ... must be from grid file*
 *					-16 = Map proj. ... is invalid	*
 *					-18 = TIME ... must be a range	*
 *					-20 = Stack is full		*
 *					-21 = Stack is empty		*
 *					-22 = TIME ... is invalid	*
 *					-23 = LEVEL ... is invalid	*
 *					-24 = IVCORD ... is invalid	*
 **									*
 * Log:									*
 * M. desJardins/GSFC	10/85						*
 * M. desJardins/GSFC	 7/88	Added new stack subroutines		*
 * G. Huffman/GSC	 9/88	Error messages				*
 * K. Brill/GSC		 8/89	Subsetting				*
 * K. Brill/GSC		10/89	Subsetting				*
 * T. Lee/GSC		 4/96	Single dimension for dgg		*
 * K. Tyle/GSC           5/96   Moved IGDPT outside do-loop             *
 * K. Brill/HPC		 1/02	CALL DG_SSUB and DG_ESUB		*
 * K. Brill/HPC		11/02	Eliminate use of the SUBA logical array	*
 * R. Tian/SAIC		10/05	Recoded from Fortran			*
 ************************************************************************/
{
    int num1, num2, num, kxd, kyd, ksub1, ksub2, i, ier, zero;
    float *gnum1, *gnum2, *gnum, dg1, dg2;
/*----------------------------------------------------------------------*/
    *iret = 0;
    zero = 0;

    dg_ssub ( iret );

    /*
     * Get the grids.
     */
    dg_gett ( &num1, &num2, iret );
    if ( *iret != 0 ) return;

    /*
     * Get a new grid number.
     */
    dg_nxts ( &num, iret );
    if ( *iret != 0 ) return;

    /*
     * Grid number to grid.
     */
    dg_getg ( &num1, &gnum1, &kxd, &kyd, &ksub1, &ksub2, iret );
    dg_getg ( &num2, &gnum2, &kxd, &kyd, &ksub1, &ksub2, iret );
    dg_getg ( &num,  &gnum,  &kxd, &kyd, &ksub1, &ksub2, iret );

    /*
     * Subtract the grids.
     */
    for ( i = ksub1 - 1; i < ksub2; i++ ) {
        dg1 = gnum1[i];
        dg2 = gnum2[i];
        if ( ERMISS (dg1) || ERMISS (dg2) ) {
	    gnum[i] = RMISSD;
	} else {
	    gnum[i] = dg1 - dg2;
	}
    }

    /*
     * Make a name of the form 'TDF'//S and update header;
     * update stack.
     */
    dg_updh ( "TDF", &num, &num1, &num2, iret );
    dg_puts ( &num, iret );
    dg_esub ( &num, &zero, &zero, &zero, &ier );
    if ( ier != 0 ) *iret = ier;

    return;
}
