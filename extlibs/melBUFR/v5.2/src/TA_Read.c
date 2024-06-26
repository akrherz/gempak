/*
 * TableA_Read - VERSION: %I%  %E% %T%
 */
/*
 * TableA_Read - Read descriptors for Code Table A from the given filename.
 * Return 1 on error, else 0.
 */

#include <mel_bufr.h>

#include <ctype.h>

#define MAX_BUF 1024

/*
..............START PROLOGUE....................................
 
  MODULE NAME:         TableA_Read
 
  DESCRIPTION:         Read descriptors for Code Table A from the given 
			filename.  Return 1 on error, else 0.
 
  CLASSIFICATION:      UNCLASSIFIED
 
  RESTRICTIONS:        NONE
 
  ORIGINAL PROGRAMMER, DATE:    J.R. Akin
				
  CURRENT PROGRAMMER:  V.L. Pastor
 
  LIBRARIES OF RESIDENCE:
 
  USAGE:
	int TableA_Read( char* FileName )
 
  PARAMETERS:
     NAME               TYPE        USAGE           DESCRIPTION
  -------------      ----------    -------   -------------------------
  FileName              Char        In          Input file name
 
  ERROR CONDITIONS:
    CONDITION              ACTION
  -------------------   ----------------------------------------------
  Filename is NULL      Write error message to buffer
                        Return with an error
  File doesn't open     Write error message to buffer
                        Return with an error
  No index              Write error message to buffer
                        Return with an error
 
  ADDITIONAL COMMENTS:
 
..............MAINTENANCE SECTION...............................
 
  MODULES CALLED:
 
  NAME                  DESCRIPTION
  ----                  -----------
  BUFR_Err_Set          Write error message to a buffer
  BUFR_IsComment        Check to see if line is a BUFR table comment line
  BUFR_strdup           Places a string into a pointer
 
  LOCAL VARIABLES AND STRUCTURES:
 
  NAME         	TYPE    	DESCRIPTION
  ----         	----    	-----------
  TableA        TableA_t        External table A array
  fp            FILE            File pointer
  buf           char            Character array for diagnostic messages
  tokbuf        char            Character array
  ndex          int             Table A index
  ndexp         char            Pointer to Table A index
  valp          char            Pointer to value
 
  METHOD:
        If the input filename is empty then
          Perform BUFR_Err_Set to write error message
          Return with error
        Endif
        If there is an open error then
          Perform BUFR_Err_Set to write error message
          Return with error
        Endif
        Loop while reading the file until the file is empty
          If the current line is a comment then
            drop to the bottom of the loop
          Endif
          Perform strcpy to copy the line buf to tokbuf
          Loop on the length of the line
            If the end of the line has whitespace then
              remove the whitespace
            Endif
          Endloop
          Perform strtok to get the next character pointer in a line
          If there is no character pointer then
            Perform BUFR_Err_Set to write error message
            Close the file
            Return with error
          Else
            Perform atoi to place the number in ndex
          Endif
          If the ndex is < 0 or greater than the maximum number of entries then
            Perform BUFR_Err_Set to write error message
            Close the file
            Return with error
          Endif
          If the table name is NULL then
            Perform BUFR_Err_Set to write error message
            Close the file
            Return with error
          Endif
          Loop through the string looking for next non-whitespace
           If the current character is a non-white space then
                break from loop
           Endif
          End Loop
          If the character is an end-of-line or a NULL character then
            Perform BUFR_Strdup to place 3 question marks in the table
          Else
            Perform BUFR_Strdup to place the string in the table
          Endif
        End loop
        Close the file
        Return with no error

 
  INCLUDE FILES:
    NAME                DESCRIPTION
  --------------     -------------------------------------------------
   mel_bufr.h        Defines bufr values
 
  COMPILER DEPENDENCES:  CC
 
  COMPILER OPTIONS:      -O
 
  MAKEFILE:
 
  RECORD OF CHANGES:
    INITIAL INSTALLATION:
    092997  LAH: Added uchar cast to correct Linux warning 
                 Added include to get character types 
    100997  LAH: Added ulong_t cast  
    022498 LAH:  Added prints to bufr_log file.

..............END PROLOGUE...................................... */
#if PROTOTYPE_NEEDED

int TableA_Read( char* FileName )

#else

int TableA_Read( FileName )
char*       FileName;

#endif
{
    extern TableA_t TableA[MAX_TABLE_A_ENTRIES];
    extern BUFR_Cntl_t BUFR_Cntl;

    FILE* fp;
    int   line;
    char  buf[MAX_BUF];
    char  tokbuf[MAX_BUF];  /* copy of buf[] for strtok() to work on */

    int   ndex;
    char  *ndxp;
    char  *valp;

    /*  092997  LAH: Added cast to correct Linux warning */
    if( FileName == NULL || *FileName == (char) NULL )
    {
        BUFR_Err_Set( "TableA_Read", "NULL Table A filename given" );
        fprintf(BUFR_Cntl.bufr_log,"%s: %s\n", "TableA_Read", 
	     "NULL Table A filename given" );
        return 1;
    }

    if( (fp = fopen( FileName, "r" )) == NULL )
    {
        sprintf( buf, "Can't open Table A file \"%s\"", FileName );
        BUFR_Err_Set( "TableA_Read", buf );
        fprintf(BUFR_Cntl.bufr_log,"%s: %s\n", "TableA_Read", buf);
        return 1;
    }

#if TRACE_PRINT
    if( BUFR_TraceLevel() > 1 )
    {
        fprintf(BUFR_Cntl.bufr_log, "Reading Table A (%s)\n", FileName );
    }
#endif

    for( line=1; fgets( buf, MAX_BUF, fp ) != NULL; line++ )
    {
        if( BUFR_IsComment( buf ) )
            continue;

        /*
         * Since strtok() mangles the string it works on, let strtok() use
         * a copy of buf.
         */

        strcpy( tokbuf, buf );

        /*
         * Trim trailing whitespace in 'tokbuf' (clean up description)
         */

       /*  092997  LAH: Added char cast to correct Linux warning */
       /* 100997 LAH: Added ulong_t cast */
       for( valp=&tokbuf[strlen(tokbuf)-1]; isspace( (ulong_t) *valp ); valp-- )
            *valp = (char) NULL;

        /* Get index */

        if( (ndxp = strtok( tokbuf, " \t" )) == NULL )
        {
            sprintf( buf,"%s, Line %d: Missing index", FileName, line );
            BUFR_Err_Set( "TableA_Read", buf );
            fprintf(BUFR_Cntl.bufr_log,"%s: %s\n", "TableA_Read", buf);
            fclose( fp );
            return 1;
        }
        else
            ndex = atoi( ndxp );

        if( ndex < 0 || ndex >= MAX_TABLE_A_ENTRIES )
        {
            sprintf( buf,
                "%s, Line %d: Index value of %d is outside the range 0 to %d",
                FileName, line, ndex, MAX_TABLE_A_ENTRIES-1 );

            BUFR_Err_Set( "TableA_Read", buf );
            fprintf(BUFR_Cntl.bufr_log,"%s: %s\n", "TableA_Read", buf);

            fclose( fp );
            return 1;
        }

        if( TableA[ndex] != NULL )
        {
            sprintf( buf,
                "%s, Line %d: Index value of %d has already been defined",
                FileName, line, ndex );

            BUFR_Err_Set( "TableA_Read", buf );
            fprintf(BUFR_Cntl.bufr_log,"%s: %s\n", "TableA_Read", buf);

            fclose( fp );
            return 1;
        }

        /*
         * strok() placed a null byte after the index number. We want to
         * ignore whitespace before the description but not within it.
         * Since strtok() doesn't provide a convenient way to deal with
         * this, set 'valp' to point to the first non-whitespace after
         * the null byte at the end of index value.  Trailing whitespace
         * has already been trimmed.
         */

        /* Ignore leading whitespace. */

        /*  092997  LAH: Added cast to correct Linux warning */
        for( valp=ndxp+strlen(ndxp)+1; *valp != (char) NULL; valp++ )
        {
            /* 100997 LAH: Added ulong_t cast */
            if( !isspace( (ulong_t) *valp ) )
                break;
        }

        /*  092997  LAH: Added cast to correct Linux warning */
        if( *valp == '\n' || *valp == (char) NULL )
            TableA[ndex] = BUFR_strdup( "???" );
        else
            TableA[ndex] = BUFR_strdup( valp );

#if DEBUG_PRINT
        if( BUFR_DebugLevel() > 8 )
        {
            fprintf(BUFR_Cntl.bufr_log, "%3d %s\n", ndex, TableA[ndex] );
        }
#endif
    }

    (void) fclose( fp );

    return 0;
}
