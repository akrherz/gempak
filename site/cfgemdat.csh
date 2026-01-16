#!/bin/csh
#########################################################################
#
#  gemdat.csh 
#
#  This source file sets the site-specific data environment variables.
#
##
# Log:
# S. Jacobs/NCEP	 4/01	Started log
# S. Jacobs/NCEP	 5/01	Changed SPDATA
# S. Jacobs/NCEP	12/02	Added MODEL_SND and MODEL_SFC
# S. Jacobs/NCEP	10/07	Added MSCDT
# T. Piper/SAIC		01/08	Added NADATA1 and NADATA2 definitions
# B. Hebbard/SDB	04/24	Updated for New Compute Farm locations
#########################################################################

setenv NADATA1		/df/data
setenv NADATA2		/df/data/ldm

setenv AFOS_ALARMS	$NADATA2/afos/afos_alarms

setenv RAD		$NADATA1/images/radar
setenv SAT		$NADATA1/images/satellite

setenv MISC_DATA	$NADATA1/misc
setenv MSCDT		$NADATA1/misc

setenv SPDATA		$NADATA1/model
setenv MODEL		$NADATA1/model/gempak
setenv MODEL_SFC	$NADATA1/model/sfc
setenv MODEL_SND	$NADATA1/model/snd
setenv NTRANS_META	$NADATA1/model/meta

setenv OBS		$NADATA2/decoders
setenv TEXT_DATA2	$NADATA2/newnwx
setenv TEXT_DATA	$NADATA2/nwx

setenv GRPHGD		$HOME
