	SUBROUTINE SFKDAT  ( iflno, dattim, newfil, datcur,
     +			     ntime, times, iret )
C************************************************************************
C* SFKDAT								*
C*									*
C* This subroutine processes the date/time entered by the user.		*
C*									*
C* SFKDAT  ( IFLNO, DATTIM, NEWFIL, DATCUR, NTIME, TIMES,		*
C*           IRET )							*
C*									*
C* Input parameters:							*
C*	IFLNO		INTEGER		Surface file number		*
C*	DATTIM		CHAR*		Date/time input			*
C*	NEWFIL		LOGICAL		New file flag			*
C*									*
C* Input and output parameters:						*
C*	DATCUR		CHAR*		Current date/time		*
C*									*
C* Output parameters:							*
C*	NTIME		INTEGER		Number of times 		*
C*	TIMES (NTIME)	CHAR*		Times 				*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					 -1 = invalid time		*
C**									*
C* Log:									*
C* K. Tyle/GSC		 4/97	Based on SFLDAT				*
C* T. Piper/SAIC	 4/02	Fixed UMR; increased tout to 20		*
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER*(*)   datcur, dattim, times (*)
	LOGICAL         newfil
C*
	CHARACTER	timfil (LLMXTM)*20, tout*20
C------------------------------------------------------------------------
	iret = 0
C
C*	Check for current valid time.
C
	CALL ST_LCUC  ( dattim, dattim, ier )
	IF  ( ( dattim .ne. datcur ) .or. ( dattim .eq. 'LIST' ) .or.
     +	      ( newfil ) .or. ( dattim .eq. ' ' ) )  THEN
	    datcur = ' '
C
C*	    Get times from file.
C
	    CALL SF_GTIM  ( iflno, LLMXTM, ntimf, timfil, ier )
	    IF  ( ier .ne. 0 )  THEN
	        CALL ER_LMSG  ( 0, 'SF', ier, ' ', ierr )
		iret   = -1
	        RETURN
	    END IF
C
C*	    Find the time.
C
	    CALL TI_FIND  ( dattim, ntimf, timfil, tout, ntime, 
     +	                    times, ier )
	    IF  ( ier .eq. 0 )  THEN
	        datcur = dattim
	      ELSE 
		iret   = -1
	    END IF
	END IF
C*
	RETURN
	END
