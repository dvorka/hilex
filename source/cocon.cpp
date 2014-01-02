#include "source/cocon.h"

// print info when hunting bugs
// #define DEBUG

#define EXIT_PROGRAM exit(0);

extern bool HTMLOutput;
extern bool CRLF;

//- struct StaticTableItem --------------------------------------------------------------------------

#define ERROR_WHEN_READING \
 {                         \
  printf("\n ERROR reported from %s line %d:"                                     \
	 "\n Error reading from the file with handle %u...", __FILE__, __LINE__, Handle );\
  EXIT_PROGRAM;                                                                \
 }

void StaticTableItem::Load( int Handle )
{
 word Lng;

 // DO NOT save ID - its index

 // load Action
 if( read( Handle, &Action, 2u ) != 2u ) ERROR_WHEN_READING

 // load RawBeg
 if( read( Handle, &Lng, 2u ) != 2u ) ERROR_WHEN_READING

 if( Lng == 0 )
 {
  RawBeg = NULL;
 }
 else
 {
  RawBeg = new char[Lng]; // space for NULL already included
	   if(!RawBeg) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
  if( read( Handle, RawBeg, Lng ) != Lng ) ERROR_WHEN_READING
 }

 // save RawEnd
 if( read( Handle, &Lng, 2u ) != 2u ) ERROR_WHEN_READING

 if( Lng == 0 )
 {
  RawEnd = NULL;
 }
 else
 {
  RawEnd = new char[Lng]; // space for NULL already included
	   if(!RawEnd) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
  if( read( Handle, RawEnd, Lng ) != Lng ) ERROR_WHEN_READING
 }

 return;
}

//----------------------------------------------------------------------------------

StaticTableItem::~StaticTableItem( void )
{
 #ifdef DEBUG
  printf("\n Destructor ~StaticTableItem deleting"
	 "\n  RawBeg : %s"
	 "\n  RawEnd : %s"
	 ,
	 RawBeg,
	 RawEnd
	 );
 #endif

 if( RawBeg ) { delete RawBeg; RawBeg=NULL; }
 if( RawEnd ) { delete RawEnd; RawEnd=NULL; }
}

//- class COCONator methods ---------------------------------------------------------------------------------

COCONator::COCONator( char *FileName )
// loads table and automat from *.BCA file
{
 Load( FileName );
}

//----------------------------------------------------------------------------------

void COCONator::Load( char *FileName )
{
 char Magic[4];

 #ifdef DEBUG
  printf("\n\n Loading coconator %s...", FileName );
 #endif

 int      Handle;
 word     i;

 if( !strstr( FileName, ".bca") )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Only BCA files accepted by Coconator ( %s filename not OK )...", __FILE__, __LINE__, FileName );
  EXIT_PROGRAM;
 }

 if((Handle = open( FileName, O_RDONLY | O_BINARY, S_IWRITE | S_IREAD)) == -1)
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Error opening file \"%s\"...", __FILE__, __LINE__, FileName );
  EXIT_PROGRAM;
 }

 // --- load magic ---

 if( read( Handle, Magic, 3u ) != 3u ) ERROR_WHEN_READING

 Magic[3]=0;

 if( !strstr( Magic, "BCA" ) )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Only BCA files accepted by Coconator - %s is not...", __FILE__, __LINE__, FileName );
  EXIT_PROGRAM;
 }

 // --- load options for translation ---

 if( read( Handle, &HTMLOutput, 1u ) != 1u ) ERROR_WHEN_READING

 if( read( Handle, &CRLF, 1u ) != 1u ) ERROR_WHEN_READING

 // --- load cocon encapsulate mucus ---

 // number of items in table
 if( read( Handle, &NrOfItems, 2u ) != 2u ) ERROR_WHEN_READING

 // alloc an array
 CoconMucus = new StaticTableItem[ NrOfItems ];
	      if(!CoconMucus) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 // load table items self
 for( i=0; i<NrOfItems; i++ )
  CoconMucus[i].Load( Handle );

 #ifdef DEBUG
  PrintCoconMucus();
 #endif

 // --- Load automat ---

 Coconizer = new StaticAutomat( Handle );
	     if(!Coconizer) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 Coconizer->Print();

 close( Handle );

}

//----------------------------------------------------------------------------------

void COCONator::PrintCoconMucus( void )
{
 #ifdef DEBUG

 word i;

  for( i=0; i<NrOfItems; i++ )
  {
   printf(
	  "\n CoconMucus[%u]:"
	  "\n  Action: %s"
	  "\n  RawBeg: %s"
	  "\n  RawEnd: %s"
	  ,
	  i,
	  CoconMucus[i].Action,
	  CoconMucus[i].RawBeg,
	  CoconMucus[i].RawEnd
	 );
  }

 #endif
}

//----------------------------------------------------------------------------------

COCONator::~COCONator( void )
{
 word i;

 for( i=0; i<NrOfItems; i++ )
  CoconMucus[i].~StaticTableItem();

 delete CoconMucus;

 delete Coconizer;

 delete Buf;

}

//----------------------------------------------------------------------------------

void COCONator::Coconize( char *InFileName, char *OutFileName )
{
 #ifdef DEBUG
  printf("\n\n - COCONIZE BEG ------------------------------------------------------");
 #endif

 // allocate buffer
 Buf=new Buffer( InFileName, OutFileName, 10000u ); // output buffer not used
     if(!Buf) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 Beg=0;
 OldBeg=0;



 // write head of page
 if( CoconMucus[0].RawBeg )
  Buf->Save( CoconMucus[0].RawBeg, strlen(CoconMucus[0].RawBeg) );




 while( GetHumanoid() )
 {
  ;

  #ifdef DEBUG
   printf("\n -------> After GetHumanoid <-------");
  #endif
 }



 // write tail of page
 if( CoconMucus[0].RawEnd )
  Buf->Save( CoconMucus[0].RawEnd, strlen(CoconMucus[0].RawEnd) );



 #ifdef DEBUG
  printf("\n - COCONIZE END ------------------------------------------------------\n");
 #endif

 return;
}

//-----------------------------------------------------------------------------

bool COCONator::GetHumanoid( void )
// - takes elements from input file
// - input is buffered
// - when the last item is get, buffer is automaticly deleted
{

 ElementID EleID;
 bool      Success;
 dword     SuccChar;
 char      C;

 #ifdef DEBUG
  printf("\n - HUMAN -");
 #endif

  OldBeg=Beg;

  Success=Coconizer->Accept( Buf, Beg, End, (LexicatorType)EleID );

  if( Success )
  {
   #ifdef DEBUG
    printf("\n HUMAN Element: Beg %lu End %lu ElementID %u\n", Beg, End, EleID );
   #endif


   // element successfuly identified -> do the ACTION with it
   switch( CoconMucus[EleID].Action )
   {
    case ACTION_NO:	// means do nothing with element

		    Buf->Copy( Beg, End ); // only copy it to output

		   break;

    case ACTION_REMOVE: // removing element -> do not copy it to output

		   break;

    case ACTION_REPLACE: // replace element with new thing

		    if( CoconMucus[EleID].RawBeg ) // raw beg is used as raw
		     Buf->Save( CoconMucus[EleID].RawBeg, strlen(CoconMucus[EleID].RawBeg) );

		   break;

    case ACTION_ENCAPSULATE:

		    // write encapsulated element to the output file ( through buffer )

		    if( CoconMucus[EleID].RawBeg )
		     Buf->Save( CoconMucus[EleID].RawBeg, strlen(CoconMucus[EleID].RawBeg) );

		    Buf->Copy( Beg, End );

		    if( CoconMucus[EleID].RawEnd )
		     Buf->Save( CoconMucus[EleID].RawEnd, strlen(CoconMucus[EleID].RawEnd) );

		   break;
   }


   OldBeg=Beg=End+1;	// move in input

   return TRUE;

  }
  else
  {
   Beg=OldBeg;
   Buf->Seek( Beg );

   SuccChar=Buf->GetChar( C );

   if( !SuccChar )
   {
    #ifdef DEBUG
     printf("\n - HUMAN cleaned, no input -");
    #endif
    return FALSE; // end of input
   }

   // if( IsWhitespace() )
   if( C==' '||C=='\t'||C=='\n' ) // step cross white space which was no accepted
   {
    #ifdef DEBUG
     printf("\n HUMAN WhiteSpace: \'%d\'\n",C);
    #endif

    // write whitespace to file

    Buf->PutChar( C );

    Beg+=SuccChar;	// was returned nr of bytes moved across:
			// if 10 (\n) inside getchar I jump cross 13 after

    // dive because this is whitespace
    return GetHumanoid();

   }
   else
   {
    #ifdef DEBUG
     printf("\n - HUMAN cleaned, not accepted whitespace %c -", C );
    #endif
    return FALSE; // end of input
   }
  } // if success
}

//- EOF ----------------------------------------------------------------------------
