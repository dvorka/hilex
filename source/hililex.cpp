#include "source\hililex.h"

// print info when hunting bugs
// #define DEBUG

//-----------------------------------------------------------------------------

bool IsWhiteSpace( char C )
{

 switch( C )
 {
  case ' ':
  case '\t':
  case '\n':
	   return TRUE;
  default:
	   return FALSE;
 }

}

//- class Lexicator methods --------------------------------------------------------

Lexicator::Lexicator( void )
{
 Correct = TRUE;

 ConstructBuildInHiliLex();
}

//-----------------------------------------------------------------------------

word Lexicator::SniftSnift( char *InFileName, char *OutFileName )
{
 Buffer far *Buf=new Buffer( InFileName, OutFileName, 10000u ); if(!Buf) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 dword  Beg=0,
        End,
        OldBeg=0;

 ElementID EleID;
 bool      Success;
 char      C;

 #ifdef DEBUG
  printf("\n\n - Snifter ----------------------------------------------------\n");
 #endif

 for(;;)
 {
  OldBeg=Beg;

  Success=Akceptor->Accept( Buf, Beg, End, (LexicatorType)EleID );

  if( Success )
  {
   #ifdef DEBUG
    printf("\n Element: Beg %lu End %lu ElementID %u\n", Beg, End, EleID );
   #endif

   OldBeg=Beg=End+1;
   Buf->Append( Beg, End );

   continue;
  }
  else
  {
   Beg=OldBeg;
   Buf->Seek( Beg );

   Success=Buf->GetChar( C );

   if( !Success )
   {
    delete Buf;
    #ifdef DEBUG
     printf("\n - Snifter ----------------------------------------------------");
    #endif
    return HI_OK; // end of input
   }

   if( IsWhiteSpace(C) ) // step cross white space which was no accepted
   {
    #ifdef DEBUG
     printf("\n WhiteSpace: \'%d\'\n",C);
    #endif

    Beg++;
   }
   else
   {
    delete Buf;
    #ifdef DEBUG
     printf("\n\n - Snifter ----------------------------------------------------");
    #endif
    return HI_OK; // end of input
   }
  } // if success
 }
}

//-----------------------------------------------------------------------------

Lexicator::~Lexicator( void )
{
 delete Akceptor;
}

//-----------------------------------------------------------------------------

void Lexicator::PrepareForPop( char *InFileName )
{
 Buf=new Buffer( InFileName, NULL, 10000u ); // output buffer not used
     if(!Buf) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 Beg=0;
 OldBeg=0;
}

//-----------------------------------------------------------------------------

void Lexicator::CancelPop( void )
{
 delete Buf;
}

//-----------------------------------------------------------------------------

bool Lexicator::Pop( SyntactNode *&Element )
// - takes elements from input file
// - input is buffered
// - lex element "COMMENTS" is ignored -> so not returned
// - when the last item is get, buffer is automaticly deleted
{

 ElementID EleID;
 bool      Success;
 char      C;

 #ifdef DEBUG
  printf("\n - POP -");
 #endif

  OldBeg=Beg;

  Success=Akceptor->Accept( Buf, Beg, End, (LexicatorType)EleID );

  if( Success )
  {
   #ifdef DEBUG
    printf("\n POP Element: Beg %lu End %lu ElementID %u\n", Beg, End, EleID );
   #endif

   // create node and return it's adress

   switch( EleID )
   {
    case LEXICATOR_LBRACE:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_LBRACE;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_RBRACE:
	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_RBRACE;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_SEMICOLON:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_SEMICOLON;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_DESCR:       // keywords
    case LEXICATOR_ACTION:
    case LEXICATOR_RAW:
    case LEXICATOR_RAWBEG:
    case LEXICATOR_RAWEND:
    case LEXICATOR_FONT:
    case LEXICATOR_SIZE:
    case LEXICATOR_COLOR:
    case LEXICATOR_UNDERSCORE:
    case LEXICATOR_ITALIC:
    case LEXICATOR_BOLD:
    case LEXICATOR_OUTPUT:
    case LEXICATOR_TITLE:
    case LEXICATOR_FACEFONT:
    case LEXICATOR_BASECOLOR:
    case LEXICATOR_BASESIZE:
    case LEXICATOR_BGCOLOR:
    case LEXICATOR_BACKGROUND:
    case LEXICATOR_WIDTH:
    case LEXICATOR_INTABLE:
    case LEXICATOR_OUTTABLE:
    case LEXICATOR_CRLF:
    case LEXICATOR_HTMLOUTPUT:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_k;
	   Element->Data.k.Key	=(TokenType)EleID; // HAS THE SAME INT VALUE as returned from automat
						   // see hitypes.h
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_EQU:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_EQU;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_STRING:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_s;
	   Element->Data.s.Beg	=Beg;
	   Element->Data.s.End	=End;
	   // load element
	   Buf->Load( Beg, End, Element->Data.s.Element );
	   Element->Data.s.ID	=EleID;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_COMMENTS:
	  // dive for valid element -> comments are ignored
	  OldBeg=Beg=End+1;
	  return Pop( Element );

    case LEXICATOR_LPARENT:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_LPARENT;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_RPARENT:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_RPARENT;
	  OldBeg=Beg=End+1;
	  return TRUE;

    case LEXICATOR_ELEMENT:	// head keywords
    case LEXICATOR_SETUP:

	  Element=new SyntactNode; if(!Element) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   Element->Type 	=SNODE_h;
	   Element->Data.h.Key	=(TokenType)EleID; // HAS THE SAME INT VALUE
						   // see hitypes.h
	  OldBeg=Beg=End+1;
	  return TRUE;

    default:
	  OldBeg=Beg=End+1;
	  return FALSE;
   }

  }
  else
  {
   Beg=OldBeg;
   Buf->Seek( Beg );

   Success=Buf->GetChar( C );

   if( !Success )
   {
    delete Buf;
    #ifdef DEBUG
     printf("\n - POP cleaned -");
    #endif
    return FALSE; // end of input
   }

   if( IsWhiteSpace(C) ) // step cross white space which was no accepted
   {
    #ifdef DEBUG
     printf("\n POP WhiteSpace: \'%d\'\n",C);
    #endif

    Beg++;

    // dive because this is whitespace
    return Pop( Element );

   }
   else
   {
    delete Buf;

    Correct=FALSE;


    #ifdef DEBUG
     printf("\n - POP cleaned, not accepted whitespace %c -", C );
    #endif
    return FALSE; // end of input
   }
  } // if success
}

//-----------------------------------------------------------------------------

bool Lexicator::CorrectFinish( dword &ErrPos )
{
 if( !Correct )
 {
  ErrPos=Beg;

  return FALSE;
 }
 else
 {
  ErrPos=0;

  return TRUE;
 }
}

//-----------------------------------------------------------------------------

word Lexicator::ConstructBuildInHiliLex( void )
{
 DynamicAutomat far *DAkceptor = new DynamicAutomat(),
		far *HelpDAkceptor;

		if(!DAkceptor) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 word		    i;

 // position in array is the same as ID of element in LexicatorsType
 // see hitypes.h

 char *Descriptor[] ={
			 // ghost
		       ""
		       , // element
		       "[Ee][Ll][Ee][Mm][Ee][Nn][Tt]"
		       , // descr
		       "[Dd][Ee][Ss][Cc][Rr]"
		       , // action
		       "[Aa][Cc][Tt][Ii][Oo][Nn]"
		       , // raw
		       "[Rr][Aa][Ww]"
		       , // raw_beg
		       "[Rr][Aa][Ww]_[Bb][Ee][Gg]"
		       , // raw_end
		       "[Rr][Aa][Ww]_[Ee][Nn][Dd]"
		       , // font
		       "[Ff][Oo][Nn][Tt]"
		       , // size
		       "[Ss][Ii][Zz][Ee]"
		       , // color
		       "[Cc][Oo][Ll][Oo][Rr]"
		       , // underscore
		       "[Uu][Nn][Dd][Ee][Rr][Ss][Cc][Oo][Rr][Ee]"
		       , // italic
		       "[Ii][Tt][Aa][Ll][Ii][Cc]"
		       , // bold
		       "[Bb][Oo][Ll][Dd]"
		       , // setup
		       "[Ss][Ee][Tt][Uu][Pp]"
		       , // output
		       "[Oo][Uu][Tt][Pp][Uu][Tt]"
		       , // crlf
		       "[Cc][Rr][Ll][Ff]"
		       , // htmloutput
		       "[Hh][Tt][Mm][Ll][Oo][Uu][Tt][Pp][Uu][Tt]"
		       , // title
		       "[Tt][Ii][Tt][Ll][Ee]"
		       , // face_font
		       "[Ff][Aa][Cc][Ee]_[Ff][Oo][Nn][Tt]"
		       , // basecolor
		       "[Bb][Aa][Ss][Ee][Cc][Oo][Ll][Oo][Rr]"
		       , // basesize
		       "[Bb][Aa][Ss][Ee][Ss][Ii][Zz][Ee]"
		       , // bgcolor
		       "[Bb][Gg][Cc][Oo][Ll][Oo][Rr]"
		       , // background
		       "[Bb][Aa][Cc][Gg][Rr][Oo][Uu][Nn][Dd]"
		       , // width
		       "[Ww][Ii][Dd][Tt][Hh]"
		       , // intable
		       "[Ii][Nn][Tt][Aa][Bb][Ll][Ee]"
		       , // outtable
		       "[Oo][Uu][Tt][Tt][Aa][Bb][Ll][Ee]"
		       , // lbrace
		       "\\{"
		       , // rbrace
		       "\\}"
		       , // semicolon
		       ";"
		       , // equ
		       "="
		       , // string
		       "\\\"(([^\\\"])|(\\\\\\\"))*\\\""
		       , // comments
		       "#[^\\n]*\\n"
		       , // lparent
		       "\\("
		       , // rparent
		       "\\)"

		     };

 // begin with the first element ( 0 is ghost )
 i=1;

  DAkceptor->StringToDynaAuto( Descriptor[i], i ); // i corresponds to enum ID

 i++;

 // make the rest
 while( i<=LEXICATOR_RPARENT ) // the highes lex element ID
 {

  HelpDAkceptor=new DynamicAutomat(); if(!HelpDAkceptor) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
   HelpDAkceptor->StringToDynaAuto( Descriptor[i], i );
   DAkceptor->Wampirator( *HelpDAkceptor );
  delete HelpDAkceptor;

  i++;
 }

 DAkceptor->Save("HILEX.A");

 Akceptor=new StaticAutomat("HILEX.A"); if(!Akceptor) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 #ifdef DEBUG
  Akceptor->Print();
 #endif

 delete DAkceptor;

 return HI_OK;
}

//-EOF ----------------------------------------------------------------------------
