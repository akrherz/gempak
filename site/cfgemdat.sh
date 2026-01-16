#!/bin/sh
#########################################################################
#
#  gemdat.sh 
#
#  This Bourne-shell script sets the site-specific data environment variables.
#
##
# Log:
# S. Jacobs/NCEP	 4/01	Started log
# S. Jacobs/NCEP	 5/01	Changed SPDATA
# S. Jacobs/NCEP	12/02	Added MODEL_SND and MODEL_SFC
# S. Jacobs/NCEP	10/07	Added MSCDT
# T. Piper/SAIC         01/08   Added NADATA1 and NADATA2 definitions
# B. Hebbard/SDB	04/24	Updated for New Compute Farm locations
#########################################################################

export NADATA1=/df/data
export NADATA2=/df/data/ldm

export AFOS_ALARMS=$NADATA2/afos/afos_alarms

export RAD=$NADATA1/images/radar
export SAT=$NADATA1/images/satellite

export MISC_DATA=$NADATA1/misc
export MSCDT=$NADATA1/misc

export SPDATA=$NADATA1/model
export MODEL=$NADATA1/model/gempak
export MODEL_SFC=$NADATA1/model/sfc
export MODEL_SND=$NADATA1/model/snd
export NTRANS_META=$NADATA1/model/meta

export OBS=$NADATA2/decoders
export TEXT_DATA2=$NADATA2/newnwx
export TEXT_DATA=$NADATA2/nwx

export GRPHGD=$HOME
