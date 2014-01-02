#include "source\dynauto.h"

// print info when hunting bugs
// #define DEBUG

#define EXIT_PROGRAM exit(0);

#define OPTIMIZE
#define PRINT_OPTIMIZE

extern bool Verbose;

//- DynamicJmpTableItem methods ---------------------------------------------------

void DynaJmpTableItem::Print( void )
{
 #ifdef DEBUG

  printf("\n  DJmpTableItem %Fp: from \'%c\' to \'%c\' jmp %u flags ", this, From, To, JmpTo );

  // now print flags
  if( Flags&JMP_OK )     	printf("O");
  if( Flags&JMP_NOK )    	printf("N");
  if( Flags&JMP_ACCEPT ) 	printf("A");
  if( Flags&JMP_END )    	printf("E");
  if( Flags&JMP_REST )   	printf("R");
  if( Flags&JMP_NOT_SET )    	printf("-");
  if( Flags&JMP_ELEMENT_ID ) 	printf("I");
  if( Flags&JMP_STOP )       	printf("S");

 #endif
}

//- DynaJmpTable methods ---------------------------------------------------

DynaJmpTable::DynaJmpTable( char far *Expr )
{
 Items=new List; if(!Items) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 JumpsAlreadySet=FALSE;	// true if jumps already set,
			// if true => duplicating corresponding items

 SwitchToDynaJmpTable( Expr );
}

//-----------------------------------------------------------------------------

DynaJmpTable::DynaJmpTable( DynaJmpTableItem far *TableItem )
// for green ghosts - adds one item from 0 to 255 and JMP_STOP ( already filled )
{
 Items=new List; if(!Items) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 JumpsAlreadySet=FALSE;	// true if jumps already set,
			// if true => duplicating corresponding items

 // insert universal item
 Items->Insert( TableItem );
}

//-----------------------------------------------------------------------------

word DynaJmpTable::SwitchToDynaJmpTable( char far *Expr )
// analyze string expression and create JmpTable
//
// accepts and copies switch
// types:
//              a	1.
//       	\a      2.
//              \\      3.
//              [...]   4.
//
// wrong input: (a), \\\, ...
{
  DynaJmpTableItem far *TableItem;
  word 		      e;
  bool		      FillTo=FALSE,
		      Neg=FALSE;	// false if [...], true if [^...]



  switch( Expr[0] )
  {
   case '\\':	// type 2., type 3.

	/*
	  input format:
	   Expr[0]='\\';
	   Expr[1]=character;
	   Expr[2]=0;
	*/

	TableItem=new DynaJmpTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	 // detect special characters such as \n, \t, ...
	 switch( Expr[1] )
	 {
	  case 'n':
		    TableItem->From='\n';
		    TableItem->To  ='\n';
		    break;
	  case 't':
		    TableItem->From='\t';
		    TableItem->To  ='\t';
		    break;
	  default:
		    TableItem->From=Expr[1];
		    TableItem->To  =Expr[1];
		    break;
	 }
	 TableItem->JmpTo=0;
	 TableItem->Flags= JMP_OK|JMP_NOT_SET;
	Items->Insert( TableItem );

	break;

   case '[':	// type 4. ( errors already fixed )

	/*
	  input format:
	   Expr[0]=[;
			A-CB-Dghj-z\]
	   Expr[n]=];

	  so elements have this format:
	   X
	   \X
	   X-X
	   \X-\X

	  -> treat each...
	*/

	e=1;	// now points behind [

	// check if it is NEGATION ( type [^...] )
	if( Expr[e]=='^' )
	{
	 e++;      	// move behind negation
	 Neg=TRUE;      // set flag
	}
	else
	 Neg=FALSE;

	do
	{
	 switch( Expr[e] )
	 {
	  case '\\':	// take whats behind
		     e++;

		     if( FillTo )
		     {
		      if( TableItem )
		      {
		       // detect special characters such as \n, \t, ...
		       switch( Expr[e] )
		       {
			case 'n':
				  TableItem->To  ='\n';
				  break;
			case 't':
				  TableItem->To  ='\t';
				  break;
			default:
				  TableItem->To  =Expr[e];

				  if( TableItem->From > TableItem->To )
				  {
				   printf("\n ERROR reported from %s line %d:"
					  "\n  switch interval %d-%d has no sense ( TO is bigger then FROM )...", __FILE__, __LINE__, TableItem->From, TableItem->To  );
				   exit(0);
				  }

				  break;
		       }
		       FillTo=FALSE;
		      }
		      else // TableItem
		      {
		       printf("\n ERROR reported from %s line %d:"
			      "\n  Regular switch \"%s\" has not the right format...", __FILE__, __LINE__, Expr );
		       EXIT_PROGRAM
		      }
		     }
		     else // FillTo
		     {
		      TableItem=new DynaJmpTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
		       // detect special characters such as \n, \t, ...
		       switch( Expr[e] )
		       {
			case 'n':
				  TableItem->From='\n';
				  TableItem->To  ='\n';
				  break;
			case 't':
				  TableItem->From='\t';
				  TableItem->To  ='\t';
				  break;
			default:
				  TableItem->From=Expr[e];
				  TableItem->To  =Expr[e];

				  if( TableItem->From > TableItem->To )
				  {
				   printf("\n ERROR reported from %s line %d:"
					  "\n  switch interval %c-%c has no sense ( TO is bigger then FROM )...", __FILE__, __LINE__, TableItem->From, TableItem->To  );
				   exit(0);
				  }
				  break;
		       }

		       TableItem->JmpTo=0;

		       if( Neg )
			TableItem->Flags=JMP_NOK|JMP_NOT_SET|JMP_STOP;
		       else
			TableItem->Flags=JMP_OK|JMP_NOT_SET;

		      Items->Insert( TableItem );
		     }

		     break;

	  case '-':	// valid - ... so fill To of the last TableItem

		     FillTo=TRUE;
		     e++;

	  default:
		     if( FillTo )
		     {
		      if( TableItem )
		      {
		       TableItem->To=Expr[e];

		       if( TableItem->From > TableItem->To )
		       {
			printf("\n ERROR reported from %s line %d:"
			       "\n  switch interval %c-%c in \"%s\"has no sense ( TO is bigger then FROM )...", __FILE__, __LINE__, TableItem->From, TableItem->To, Expr  );
			exit(0);
		       }

		       FillTo=FALSE;
		      }
		      else // TableItem
		      {
		       printf("\n ERROR reported from %s line %d:"
			      "\n  Regular switch \"%s\" has not the right format...", __FILE__, __LINE__, Expr );
		       EXIT_PROGRAM
		      }
		     }
		     else // FillTo
		     {
		      TableItem=new DynaJmpTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
		       TableItem->From =Expr[e];
		       TableItem->To   =Expr[e];
		       TableItem->JmpTo=0;

		       if( Neg )
			TableItem->Flags=JMP_NOK|JMP_NOT_SET|JMP_STOP;
		       else
			TableItem->Flags=JMP_OK|JMP_NOT_SET;

		      Items->Insert( TableItem );
		     }

		     break;

	 }	// switch
	}
	while( Expr[++e]!=']' );

	break;

   default:     // type 1.

	/*
	  input format:
	   Expr[0]=character;
	   Expr[1]=0;
	*/

	TableItem=new DynaJmpTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	 // detect special characters such as ., $, ^...
	 switch( Expr[0] )
	 {
	  case '.':
		    TableItem->From ='\n';
		    TableItem->To   ='\n';
		    TableItem->Flags=JMP_NOK|JMP_NOT_SET|JMP_STOP;
		    Neg=TRUE;		// rest will be ok
		    break;
	  default:
		    TableItem->From =Expr[0];
		    TableItem->To   =Expr[0];
		    TableItem->Flags= JMP_OK|JMP_NOT_SET;
		    break;

	 }
	 TableItem->JmpTo=0;
	Items->Insert( TableItem );

	break;

  } // switch

 // fill the last item in table -> REST
 TableItem=new DynaJmpTableItem; if(!TableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
  TableItem->From=0;
  TableItem->To=0;
  TableItem->JmpTo=0;

  if( Neg )
   TableItem->Flags= JMP_OK |JMP_REST|JMP_NOT_SET;
  else
   TableItem->Flags= JMP_NOK|JMP_REST|JMP_NOT_SET|JMP_STOP;

 Items->Insert( TableItem );

 return HI_OK;
}

//-----------------------------------------------------------------------------

void DynaJmpTable::Print( void )
{
 #ifdef DEBUG

  printf("\n DJmpTable 0x%Fp:", this );

  ListItem far *Item;

  Item=Items->GetHead();

  while( Item )
  {
   ((DynaJmpTableItem far *)Item)->Print();
   Item=Items->Next(Item);
  }

 #endif
}

//-----------------------------------------------------------------------------

void DynaJmpTable::Shift( word ShiftNum )
{
  ListItem far *Item;

  Item=Items->GetHead();

  while( Item )
  {
   if( !(((DynaJmpTableItem far *)Item)->Flags&JMP_ELEMENT_ID) )
    ((DynaJmpTableItem far *)Item)->JmpTo += ShiftNum;

   Item=Items->Next(Item);
  }
}

//-----------------------------------------------------------------------------

void DynaJmpTable::SetJmpInOKs( AutoItemID ID, ElementID EleID )
{
 DynaJmpTableItem far *ElementTableItem;
 ListItem         far *Item;

  Item=Items->GetHead();

  while( Item )
  {

   if( ((DynaJmpTableItem far *)Item)->Flags & JMP_OK )
   {
    // MUST BE SET ACCEPT ---OR--- WHERE TO JUMP ( NEVER BOTH )

    if( ID==INFINITY ) // set accept
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=0;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags|JMP_ACCEPT;
      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->Flags
      |=
      JMP_ACCEPT;
     }
    }
    else // set where to jump
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=ID;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags;
       // ACCEPTED must be cleared ( it would not jump! )
       ElementTableItem->Flags|=JMP_ACCEPT;
       ElementTableItem->Flags^=JMP_ACCEPT;

      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->JmpTo
      =
      ID;
     }
    }
   }

   Item=Items->Next(Item);
  }

 // insert items where is element ID if in table is accept
 if( ID==INFINITY )
 {
  ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
   ElementTableItem->From=0;
   ElementTableItem->To=0;
   ElementTableItem->JmpTo=EleID;
   ElementTableItem->Flags=JMP_ELEMENT_ID;
  Items->InsertBeforeTail( ElementTableItem );
 }

}

//-----------------------------------------------------------------------------

void DynaJmpTable::SetJmpInNOKs( AutoItemID ID )
{
 DynaJmpTableItem far *ElementTableItem;
 ListItem 	  far *Item;

  Item=Items->GetHead();

  while( Item )
  {

   if( ((DynaJmpTableItem far *)Item)->Flags & JMP_NOK )
   {
    // MUST BE SET END ---OR--- WHERE TO JUMP ( NEVER BOTH )

    if( ID==INFINITY ) // set accept
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=0;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags|JMP_END;
      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->Flags
      |=
      JMP_END;
     }
    }
    else
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=ID;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags;
      // ACCEPTED must be cleared ( it would not jump! )
      ElementTableItem->Flags|=JMP_END;
      ElementTableItem->Flags^=JMP_END;

      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->JmpTo
      =
      ID;
     }
    }
   }

   Item=Items->Next(Item);
  }
}

//-----------------------------------------------------------------------------

void DynaJmpTable::SetJmpInOKsAndMuteToNOKs( AutoItemID ID )
{
 DynaJmpTableItem far *ElementTableItem;
 ListItem         far *Item;

  Item=Items->GetHead();

  while( Item )
  {

   if( ((DynaJmpTableItem far *)Item)->Flags & JMP_OK )
   {
    // mute
    ((DynaJmpTableItem far *)Item)->Flags ^= JMP_OK;
    ((DynaJmpTableItem far *)Item)->Flags |= JMP_NOK;

    // MUST BE SET END ---OR--- WHERE TO JUMP ( NEVER BOTH )

    if( ID==INFINITY ) // set accept
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=0;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags|JMP_END;
      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->Flags
      |=
      JMP_END;
     }
    }
    else
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=ID;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags;
      // ACCEPTED must be cleared ( it would not jump! )
      ElementTableItem->Flags|=JMP_END;
      ElementTableItem->Flags^=JMP_END;

      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->JmpTo
      =
      ID;
     }
    }
   }

   Item=Items->Next(Item);
  }
}

//-----------------------------------------------------------------------------

void DynaJmpTable::SetJmpInNOKsAndMuteToOKs( AutoItemID ID, ElementID EleID )
{
 DynaJmpTableItem far *ElementTableItem;
 ListItem         far *Item;

  Item=Items->GetHead();

  while( Item )
  {

   if( ((DynaJmpTableItem far *)Item)->Flags & JMP_NOK )
   {
    // mute
    ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOK;
    ((DynaJmpTableItem far *)Item)->Flags |= JMP_OK;

    // MUST BE SET ACCEPT ---OR--- WHERE TO JUMP ( NEVER BOTH )

    if( ID==INFINITY ) // set accept
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=0;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags|JMP_ACCEPT;
      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->Flags
      |=
      JMP_ACCEPT;
     }
    }
    else // set where to jump
    {
     if( !(((DynaJmpTableItem far *)Item)->Flags & JMP_NOT_SET) )
     {
      // if already set duplicate

      ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       ElementTableItem->From =((DynaJmpTableItem far *)Item)->From;
       ElementTableItem->To   =((DynaJmpTableItem far *)Item)->To;
       ElementTableItem->JmpTo=ID;
       ElementTableItem->Flags=((DynaJmpTableItem far *)Item)->Flags;
       // ACCEPTED must be cleared ( it would not jump! )
       ElementTableItem->Flags|=JMP_ACCEPT;
       ElementTableItem->Flags^=JMP_ACCEPT;

      Items->InsertAfterHead( ElementTableItem );
     }
     else
     {
      ((DynaJmpTableItem far *)Item)->Flags ^= JMP_NOT_SET; // null

      ((DynaJmpTableItem far *)Item)->JmpTo
      =
      ID;
     }
    }
   }

   Item=Items->Next(Item);
  }

 // insert items where is element ID if in table is accept
 // here because of while ( put this item to the end of jmp table )
 if( ID==INFINITY )
 {
  ElementTableItem=new DynaJmpTableItem; if(!ElementTableItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
   ElementTableItem->From=0;
   ElementTableItem->To=0;
   ElementTableItem->JmpTo=EleID;
   ElementTableItem->Flags=JMP_ELEMENT_ID;
  Items->InsertBeforeTail( ElementTableItem );
 }

}

//-----------------------------------------------------------------------------

word DynaJmpTable::Save( int Handle, AutoItemID ID )
{
 ListItem 		far *Item;
 StatJmpTableItem       TableItem;
 word 			Step;


 // save NrOfItems in JmpTable
 if( write( Handle, &(Items->NrOfItems), 2u ) != 2u )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Error writing to the file...", __FILE__, __LINE__ );
  EXIT_PROGRAM
 }



 // save JmpTable and shuffle items:
 //  FROM-TO
 //  ..
 //  REST
 //  ..
 //  ELEMENT_ID

 // => Go through three times

 for( Step=1; Step<=3; Step++ )
 {
  Item=Items->GetHead();

  while( Item )
  {

   if(
      Step==3
      &&
      (((DynaJmpTableItem far *)Item)->Flags&JMP_ELEMENT_ID)
     )
   {
    TableItem.From=((DynaJmpTableItem far *)Item)->From;
    TableItem.To=((DynaJmpTableItem far *)Item)->To;
    TableItem.JmpTo=((DynaJmpTableItem far *)Item)->JmpTo;
    TableItem.Flags=((DynaJmpTableItem far *)Item)->Flags;

    if( write( Handle, &TableItem, sizeof(TableItem) ) != sizeof(TableItem) )
    {
     printf("\n ERROR reported from %s line %d:"
	    "\n  Error writing to the file...", __FILE__, __LINE__ );
     EXIT_PROGRAM
    }
   }
   else
   {
    if(
       Step==2
       &&
       (((DynaJmpTableItem far *)Item)->Flags&JMP_REST)
      )
    {
     TableItem.From=((DynaJmpTableItem far *)Item)->From;
     TableItem.To=((DynaJmpTableItem far *)Item)->To;
     TableItem.JmpTo=((DynaJmpTableItem far *)Item)->JmpTo;
     TableItem.Flags=((DynaJmpTableItem far *)Item)->Flags;

     if( write( Handle, &TableItem, sizeof(TableItem) ) != sizeof(TableItem) )
     {
      printf("\n ERROR reported from %s line %d:"
	     "\n  Error writing to the file...", __FILE__, __LINE__ );
      EXIT_PROGRAM
     }
    }
    else
    {
     if(
	Step==1
	&&
	!(((DynaJmpTableItem far *)Item)->Flags&JMP_REST)
	&&
	!(((DynaJmpTableItem far *)Item)->Flags&JMP_ELEMENT_ID)
       )
     {

      TableItem.From=((DynaJmpTableItem far *)Item)->From;
      TableItem.To=((DynaJmpTableItem far *)Item)->To;
      TableItem.JmpTo=((DynaJmpTableItem far *)Item)->JmpTo;
      TableItem.Flags=((DynaJmpTableItem far *)Item)->Flags;

      // OK, STOP and jmp to self has no sense -> loop -> so clear it
      if( TableItem.Flags&JMP_OK
	   &&
	  TableItem.Flags&&JMP_STOP
	   &&
	  TableItem.JmpTo == ID		// jumping to self
	)
      {
       TableItem.Flags|=JMP_STOP;	// clear
       TableItem.Flags^=JMP_STOP;
      }

      if( write( Handle, &TableItem, sizeof(TableItem) ) != sizeof(TableItem) )
      {
       printf("\n ERROR reported from %s line %d:"
	      "\n  Error writing to the file...", __FILE__, __LINE__ );
       EXIT_PROGRAM
      }
     }
    }
   }



   Item=Items->Next(Item);
  }
 } // for

 return HI_OK;
}

//-----------------------------------------------------------------------------

DynaJmpTable::~DynaJmpTable( void )
{
 delete Items;
}

//- DynaAutoItem methods -------------------------------------------------------

void DynaAutoItem::Print( void )
{
 #ifdef DEBUG
  printf("\n DAutoItem JmpTable %Fp", JmpTable );
 #endif
}

//- DynamicAutomat methods ----------------------------------------------------

DynamicAutomat::DynamicAutomat( void )
{
 #ifdef DEBUG
  printf("\n\n DynamicAutomat:");
 #endif

 LastID     =0;
 Items      =new List; 		if(!Items) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 Registry   =new BoxStructure;  if(!Registry) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 InputStates=new List;          if(!InputStates) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
}

//-----------------------------------------------------------------------------

DynamicAutomat::~DynamicAutomat( void )
{
 #ifdef DEBUG
  printf("\n ~DynamicAutomat:"
	 "\n  Deleting items and JmpTables"
	);
 #endif

 // first delete JmpTables
 ListItem far *Item;

 Item=Items->GetHead();

 while( Item )
 {
  #ifdef DEBUG
   ((DynaAutoItem far *)Item)->JmpTable->Print();
  #endif

  delete ((DynaAutoItem far *)Item)->JmpTable;
  Item=Items->Next(Item);
 }

 // then delete Items
 delete Items;

 delete InputStates;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::AddItem( AutoItemID &ID, DynaAutoItem far *&Item )
{
 Item=new DynaAutoItem; if(!Item) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 ID=Item->ID=LastID++;

 Item->JmpTable=NULL;

 Items->Insert( Item );

 return HI_OK;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::StringToDynaAuto( char far *Infix, ElementID ID )
{
 TempID=ID;

 // convert string in infix to string in prefix
 RegularString far *RString=new RegularString( Infix ); if(!RString) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 RString->ToPrefix();

 // convert string in prefix to tree
 RegularTree   far *RTree=new RegularTree(); if(!RTree) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 RTree->PrefixToTree( RString->Prefix );

 delete RString;

 // convert tree to dynamic automat
 TreeToDynaAuto( RTree );

 delete RTree;

 return HI_OK;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::TreeToDynaAuto( RegularTree far *Tree )
{
 RegularBox far *ResultInBox=NULL;

 // build box structure together with auto
 TreeToDynaAutoLevel( Tree->Root, ResultInBox );

 // fill "jmp" in jump tables
 FillJmpTables( ResultInBox );

 Print();

 // delete box structure
 delete Registry;

 return HI_OK;
}

//-----------------------------------------------------------------------------

void DynamicAutomat::JoinLists(
			       List          far *FromList,
			       void    	     far *ToBox,
			       ListSignature ToList
			      )
{
 BoxListItem far *BoxItem=new BoxListItem; if(!BoxItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 BoxItem->List		=ToList;
 BoxItem->Target	=ToBox;

 FromList->Insert( BoxItem );
}

//-----------------------------------------------------------------------------

word DynamicAutomat::TreeToDynaAutoLevel( RegularNode far *Node,
					  RegularBox  far *&Box
					)
{
 RegularBox far *Box1,
	    far *Box2,
	    far	*ContinueBox;

 if( Node==NULL ) return NULL;

 // convert self
 SubtreeToBox( Node, Box1 );
 Box=Box1;

 // case only node, no continue ( pointer to Box is returned in parameter )
 if( Node->Continue==NULL ) return HI_OK;

 Node=Node->Continue;

 // case two and more nodes
 while( Node!=NULL )
 {
  SubtreeToBox( Node, Box2 );

  #ifdef DEBUG
   printf("\n\n Tree->Box CONTINUE:");
  #endif

  ContinueBox = new RegularBox( CONTINUE, Registry ); if(!ContinueBox) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  // join boxes inside continue box...
  // Continue In to Box1 IN
  // Box1 OK to Box2 IN
  // Box1 NOK to Continue NOK
  // Box2 OK to Continue OK
  // Box2 NOK to Continue NOK

  JoinLists( ContinueBox->IN, Box1,        IN  );
  JoinLists( Box1->OK       , Box2,   	   IN  );
  JoinLists( Box1->NOK      , ContinueBox, NOK );
  JoinLists( Box2->OK       , ContinueBox, OK  );
  JoinLists( Box2->NOK      , ContinueBox, NOK );

  Node=Node->Continue;
  Box1=ContinueBox;
 }

 Box=ContinueBox;

 return HI_OK;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::SubtreeToBox( RegularNode far *Node,
				   RegularBox  far *&Box
				 )
// first construct son(s) then continue
{
 RegularBox   far *LBox,
	      far *RBox,
	      far *OldBox,
	      far *OrBox;
 DynaAutoItem far *AutoItem;
 AutoItemID   ID;
 word	      i;
 bool         First=TRUE;



 if( Node==NULL )
 {
  Box=NULL;
  return HI_OK;
 }

 switch( Node->Type )
 {
  case SWITCH:
	       #ifdef DEBUG
		printf("\n\n Tree->Box SWITCH:");
	       #endif

	       Box = new RegularBox( SWITCH, Registry ); if(!Box) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

	       // add "stav" to automat,
	       AddItem( ID, AutoItem );

	       // construct jmp table and join it
	       AutoItem->JmpTable=new DynaJmpTable( Node->Data.Switch.Expr ); if(!(AutoItem->JmpTable)) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

	       // IN point to Item of the automat ( addr of AutoItem )
	       JoinLists( Box->IN, AutoItem, NOT_A_LIST );

	       return HI_OK;

  case OR:
	       #ifdef DEBUG
		printf("\n\n Tree->Box OR:");
	       #endif

	       // first do LSON and RSON,
	       // then construct OR and continue
	       OrBox     =new RegularBox( OR, Registry ); if(!OrBox) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

	       TreeToDynaAutoLevel( Node->Data.Or.LeftSon,  LBox );
	       TreeToDynaAutoLevel( Node->Data.Or.RightSon, RBox );

	       if( LBox==NULL  || RBox==NULL )
	       {
		printf("\n ERROR reported from %s line %d:"
		       "\n  OR operation has bad or only one argument...", __FILE__, __LINE__ );
		EXIT_PROGRAM
	       }

	       // join LBox and RBox inside OR box (NDET version)...
	       // ORBox In to LBox IN		(NDET)
	       // ORBox In to RBox IN           (NDET)
	       // LBox OK  to ORBox OK
	       // LBox NOK 	stays empty
	       // RBox OK  to ORBox OK
	       // RBox NOK 	stays empty
	       // GREEN ARROW:  OrBox IN to OrBox NOK (NDET)
	       //		( simulates fail inside )

	       JoinLists( OrBox->IN        , LBox,   	 IN  );
	       JoinLists( OrBox->IN        , RBox,   	 IN  );
	       JoinLists( OrBox->IN        , OrBox,   	NOK  ); // green arrow
	       JoinLists( LBox->OK         , OrBox,   	 OK  );
	       JoinLists( RBox->OK         , OrBox,   	 OK  );

	       Box=OrBox;

	       return HI_OK;

  case REPEATER:
	       #ifdef DEBUG
		printf("\n\n Tree->Box REPEATER from %u to %u:", Node->Data.Repeater.From, Node->Data.Repeater.To );
	       #endif

	       if( Node->Data.Repeater.From == 0
		   &&
		   Node->Data.Repeater.From == Node->Data.Repeater.To
		 )
	       {
		printf("\n ERROR reported from %s line %d:"
		       "\n  Repeater {0,0} has no sense...", __FILE__, __LINE__ );
		EXIT_PROGRAM
	       }



	       // create repeater box
	       RBox = new RegularBox( REPEATER, Registry ); if(!RBox) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	       OldBox=NULL;



	       // create FROM boxes
	       for( i=1; i<=Node->Data.Repeater.From; i++ )
	       {
		TreeToDynaAutoLevel( Node->Data.Repeater.Son, Box ); // create what will be repated

		// fail must point to repeaters NOK not to NULL
		JoinLists( Box->NOK       , RBox,   	 NOK  );

		if( OldBox )
		{
		 JoinLists( OldBox->OK     , Box,   	 IN  );
		}

		OldBox=Box;


		if( First )
		{
		 First = FALSE;
		 JoinLists( RBox->IN, Box, IN );
		}
	       } // for


	       First=TRUE; // now it means the first inside interval


	       if( Node->Data.Repeater.To==INFINITY )
	       {
		// create one node and make loop
		TreeToDynaAutoLevel( Node->Data.Repeater.Son, Box ); // create what will be repated

		// make loop
		JoinLists( Box->OK        , Box,   	 IN  );
		JoinLists( Box->NOK       , RBox,   	 OK  );

		if( OldBox ) // join with previously created boxes
		{
		 JoinLists( OldBox->OK      , Box,   	 IN  );
		}
		else // OldBox==NULL => {0,INF}
		{
		 JoinLists( RBox->IN        , Box,   	 IN  );
		}

		Box=RBox;
		return HI_OK;
	       }
	       else // not a loop
	       {

		// create up to TO boxes
		for( ; i<=Node->Data.Repeater.To; i++ )
		{
		 TreeToDynaAutoLevel( Node->Data.Repeater.Son, Box ); // create what will be repated

		 if( OldBox )
		 {
		  JoinLists( OldBox->OK        , Box,   	 IN  );

		  if( First )
		   First=FALSE;
		  else
		   JoinLists( OldBox->NOK       , RBox,  	 OK  );
		 }
		 else // OldBox==NULL
		 {
		  // oldbox==NULL => {0,X}
		  JoinLists( RBox->IN        , Box,   	 IN  );
		  // fail must point to repeaters OK
		  JoinLists( Box->NOK        , RBox,   	 OK  );
		 }

		 OldBox=Box;
		} // for

		// now join last box with repeater box
		if( OldBox )
		{
		 JoinLists( OldBox->OK      , RBox,   	 OK  );
		 // if OldBox->NOK is empty => it means END
		 if( First )
		  First=FALSE;
		 else
		  JoinLists( OldBox->NOK    , RBox,  	 OK  );

		 // if {0,X} all NOKs are OK
		 if( Node->Data.Repeater.From == 0 )
		  JoinLists( OldBox->NOK    , RBox,   	 OK  );
		}

		// if there is more than TO repeatings, the
		// rest of automat "to zamitne"

		Box=RBox;
		return HI_OK;
	       } // else not a loop
 }

 return HI_OK;
}

//-----------------------------------------------------------------------------

struct ResultBagItem : public ListItem
{
 DynaAutoItem far *AutoItem;	// pointer to something such as switch
 word		  TypeFlag;
 word		  Flag;
};

//-----------------------------------------------------------------------------

word DynamicAutomat::DiveForSwitch(
				    BoxListItem  far *BLItem,
				    word         &Flag,
				    word         &TypeFlag,
				    List         far *Bag
				  )
{
 ResultBagItem far *BagItem;
 BoxListItem   far *BoxItem;



 while( TRUE )
 {
  if( BLItem==NULL )	// list is empty => there is any way to continue
  {
   #ifdef DEBUG
    printf("\n IN list empty, result stored and returning ( top )...");
   #endif

   BagItem           =new ResultBagItem; if(!BagItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
    BagItem->AutoItem= NULL;
    BagItem->TypeFlag= TypeFlag;
    BagItem->Flag    = HI_OK;
   Bag->Insert( BagItem );

   return HI_OK;
  } // if

  switch( BLItem->List )
  {
   case IN:
	   BoxItem=(BoxListItem far *)((RegularBox far *)BLItem->Target)->IN->GetHead();

	   if( BoxItem==NULL ) // list empty => there is any way to continue
	   {
	    BagItem           =new ResultBagItem; if(!BagItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	     BagItem->AutoItem= NULL;
	     BagItem->TypeFlag= TypeFlag;
	     BagItem->Flag    = HI_OK;	// OK/NOK podle type flagu
	    Bag->Insert( BagItem );

	    #ifdef DEBUG
	     printf("\n IN list empty, result stored and returning...");
	    #endif

	    return HI_OK;
	   }
	   else          // work in list
	   {
	    #ifdef DEBUG
	     printf("\n Diving from IN list...");
	    #endif

	    while( BoxItem )
	    {
	     DiveForSwitch( BoxItem, Flag, TypeFlag, Bag ); // continue in diving

	     BoxItem=(BoxListItem far *)((RegularBox far *)BLItem->Target)->IN->Next(BoxItem);
	    }
	   }
	   return HI_OK;

   case NOT_A_LIST: // points to state of automat item => YEA

	   // deepest point of recursion => store result to the bag
	   BagItem           =new ResultBagItem; if(!BagItem) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	    BagItem->AutoItem= (DynaAutoItem far *)BLItem->Target; // pointer to automat state
	    BagItem->TypeFlag= TypeFlag;
	    BagItem->Flag    = HI_OK;
	   Bag->Insert( BagItem );

	   #ifdef DEBUG
	    printf("\n Pointing to automat item, result stored as nr. %u and returning...", Bag->NrOfItems );
	   #endif

	   return HI_OK;

   case OK:
	   #ifdef DEBUG
	    printf("\n TypeFlag set to OK and diving...");
	   #endif

	   TypeFlag=OK;

	   BLItem=(BoxListItem far *)((RegularBox far *)BLItem->Target)->OK->GetHead();
	   break;

   case NOK:
	   #ifdef DEBUG
	    printf("\n TypeFlag set to NOK and diving...");
	   #endif

	   TypeFlag=NOK;

	   BLItem=(BoxListItem far *)((RegularBox far *)BLItem->Target)->NOK->GetHead();
	   break;

   default:
	   printf("\n ERROR reported from %s line %d:"
		  "\n  Diving to unknown list ( not IN, OK, NOK, NOT_A_LIST )...", __FILE__, __LINE__ );
	   EXIT_PROGRAM;

  } // switch
 } // loop
}

//-----------------------------------------------------------------------------

void DynamicAutomat::FinishDiveForOK( BoxListItem far *BLItem, List far *Bag )
{
 ResultBagItem  far *BagItem;



 // take each node of Bag
 BagItem=(ResultBagItem  far *)(Bag->GetHead());

 while( BagItem )
 {

  if( BagItem->Flag==HI_ACCEPT
       ||
      (BagItem->AutoItem==NULL && BagItem->Flag==HI_OK)
    )
  {

   #ifdef DEBUG
    printf("\n	->");
   #endif

   // set/mute OKs in jmp table
   switch( BagItem->TypeFlag )
   {
    case NOK:
	    #ifdef DEBUG
	     printf(" mute OKs to NOKs and set END in jmp table");
	    #endif
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInOKsAndMuteToNOKs( INFINITY );
	    break;
    case OK:
	    #ifdef DEBUG
	     printf(" set OKs jmps in jmp table to ACCEPT");
	    #endif

	    // set OK jmps in jmp table to ACCEPT ( does by infinity )
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInOKs( INFINITY, TempID );
	    break;
    default:

	    // typeflag is initialized to IN, valid values of
	    // typeflag is OK or NOK, IN means that list is empty
	    // and because I started from OK list it is accept

	    #ifdef DEBUG
	     printf(" POINTS TO IN -> set OKs in jmp table to ACCEPT");
	    #endif

	    // set OK jmps in jmp table to ACCEPT ( does by infinity )
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInOKs( INFINITY, TempID );
	    break;
   } // switch
  } // if
  else
  {

   #ifdef DEBUG
    printf("\n	-> OK points to %Fp ID %u", BagItem->AutoItem, BagItem->AutoItem->ID );
   #endif

   // set OKs in jmp table
   ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInOKs( BagItem->AutoItem->ID );

  } // else

  BagItem=(ResultBagItem  far *)(Bag->Next(BagItem));
 } // while


/*

 // take each node of Bag
 BagItem=(ResultBagItem  far *)(Bag->GetHead());

 while( BagItem )
 {

  if( BagItem->Flag==HI_ACCEPT
       ||
      (BagItem->AutoItem==NULL && BagItem->Flag==HI_OK)
    )
  {

   #ifdef DEBUG
    printf("\n	-> OK is ACCEPT");
   #endif

   // set ok jmps in jmp table to ACCEPT ( do it by infinity )
   ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInOKs( INFINITY, TempID );
  }
  else
  {
   #ifdef DEBUG
    printf("\n	-> OK points to %Fp ID %u", BagItem->AutoItem, BagItem->AutoItem->ID );
   #endif

   // set OKs in jmp table
   ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInOKs( BagItem->AutoItem->ID );
  }

  BagItem=(ResultBagItem  far *)(Bag->Next(BagItem));
 } // while
*/

}

//-----------------------------------------------------------------------------

void DynamicAutomat::FinishDiveForNOK( BoxListItem far *BLItem, List far *Bag )
{
 ResultBagItem  far *BagItem;



 // take each node of Bag
 BagItem=(ResultBagItem  far *)(Bag->GetHead());

 while( BagItem )
 {

  if( BagItem->Flag==HI_ACCEPT
       ||
      (BagItem->AutoItem==NULL && BagItem->Flag==HI_OK)
    )
  {
   #ifdef DEBUG
    printf("\n	->");
   #endif

   // set/mute NOKs in jmp table
   switch( BagItem->TypeFlag )
   {
    case OK:
	    #ifdef DEBUG
	     printf(" mute NOKs to OKs and set ACCEPT in jmp table");
	    #endif
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInNOKsAndMuteToOKs( INFINITY, TempID );
	    break;
    case NOK:
	    #ifdef DEBUG
	     printf(" set NOKs jmps in jmp table to END");
	    #endif

	    // set NOK jmps in jmp table to END ( does by infinity )
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInNOKs( INFINITY );
	    break;
    default:

	    // typeflag is initialized to IN, valid values of
	    // typeflag is OK or NOK, IN means that list is empty
	    // and because I started from NOK list its end

	    #ifdef DEBUG
	     printf(" POINTS TO IN -> set NOKs in jmp table to END");
	    #endif

	    // set NOK jmps in jmp table to END ( does by infinity )
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInNOKs( INFINITY );
	    break;
   } // switch
  } // if
  else
  {
   #ifdef DEBUG
    printf("\n	-> NOK points to %Fp ID %u", BagItem->AutoItem, BagItem->AutoItem->ID );
   #endif

   // set/mute NOKs in jmp table
   switch( BagItem->TypeFlag )
   {
    case OK:
	    #ifdef DEBUG
	     printf(" SET JMP && MUTE (OK)");
	    #endif
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInNOKsAndMuteToOKs( BagItem->AutoItem->ID );
	    break;
    case NOK:
	    #ifdef DEBUG
	     printf(" SET JMP (NOK)");
	    #endif
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInNOKs( BagItem->AutoItem->ID );
	    break;
    default:
	    #ifdef DEBUG
	     printf(" SET JMP && MUTE (IN)");
	    #endif
	    ((DynaAutoItem far *)(BLItem->Target))->JmpTable->SetJmpInNOKsAndMuteToOKs( BagItem->AutoItem->ID );
	    break;
   } // switch
  } // else

  BagItem=(ResultBagItem  far *)(Bag->Next(BagItem));
 } // while
}

//-----------------------------------------------------------------------------

word DynamicAutomat::ProcessSwitch( RegularBox far *SwitchBox )
// - TypeFlag contains OK or NOK. Is set by last list I go through
// - flag contains HI_OK ... normal return
//                 HI_ACCEPT ... nejvnejsnejsi box && a jsem v listu OK
{
 BoxListItem  far *BLItem,
	      far *BoxItem;
 List         far *Bag=new List; if(!Bag) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 word 	          Flag,
		  TypeFlag;	// type of list OK/NOK i go through last ( IN list ignored )



 if( SwitchBox==NULL )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Input box is null...", __FILE__, __LINE__ );
  delete Bag;
  EXIT_PROGRAM
 }
 else
  BLItem = (BoxListItem far *)(SwitchBox->IN->GetHead());



 // check if it is really switch
 if( BLItem->List != NOT_A_LIST )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Target is not auto item...", __FILE__, __LINE__ );
  delete Bag;
  EXIT_PROGRAM
 }
 #ifdef DEBUG
 else
  printf("\n Processing switch with ID %u:", ((DynaAutoItem far *)(BLItem->Target))->ID );
 #endif


 // --- Dive from OK list ---
 #ifdef DEBUG
  printf("\n OK LIST will be done...");
 #endif

 #define MAKE_OK_LIST			\
  TypeFlag=IN;                          \
					\
  DiveForSwitch( BoxItem,               \
		 Flag,                  \
		 TypeFlag,              \
		 Bag                    \
	       );                       \
					\
  FinishDiveForOK( BLItem, Bag );

 if( (BoxItem = (BoxListItem far *)(SwitchBox->OK->GetHead())) == NULL )
 {
  MAKE_OK_LIST
 }
 else
 {
  while( BoxItem )
  {
   MAKE_OK_LIST

   Bag->Destroy();

   BoxItem = (BoxListItem far *)(SwitchBox->OK->Next(BoxItem));
  }
 }



 // --- Dive from NOK list ---

 #ifdef DEBUG
  printf("\n NOK LIST will be done...");
 #endif

 // typeflag is initialized to IN, valid values of
 // typeflag is OK or NOK, IN means that list is empty ( was set nowhere )

 #define MAKE_NOK_LIST			\
  TypeFlag=IN;                          \
					\
  DiveForSwitch( BoxItem,               \
		 Flag,                  \
		 TypeFlag,              \
		 Bag                    \
	       );                       \
					\
  FinishDiveForNOK( BLItem, Bag );

 if( (BoxItem = (BoxListItem far *)(SwitchBox->NOK->GetHead())) == NULL )
 {
  MAKE_NOK_LIST
 }
 else
 {
  while( BoxItem )
  {
   MAKE_NOK_LIST

   BoxItem = (BoxListItem far *)(SwitchBox->NOK->Next(BoxItem));
  }
 }

 delete Bag;

 return HI_OK;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::FillJmpTables( RegularBox far *Box )
// Rekurzivni pruchod, cil je propojit vystup ze switche s dalsim switchem,
// tj. dat tam adresu stavu na ktery ukazuje.
//	Vezmu tedy stav, najdu kam ukazuje a tak dale...
// Jakmile, zjistim kam ukazuje, uz tam dam primo cislo => tim se budou rusit
// cykly
{
 List far *Bag		=new List; if(!Bag) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 word 	  TypeFlag      =IN,
	  Flag          =HI_OK;

 #ifdef DEBUG
  printf("\n\n FIND INPUT STATES OF AUTOMAT:");
 #endif

 // --- Find input state of automat ---
 ResultBagItem 		far *BagItem;
 BoxListItem   		far *BoxItem;
 InputStatesItem 	far *Input;

 BoxItem=(BoxListItem far *)(Box->IN->GetHead());

 while( BoxItem )
 {

  DiveForSwitch( BoxItem,
		 Flag,
		 TypeFlag,
		 Bag
		);

  // work on bag
  BagItem = (ResultBagItem far *)Bag->GetHead();

  while( BagItem )
  {
   if( BagItem->AutoItem == NULL )
   {
    #ifdef DEBUG
     printf("\n  No switch on the way - GREEN ARROW WORK" );
    #endif


   }
   else
   {
    // add it to List of input states
    Input  =  new InputStatesItem; if(!Input) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
     Input->AutoItem		=BagItem->AutoItem;
     InputStates->Insert( Input );

    #ifdef DEBUG
     printf("\n  Input state of automat %Fp, ID is %u ", BagItem->AutoItem, BagItem->AutoItem->ID );
    #endif
   }

   BagItem=(ResultBagItem far *)Bag->Next(BagItem);
  }

  Bag->Destroy();

  BoxItem=(BoxListItem far *)(Box->IN->Next(BoxItem));
 }

 delete Bag;

 // --- Now go through registry and fill each Jmp table ---

 #ifdef DEBUG
  printf("\n\n FILL JMP TABLES:");
 #endif

 ListItem far *Item;

 Item=Registry->GetHead();

 while( Item )
 {
  #ifdef DEBUG
   printf("\n\n Processing Box 0x%Fp type %d", ((RegistryItem far *)Item)->Box, ((RegistryItem far *)Item)->Box->Type );
  #endif

   switch( ((RegistryItem far *)Item)->Box->Type )
   {
    case SWITCH:
	    #ifdef DEBUG
	     printf(" -> it's SWITCH");
	    #endif

	    ProcessSwitch( ((RegistryItem far *)Item)->Box );

	    break;
    default:
	    #ifdef DEBUG
	     printf(" -> it's not interesting");
	    #endif
	    ;
   }

  Item=Registry->Next(Item);
 }

 return HI_OK;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::Wampirator( DynamicAutomat &Blood )
// - vysaje jiny automat
{
 ListItem far *ItHead,
	  far *ItTail,
	  far *InHead,
	  far *InTail;
 ListItem far *Item;
 word     NrOfItems,
	  ItItems,
	  InItems;


 // move items to me and shift jmp tables
 Blood.Cut( ItHead, ItTail, ItItems, InHead, InTail, InItems );
 NrOfItems=GetNrOfItems();



 // --- states of automat ---
 Item=ItHead;

 while( Item )
 {
  ((DynaAutoItem far *)Item)->JmpTable->Shift( NrOfItems );
  ((DynaAutoItem far *)Item)->ID += NrOfItems;

  Item=Item->Next;
 }

 Items->Paste( ItHead, ItTail, ItItems );



 // --- move input states to me ---
 InputStates->Paste( InHead, InTail, InItems );
 // += not needed because from input list I points directly to Items list
 // where IDs were already changed

 // --- change last ID ---
 LastID+=Blood.GetNrOfItems();

 return HI_OK;
}

//-----------------------------------------------------------------------------

word DynamicAutomat::Cut( ListItem far *&ItHead, ListItem far *&ItTail, word &ItItems,
			  ListItem far *&InHead, ListItem far *&InTail, word &InItems )
{
 ItHead=Items->GetHead();
 ItTail=Items->GetTail();
 ItItems=Items->NrOfItems;
 Items->LeaveContent();

 InHead=InputStates->GetHead();
 InTail=InputStates->GetTail();
 InItems=InputStates->NrOfItems;
 InputStates->LeaveContent();

 // make pointers OK ( used in Wampirator )
 ItHead->Last=ItTail->Next=InHead->Last=InTail->Next = NULL;

 return HI_OK;
}

//-----------------------------------------------------------------------------

void DynamicAutomat::Print( void )
{
 ListItem far *Item;

 #ifdef DEBUG
  printf("\n\n - DYNA PRINT ------------------------------------------------------------"
	 "\n DynamicAutomat profile:"
	 "\n  NrOfStates:      %u"
	 "\n  NrOfInputStates: %u"
	 "\n  ListOfInputStates:"
	 ,
	 LastID,
	 InputStates->NrOfItems
	);
 #endif

 // InputStates
 Item=InputStates->GetHead();

 while( Item )
 {
  #ifdef DEBUG
   printf(" %u", ((InputStatesItem far *)Item)->AutoItem->ID );
  #endif

  Item=InputStates->Next(Item);
 }

 // States
 Item=Items->GetHead();

 while( Item )
 {
  #ifdef DEBUG
   printf("\n State %u:", ((DynaAutoItem far *)Item)->ID );
   ((DynaAutoItem far *)Item)->JmpTable->Print();
  #endif
  Item=Items->Next(Item);
 }

 #ifdef DEBUG
  printf("\n - DYNA PRINT ------------------------------------------------------------\n");
 #endif
}

//----------------------------------------------------------------------------------

#define ERROR_WHEN_WRITING \
 {                         \
  printf("\n ERROR reported from %s line %d:"					  \
	 "\n  Error writing to the file \"%s\"...", __FILE__, __LINE__, FileName );\
  EXIT_PROGRAM								\
 }

word DynamicAutomat::Save( char *FileName )
{
 // before save OPTIMIZE AUTOMAT
 #ifdef OPTIMIZE
   ReduceNrOfInputStates();
 #endif



 #ifdef DEBUG
  printf("\n\n Saving automat %Fp to file %s...", this, FileName );
 #endif

 ListItem far *Item;
 int  Handle;

 if((Handle = open( FileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
		     S_IREAD | S_IWRITE)) == -1)
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Error opening file \"%s\"...", __FILE__, __LINE__, FileName );
  EXIT_PROGRAM
 }


 // NrOfStates
 if( write( Handle, &LastID, 2u ) != 2u ) ERROR_WHEN_WRITING
 // NrOfStates of input states
 if( write( Handle, &(InputStates->NrOfItems), 2u ) != 2u ) ERROR_WHEN_WRITING
 // input states self
 Item=InputStates->GetHead();
 while( Item )
 {
  if( write( Handle, &(((InputStatesItem far *)Item)->AutoItem->ID), 2u ) != 2u ) ERROR_WHEN_WRITING
  Item=InputStates->Next(Item);
 }
 // states self
 Item=Items->GetHead();
 while( Item )
 {
  // save jmp table
  ((DynaAutoItem far *)Item)->JmpTable->Save(Handle, ((DynaAutoItem far *)Item)->ID );
  Item=Items->Next(Item);
 }

 close( Handle );

 return HI_OK;
}

//----------------------------------------------------------------------------------

#define ERROR_WHEN_APPENDING \
 {                           \
  printf("\n ERROR reported from %s line %d:"					  \
	 "\n  Error writing to the file with handle %d...", __FILE__, __LINE__, Handle );\
  EXIT_PROGRAM								\
 }

word DynamicAutomat::Append( int Handle )
{
 // before save OPTIMIZE AUTOMAT
 #ifdef OPTIMIZE
  #ifdef PRINT_OPTIMIZE
  if( Verbose ) printf("(%u",InputStates->NrOfItems);
  #endif
   ReduceNrOfInputStates();
  #ifdef PRINT_OPTIMIZE
  if( Verbose ) printf("/%u)",InputStates->NrOfItems);
  #endif
 #endif



 #ifdef DEBUG
  printf("\n\n Appending automat %Fp to file with handle %d...", this, Handle );
 #endif

 ListItem far *Item;

 // NrOfStates
 if( write( Handle, &LastID, 2u ) != 2u ) ERROR_WHEN_APPENDING
 // NrOfStates of input states
 if( write( Handle, &(InputStates->NrOfItems), 2u ) != 2u ) ERROR_WHEN_APPENDING
 // input states self
 Item=InputStates->GetHead();
 while( Item )
 {
  if( write( Handle, &(((InputStatesItem far *)Item)->AutoItem->ID), 2u ) != 2u ) ERROR_WHEN_APPENDING
  Item=InputStates->Next(Item);
 }
 // states self
 Item=Items->GetHead();
 while( Item )
 {
  // save jmp table
  ((DynaAutoItem far *)Item)->JmpTable->Save(Handle, ((DynaAutoItem far *)Item)->ID );
  Item=Items->Next(Item);
 }

 return HI_OK;
}

//----------------------------------------------------------------------------------
word DynamicAutomat::RdcAreSame( DynaAutoItem far *State1,
				 DynaAutoItem far *State2
			       )
// each jmp table:

// -if has 2 items
//  the first item must have only these flags: O
//  the second item must have only these flags: NERS
//  both tables must have the same from-to in the first item
//  => returns 1
//
// -if has 3 items
//  the first item must have only these flags: O
//  the second item must have only these flags: O
//  the third item must have only these flags: NERS
//  both tables must have the same from-to in the first and the second item
//  => returns 2
//
// => if tables not same returns 0
{
 DynaJmpTableItem far *ItemsState1,
		  far *ItemsState2;


 // --- CASE 1 ---

 if( State1->JmpTable->Items->NrOfItems == 2
      &&
     State2->JmpTable->Items->NrOfItems == 2
   )
 {
  byte Item1Flags = JMP_OK,
       Item2Flags = JMP_NOK|JMP_END|JMP_REST|JMP_STOP;


  ItemsState1 = (DynaJmpTableItem far *)(State1->JmpTable->Items->GetHead());
  ItemsState2 = (DynaJmpTableItem far *)(State2->JmpTable->Items->GetHead());

  if( ItemsState1->Flags == Item1Flags
       &&
      ItemsState2->Flags == Item1Flags
       &&
      ItemsState1->From  == ItemsState2->From
       &&
      ItemsState1->To    == ItemsState2->To
    )
  {

   ItemsState1 = (DynaJmpTableItem far *)State1->JmpTable->Items->Next(ItemsState1);
   ItemsState2 = (DynaJmpTableItem far *)State2->JmpTable->Items->Next(ItemsState2);

   if( ItemsState1->Flags == Item2Flags
	&&
       ItemsState2->Flags == Item2Flags
     )
   {
    #ifdef DEBUG
     printf("\n RdcAreSame: states %u and %u are same - case 1...", State1->ID, State2->ID );
    #endif

    return TRUE;
   }
  }
 }

 // --- CASE 2 ---

 if( State1->JmpTable->Items->NrOfItems == 3
      &&
     State2->JmpTable->Items->NrOfItems == 3
   )
 {
  byte Item1Flags = JMP_OK,
       Item2Flags = JMP_OK,
       Item3Flags = JMP_NOK|JMP_END|JMP_REST|JMP_STOP;


  ItemsState1 = (DynaJmpTableItem far *)(State1->JmpTable->Items->GetHead());
  ItemsState2 = (DynaJmpTableItem far *)(State2->JmpTable->Items->GetHead());

  if( ItemsState1->Flags == Item1Flags
       &&
      ItemsState2->Flags == Item1Flags
       &&
      ItemsState1->From  == ItemsState2->From
       &&
      ItemsState1->To    == ItemsState2->To
    )
  {

   ItemsState1 = (DynaJmpTableItem far *)State1->JmpTable->Items->Next(ItemsState1);
   ItemsState2 = (DynaJmpTableItem far *)State2->JmpTable->Items->Next(ItemsState2);

   if( ItemsState1->Flags == Item2Flags
	&&
       ItemsState2->Flags == Item2Flags
     )
   {

    ItemsState1 = (DynaJmpTableItem far *)State1->JmpTable->Items->Next(ItemsState1);
    ItemsState2 = (DynaJmpTableItem far *)State2->JmpTable->Items->Next(ItemsState2);

    if( ItemsState1->Flags == Item3Flags
	 &&
	ItemsState2->Flags == Item3Flags
      )
    {
     #ifdef DEBUG
      printf("\n RdcAreSame: states %u and %u are same - case 2...", State1->ID, State2->ID );
     #endif

     return TRUE+TRUE;
    }
   }
  }
 }

 return FALSE;
}

//----------------------------------------------------------------------------------

void DynamicAutomat::ReduceNrOfInputStates( void )
// optimizing automat:
// reducing number of input states
//
// STEP1
// joins these tables:
//	JMP_OK,
//	JMP_NOK|JMP_END|JMP_REST|JMP_NOT_SET|JMP_STOP;
{
 #ifdef DEBUG
  printf("\n\n - AUTO OPTIMIZER BEFORE ----------------------------------------------------");
  Print();
 #endif

 // InputStates
 InputStatesItem  far *InputState=(InputStatesItem far *)InputStates->GetHead();
 InputStatesItem  far *State,
		  far *StateBackup;
 DynaJmpTableItem far *TableItem;
 List 		  far *Brothers=new List; if(!Brothers) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }


 while( InputState )
 {
  #ifdef DEBUG
   printf("\n\n Searching brother for ID %u...", InputState->AutoItem->ID );
  #endif



  // search brothers ONLY between input states
  State=(InputStatesItem far *)InputStates->GetHead();;

  while( State )
  {

   if( InputState != State )
   {
    if( RdcAreSame( InputState->AutoItem, State->AutoItem ) )
    {
     // move state from input states list to brothers list
     StateBackup=State;
     State      =(InputStatesItem far *)InputStates->Next(State);

     InputStates->Get( StateBackup ); // taken out of list but not deleted
     #ifdef DEBUG
      printf("\n RdcInputStates: input state %u removed...", StateBackup->AutoItem->ID );
     #endif

     // move state to brother list
     Brothers->Insert( StateBackup );

     continue;
    }
   }

   State=(InputStatesItem far *)InputStates->Next(State);
  }

  // if there are some brothers
  if( !(Brothers->Empty()) )
  {
   // join brothers to the first jmp table, delete not used jmp table
   // and put there ghost jmp table

   State=(InputStatesItem far *)Brothers->GetHead();;

   while( State )
   {
    // take item
    TableItem = (DynaJmpTableItem far *)State->AutoItem->JmpTable->Items->GetHead();
    // move table item to the new place ( the rest of table leave as is )
    State->AutoItem->JmpTable->Items->Get(TableItem);
    // insert table item to the new position
    InputState->AutoItem->JmpTable->Items->InsertAfterHead( TableItem );

    // if same == TRUE+TRUE do the same thing again ( 2 because I get 1 item in previous lines )
    if( State->AutoItem->JmpTable->Items->NrOfItems == 2 )
    {
     // take item
     TableItem = (DynaJmpTableItem far *)State->AutoItem->JmpTable->Items->GetHead();
     // move table item to the new place ( the rest of table leave as is )
     State->AutoItem->JmpTable->Items->Get(TableItem);
     // insert table item to the new position
     InputState->AutoItem->JmpTable->Items->InsertAfterHead( TableItem );
    }

    // take the next state
    State=(InputStatesItem far *)Brothers->Next(State);;
   }

   // make brothers list empty
   Brothers->Destroy();

  }

  // work on the next input state
  InputState=(InputStatesItem far *)InputStates->Next(InputState);
 }

 #ifdef DEBUG
  printf("\n\n - AUTO OPTIMIZER AFTER ----------------------------------------------------");
  Print();
 #endif

 delete Brothers;
}

//- EOF ----------------------------------------------------------------------------

