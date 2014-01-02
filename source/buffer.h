#ifndef __BUFFER_H
 #define __BUFFER_H

 #include "alloc.h"
 #include "conio.h"
 #include "fcntl.h"
 #include "io.h"
 #include "sys\stat.h"
 #include "stdio.h"
 #include "stdlib.h"
 #include "source\hitypes.h"
 #include "source\hierrors.h"

 // --- BINARY BUFFER ---

 class far Buffer
 {

  public:
   Buffer( char *InputFileName, char *OutputFileName, word BufferSize, bool iUnix=FALSE );
    dword GetChar( char &C );	// if 0xD 0xA returns only 0xA ( UNIX 0xA, DOS 0xD 0xA )
    void  PutChar( char C );	// Unix==FALSE => 0xA->0xD 0xA  to OutBuffer
    word  Load( dword From, dword To, char far *&Ptr );
    word  Copy( dword From, dword To ); // from input buffer to output buffer
    word  Save( char far *Ptr, word Size  ); // copy string to output buffer
    bool  Seek( dword iPos );	// seek in input file
    dword GetPos( void );

    word Append( dword From, dword To ); // not buffered output

   ~Buffer();

  private:
   char     far *InBuffer;
   char     far *OutBuffer;

   int      InHandle,	// handle of corresponding file
	    OutHandle;	// handle of output file

   long     InputLng;   // lng of input file in bytes

   word	    BufSize;	// size of buffer in bytes

	    // --- INPUT ---
   dword    InPos,	// Pos in file, points to not returned
	    InBeg,	// begin of interval in file
	    InEnd;	// end of interval in file

   word     InBufPos,	// position in buffer
	    InBufEnd;   // bytes read to buffer

	    // --- OUTPUT ---
   word     OutBufPos,	// position in output buffer
	    OutBufEnd;  // bytes read to buffer

   bool	    Unix;
 };

#endif
