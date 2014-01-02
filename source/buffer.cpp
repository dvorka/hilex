#include "source\buffer.h"

// print info when hunting bugs
// #define DEBUG

extern bool Verbose;
extern bool OutToStd;
extern bool HTMLOutput;
extern bool CRLF;

//- class Bufffer methods ---------------------------------------------------------------------------------

#define ERROR_WHEN_READING \
 {                         \
  printf("\n ERROR reported from %s line %d:"					  \
	 "\n  Error reading from the file...", __FILE__, __LINE__ );\
  exit(0);                                                                \
 }

Buffer::Buffer( char *InputFileName, char *OutputFileName, word BufferSize, bool iUnix )
{

 // alloc inner buffers
 if( InputFileName!=NULL )
 {
  InBuffer=(char far *)farmalloc(BufferSize);
  if(!InBuffer) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 }
 else
  InBuffer=NULL;

 if( OutputFileName!=NULL )
 {
  OutBuffer=(char far *)farmalloc(BufferSize);
  if(!OutBuffer) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 }
 else
  OutBuffer=NULL;

 // open file in
 // ---------------
 // - BINARY MODE -
 // ---------------
 // so 0xD 0xA => 0xA translation is done

 if( InputFileName!=NULL )
 {
  if((InHandle = open( InputFileName, O_RDONLY | O_BINARY, S_IWRITE | S_IREAD)) == -1)
  {
   printf("\n ERROR reported from %s line %d:"
	  "\n  Error opening input file %s...", __FILE__, __LINE__, InputFileName );
   exit(0);
  }
 }

 // open file in
 // ---------------
 // - BINARY MODE -
 // ---------------
 // so 0xD 0xA => 0xA translation is done

 if( OutputFileName!=NULL )
 {
  if((OutHandle = open( OutputFileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IWRITE | S_IREAD)) == -1)
  {
   printf("\n ERROR reported from %s line %d:"
	  "\n  Error opening output file %s...", __FILE__, __LINE__, OutputFileName );
   exit(0);
  }
 }

 Unix=iUnix;

 InBeg
 =InPos
 =InBufPos
 =0;

 BufSize  =BufferSize;
 InputLng =filelength( InHandle );
 InEnd    =BufferSize-1;

 if( (InBufEnd = read( InHandle, InBuffer, BufSize )) == 0xFFFF ) ERROR_WHEN_READING
 else
 {
  if( Verbose ) printf("-");
 }

 #ifdef DEBUG
  printf("\n Buffer: loaded interval <%lu,%lu> buf <0,%u>\n", InBeg, InEnd, InBufEnd );
 #endif

 // pointers for output buffer
 OutBufPos = 0;

}

//--------------------------------------------------------------------------------

dword Buffer::GetChar( char &C )
// - returns 0 if end of file else
//   returns how many positions moved in input ( if 0xD0xA returns 2 else 1 )
//
// - BufPos point to not returned ( counted from 0 ! )
// - BufSize size of buffer in bytes
// - BufEnd nr of bytes in buffer
{

 #ifdef DEBUG
  printf("\n Buffer: G profile before int <%lu,%lu> buf <0,%u> bufpos %u pos %lu\n", InBeg, InEnd, InBufEnd, InBufPos, InPos );
 #endif

 if( InBufPos >= InBufEnd && InEnd > InputLng ) return FALSE;

 // --- Fault ---
 if( InBufPos >= InBufEnd ) //
 {
  InBeg+=BufSize;
  InEnd+=BufSize;

  //  InPos =InPos-InBufEnd+BufSize; was changed to ++InPos;

  ++InPos;	// move in input file

  if((InBufEnd=read( InHandle, InBuffer, BufSize )) == 0xFFFF )
   return FALSE; // end of file
  else
  {
   if( Verbose ) printf("-");
  }

  #ifdef DEBUG
   printf("\n Buffer: G loaded interval <%lu,%lu> buf <0,%u> pos %lu\n", InBeg, InEnd, InBufEnd, InPos );
  #endif

  InBufPos=0;

  C=InBuffer[InBufPos++];

  if( C==0xD ) // get the second char 0xA->0xD 0xA
  {
   return GetChar( C )+1lu;	// probably 0xA
  }
  else
   return 1lu;
 }
 else
 {
  C=InBuffer[InBufPos];

  InBufPos++;	// move to the not returned position in buffer

  if( ++InPos >= InputLng+1 ) return FALSE;	// move in file

  if( C==0xD ) // get the second char 0xA->0xD 0xA
  {
   return GetChar( C )+1lu;	// probably 0xA
  }
  else
   return 1lu;
 }
}

//--------------------------------------------------------------------------------

void Buffer::PutChar( char C )
// is used to put whitespaces to output
{
 char CrLf[]={ 13, 10 };



 if( C==0xD && CRLF==FALSE )	// CR, if not wanted jump across, else put ( will not be duplicated )
  return;

 if( CRLF && C==0xA )
  Save( CrLf, 2u );
 else
  Save( &C, 1u );


/*
 word k;

 OutBuffer[OutBufPos++] = C;

 // check fault of output buffer
 if( OutBufPos >= BufSize )
 {
  if( OutToStd )
  {
   for( k=0; k<BufSize; k++ )
    fprintf( stdout, "%c", OutBuffer[k] );
  }
  else // save to file
  {
   // save content of buffer to output file
   if( write( OutHandle, OutBuffer, BufSize )==-1 )
   {
    printf("\n ERROR reported from %s line %d:"
	  "\n  Failed when write to output file...", __FILE__, __LINE__ );
    exit(0);
   }
   else
   {
    if( Verbose ) printf("-");
   }
  }
  // set pointer
  OutBufPos =  0;
 }
*/
}

//--------------------------------------------------------------------------------

word Buffer::Load( dword From, dword To, char far *&Ptr )
{
 word  Lenght = (word)To-(word)From+1,
       Begin,
       i,
       j;
 dword lBegin;

 if( InBeg<=From && InEnd>=To ) // is interval in memory
 {

  // alloc memory for element
  Ptr = new char[Lenght+1];
			if(!Ptr) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  // set pointers
  lBegin = From%(dword)BufSize;
  Begin  = (word)lBegin;

  // copy it to buffer
  for( i=Begin, j=0; j<Lenght; i++, j++ )
  {
   Ptr[j] = InBuffer[i];
  }

  // finish string with NULL
  Ptr[j]='\0';

  return HI_OK;
 }
 else
 {
  // is not in buffer -> load it from file ( leave everything in original pos )

  // remember position in file to set it after load to original pos
  long TellPos=tell( InHandle );

  // seek to beginning of element in file
  lseek( InHandle, (long)From, SEEK_SET);

  // alloc memory for element
  Ptr = new char[Lenght+1];
			if(!Ptr) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  if( read( InHandle, Ptr, Lenght ) == -1 )
  {
   printf("\n ERROR reported from %s line %d:"
	  "\n  Buffer: fail to load element in interval <%lu,%lu>", __FILE__, __LINE__, From, To );
   exit(0);
  }
  else
  {
   if( Verbose ) printf("-");
  }

  // finish string with NULL
  Ptr[j]='\0';

  // set position in file to original position
  lseek( InHandle, TellPos, SEEK_SET);

  return HI_OK;
 }
}

//--------------------------------------------------------------------------------

word Buffer::Copy( dword From, dword To )
// copy copies strings from input buffer to output buffer,
// it is input file, in input file I must transform < and others
// to html: < -> &lt; ( if wanted )
{
 word  Lenght = (word)To-(word)From+1,
       Begin,
       i,
       j;
 dword lBegin;
 char  CrLf[]={ 13, 10 };

 if( InBeg<=From && InEnd>=To ) // is interval in memory
 {
  // copy it from input buffer to output buffer

  // set pointers
  lBegin = From%(dword)BufSize;
  Begin  = (word)lBegin;

  // copy it to output buffer
  for( i=Begin, j=0; j<Lenght; i++, j++ )
  {

   if( InBuffer[i]==0xD  ) // filter out CR, will be inserted later
    continue;


   if( HTMLOutput )
   {
    // transformation to HTML
    switch( InBuffer[i] )
    {
     case '<':
	     Save( "&lt;", 4u );
	     break;
     case '>':
	     Save( "&gt;", 4u );
	     break;
     case '&':
	     Save( "&amp;", 5u );
	     break;
     case '"':
	     Save( "&quot;", 6u );
	     break;
     default:
	     if( CRLF && InBuffer[i]==10 )
	      Save( CrLf, 2u );
	     else
	      Save( InBuffer+i, 1u );
	     break;
    }
   }
   else
   {
    if( CRLF && InBuffer[i]==10 )
     Save( CrLf, 2u );
    else
     Save( InBuffer+i, 1u );
   }

  }

  return HI_OK;
 }
 else	// interval must be loaded
 {
  // remember position in file to set it after load to original pos
  long TellPos=tell( InHandle );

  // seek to beginning of element in file
  lseek( InHandle, (long)From, SEEK_SET);



  word FreeInBuffer,
       HlpLenght;

  char far *HelpBuffer=(char far *)farmalloc(BufSize);
			if(!HelpBuffer) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }


  while( Lenght )
  {
   FreeInBuffer = BufSize;

   if( Lenght > FreeInBuffer )
    HlpLenght    = FreeInBuffer;
   else
    HlpLenght    = Lenght;

   if( read( InHandle, HelpBuffer, HlpLenght ) == -1 )
   {
    printf("\n ERROR reported from %s line %d:"
	   "\n  Buffer: fail to load element in interval <%lu,%lu>", __FILE__, __LINE__, From, To );
    exit(0);
   }
   else
   {
    if( Verbose ) printf("-");
   }


   Lenght    -= HlpLenght;

   // copy it to output buffer
   for( i=0, j=0; j<Lenght; i++, j++ )
   {

    if( InBuffer[i]==0xD ) 	  // filter out CRs
     continue;

    if( CRLF && InBuffer[i]==0xA ) // will be add here if wanted
     Save( CrLf, 2u );

    if( HTMLOutput )
    {
     // transformation to HTML
     switch( InBuffer[i] )
     {
      case '<':
	      Save( "&lt;", 4u );
	      break;
      case '>':
	      Save( "&gt;", 4u );
	      break;
      case '&':
	      Save( "&amp;", 5u );
	      break;
      case '"':
	      Save( "&quot;", 6u );
	      break;
      default:
	      if( CRLF && HelpBuffer[i]==10 )
	       Save( CrLf, 2u );
	      else
	       Save( HelpBuffer+i, 1u );
	      break;
     }
    }
    else
    {
     if( CRLF && HelpBuffer[i]==10 )
      Save( CrLf, 2u );
     else
      Save( HelpBuffer+i, 1u );
    }
   }
  } // while

  farfree(HelpBuffer);



  // set position in file to original position
  lseek( InHandle, TellPos, SEEK_SET);

  return HI_OK;

 } // else

}

//--------------------------------------------------------------------------------

word Buffer::Save( char far *Ptr, word Size )
// is used to copy inner strings to output
{
 word j,
      k;

 if( Ptr==NULL ) return HI_OK;

 // copy it to buffer
 for( j=0; j<Size; j++ )
 {

  if( Ptr[j]==0xD && CRLF==FALSE ) // CR, if not wanted jump across
   continue;

  OutBuffer[OutBufPos++] = Ptr[j];

  // check fault of output buffer
  if( OutBufPos >= BufSize )
  {
   if( OutToStd )
   {
    for( k=0; k<BufSize; k++ )
     fprintf( stdout, "%c", OutBuffer[k] );
   }
   else // save to file
   {
    // save content of buffer to output file
    if( write( OutHandle, OutBuffer, BufSize )==-1 )
    {
     printf("\n ERROR reported from %s line %d:"
	   "\n  Failed when write to output file...", __FILE__, __LINE__ );
     exit(0);
    }
    else
    {
     if( Verbose ) printf("-");
    }
   }

   // set pointer
   OutBufPos =  0;
  }
 } // for

 return HI_OK;

}

//--------------------------------------------------------------------------------

word Buffer::Append( dword From, dword To )
{
 // dodelat, kdyz je mezi dvemi nasledujicimi intervaly

 word Size=(word)To-(word)From+1u;

 write( OutHandle, (InBuffer+(word)From), Size );

 return HI_OK;
}

//--------------------------------------------------------------------------------

bool Buffer::Seek( dword iPos )
// returns FALSE if iPos is too long
{

 dword Where;

// iPos++;

 // DO NOT load if already loaded
 if( InBeg<=iPos && iPos<InEnd )
 {
  InPos   =iPos;
  InBufPos=(word)(iPos%((dword)BufSize));

  if(
      InPos > InputLng
      ||
      InBufPos >=InBufEnd
    )
  {
   InPos=InputLng;
   return FALSE;
  }

  #ifdef DEBUG
   printf("\n Buffer: Seek iPos %lu Pos %lu already loaded <%lu,%lu> buf <0,%u> bufpos %u\n", iPos, InPos,  InBeg, InEnd, InBufEnd, InBufPos );
  #endif
 }
 else // else interval must be loaded
 {
  Where=(iPos/((dword)BufSize))*((dword)BufSize);

  lseek( InHandle, Where, SEEK_SET);

  InBeg   =Where;
  InPos   =iPos;
  InBufPos=(word)(iPos%((dword)BufSize));
  InEnd   =InBeg+BufSize-1;

  if( (InBufEnd = read( InHandle, InBuffer, BufSize )) == 0xFFFF )
  {
   // end of file
   InPos=InputLng;

   return FALSE;
  }
  else
  {
   if( Verbose ) printf("-");
  }


  if( InPos > InputLng ) { InputLng=InPos; return FALSE; }

  #ifdef DEBUG
   printf("\n Buffer: Seek iPos %lu Pos %lu <%lu,%lu> buf <0,%u> bufpos %u\n", iPos, InPos, InBeg, InEnd, InBufEnd, InBufPos );
  #endif
 }

 return TRUE;
}

//--------------------------------------------------------------------------------

dword Buffer::GetPos( void )
// - Pos points to not returned => return dec
// - position in input file
{
 return InPos-1ul;
}

//--------------------------------------------------------------------------------

Buffer::~Buffer( void )
{
 word k;

 if( InBuffer )
 {
  close( InHandle );
  farfree( InBuffer );
 }

 if( OutBuffer )
 {
  // what's inside buffer and not written must be written
  if( OutBufPos != 0 )
  {
   if( OutToStd )
   {
    for( k=0; k<OutBufPos; k++ )
     fprintf( stdout, "%c", OutBuffer[k] );
   }
   else // save to file
   {
    // save content of buffer to output file
    if( write( OutHandle, OutBuffer, OutBufPos )==-1 )
    {
     printf("\n ERROR reported from %s line %d:"
	    "\n  Failed when write to output file...", __FILE__, __LINE__ );
     exit(0);
    }
    else
    {
     if( Verbose ) printf("-");
    }
   }
  }

  farfree( OutBuffer );
  close( OutHandle );
 }
}

//- EOF ----------------------------------------------------------------------------
