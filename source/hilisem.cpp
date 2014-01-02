#include "source\hilisem.h"

// print info when hunting bugs
// #define DEBUG

#define EXIT_PROGRAM exit(0);

extern bool Verbose;
extern bool HTMLOutput;
extern bool CRLF;

//---------------------------------------------------------------------------------------

void QuotesAway( char far *String )
// delete quotes and \X transforms to X inside string
{
 word Dest,
      Src,
      Lng = strlen( String );

 if( String == NULL ) return;

 for( Src=Dest=0; Src<(Lng-2); Src++, Dest++ )
 {
  String[Dest]=String[Src+1];

  if( String[Dest] == '\\')
  {
   String[Dest]=String[Src+2];
   Src++;
  }

 }

 String[Dest] = 0;
}

//---------------------------------------------------------------------------------------

#define ERROR_WHEN_WRITING \
 {                         \
  printf("\n ERROR reported from %s line %d:"                                     \
	 "\n  Error writing to the file...", __FILE__, __LINE__ );\
  EXIT_PROGRAM                                                          \
 }

//- class DynaCfgFile methods ---------------------------------------------------------

DynaCfgFile::DynaCfgFile( void )
{
 Records = new List; if(!Records) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
}

//---------------------------------------------------------------------------------------

void DynaCfgFile::PrintTokens( void )
{
 #ifdef DEBUG

  ListItem far *Item=Records->GetHead();

  while( Item )
  {
   printf("\n\n * FileRow ***********************************************"
          "\n Head %Fp Tail %Fp Item %Fp Item->Next %Fp ",
          Records->Head,
          Records->Tail,
          Item,
          Item->Next
         );

   ((DynaCfgRecordList far *)Item)->Print();

   Item=Records->Next( Item );
  }

 #endif
}

//---------------------------------------------------------------------------------------

DynaCfgFile::~DynaCfgFile()
{
 #ifdef DEBUG
  printf("\n Destructor of ~DynaCfgFile, %u records", Records->NrOfItems );
 #endif

 delete Records;
}

//- class DynaCfgRecordList methods ---------------------------------------------------------

DynaCfgRecordList::DynaCfgRecordList( void )
{
 Tokens = new List; if(!Tokens) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
}

//---------------------------------------------------------------------------------------

void DynaCfgRecordList::Print( void )
{
 #ifdef DEBUG

  ListItem far *Item=Tokens->GetHead();

  while( Item )
  {
   printf("\n - Token ---------------------------------------------------");

   ((DynaCfgToken far *)Item)->Print();

   Item=Tokens->Next( Item );
  }

 #endif
}
//---------------------------------------------------------------------------------------

DynaCfgRecordList::~DynaCfgRecordList()
{
 #ifdef DEBUG
  printf("\n Virtual destructor of ~DynaCfgRecordList, %u Token lists", Tokens->NrOfItems );
 #endif

 delete Tokens;
}

//- struct DynaCfgToken methods --------------------------------------------------------------------------------------

void DynaCfgToken::Print( void )
{
 #ifdef DEBUG

  printf(
         "\n DynaCfgToken:"
         "\n  Idf   : %u"
         "\n  String: %s"
         ,
         Idf,
         String
        );

 #endif
}

//---------------------------------------------------------------------------------------

DynaCfgToken::~DynaCfgToken( void )
{
 #ifdef DEBUG
  printf("\n Virtual destructor ~DynaCfgToken deleting %s", String );
 #endif

 if( String ) delete String;
}

//- class DynaCfgRecord methods ------------------------------------------------------------

void DynaCfgRecord::Print( void )
{
 #ifdef DEBUG

  printf("\n DynaCfgRecord, record type: %u"
	 ,
         Type
        );

  switch( Type )
  {
   case RECORD_SETUP_TRANSLATION:
	 printf("\n  Output     : %u"
		"\n  CRLF       : %u"
		"\n  HtmlOutput : %u"
		,
		Data.Setup.OutType,
		Data.Setup.CRLF,
		Data.Setup.HTMLOutput
	       );

        break;
   case RECORD_SETUP_PAGE:

         printf("\n  Title     : %s"
                "\n  FaceFont  : %s"
                "\n  BaseColor : %s"
                "\n  BaseSize  : %s"
                "\n  BgColor   : %s"
                "\n  Background: %s"
		"\n  Width     : %s"
		,
		Data.Setup.Title,
		Data.Setup.FaceFont,
		Data.Setup.BaseColor,
		Data.Setup.BaseSize,
		Data.Setup.BgColor,
		Data.Setup.Background,
		Data.Setup.Width
	       );
        break;
   case RECORD_NORMAL:
         printf("\n  Underscore: %d"
                "\n  Italic    : %d"
		"\n  Bold      : %d"
                "\n  Descr     : %s"
                "\n  Font      : %s"
                "\n  Size      : %s"
		"\n  Color     : %s"
                ,
                Data.Normal.Underscore,
                Data.Normal.Italic,
                Data.Normal.Bold,
                Data.Normal.Descr,
                Data.Normal.Font,
                Data.Normal.Size,
                Data.Normal.Color
               );
        break;
   case RECORD_RAW:
         printf(
                "\n  Descr : %s"
                "\n  RawBeg: %s"
                "\n  RawEnd: %s"
                ,
                Data.Raw.Descr,
                Data.Raw.RawBeg,
                Data.Raw.RawEnd
               );
        break;
   case RECORD_REPLACE:
	 printf(
		"\n  Descr : %s"
		"\n  Raw   : %s"
		,
		Data.Replace.Descr,
		Data.Replace.Raw
	       );
	break;
   case RECORD_REMOVE:
	 printf(
		"\n  Descr : %s"
		,
		Data.Remove.Descr
	       );
        break;
   default:
        printf("\n  Unknown record");

  }

 #endif
}

//---------------------------------------------------------------------------------------

DynaCfgRecord::~DynaCfgRecord( void )
{
 #ifdef DEBUG
  printf("\n Virtual destructor ~DynaCfgRecord, record type %u", Type );
 #endif

  switch( Type )
  {
   case RECORD_SETUP_TRANSLATION:

         #ifdef DEBUG
	 printf("\n  Output    : %u"
		"\n  CRLF      : %u"
		"\n  HTMLOutput: %u"
		,
		Data.Setup.OutType,
		Data.Setup.CRLF,
		Data.Setup.HTMLOutput
	       );
         #endif

        break;
   case RECORD_SETUP_PAGE:

         #ifdef DEBUG
         printf("\n  Title     : %s"
                "\n  FaceFont  : %s"
                "\n  BaseColor : %s"
                "\n  BaseSize  : %s"
                "\n  BgColor   : %s"
                "\n  Background: %s"
		"\n  Width     : %s"
		,
		Data.Setup.Title,
		Data.Setup.FaceFont,
		Data.Setup.BaseColor,
		Data.Setup.BaseSize,
		Data.Setup.BgColor,
		Data.Setup.Background,
		Data.Setup.Width
	       );
	 #endif

	 if( Data.Setup.Title )         delete Data.Setup.Title;
	 if( Data.Setup.FaceFont )      delete Data.Setup.FaceFont;
	 if( Data.Setup.BaseColor )     delete Data.Setup.BaseColor;
	 if( Data.Setup.BaseSize )      delete Data.Setup.BaseSize;
	 if( Data.Setup.BgColor )       delete Data.Setup.BgColor;
	 if( Data.Setup.Background )    delete Data.Setup.Background;
	 if( Data.Setup.Width )    	delete Data.Setup.Width;

        break;

   case RECORD_NORMAL:

         #ifdef DEBUG
         printf("\n  Underscore: %d"
                "\n  Italic    : %d"
                "\n  Bold      : %d"
                "\n  Descr     : %s"
                "\n  Font      : %s"
		"\n  Size      : %s"
                "\n  Color     : %s"
                ,
                Data.Normal.Underscore,
                Data.Normal.Italic,
                Data.Normal.Bold,
                Data.Normal.Descr,
                Data.Normal.Font,
                Data.Normal.Size,
                Data.Normal.Color
               );
         #endif

         if( Data.Normal.Descr )        delete Data.Normal.Descr;
         if( Data.Normal.Font )         delete Data.Normal.Font;
         if( Data.Normal.Size )         delete Data.Normal.Size;
         if( Data.Normal.Color )        delete Data.Normal.Color;

        break;
   case RECORD_RAW:

         #ifdef DEBUG
         printf(
                "\n  Descr : %s"
                "\n  RawBeg: %s"
                "\n  RawEnd: %s"
                ,
                Data.Raw.Descr,
                Data.Raw.RawBeg,
                Data.Raw.RawEnd
               );
         #endif

         if( Data.Raw.Descr )   delete Data.Raw.Descr;
         if( Data.Raw.RawBeg )  delete Data.Raw.RawBeg;
         if( Data.Raw.RawEnd )  delete Data.Raw.RawEnd;

        break;
   case RECORD_REPLACE:

         #ifdef DEBUG
         printf(
                "\n  Descr : %s"
		"\n  Raw   : %s"
		,
		Data.Replace.Descr,
		Data.Replace.Raw
	       );
         #endif

	 if( Data.Replace.Descr )   delete Data.Replace.Descr;
	 if( Data.Replace.Raw )     delete Data.Replace.Raw;

        break;
   case RECORD_REMOVE:

         #ifdef DEBUG
         printf(
                "\n  Descr : %s"
		,
		Data.Remove.Descr
	       );
	 #endif

	 if( Data.Remove.Descr )   delete Data.Remove.Descr;

        break;
   case RECORD_NO_ACTION:

         #ifdef DEBUG
         printf(
                "\n  Descr : %s"
		,
		Data.No.Descr
	       );
	 #endif

	 if( Data.No.Descr )   delete Data.No.Descr;

        break;

   default:
	printf("\n ERROR: Unknown record in virtual destructor of ~DynaCfgRecord...");
	exit(0);
  }


 return;
}

//- class DynaBinaryCfgTableItem methods ------------------------------------------------------------

void DynaBinaryCfgTableItem::Print( void )
{
 #ifdef DEBUG

  printf("\n DynaTableItem ID %u:"
	 "\n  Action: %d"
	 "\n  RawBeg: %s"
	 "\n  RawEnd: %s"
	 ,
	 ID,
	 Action,
	 RawBeg,
         RawEnd
        );

 #endif
}

//---------------------------------------------------------------------------------------

void DynaBinaryCfgTableItem::Save( int Handle )
{
 word Lng;

 // DO NOT save IDs - its index

 // save Action
 if( write( Handle, &Action, 2u ) != 2u ) ERROR_WHEN_WRITING

 // save RawBeg
 if( RawBeg==NULL )
 {
  Lng = 0;

  if( write( Handle, &Lng, 2u ) != 2u ) ERROR_WHEN_WRITING
 }
 else
 {
  Lng = strlen(RawBeg)+1u;

  if( write( Handle, &Lng, 2u ) != 2u ) ERROR_WHEN_WRITING
  if( write( Handle, RawBeg, Lng ) != Lng ) ERROR_WHEN_WRITING
 }

 // save RawEnd
 if( RawEnd==NULL )
 {
  Lng = 0;

  if( write( Handle, &Lng, 2u ) != 2u ) ERROR_WHEN_WRITING
 }
 else
 {
  Lng = strlen(RawEnd)+1u;

  if( write( Handle, &Lng, 2u ) != 2u ) ERROR_WHEN_WRITING
  if( write( Handle, RawEnd, Lng ) != Lng ) ERROR_WHEN_WRITING
 }

 return;
}

//---------------------------------------------------------------------------------------

DynaBinaryCfgTableItem::~DynaBinaryCfgTableItem( void )
{
 #ifdef DEBUG
  printf("\n Virtual destructor ~DynaBinaryCfgTableItem deleting"
	 "\n  Action: %d"
	 "\n  RawBeg: %s"
	 "\n  RawEnd: %s"
         ,
	 Action,
	 RawBeg,
         RawEnd
         );
 #endif

 if( RawBeg ) delete RawBeg;
 if( RawEnd ) delete RawEnd;
}

//- class Semantator methods ------------------------------------------------------------

Semantator::Semantator( SyntactNode far *SyntactRoot )
{
 Root        = SyntactRoot;

 Table       = new List; if(!Table) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 Auto        = new DynamicAutomat; if(!Auto) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
}

//---------------------------------------------------------------------------------------

Semantator::~Semantator( void )
{
 delete Table;

 delete Auto;
}

//---------------------------------------------------------------------------------------

bool Semantator::DiveForR3s( SyntactNode far *Node )
// returns true if subtree was deleted and false if not
{
 // go through tree and search for R3 nodes ( root of subtree of one record )

 switch( Node->Type )
 {
  case SNODE_R2:

         // if node was deleted set it to NULL,
         // destructor will not dive through it
         if( DiveForR3s( Node->Data.NeterminalR2.Rl ))
          Node->Data.NeterminalR2.Rl=NULL;

         if( DiveForR3s( Node->Data.NeterminalR2.Rr ))
          Node->Data.NeterminalR2.Rr=NULL;

        return FALSE;

  case SNODE_R3:

         // yes I am here => transform record which is under

	 WorkOnR3Subtree( Node );

         // subtree no longer needed -> delete it

         delete Node;

        return TRUE;

  case SNODE_T4:
  case SNODE_T5:
  case SNODE_K:
  case SNODE_S:
  case SNODE_k:
  case SNODE_s:
  case SNODE_LBRACE:
  case SNODE_RBRACE:
  case SNODE_SEMICOLON:
  case SNODE_EQU:
  case SNODE_NULL:
  case SNODE_STAR:
  case SNODE_R:
  case SNODE_T:

	 printf("\n ERROR: Misplaced node %d in DiveForR3s!", Node->Type );
	 exit(0);

  default:

	 printf("\n ERROR: Unknown node %d probably error in DiveForR3s!", Node->Type );
         exit(0);
 }

 return FALSE;
}

//---------------------------------------------------------------------------------------

void Semantator::WorkOnR3Subtree( SyntactNode far *Node )
// - make ONE RECORD ( convert it from syntact subtree )
{
 // insert new record to file list
 DynaCfgRecordList far *RecordList = new DynaCfgRecordList; if(!RecordList) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
  File->Insert( RecordList );



 // insert setup/element token - is here!

 DynaCfgToken far *Token = new DynaCfgToken; if(!Token) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // validity of keyword was checked in lexicator
 // => here only fill
 // enum IDs are same, see hitypes.h

 Token->Idf    = Node
		  ->Data.NeterminalT4.K        // T4
		  ->Data.NeterminalK.k         // I
		  ->Data.k.Key;                // i

 Token->String = Node
		  ->Data.NeterminalT4.S        // T4
		  ->Data.NeterminalS.s         // S
		  ->Data.s.Element;            // s

 // token OK -> insert it to record list
 RecordList->Insert( Token );



 // rest of tokens is under T4 node -> so dive and than insert tokens
 DiveForT4s( Node->Data.NeterminalR3.T, RecordList );

 return;
}

//---------------------------------------------------------------------------------------

void Semantator::DiveForT4s( SyntactNode        far *Node,
                             DynaCfgRecordList  far *RecordList
                           )
{
 DynaCfgToken far *Token;

 // go through tree and search for T4 nodes ( node over one TOKEN )

 switch( Node->Type )
 {
  case SNODE_T4:
         // yes I am here => transform token under
	 Token = new DynaCfgToken; if(!Token) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

	 // validity of keyword was checked in lexicator
	 // => here only fill
	 // enum IDs are same, see hitypes.h

	 Token->Idf    = Node
			  ->Data.NeterminalT4.K        // T4
			  ->Data.NeterminalK.k         // I
			  ->Data.k.Key;                // i

	 Token->String = Node
			  ->Data.NeterminalT4.S        // T4
			  ->Data.NeterminalS.s         // S
			  ->Data.s.Element;            // s

         // token OK -> insert it to record list
         RecordList->Insert( Token );

        return;
  case SNODE_T5:
         DiveForT4s( Node->Data.NeterminalT5.Tl, RecordList );
         DiveForT4s( Node->Data.NeterminalT5.Tr, RecordList );
        return;
  case SNODE_R2:
  case SNODE_R3:
  case SNODE_S:
  case SNODE_k:
  case SNODE_s:
  case SNODE_LBRACE:
  case SNODE_RBRACE:
  case SNODE_SEMICOLON:
  case SNODE_EQU:
  case SNODE_NULL:
  case SNODE_STAR:
  case SNODE_R:
  case SNODE_T:

	 printf("\n ERROR: Misplaced node %d in DiveForT4s!", Node->Type );
	 exit(0);

  default:

	 printf("\n ERROR: Unknown node %d probably error in DiveForT4s!", Node->Type );
         exit(0);
 }
}

//---------------------------------------------------------------------------------------

void Semantator::DynaCfgRecordListToDynaCfgRecord
                  (
                   DynaCfgRecordList    far *CfgRecordList,
                   DynaCfgRecord        far *&Record
                  )
// converts list to single record, allocates this record and returns
// pointer on it
{

 List far *RecordList = ((DynaCfgRecordList far *)CfgRecordList)->Tokens;

 bool Change;   // true if some change when sorting
 word i,
      L,
      Lng,
      NrOfDescrs;
 char     far *StringDescrs;
 ListItem far *Item,
          far *Item1,
          far *Item2,
          far *Item3,
          far *Item4,

          far *DescrItem
          ;

 // sort record list and check what's in
 // sort order == enum order by numbers:

 // using enum ID to sort list

 Change=TRUE;

 while( Change )
 {
  if( RecordList->NrOfItems == 1 ) break;       // nothing to sort

  Item=RecordList->GetHead();

  Change=FALSE;

  for( i=1; i<=(RecordList->NrOfItems-1); i++ ) // go through list once
  {

   if(
       ((DynaCfgToken far *)Item)->Idf
       >
       ((DynaCfgToken far *)Item->Next)->Idf
     )
   {
    #ifdef DEBUG
     //printf("\n\n %u <-> %u", ((DynaCfgToken far *)Item)->Idf, ((DynaCfgToken far *)(Item->Next))->Idf );
    #endif

    // change order in list:  1234 -> 1324 ( Item points to 2 )
    Item1 = Item->Last;
    Item2 = Item;
    Item3 = Item->Next;
    Item4 = Item->Next->Next;

    // 1234 -> 1324
    Item1->Next = Item3;
    Item2->Next = Item4;
    Item3->Next = Item2;

    Item2->Last = Item3;
    Item3->Last = Item1;
    Item4->Last = Item2;

    Change=TRUE;

    // item must stay correct -> next is done when switching items!
    // item2==item is moved to tail

    continue;
   }

   Item=RecordList->Next(Item);
  } // for
 }

 #ifdef DEBUG
  //printf("\n\n In DynaCfgRecordListToDynaCfgRecord after sorting XXXXXXXXXXXXXXX");
  //CfgRecordList->PrintTokens();
 #endif

 // now items are sorted, check if tokens are OK

 if( RecordList->Empty() )
 {
  printf("\n ERROR reported from %s line %d:"
         "\n  Record list is empty - strange -> delete or fill with something...", __FILE__,__LINE__
        );
  exit(0);
 }

 // CHECK: the first must be element or setup

 Item = RecordList->GetHead();

 switch( ((DynaCfgToken far *)Item)->Idf )
 {
  case TOKEN_ELEMENT:
         // CHECK: there must be descr(s)
         //        if there is more then one concatenate them

	 // it is name of element block, Item1 is backup for error
	 Item1=Item;

	 Item=RecordList->Next( Item );

         // now item should point to descr
	 if( Item==NULL
	      ||
	     ((DynaCfgToken far *)Item)->Idf != TOKEN_DESCR
	   )
	 {
	  printf("\n ERROR reported from %s line %d:"
		 "\n  Missing descr in %s element block...", __FILE__,__LINE__, ((DynaCfgToken far *)Item1)->String );
	  exit(0);
	 }

	 // concatenate existing descrs
         // first count total lng, then allocate and in the end copy
         // descrs to new location, delete old descrs and insert new
         // ( concatened descr put to the first node, others delete )
         // eliminate " ( belongs to lex element not to descr )

         DescrItem=Item;

         // count lng
         Lng=0; NrOfDescrs=0;

         while( Item && ((DynaCfgToken far *)Item)->Idf == TOKEN_DESCR )
         {
          NrOfDescrs++;

          // put away " on sides  ( belongs to string not to descr )

          L = strlen( ((DynaCfgToken far *)Item)->String ); // 0 not counted

          for( i=0; i<L; i++ )
           ((DynaCfgToken far *)Item)->String[i]
           =
           ((DynaCfgToken far *)Item)->String[i+1];

          ((DynaCfgToken far *)Item)->String[L-2]=0; // kill " at tail

          L -= 2;

          if( L == 0 )
          {
           printf("\n ERROR reported from %s line %d:"
                  "\n  Descriptor is empty string in HILEX cfg file...", __FILE__,__LINE__);
           exit(0);
          }

          // done

	  Lng+=L;

          Item=RecordList->Next( Item );
         }

         if( NrOfDescrs>1 )
         {
          // concatenate

          Item=DescrItem;

          StringDescrs          = new char[Lng+1];      // add place for 0
				  if(!StringDescrs) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	  StringDescrs[0]       = 0;

          NrOfDescrs=0;

          while( Item && ((DynaCfgToken far *)Item)->Idf == TOKEN_DESCR )
          {
           NrOfDescrs++;

           strcat( StringDescrs, ((DynaCfgToken far *)Item)->String );

           Item3 = Item;        // backup item ( will be deleted )
           Item  = RecordList->Next( Item );

           // delete result of descr nodes ( one node is enough )
           if( NrOfDescrs>1 ) RecordList->Delete( Item3 );

          }

          // put concatenated descr to the first node
          delete ((DynaCfgToken far *)DescrItem)->String;
          ((DynaCfgToken far *)DescrItem)->String = StringDescrs;
         }

	 // Item points to the element after descr node
	 // now here must be action
	 if( ((DynaCfgToken far *)Item)->Idf != TOKEN_ACTION )
	 {
	  printf("\n ERROR reported from %s line %d:"
		 "\n  Missing ACTION in HILEX cfg file...", __FILE__,__LINE__);
	  exit(0);
	 }

	 // ACTION -> 3 cases:
	 //  Encapsulate ... Raw_Beg&RawEnd || Null || Color...
	 //  Remove ... there must be NULL
	 //  Replace ... there must be Raw

	 if( !stricmp( "\"remove\"", ((DynaCfgToken far *)Item)->String )
	      ||
	     !stricmp( "\"no\"", ((DynaCfgToken far *)Item)->String )
	 )
	 {
	  // now there must be NULL
	  Item = RecordList->Next( Item );

	  if( Item!=NULL && Verbose)
	  {
	   printf("\n WARNING reported from %s line %d:"
		  "\n  No settings allowed when action REMOVE or NO => ignoring options...\n", __FILE__,__LINE__);
	  }
	 }
	 else
	 {
	  if( !stricmp( "\"replace\"", ((DynaCfgToken far *)Item)->String ) )
	  {
	   // now there must be Raw
	   Item = RecordList->Next( Item );

	   if( Item!=NULL && ((DynaCfgToken far *)Item)->Idf != TOKEN_RAW )
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  When action REPLACE is selected, there must be Raw section in HILEX file...", __FILE__,__LINE__);
	    exit(0);
	   }
	  }
	  else
	  {
	   if( !stricmp( "\"encapsulate\"", ((DynaCfgToken far *)Item)->String ) )
	   {
	    // CHECK: rawbeg&&rawend
	    //         ||
	    //        font, size, color, ...
	    // together -> error

	    Item = RecordList->Next( Item );

	    // if nothing default is used
	    if( Item==NULL && Verbose)
	    {
	     printf("\n WARNING reported from %s line %d:"
		    "\n  No settings when action ENCAPSULATE => using default...\n", __FILE__,__LINE__);
	    }


	    if( ((DynaCfgToken far *)Item)->Idf == TOKEN_RAWEND )
	    {
	     printf("\n ERROR reported from %s line %d:"
		    "\n  Missing RAW_BEG in HILEX cfg file...", __FILE__,__LINE__);
	     exit(0);
	    }

	    if( ((DynaCfgToken far *)Item)->Idf == TOKEN_RAWBEG )
	    {
	     // now there must be rawend
	     Item = RecordList->Next( Item );

	     if( Item==NULL
		  ||
		 ((DynaCfgToken far *)Item)->Idf != TOKEN_RAWEND
	       )
	     {
	      printf("\n ERROR reported from %s line %d:"
		     "\n  Missing RAW_END in HILEX cfg file...", __FILE__,__LINE__);
	      exit(0);
	     }
	    }
	   }
	   else
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  Unknown action %s in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
	    exit(0);
	   }
	  }
	 }

	 // the rest should be OK

	break;

  case TOKEN_SETUP:
	 // if string == translation there must be output/CRLF/HtmlOutput
	 //			or
	 //		 empty
	 //
	 // if string == page can be empty or with biggest enum type
         // another string is error

         Item = RecordList->GetHead();          // now item points to setup

         if( Item==NULL ) // empty - OK no options -> default will be used
          break;

         // setuping translation or page
         if( !stricmp( "\"translation\"", ((DynaCfgToken far *)Item)->String ) )
         {
          // now here must be output or empty
          Item = RecordList->Next( Item );

	  if( Item==NULL ) // empty - OK no options -> default will be used
           break;

          if( ((DynaCfgToken far *)Item)->Idf != TOKEN_OUTPUT )
          {
           printf("\n ERROR reported from %s line %d:"
                  "\n  In SETUP TRANSLATION can be only OUTPUT keyword used,"
                  "\n  nothing else in HILEX cfg file...", __FILE__,__LINE__);
           exit( 0 );
          }

          if(
	     !
	     (
	      !stricmp( "\"html_page\"",  ((DynaCfgToken far *)Item)->String )
	       ||
              !stricmp( "\"html_table\"", ((DynaCfgToken far *)Item)->String )
               ||
              !stricmp( "\"no_encaps\"",  ((DynaCfgToken far *)Item)->String )
             )
	    )
	  {
	   printf("\n ERROR reported from %s line %d:"
		  "\n  Option %s not allowed in OUTPUT section in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
	   exit( 0 );
	  }

	  break;
	 }

	 // page is valid keyword of setup, rest is OK ( there is not output )
	 if( !stricmp( "\"encapsulation\"", ((DynaCfgToken far *)Item)->String ) )
	  break;


         printf("\n ERROR reported from %s line %d:"
                "\n  Unknown SETUP option %s in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
         exit(0);

  default:
        printf("\n ERROR reported from %s line %d:"
               "\n  Element or setup identifier missing in HILEX cfg file...", __FILE__,__LINE__);
        exit(0);
 }



 // List is now checked and ok => transform it to record
 // => fill DynaCfgRecord

 Item = RecordList->GetHead();

 switch( ((DynaCfgToken far *)Item)->Idf )
 {
  case TOKEN_ELEMENT:

	 // time to delete block name ( is ignored)
	 delete ((DynaCfgToken far *)Item)->String;
	 ((DynaCfgToken far *)Item1)->String = NULL;

	 Item=RecordList->Next( Item );

	 // now pointing to descr

	 DescrItem=Item;

	 Item=RecordList->Next( Item );

	 // now pointing to action: ENCAPSULATE/REMOVE/REPLACE/NO
	 if( !stricmp( "\"no\"", ((DynaCfgToken far *)Item)->String ) )
	 {
	  // no options needed, only save fill type of record
	  // -> says which action schould be done
	  Record->Type                   = RECORD_NO_ACTION;
	  Record->Data.No.Descr = ((DynaCfgToken far *)DescrItem)->String;
	  ((DynaCfgToken far *)DescrItem)->String = NULL;

	  break;
	 }



	 if( !stricmp( "\"remove\"", ((DynaCfgToken far *)Item)->String ) )
	 {
	  // no options needed, only save fill type of record
	  // -> says which action schould be done
	  Record->Type                   = RECORD_REMOVE;
	  Record->Data.Remove.Descr = ((DynaCfgToken far *)DescrItem)->String;
	  ((DynaCfgToken far *)DescrItem)->String = NULL;

	  break;
	 }



	 if( !stricmp( "\"replace\"", ((DynaCfgToken far *)Item)->String ) )
	 {
	  Record->Type                   = RECORD_REPLACE;
	  Record->Data.Remove.Descr = ((DynaCfgToken far *)DescrItem)->String;
	  ((DynaCfgToken far *)DescrItem)->String = NULL;

	  // move to option raw ( is there - was checked at the top )
	  Item=RecordList->Next( Item );

	  Record->Data.Replace.Raw = ((DynaCfgToken far *)Item)->String;
	  ((DynaCfgToken far *)Item)->String=NULL;
	  QuotesAway( Record->Data.Replace.Raw );

	  break;
	 }



	 if( !stricmp( "\"encapsulate\"", ((DynaCfgToken far *)Item)->String ) )
	 {
	  // move after action
	  Item=RecordList->Next( Item );

	  if( Item==NULL )
	  {
	   // item is NULL -> user wants not encapsulate this element
	   // -> init it, do not forget to init descr
	   Record->Type                   = RECORD_NORMAL;
	   Record->Data.Normal.Font       = NULL;
	   Record->Data.Normal.Size       = NULL;
	   Record->Data.Normal.Color      = NULL;
	   Record->Data.Normal.Underscore = FALSE;
	   Record->Data.Normal.Italic     = FALSE;
	   Record->Data.Normal.Bold       = FALSE;
	   Record->Data.Normal.Descr = ((DynaCfgToken far *)DescrItem)->String;
	   ((DynaCfgToken far *)DescrItem)->String = NULL;

	   break;
	  }

	  if( ((DynaCfgToken far *)Item)->Idf == TOKEN_RAWBEG )
	  {
	   Record->Type                  = RECORD_RAW;
	   Record->Data.Raw.RawBeg       = NULL;
	   Record->Data.Raw.RawEnd       = NULL;

	   Record->Data.Raw.RawBeg = ((DynaCfgToken far *)Item)->String;
	   ((DynaCfgToken far *)Item)->String=NULL;
	   QuotesAway( Record->Data.Raw.RawBeg );

	   Item=RecordList->Next( Item );

	   Record->Data.Raw.RawEnd = ((DynaCfgToken far *)Item)->String;
	   ((DynaCfgToken far *)Item)->String=NULL;
	   QuotesAway( Record->Data.Raw.RawEnd );

	   // set descr
	   Record->Data.Raw.Descr = ((DynaCfgToken far *)DescrItem)->String;

	   ((DynaCfgToken far *)DescrItem)->String=NULL;

	   // the rest of string ignore
	   break;
	  }

	  // it's normal, init it
	  Record->Type                   = RECORD_NORMAL;
	  Record->Data.Normal.Font       = NULL;
	  Record->Data.Normal.Size       = NULL;
	  Record->Data.Normal.Color      = NULL;
	  Record->Data.Normal.Underscore = FALSE;
	  Record->Data.Normal.Italic     = FALSE;
	  Record->Data.Normal.Bold       = FALSE;

	  // set descr
	  Record->Data.Normal.Descr = ((DynaCfgToken far *)DescrItem)->String;

	  ((DynaCfgToken far *)DescrItem)->String = NULL;

	  #ifdef DEBUG
	   //printf("\n\n In DynaCfgRecordListToDynaCfgRecord after descr:");
	   //CfgRecordList->PrintTokens();
	  #endif

	  // -> work on the rest ( now pointing behind action )

	  while( Item )
	  {

	   switch( ((DynaCfgToken far *)Item)->Idf )
	   {
	    case TOKEN_FONT:

		 Record->Data.Normal.Font = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		 break;
	    case TOKEN_SIZE:

		 Record->Data.Normal.Size = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		 break;
	    case TOKEN_COLOR:

		 Record->Data.Normal.Color = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		 break;
	    case TOKEN_UNDERSCORE:

		 if( !stricmp( "\"true\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Normal.Underscore=TRUE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;
		  break;
		 }

		 if( !stricmp( "\"false\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Normal.Underscore=FALSE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;
		  break;
		 }

		 // neither true nor false -> error
		 printf("\n ERROR reported from %s line %d:"
		       "\n  Underscore option %s is not TRUE/FALSE in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
		 exit(0);

	    case TOKEN_ITALIC:

		 if( !stricmp( "\"true\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Normal.Italic=TRUE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;
		  break;
		 }

		 if( !stricmp( "\"false\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Normal.Italic=FALSE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;
		  break;
		 }

		 // neither true nor false -> error
		 printf("\n ERROR reported from %s line %d:"
		       "\n  Italic option %s is not TRUE/FALSE in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
		 exit(0);

	    case TOKEN_BOLD:

		 if( !stricmp( "\"true\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Normal.Bold=TRUE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;
		  break;
		 }

		 if( !stricmp( "\"false\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Normal.Bold=FALSE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;
		  break;
		 }

		 // neither true nor false -> error
		 printf("\n ERROR reported from %s line %d:"
		       "\n  Bold option %s is not TRUE/FALSE in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
		 exit(0);


	    default:
		// unknown node -> ignore it -> stop work, but not error
		Item=NULL;

		if( Verbose )
		{
		 printf("\n WARNING reported from %s line %d:"
		       "\n  Unknown option ID %d containing %s in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->Idf, ((DynaCfgToken far *)Item)->String );
		}

		continue;       // exit while
	   }

	   Item=RecordList->Next( Item );
	  } // while item
	 } // if( Encapsulate )

	break;

  case TOKEN_SETUP:

	 Item = RecordList->GetHead();          // setup

	 if( Item==NULL ) // empty - OK no options -> default will be used
	  break;

	 // now item should point behind Setup
	 if( !stricmp( "\"translation\"", ((DynaCfgToken far *)Item)->String ) )
	 {
	  Record->Type 			= RECORD_SETUP_TRANSLATION;
	  // now set default, if is in cfg file will be changed...
	  // globals are already set
	  Record->Data.Setup.OutType 	= HTML_PAGE;
	  Record->Data.Setup.CRLF       = FALSE;
	  Record->Data.Setup.HTMLOutput = TRUE;


	  Item = RecordList->Next( Item );

	  if( Item==NULL ) // empty - OK no options -> default used ( already set )
	  {
	   break;
	  }
	  else //
	  {

	   while( Item )
	   {

	    switch( ((DynaCfgToken far *)Item)->Idf )
	    {
	     case TOKEN_OUTPUT:

		if( !stricmp( "\"html_page\"",  ((DynaCfgToken far *)Item)->String ) )
		 Record->Data.Setup.OutType = HTML_PAGE;

		if( !stricmp( "\"html_table\"",  ((DynaCfgToken far *)Item)->String ) )
		 Record->Data.Setup.OutType = HTML_TABLE;

		if( !stricmp( "\"no_encaps\"",  ((DynaCfgToken far *)Item)->String ) )
		 Record->Data.Setup.OutType = NO_ENCAPS;

		// delete string
		delete ((DynaCfgToken far *)Item)->String;
		((DynaCfgToken far *)Item)->String = NULL;

		break;

	     case TOKEN_CRLF:

		 if( !stricmp( "\"true\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Setup.CRLF=TRUE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;

		  // set global - will be used instead of record things
		  CRLF = Record->Data.Setup.CRLF;

		  break;
		 }

		 if( !stricmp( "\"false\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Setup.CRLF=FALSE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;

		  // set global - will be used instead of record things
		  CRLF = Record->Data.Setup.CRLF;

		  break;
		 }

		// neither true nor false -> error
		printf("\n ERROR reported from %s line %d:"
		       "\n  CRLF option %s is not TRUE/FALSE in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
		exit(0);

	     case TOKEN_HTMLOUTPUT:

		 if( !stricmp( "\"true\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Setup.HTMLOutput=TRUE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;

		  // set global - will be used instead of record things
		  HTMLOutput = Record->Data.Setup.HTMLOutput;

		  break;
		 }

		 if( !stricmp( "\"false\"", ((DynaCfgToken far *)Item)->String) )
		 {
		  Record->Data.Setup.HTMLOutput=FALSE;
		  delete ((DynaCfgToken far *)Item)->String;
		  ((DynaCfgToken far *)Item)->String=NULL;

		  // set global - will be used instead of record things
		  HTMLOutput = Record->Data.Setup.HTMLOutput;

		  break;
		 }

		// neither true nor false -> error
		printf("\n ERROR reported from %s line %d:"
		       "\n  HtmlOutput option %s is not TRUE/FALSE in HILEX cfg file...", __FILE__,__LINE__, ((DynaCfgToken far *)Item)->String );
		exit(0);

	     default:
		// unknown node -> ignore it -> stop work, but not error
		Item=NULL;
		continue;       // exit while
	    }

	    Item=RecordList->Next( Item );
	   } // while item
	  } // else
	  break;
	 }
	 else // setup "page" is valid keyword of setup ( already checked )
	 {
	  Record->Type                  = RECORD_SETUP_PAGE;
	  Record->Data.Setup.Title      = NULL;
	  Record->Data.Setup.FaceFont   = NULL;
	  Record->Data.Setup.BaseColor  = NULL;
	  Record->Data.Setup.BaseSize   = NULL;
	  Record->Data.Setup.BgColor    = NULL;
	  Record->Data.Setup.Background = NULL;
	  Record->Data.Setup.Width 	= NULL;

          // now go through and fill it
          Item = RecordList->Next( Item );

          while( Item )
          {

	   switch( ((DynaCfgToken far *)Item)->Idf )
	   {
	    case TOKEN_TITLE:

		 Record->Data.Setup.Title = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    case TOKEN_FACEFONT:

		 Record->Data.Setup.FaceFont = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    case TOKEN_BASECOLOR:

		 Record->Data.Setup.BaseColor = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    case TOKEN_BASESIZE:

		 Record->Data.Setup.BaseSize = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    case TOKEN_BGCOLOR:

		 Record->Data.Setup.BgColor = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    case TOKEN_BACKGROUND:

		 Record->Data.Setup.Background = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    case TOKEN_WIDTH:

		 Record->Data.Setup.Width = ((DynaCfgToken far *)Item)->String;
		 ((DynaCfgToken far *)Item)->String=NULL;

		break;

	    default:
		// unknown node -> ignore it -> stop work, but not error
		Item=NULL;
		continue;       // exit while
	   }

	   Item=RecordList->Next( Item );
	  } // while item
	 } // else

	 break;

  default:
	printf("\n ERROR reported from %s line %d:"
	       "\n  Element or setup identifier missing in HILEX cfg file...", __FILE__,__LINE__);
	exit(0);
 }

 // now record is filled -> delete old list and return addr of
 // newly created single record

 RecordList->Destroy();

 return;
}

//---------------------------------------------------------------------------------------

void Semantator::RecordsToDynaTable( void )
// - converts everything inside to raw mode
// - converts DESCRS to AUTOMAT
// - converts records to dynamic table ( declared inside semantator )
{
 int i;

 ElementID ID;
 // in DynaBinaryCfgTableItem
 // ID which automat returns when this element is accepted
 // - if ID==0 this struct contains beg and end
 //   of html page or table which is encapsulated
 //   using hilex
 // - else contains element things,
 //   later will be element encapsulated
 //   this way: "RawBeg" Element "RawEnd"

 // if CRs not wanted are filtered out in  Buffer.cpp/Save
 char *HtmlPageHeader[]
       =
      { // 0
      "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">"
      "\r\n"
      "\r\n<HTML>"
      "\r\n"
      "\r\n<HEAD>"
      "\r\n <!-- This Web page was converted to HMTL using HILEX -->"
      "\r\n <TITLE>"
      , // 1
      "Page converted by HILEX"
      , // 2
      "</TITLE>"
      "\r\n</HEAD>"
      "\r\n"
      "\r\n<BODY TEXT="
      , // 3
      "\"#FFFF00\""
      , // 4
      " BGCOLOR="
      , // 5
      "\"#000080\""
      , // 6
      " BACKGROUND="
      , // 7
      "\"\""
      , // 8
      ">"
      "\r\n <PRE>"
      "\r\n <FONT FACE="
      , // 9
      "\"COURIER NEW, ARIAL\""
      , // 10
      " SIZE="
      , // 11
      "\"+0\""
      , // 12
      " >"
      "\r\n"
      };

 char HtmlPageTail[]
       =
      "\r\n </FONT>"
      "\r\n </PRE>"
      "\r\n</BODY> "
      "\r\n</HTML>"
      "\r\n"
      ;

 char *HtmlTableHeader[]
       =
      {  // 0
       "<!-- This Web table was converted to HMTL using HILEX -->"
       "\r\n<CENTER>"
       "\r\n<TABLE BORDER=0>"
       "\r\n<TR>"
       , // 1
       "\r\n <TD"
       , // 2
       " WIDTH="
       , // 3
       "\"500\""
       , // 4
       " BGCOLOR="
       , // 5
       "\"#000080\""
       , // 6
       ">"
       "\r\n <PRE>"
       "\r\n <FONT FACE="
       , // 7
       "\"COURIER NEW, ARIAL\""
       , // 8
       " SIZE="
       , // 9
       "\"+0\""
       , // 10
       " COLOR="
       , // 11
       "\"0xFFFF00\""
       , // 12
       " >"
       "\r\n"
       "\r\n"
      };

 char HtmlTableTail[]
       =
      "\r\n </FONT>"
      "\r\n </PRE>"
      "\r\n </TD>"
      "\r\n</TR>"
      "\r\n</TABLE>"
      "\r\n</CENTER>"
      "\r\n"
      ;

 char far *Buffer       = new char[5000];
	  if(!Buffer) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	  Buffer[0]     = 0;

 // --- SORT ---
 // sort records inside file using enum RECORD_ types
 // setups will be at beginning, elements at tail:
 //
 //  RECORD_SETUP_TRANSLATION           // sort to head
 //  RECORD_SETUP_PAGE                  // sort to head
 //  RECORD_NORMAL                   	// do not sort them
 //  RECORD_RAW                      	// do not sort them

 bool           Change;

 ListItem       far *Item,
                far *Item1,
		far *Item2,
		far *Item3,
                far *Item4;

 #ifdef DEBUG
  printf("\n\n - Before sorting ---------------------------------------------"); PrintListOfRecords();
 #endif

 Change=TRUE;

 while( Change )
 {
  if( ListOfRecords->NrOfItems == 1 ) break;    // nothing to sort

  Item=ListOfRecords->GetHead();

  Change=FALSE;

  for( i=1; i<=(ListOfRecords->NrOfItems-1); i++ ) // go through list once
  {

   if(
       (
	((DynaCfgRecord far *)Item->Next)->Type <= RECORD_SETUP_PAGE
       )
       &&
       (
	((DynaCfgRecord far *)Item)->Type
	>
	((DynaCfgRecord far *)Item->Next)->Type
       )
     )
   {
    #ifdef DEBUG
     printf("\n\n %u <-> %u", ((DynaCfgRecord far *)Item)->Type, ((DynaCfgRecord far *)(Item->Next))->Type );
    #endif

    // change order in list:  1234 -> 1324 ( Item points to 2 )
    Item1 = Item->Last;
    Item2 = Item;
    Item3 = Item->Next;
    Item4 = Item->Next->Next;

    // 1234 -> 1324
    Item1->Next = Item3;
    Item2->Next = Item4;
    Item3->Next = Item2;

    Item2->Last = Item3;
    Item3->Last = Item1;
    Item4->Last = Item2;

    Change=TRUE;

    // item must stay correct -> next is done when switching items!
    // item2==item is moved to tail

    continue;
   }

   Item=ListOfRecords->Next(Item);
  } // for
 }


 #ifdef DEBUG
  printf("\n\n - After sorting ---------------------------------------------"); PrintListOfRecords();
 #endif

 // --- WORK ---

 DynaCfgRecord          far *Record;
 DynaBinaryCfgTableItem far *TableItem;

 bool                   StartTag;
 word                   Lng;

 // Cases:
 //  1) Setup translation       page/table/no
 //     Setup page
 //
 //  2) Setup translation
 //
 //  3) Setup page
 //
 //  4) X
 //

 OutputType OutType = HTML_PAGE;

 Record  = (DynaCfgRecord far *)(ListOfRecords->GetHead());

 if( Record==NULL )
 {
  printf("\n ERROR reported from %s line %d:"
         "\n  HILEX cfg file is empty...", __FILE__,__LINE__);
  exit(0);
 }

 switch( Record->Type )
 {
  case RECORD_SETUP_TRANSLATION: // case 1 or case 2

        OutType=Record->Data.Setup.OutType;

	Item=Record;    // backup setup Record

        // take next
        Record  = (DynaCfgRecord far *)(ListOfRecords->Next(Record));

        if( Record==NULL )
	{
         printf("\n\n ERROR reported from %s line %d:"
		"\n  HILEX cfg contains no element section...", __FILE__,__LINE__);
         exit(0);
        }

        ListOfRecords->Delete( Item ); // delete setup record

        if( Record->Type != RECORD_SETUP_PAGE || !Record ) // case 2
        {
	 // no concrete title/facefont/... -> construct default header

         switch( OutType )
         {
	  // -> construct default header for HTML_PAGE
	  case HTML_PAGE:

		 Buffer[0]=0;

		 for( i=0; i<=12; i++ )
		  strcat( Buffer, HtmlPageHeader[i] );

		break;

	  // -> construct default header for HTML_TABLE
	  case HTML_TABLE:

		 Buffer[0]=0;

		 strcat( Buffer, HtmlTableHeader[0] );
		 strcat( Buffer, HtmlTableHeader[1] );

		 for( i=4; i<=12; i++ )
		  strcat( Buffer, HtmlTableHeader[i] );

		break;

	  default:              // NO_ENCAPS

		 Buffer[0]=0;

                break;
         }

         break;
        }

	// else case 1: concrete settings title/facefont/...
	// go to next case ( no break )

  case RECORD_SETUP_PAGE: // case 3
	// only setup page -> default of output is html page
	// -> construct html page with settings from this node

        // if from previous case -> jump to each case

        switch( OutType )
        {
         case HTML_PAGE:

                Buffer[0]=0;

                strcat( Buffer, HtmlPageHeader[0] );

                // title
                if( Record->Data.Setup.Title )
		{
		 strcat( Buffer, Record->Data.Setup.Title );
                  delete Record->Data.Setup.Title;
                  Record->Data.Setup.Title = NULL;
		}
                else
                strcat( Buffer, HtmlPageHeader[1] );

		strcat( Buffer, HtmlPageHeader[2] );

		// base text color
                if( Record->Data.Setup.BaseColor )
                {
                 strcat( Buffer, Record->Data.Setup.BaseColor );
                  delete Record->Data.Setup.BaseColor;
		  Record->Data.Setup.BaseColor = NULL;
		}
                else
                strcat( Buffer, HtmlPageHeader[3] );

		strcat( Buffer, HtmlPageHeader[4] );

		// bg color
		if( Record->Data.Setup.BgColor )
		{
		 strcat( Buffer, Record->Data.Setup.BgColor );
		  delete Record->Data.Setup.BgColor;
		  Record->Data.Setup.BgColor = NULL;
		}
		else
		strcat( Buffer, HtmlPageHeader[5] );

		// background
		if( Record->Data.Setup.Background )
		{
		 strcat( Buffer, HtmlPageHeader[6] );	// Background=

		 strcat( Buffer, Record->Data.Setup.Background );
		  delete Record->Data.Setup.Background;
		  Record->Data.Setup.Background = NULL;
		}

		strcat( Buffer, HtmlPageHeader[8] );

		// font
		if( Record->Data.Setup.FaceFont )
		{
		 strcat( Buffer, Record->Data.Setup.FaceFont );
		  delete Record->Data.Setup.FaceFont;
		  Record->Data.Setup.FaceFont = NULL;
		}
		else
		strcat( Buffer, HtmlPageHeader[9] );

		strcat( Buffer, HtmlPageHeader[10] );

		// size
		if( Record->Data.Setup.BaseSize )
		{
		 strcat( Buffer, Record->Data.Setup.BaseSize );
		  delete Record->Data.Setup.BaseSize;
		  Record->Data.Setup.BaseSize = NULL;
		}
		else
		strcat( Buffer, HtmlPageHeader[11] );

		strcat( Buffer, HtmlPageHeader[12] );

		// delete setup page record - no longer needed
		ListOfRecords->Delete( Record );

	       break;

	  case HTML_TABLE:

		 Buffer[0]=0;

		 strcat( Buffer, HtmlTableHeader[0] );
		 strcat( Buffer, HtmlTableHeader[1] );

		 // width ( only if user wants it ) else nothing
		 if( Record->Data.Setup.Width )
		 {
		  strcat( Buffer, HtmlTableHeader[2] );
		  strcat( Buffer, Record->Data.Setup.Width );

		  delete Record->Data.Setup.Width;
		  Record->Data.Setup.Width = NULL;
		 }

		 // bg color
		 strcat( Buffer, HtmlTableHeader[4] );

		 if( Record->Data.Setup.BgColor )
		 {
		  strcat( Buffer, Record->Data.Setup.BgColor );

		  delete Record->Data.Setup.BgColor;
		  Record->Data.Setup.BgColor = NULL;
		 }
		 else
		  strcat( Buffer, HtmlTableHeader[5] );

		 strcat( Buffer, HtmlTableHeader[6] );

		 // font
		 if( Record->Data.Setup.FaceFont )
		 {
		  strcat( Buffer, Record->Data.Setup.FaceFont );
		   delete Record->Data.Setup.FaceFont;
		   Record->Data.Setup.FaceFont = NULL;
		 }
		 else
		  strcat( Buffer, HtmlTableHeader[7] );

		 strcat( Buffer, HtmlTableHeader[8] );

		 // size
		 if( Record->Data.Setup.BaseSize )
		 {
		  strcat( Buffer, Record->Data.Setup.BaseSize );
		   delete Record->Data.Setup.BaseSize;
		   Record->Data.Setup.BaseSize = NULL;
		 }
		 else
		  strcat( Buffer, HtmlTableHeader[9] );

		 strcat( Buffer, HtmlTableHeader[10] );

		 // base text color
		 if( Record->Data.Setup.BaseColor )
		 {
		  strcat( Buffer, Record->Data.Setup.BaseColor );
		   delete Record->Data.Setup.BaseColor;
		   Record->Data.Setup.BaseColor = NULL;
		 }
		 else
		  strcat( Buffer, HtmlTableHeader[11] );

		 strcat( Buffer, HtmlTableHeader[12] );

		 // delete setup page record - no longer needed
		 ListOfRecords->Delete( Record );

		break;

	  default: // NO_ENCAPS

		 // record RECORD_SETUP_PAGE is not used in this case
		 // => delete it

		 ListOfRecords->Delete( Record );

		 Buffer[0]=0;

                break;
        }

        break;

  default:                      // case 4
        // default is default html page => so construct default html page

         Buffer[0]=0;

         for( i=0; i<=12; i++ )
	  strcat( Buffer, HtmlPageHeader[i] );

         break;
 }


 // construct table item with ID 0 => encapsulation of file

 TableItem = new DynaBinaryCfgTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  TableItem->ID = ID = 0;

  // fill RawBeg
  // put there content of buffer
  Lng = strlen( Buffer );

  if( Lng )
  {
   TableItem->RawBeg = new char[Lng+1]; // allocate
		       if(!(TableItem->RawBeg)) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
   strcpy( TableItem->RawBeg, Buffer ); // copy content
  }
  else
  {
   TableItem->RawBeg = NULL;
  }

  // fill RawEnd
  switch( OutType )
  {
   case HTML_PAGE:

                 Lng = strlen( HtmlPageTail );

                 TableItem->RawEnd = new char[Lng+1]; // allocate
				     if(!(TableItem->RawEnd)) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
                 strcpy( TableItem->RawEnd, HtmlPageTail ); // copy content

                break;

   case HTML_TABLE:

                 Lng = strlen( HtmlTableTail );

                 TableItem->RawEnd = new char[Lng+1]; // allocate
				     if(!(TableItem->RawEnd)) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
                 strcpy( TableItem->RawEnd, HtmlTableTail ); // copy content

                break;

   default: // NO_ENCAPS

                 TableItem->RawEnd = NULL;

                break;
         }



  // insert item to dynamic table

  Table->Insert( TableItem );

  #ifdef DEBUG
   PrintTable();
  #endif

  // make --ELEMENTS-- the same way -> take the next

  DynamicAutomat far *HelpAuto;
  bool               FirstAutoDone = FALSE;

  Record  = (DynaCfgRecord far *)(ListOfRecords->GetHead());

  if( Record==NULL )
  {
   printf("\n ERROR reported from %s line %d:"
          "\n  HILEX cfg contains no element section...", __FILE__,__LINE__);
   exit(0);
  }

  ID      = 1;

  while( Record )
  {

   TableItem            = new DynaBinaryCfgTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

    TableItem->ID       = ID++;

   Buffer[0]=0;

   switch( Record->Type )
   {
    case RECORD_NO_ACTION:

	   #ifdef DEBUG
	    printf("\n\n Creating automat for ID %u...", TableItem->ID );
	   #endif

	   TableItem->RawBeg = TableItem->RawEnd = NULL;

	   if( Record->Data.No.Descr == NULL )
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  Descr is NULL...", __FILE__, __LINE__ );
	    exit( 0 );
	   }

	   if( FirstAutoDone )
	   {
	    HelpAuto=new DynamicAutomat; if(!HelpAuto) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	     HelpAuto->StringToDynaAuto( Record->Data.No.Descr, TableItem->ID );
		 Auto->Wampirator( *HelpAuto );
	    delete HelpAuto;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }
	   else
	   {
	    Auto->StringToDynaAuto( Record->Data.No.Descr, TableItem->ID );
	    FirstAutoDone = TRUE;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }

	   // delete descr
	   delete Record->Data.No.Descr;
	   Record->Data.No.Descr = NULL;

	   // fill Action
	   TableItem->Action = ACTION_NO;

	 break;

    case RECORD_REMOVE:

	   #ifdef DEBUG
	    printf("\n\n Creating automat for ID %u...", TableItem->ID );
	   #endif

	   TableItem->RawBeg = TableItem->RawEnd = NULL;

	   if( Record->Data.Remove.Descr == NULL )
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  Descr is NULL...", __FILE__, __LINE__ );
	    exit( 0 );
	   }

	   if( FirstAutoDone )
	   {
	    HelpAuto=new DynamicAutomat; if(!HelpAuto) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	     HelpAuto->StringToDynaAuto( Record->Data.Remove.Descr, TableItem->ID );
		 Auto->Wampirator( *HelpAuto );
	    delete HelpAuto;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }
	   else
	   {
	    Auto->StringToDynaAuto( Record->Data.Remove.Descr, TableItem->ID );
	    FirstAutoDone = TRUE;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }

	   // delete descr
	   delete Record->Data.Remove.Descr;
	   Record->Data.Remove.Descr = NULL;

	   // fill Action
	   TableItem->Action = ACTION_REMOVE;

	 break;

    case RECORD_REPLACE:

	   #ifdef DEBUG
	    printf("\n\n Creating automat for ID %u...", TableItem->ID );
	   #endif

	   TableItem->RawBeg = Record->Data.Replace.Raw;
	   TableItem->RawEnd = NULL;

	   // mark unused pointers
	   Record->Data.Replace.Raw = NULL;

	   // --- FINISH WITH CONSTRUCTING AUTOMAT ---

	   if( Record->Data.Replace.Descr == NULL )
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  Descr is NULL...", __FILE__, __LINE__ );
	    exit( 0 );
	   }

	   if( FirstAutoDone )
	   {
	    HelpAuto=new DynamicAutomat; if(!HelpAuto) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	     HelpAuto->StringToDynaAuto( Record->Data.Replace.Descr, TableItem->ID );
		 Auto->Wampirator( *HelpAuto );
	    delete HelpAuto;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }
	   else
	   {
	    Auto->StringToDynaAuto( Record->Data.Replace.Descr, TableItem->ID );
	    FirstAutoDone = TRUE;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }

	   // delete descr
	   delete Record->Data.Replace.Descr;
	   Record->Data.Replace.Descr = NULL;

	   // fill action
	   TableItem->Action = ACTION_REPLACE;

	 break;


    case RECORD_RAW:
	   // only fill pointers, already filled

	   #ifdef DEBUG
	    printf("\n\n Creating automat for ID %u...", TableItem->ID );
	   #endif

	   TableItem->RawBeg = Record->Data.Raw.RawBeg;
	   TableItem->RawEnd = Record->Data.Raw.RawEnd;

	   // mark unused pointers
	   Record->Data.Raw.RawBeg = NULL;
	   Record->Data.Raw.RawEnd = NULL;

	   // --- FINISH WITH CONSTRUCTING AUTOMAT ---

	   if( Record->Data.Raw.Descr == NULL )
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  Descr is NULL...", __FILE__, __LINE__ );
	    exit( 0 );
	   }

	   if( FirstAutoDone )
	   {
	    HelpAuto=new DynamicAutomat; if(!HelpAuto) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	     HelpAuto->StringToDynaAuto( Record->Data.Raw.Descr, TableItem->ID );
		 Auto->Wampirator( *HelpAuto );
	    delete HelpAuto;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }
	   else
	   {
	    Auto->StringToDynaAuto( Record->Data.Raw.Descr, TableItem->ID );
	    FirstAutoDone = TRUE;

	    #ifdef DEBUG
	     Auto->Print();
	    #endif
	   }

	   // delete descr
	   delete Record->Data.Raw.Descr;
	   Record->Data.Raw.Descr = NULL;

	   // fill action
	   if( TableItem->RawBeg==NULL && TableItem->RawEnd==NULL )
	    TableItem->Action = ACTION_NO;
	   else
	    TableItem->Action = ACTION_ENCAPSULATE;


	 break;

    case RECORD_NORMAL:

	   // what's round must be constructed

	   // CONSTRUCT RawBeg
	   #ifdef DEBUG
	    printf("\n\n Creating automat for ID %u...", TableItem->ID );
	   #endif

	   Buffer[0]=0;

	   if( Record->Data.Normal.Underscore )
	    strcat( Buffer, "<U>");
	   if( Record->Data.Normal.Italic )
	    strcat( Buffer, "<I>");
	   if( Record->Data.Normal.Bold )
	    strcat( Buffer, "<B>");

           StartTag=TRUE;

           // font
           if( Record->Data.Normal.Font !=NULL )
           {
            strcat( Buffer, "<FONT FACE=");
            StartTag = FALSE;

            strcat( Buffer, Record->Data.Normal.Font);

            delete Record->Data.Normal.Font; // no longer needed
            Record->Data.Normal.Font = NULL;
           }

           // size
           if( Record->Data.Normal.Size !=NULL )
           {
            if( StartTag )
            {
             strcat( Buffer, "<FONT SIZE=");
             strcat( Buffer, Record->Data.Normal.Size);

	     StartTag = FALSE;
	    }
	    else // only add face section
	    {
	     strcat( Buffer, " SIZE=");
	     strcat( Buffer, Record->Data.Normal.Size);
	    }

	    delete Record->Data.Normal.Size; // no longer needed
	    Record->Data.Normal.Size = NULL;
	   }

	   // color
	   if( Record->Data.Normal.Color !=NULL )
	   {
	    if( StartTag )
	    {
	     strcat( Buffer, "<FONT COLOR=");
	     strcat( Buffer, Record->Data.Normal.Color);

	     StartTag = FALSE;
	    }
	    else // only add face section
	    {
	     strcat( Buffer, " COLOR=");
	     strcat( Buffer, Record->Data.Normal.Color);
	    }

	    delete Record->Data.Normal.Color; // no longer needed
	    Record->Data.Normal.Color = NULL;
	   }

	   // finish font tag
	   if( StartTag==FALSE )
	   {
	    // tag was started => must be finished
	    strcat( Buffer, ">");
	   }

	   // and fill RawBeg
	   if( Buffer[0]==0 )
	    TableItem->RawBeg    = NULL;
	   else
	   {
	    Lng                  = strlen( Buffer );
	    TableItem->RawBeg    = new char[Lng+1]; // allocate
				   if(!TableItem->RawBeg) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	    strcpy( TableItem->RawBeg, Buffer ); // copy content
	   }


          // CONSTRUCT RawEnd

          Buffer[0]=0;

	  if( StartTag==FALSE )
	   strcat( Buffer, "</FONT>");

	  if( Record->Data.Normal.Bold )
	   strcat( Buffer, "</B>");
	  if( Record->Data.Normal.Italic )
	   strcat( Buffer, "</I>");
	  if( Record->Data.Normal.Underscore )
	   strcat( Buffer, "</U>");


	  if( Buffer[0]==0 )
           TableItem->RawEnd    = NULL;
	  else
          {
           Lng                  = strlen( Buffer );
	   TableItem->RawEnd    = new char[Lng+1]; // allocate
				  if(!TableItem->RawEnd) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   strcpy( TableItem->RawEnd, Buffer ); // copy content
          }

           // --- FINISH WITH CONSTRUCTING AUTOMAT ---

           if( Record->Data.Normal.Descr == NULL )
           {
            printf("\n ERROR reported from %s line %d:"
                   "\n  Descr is NULL...", __FILE__, __LINE__ );
            exit( 0 );
           }

           if( FirstAutoDone )
           {
	    HelpAuto=new DynamicAutomat; if(!HelpAuto) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
             HelpAuto->StringToDynaAuto( Record->Data.Normal.Descr, TableItem->ID );
                 Auto->Wampirator( *HelpAuto );
            delete HelpAuto;

            #ifdef DEBUG
             Auto->Print();
            #endif
           }
           else
           {
            Auto->StringToDynaAuto( Record->Data.Normal.Descr, TableItem->ID );
            FirstAutoDone = TRUE;

            #ifdef DEBUG
             Auto->Print();
            #endif
           }

           // delete descr
           delete Record->Data.Normal.Descr;
           Record->Data.Normal.Descr = NULL;

	   // fill action
	   TableItem->Action = ACTION_ENCAPSULATE;

	 break;

    default:
        printf("\n ERROR reported from %s line %d:"
	       "\n  More than one Setup block for one of the options...", __FILE__,__LINE__);
        exit(0);
   }

   // and now insert table item to list
   Table->Insert( TableItem );

   // backup record for deleting
   Item = Record;

   // take the next record
   Record  = (DynaCfgRecord far *)(ListOfRecords->Next(Record));

   // delete record from list ( save memory )
   ListOfRecords->Delete( Item );
  }

 #ifdef DEBUG
  PrintTable();
 #endif

 delete Buffer;

 return;
}

//---------------------------------------------------------------------------------------

void Semantator::PrintTable( void )
{
 #ifdef DEBUG

  ListItem far *Item=Table->GetHead();

  while( Item )
  {
   printf("\n\n * TableItem ***********************************************");

   ((DynaBinaryCfgTableItem far *)Item)->Print();

   Item=Table->Next( Item );
  }

 #endif
}

//---------------------------------------------------------------------------------------

void Semantator::PrintListOfRecords( void )
{
 #ifdef DEBUG

  ListItem far *Item=ListOfRecords->GetHead();

  while( Item )
  {
   printf("\n\n * Record ***********************************************");

   ((DynaCfgRecord far *)Item)->Print();

   Item=ListOfRecords->Next( Item );
  }

 #endif
}

//---------------------------------------------------------------------------------------

void Semantator::Save( char *FileName )
{

 #ifdef DEBUG
  printf("\n\n Saving automat %Fp to file %s...", this, FileName );
 #endif

 int  Handle;

 if((Handle = open( FileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                     S_IREAD | S_IWRITE)) == -1)
 {
  printf("\n ERROR reported from %s line %d:"
         "\n Error opening file \"%s\"...", __FILE__, __LINE__, FileName );
  EXIT_PROGRAM
 }

 // --- save magic ---

 if( write( Handle, "BCA", 3u ) != 3u ) ERROR_WHEN_WRITING

 // --- save options for translation ---

 if( write( Handle, &HTMLOutput, 1u ) != 1u ) ERROR_WHEN_WRITING

 if( write( Handle, &CRLF, 1u ) != 1u ) ERROR_WHEN_WRITING

 // --- save encapsulation table ---

 // save nr of items in table  ( contains encapsulation things )
 if( write( Handle, &(Table->NrOfItems), 2u ) != 2u ) ERROR_WHEN_WRITING


 ListItem far *Item=Table->GetHead();

 while( Item )
 {
  ((DynaBinaryCfgTableItem far *)Item)->Save( Handle );

  Item=Table->Next( Item );
 }

 // --- save automat ---

 #ifdef DEBUG
  Auto->Print();
 #endif

 Auto->Append( Handle );

 // --- finish ---

 close( Handle );

}

//---------------------------------------------------------------------------------------

void Semantator::SyntactTreeToDynamicTable( void )
{
 // alloc File and transform syntact tree to it
 File          = new DynaCfgFile; if(!File) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

        #ifdef DEBUG
         printf("\n\n Converting syntact tree to dynamic configuration File...");
        #endif

 DiveForR3s( Root );

        #ifdef DEBUG
         printf("\n\n Deleting skelet of syntact tree ( not strings )...");
        #endif

 if( Root->Type != SNODE_R3 ) delete Root; // already deleted in DiveForR3


        #ifdef DEBUG
         File->PrintTokens();
        #endif

        #ifdef DEBUG
         printf("\n\n Converting each RecordList (== list of tokens ) to one SingleRecord...");
        #endif



 ListItem       far *Item       = File->Records->GetHead();
 DynaCfgRecord  far *Record;

 ListOfRecords = new List; if(!ListOfRecords) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 while( Item )
 {
  Record = new DynaCfgRecord; if(!Record) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  DynaCfgRecordListToDynaCfgRecord( ((DynaCfgRecordList far *)Item),
                                    Record
                                  );
        #ifdef DEBUG
         printf("\n\n Token converted to record:"); Record->Print();
        #endif

  ListOfRecords->Insert( Record );      // insert newly created record

  Item=File->Records->Next(Item);
 }

        #ifdef DEBUG
         printf("\n\n Deleting the rest of dynamic configuration file...");
        #endif

 delete File;                           // no longer needed

        #ifdef DEBUG
         printf("\n\n Converting list of records to dynamic table...");
        #endif

 RecordsToDynaTable();

        #ifdef DEBUG
         printf("\n\n Deleting list of records...");
        #endif

 PrintListOfRecords();

 delete ListOfRecords;                  // no longer needed ( now is in table )

 // now is in memory only Table ( declared inside Semantator class )

 return;
}

//- EOF ---------------------------------------------------------------------------------


