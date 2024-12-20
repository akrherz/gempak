      INTEGER FUNCTION LENGTH(KFILDO,IPACK,ND5,L3264B,LOC1,IPOS1,
     1                        LOC2,IPOS2,IER)
C
C        MARCH    2000   LAWRENCE  GSC/TDL   ORIGINAL CODING
C        JANUARY  2001   GLAHN     WRITE(KFILDO) MADE /D; COMMENTS
C        JANUARY  2001   GLAHN/LAWRENCE RESTRUCTURED SLIGHTLY;
C                                  ADDED TEST FOR IPOS1 AND IPOS2 IN
C                                  RANGE 1 TO L3264B
C        FEBRUARY 2001   GLAHN/LAWRENCE REMOVED RETURN 1
C        FEBRUARY 2002   GLAHN     PURPOSE COMMENT TO INCLUDE PACKING
C
C        PURPOSE
C           RETURNS THE NUMBER OF WHOLE OCTETS SEPARATING
C           THE WORD/BIT POSITION PAIRS LOC2,IPOS2 AND LOC1,IPOS1.
C           THE WORD POSITION IN IPACK INDICATED BY LOC2 NEEDS TO
C           BE GREATER THAN OR EQUAL TO THE WORD POSITION INDICATED
C           BY LOC1.  WHEN THE WORD POSITIONS ARE EQUAL, THEN THE
C           BIT POSITION IPOS2 MUST BE GREATER THAN OR EQUAL TO THE
C           BIT POSITION IPOS1.  OTHERWISE, THIS ROUTINE WILL
C           GENERATE AN ERROR.  THE RETURN LENGTH IS ALSO PACKED
C           INTO IPACK AT LOC1, IPOS1.
C
C           AN EXAMPLE:
C              ASSUMING THAT WE ARE WORKING ON SYSTEM WITH FOUR
C              BYTE (32 BIT) WORDS; LOC1 REPRESENTS WORD 1 IN
C              IPACK AND IPOS1 POINTS TO BIT 2 IN THAT WORD; AND
C              LOC2 REPRESENTS WORD 2 IN IPACK AND IPOS2 POINTS
C              TO BIT 31 IN THAT WORD, THE NUMBER OF WHOLE OCTETS
C              SEPARATING THESE TWO WORD/BIT POSITIONS WOULD BE 7.
C
C        DATA SET USE
C           KFILDO - UNIT NUMBER FOR OUTPUT (PRINT) FILE. (OUTPUT)
C
C        VARIABLES
C              KFILDO = UNIT NUMBER FOR OUTPUT (PRINT) FILE. (INPUT)
C            IPACK(J) = THE ARRAY THAT HOLDS THE ACTUAL PACKED MESSAGE
C                       (J=1,ND5). (INPUT/OUTPUT)
C                 ND5 = THE SIZE OF THE ARRAY IPACK( ). (INPUT)
C              L3264B = THE INTEGER WORD LENGTH IN BITS OF THE MACHINE
C                       BEING USED. VALUES OF 32 AND 64 ARE
C                       ACCOMMODATED. (INPUT)
C                LOC1 = INDICATES THE WORD POSITION AT THE LOWER
C                       END OF THE RANGE OF OCTETS.  (INPUT/OUTPUT)
C               IPOS1 = INDICATES THE BIT POSITION IN LOC1.
C                       (INPUT/OUTPUT)
C                LOC2 = INDICATES THE WORD POSITION AT THE UPPER
C                       BOUND OF THE RANGE OF OCTETS. (INPUT)
C               IPOS2 = INDICATES THE BIT POSITION IN LOC2.
C                       (INPUT)
C                 IER = RETURN STATUS CODE. (OUTPUT)
C                         0 = GOOD RETURN.
C                       1-4 = ERROR CODES GENERATED BY PKBG AND LENGTH.
C                         5 = LOC2 POINTS TO A WORD IN IPACK BEFORE
C                             THE WORD POINTED TO BY LOC1.
C                         6 = LOC2 AND LOC1 POINT TO THE SAME WORD, BUT
C                             IPOS2 POINTS TO A BIT POSITION BEFORE
C                             IPOS1.
C
C             LOCAL VARIABLES
C              LENGTH = THE WHOLE NUMBER OF OCTETS BETWEEN THE 
C                       LOC1,IPOS1 AND LOC2,IPOS2 PAIRS. THIS
C                       VALUE IS RETURNED TO THE CALLING ROUTINE.
C
C        NON SYSTEM SUBROUTINES CALLED
C           PKBG
C
      DIMENSION IPACK(ND5)
C
      IER=0
C
      IF(IPOS1.LE.0.OR.IPOS1.GT.L3264B.OR.
     1   IPOS2.LE.0.OR.IPOS2.GT.L3264B)THEN
         IER=2
         
C        CHECK TO MAKE SURE THAT LOC2 IS GREATER THAN LOC1. IF THEY
C        ARE EQUAL, MAKE SURE THAT IPOS2 IS GREATER THAN OR EQUAL TO
C        TO IPOS1.
C
      ELSEIF(LOC2.LT.LOC1)THEN
D        WRITE(KFILDO,10)
D10      FORMAT(/' LOC2 MUST BE GREATER THAN OR EQUAL TO LOC1.'/)
         IER=5
C
      ELSE IF(LOC2.EQ.LOC1)THEN
C
         IF(IPOS2.LT.IPOS1)THEN
D           WRITE(KFILDO,20)
D20         FORMAT(/' WHEN LOC2 AND LOC1 REPRESENT THE SAME WORD,'/
D    1              ' IPOS2 MUST BE GREATER THAN OR EQUAL TO IPOS1.'/)
            IER=6
C
         ENDIF
      ELSE
C
         LENGTH = (LOC2*L3264B/8 - (L3264B+1-IPOS2)/8)
     1          - (LOC1*L3264B/8 - (L3264B+1-IPOS1)/8)
         CALL PKBG(KFILDO,IPACK,ND5,LOC1,IPOS1,LENGTH,32,L3264B,IER,
     1             *900)
C
      ENDIF
C
 900  RETURN
      END
