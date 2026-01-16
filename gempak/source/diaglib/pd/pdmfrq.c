#include "pd.h"

static int floatcomp (const void* elem1, const void* elem2);

void pd_mfrq  ( const float *xa, const float *xf, const int *np,
	  	float *xo, int *iret )
/************************************************************************
 * pd_mfrq								*
 *									*
 * This subroutine creates output grid xo by modifying the values of    *
 * xf so that the frequency distribution of the xo values matches that  *
 * of the xa values.							*
 *									*
 * The frequency matching is most commonly used to correct the		*
 * frequency bias of a QPF by matching it to an analysis (Clark et al.  *
 * 2009) or another forecast known to have near unit frequency bias.	*
 *									*
 * pd_mfrq  ( xa, xf, np, xo, iret )					*
 *									*
 * Input parameters:							*
 *	*xa		const float	Input array of refernce values	*
 *	*xf		const float	Input values to be modified     *
 *	*np		const int	Number of points		*
 *									*
 * Output parameters:							*
 *	*xo		float		Array of xf values having the	*
 *	                                  same frequency distribution  	*
 *	                                  as the input array xa.	*
 *	*iret		int		Return code			*
 *					  0 = normal return		*
 * Reference:								*
 *									*
 * Clark, A. J., W. A. Gallus Jr., M. Xue, and F. Kong, 2009:  A	*
 *      comparison of precipitation forecast skill between the		*
 *      convection-allowing and large convection-parameterizing		*
 *      ensembles.  Wea. Forecasting, 24, 1121-1140.			*
 *									*
 * NOTE:  The irep array is used to track penetration into repeated	*
 *        order statistics values to facilitate accurate frequency	*
 *        matching.							*
 **									*
 * Log:									*
 * K. Brill/IMSG     20150625	First working version finished		*
 * K. Brill/IMSG     20150701   Error checking finalized		*
 ************************************************************************/
{
    int npt, noa, nof, i, jj, ii, found;
    float amx, amn, fmx, fmn, forval, swlo, ddif, fswt, awlo, qa;
    int *irep;
    float *aos, *fos;
/*----------------------------------------------------------------------*/
    *iret = 0;
    npt = *np;

    /*
     * Allocate temporary working arrays:
     */
    irep = (usint *) malloc ( npt * sizeof (usint) );
    aos = (xfloat *) malloc ( npt * sizeof (xfloat) );
    fos = (xfloat *) malloc ( npt * sizeof (xfloat) );

    /*
     * Loop through all the points to find extrema and initialize
     * working arrays.
     */
    noa = 0;
    nof = 0;
    amx = -1.1e31F;
    amn =  1.1e31F;
    fmx = -1.1e31F;
    fmn =  1.1e31F;
    for ( i = 0; i < npt; i++ ) {
        /*
         * Check for missing data.
         */
        if ( ! ERMISS ( xa [i] ) && ! ERMISS ( xf [i] ) ) {
	    aos [noa] = xa [i];
	    if ( aos [noa] > amx ) amx = aos [noa];
	    if ( aos [noa] < amn ) amn = aos [noa];
	    fos [nof] = xf [i];
            irep [nof] = 0;
	    if ( fos [nof] > fmx ) fmx = fos [nof];
	    if ( fos [nof] < fmn ) fmn = fos [nof];
	    noa++;
	    nof++;
        }
    }
/*
 *  Apply the quick sort C function to both the "analysis" (aos) and
 *  the "forecast" (fos).
*/
    qsort ( aos, noa, sizeof(float), floatcomp );
    qsort ( fos, nof, sizeof(float), floatcomp );
/*
 * Decrement noa and nof to serve as index upper bounds.
 */
    noa--;
    nof--;
    if ( noa != nof ) {
	*iret = -2;
	return;
    }

    if ( noa <= 1 || amx == amn || fmx == fmn ) {
	for ( jj = 0; jj < npt; jj++ ) {
	    xo [jj] = xf[jj];
	}
	*iret=1; /* Degenerate case implies a warning. */
	return;
    } else {
/*
 *	Do the frequency matching:
 */
	for ( jj = 0; jj < npt; jj++ ) {
	    xo [jj] = RMISSD;
	    if ( ! ERMISS (xf [jj]) && ! ERMISS (xa [jj]) ) {
		forval = xf [jj];
/*
 *		Find the position of this amount in the
 *		forecast order statistics.
 */
		found = G_FALSE;
		ii = 0;
		while ( ! found && ii < nof ) {
		    found = ( forval >= fos [ii] && forval < fos [ii+1] );
		    if ( forval == fos [ii] && fos [ii] == fos [ii+1] ) {
                        found = G_TRUE;
                        irep [ii] = irep [ii] + 1;
                        ii = ii + irep [ii];
                        if ( ii > nof ) ii = nof;
                    } else {
                        ii++;
                    }
		}

		ii = ii - 1;
/*
 *		Interpolate the quantile value position:
 */
                swlo = (float) ii;
		ddif = fos [ii+1] - fos [ii];
		if ( ddif > 1.0e-05F ) {
		    fswt = swlo + ( forval - fos[ii] ) / ddif;
		} else {
		    fswt = swlo;
		}
		if ( fswt >= (float) (ii+1) ) fswt = (float) (ii + 1);

		ii = (int) (fswt);
		if ( ii >= noa ) {
		    qa = aos [noa];
		} else {
/*
 *		    Interpolate the analysis between ii and ii+1.
 */
		    awlo = (float) ii;
		    qa = aos [ii] + ( aos [ii+1] - aos [ii] ) * ( fswt - awlo );
		    if ( qa > aos [noa] ) qa = aos [noa];
		}
		xo [jj] = qa;
	    }
	}
    }

    free (irep);
    free (aos);
    free (fos);
    return;
}
/*====================================================================*/
static int floatcomp (const void* elem1, const void* elem2)
{
    if ( *(const float*)elem1 < *(const float*)elem2 )
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}
