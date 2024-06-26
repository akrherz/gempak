/* include file containing all AODT library global variables */
#include "../inc/odtlib.h"
/* include file containing all AODT library variable definitions */
#include "../inc/odtlibdefs-x.h"
/* include file containing format statements for shared functions */
#include "../inc/odtlibfuncs.h"

int aodtv72_getIRimageinfo( int *date,int *time,int *sat,char *csat) 
/* return IR image information to application from AODT library
   Inputs : none
   Outputs: AODT library IR image date/time/satellite information
   Return : 0 : o.k.
*/
{
  char *retstrng;

  retstrng=(char *)calloc((size_t)12,(size_t)sizeof(char));
  
  /* set IR image date to return variable */
  *date=odtcurrent_v72->IR.date;
  /* set IR image time to return variable */
  *time=odtcurrent_v72->IR.time; 
  /* assign IR image satellite type to return variable */
  *sat=odtcurrent_v72->IR.sattype; 
  (void)aodtv72_sattypes(odtcurrent_v72->IR.sattype,retstrng);
  strcpy(csat,retstrng);
  csat[strlen(retstrng)]='\0';
  free(retstrng);

  return 0;
}

int aodtv72_setIRimageinfo( int date,int time,int sat)
/* set IR image date/time within AODT library memory
   Inputs : AODT library IR image date/time/satellite information
   Outputs: none
   Return : 0 : o.k.
*/
{
  /* assign IR image date to AODT library variable */
  odtcurrent_v72->IR.date=date; 
  /* assign IR image time to AODT library variable */
  odtcurrent_v72->IR.time=time; 
  /* assign IR image satellite type to AODT library variable */
  odtcurrent_v72->IR.sattype=sat; 

  return 0;
}
