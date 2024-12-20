	SUBROUTINE DM_WDTI ( iflno, irow, icol, part, idthdr, idata, 
     +			     nword, iret )
C************************************************************************
C* DM_WDTI								*
C*									*
C* This subroutine writes integer data to a DM file.			*
C*									*
C* DM_WDTI  ( IFLNO, IROW, ICOL, PART, IDTHDR, IDATA, NWORD, IRET )	*
C*									*
C* Input parameters:							*
C*	IFLNO		INTEGER		File number			*
C*	IROW		INTEGER		Row number			*
C*	ICOL		INTEGER		Column number			*
C*	PART		CHAR*4		Part name			*
C*	IDTHDR (*)	INTEGER		Data header			*
C*	IDATA (NWORD)	INTEGER		Data				*
C*	NWORD		INTEGER		Length of data array		*
C*									*
C* Output parameters:							*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					 -4 = file not open		*
C*					 -6 = write error		*
C*					 -9 = invalid row or column	*
C*					-10 = invalid part name		*
C*					-13 = no write access		*
C*					-21 = incorrect data type	*
C**									*
C* Log:									*
C* M. desJardins/GSFC	 4/87						*
C* M. desJardins/GSFC	 3/89	Modified for grid packing		*
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
	INCLUDE		'dmcmn.cmn'
C
	CHARACTER*(*)	part
	INTEGER		idata (*), idthdr (*)
C------------------------------------------------------------------------
C*	Check that file is open.
C
	CALL DM_CHKF ( iflno, iret )
	IF  ( iret .ne. 0 ) RETURN
C
C*	Check that file was opened for write access.
C
	IF  ( .not. wflag (iflno) ) THEN
	    iret = -13
	    RETURN
	END IF
C
C*	Check for valid length.
C
	IF  ( nword .le. 0 )  RETURN
C
C*	Check for valid row and column positions.
C
	IF  ( ( irow .lt. 1 ) .or. ( irow .gt. krow (iflno) ) .or.
     +	      ( icol .lt. 1 ) .or. ( icol .gt. kcol (iflno) ) ) THEN
	    iret = -9
	    RETURN
	END IF
C
C*	Get part number.
C
	iprt = 0
	DO  i = 1, kprt ( iflno )
	    IF  ( kprtnm ( i, iflno ) .eq. part ) iprt = i
	END DO
	IF  ( iprt .eq. 0 )  THEN
	    iret = -10
	    RETURN
	END IF
C
C*	Check for valid data type.
C
	IF  ( ( ktyprt ( iprt, iflno ) .ne. MDINTG ) .and.
     +	      ( ktyprt ( iprt, iflno ) .ne. MDGRID ) )  THEN
	    iret = -21
	    RETURN
	END IF
C
C*	Get length of data header.
C
	ilenhd = klnhdr ( iprt, iflno )
C
C*	Check to see if data has already been written.
C*	First get pointer to data.
C
	ipoint = kpdata (iflno) + (irow-1) * kcol (iflno) * kprt (iflno)
     +		 + (icol-1) * kprt (iflno) + (iprt-1)
	CALL DM_RINT  ( iflno, ipoint, 1, istart, iret )
C
C*	Read length of previous data.
C
	IF ( istart .ne. 0 )  THEN
	    CALL DM_RINT  ( iflno, istart, 1, lenold, iret )
	  ELSE
	    lenold = 0
	END IF
C
C*	Get parameters for grid packing.
C
	IF  ( ktyprt ( iprt, iflno ) .eq. MDGRID )  THEN
	    CALL DM_GGPM  ( ipktyp, nbits, misflg, kxky, kx, ref,
     +			    scale, difmin, nadd, iret )
	    IF  ( iret .ne. 0 )  RETURN
	    lennew = nword + ilenhd + nadd
	  ELSE
	    lennew = nword + ilenhd
	END IF
C
C*	If old space is correct length, use it.
C
	IF  ( lenold .ne. lennew )  THEN
C
C*	    Otherwise, free old space.
C
	    IF  ( lenold .gt. 0 ) THEN
		length = lenold + 1
		CALL DM_AFRE ( iflno, length, istart, ier )
	    END IF
C
C*	    Get space for current data.
C
	    length = lennew + 1
	    CALL DM_GSPC  ( iflno, length, istart, iret )
	    IF  ( iret .ne. 0 )  RETURN
C
C*	    Write length to file.
C
	    CALL DM_WINT  ( iflno, istart, 1, lennew, iret )
	    IF  ( iret .ne. 0 )  RETURN
C*
	END IF
C
C*	Write header to file.
C
	isword = istart + 1
	IF  ( ilenhd .gt. 0 )  THEN
	    CALL DM_WINT  ( iflno, isword, ilenhd, idthdr, iret )
	    IF  ( iret .ne. 0 )  RETURN
	    isword = isword + ilenhd
	END IF
C
C*	Write data to file.
C
	IF  ( ktyprt ( iprt, iflno ) .eq. MDINTG )  THEN
	    CALL DM_WINT  ( iflno, isword, nword, idata, iret )
	  ELSE
	    kword = nword + nadd
	    CALL DM_WPKG  ( iflno, rdata, idata, isword, kword,
     +			    ipktyp, nbits, misflg, kxky, kx, ref, 
     +			    scale, difmin, iret )
	END IF
	IF  ( iret .ne. 0 )  RETURN
C
C*	Write pointer to file.
C
	CALL DM_WINT  ( iflno, ipoint, 1, istart, iret )
C*
	RETURN
	END
