#include "source\static.h"

// print info when hunting bugs
// #define DEBUG

// #define ACCEPTED_TO_FILE

//- class StatJmpTable methods ---------------------------------------------------------------------------------

StatJmpTable::StatJmpTable( int Handle )
{

 // items in table
 if( read( Handle, &NrOfItems, 2u ) != 2u )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Error reading from file...", __FILE__, __LINE__  );
  exit(0);
 }

 // load jmp table
 Items=(StatJmpTableItem far *)new (StatJmpTableItem[NrOfItems]); if(!Items) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 if( read( Handle, Items, NrOfItems*sizeof(StatJmpTableItem) ) != (NrOfItems*sizeof(StatJmpTableItem)) )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Error reading from file...", __FILE__, __LINE__  );
  exit(0);
 }

}

//----------------------------------------------------------------------------------

StatJmpTable::~StatJmpTable( void )
{
 delete Items;
}

//----------------------------------------------------------------------------------

void StatJmpTable::Print( void )
{
 word i;

 for( i=0; i<NrOfItems; i++ )
 {
  #ifdef DEBUG
   printf("\n  JmpTableItem %Fp: from \'%c\' to \'%c\' jmp %u flags ", this, Items[i].From, Items[i].To, Items[i].JmpTo );

   // now print flags
   if( Items[i].Flags&JMP_OK )     	printf("O");
   if( Items[i].Flags&JMP_NOK )    	printf("N");
   if( Items[i].Flags&JMP_ACCEPT ) 	printf("A");
   if( Items[i].Flags&JMP_END )    	printf("E");
   if( Items[i].Flags&JMP_REST )   	printf("R");
   if( Items[i].Flags&JMP_NOT_SET )     printf("-");
   if( Items[i].Flags&JMP_ELEMENT_ID )  printf("I");
   if( Items[i].Flags&JMP_STOP ) 	printf("S");
  #endif
  ;
 }
}

//- struct AcceptListItem methods ---------------------------------------------------------------------------------

void AcceptItem::Print( void )
{
 #ifdef DEBUG
  printf("\n AcceptedItem: to %lu ID %u", To, ID );
 #endif
}

//- class StaticAutomat methods ---------------------------------------------------------------------------------

StaticAutomat::StaticAutomat( char *FileName )
{
 word 	j;
 Robot	far *Robotek;

 Load( FileName );

 // alocate lists for Accept
 Queue              =new List; if(!Queue) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 NextStep           =new List; if(!NextStep) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 StopNextStep       =new List; if(!StopNextStep) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 InputStatesPool    =new List; if(!InputStatesPool) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // create InputStatesPool for Accept
 for( j=0; j<NrOfInputItems; j++ )
 {
  Robotek=new Robot( InputStates[j] ); if(!Robotek) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
  InputStatesPool->Insert( Robotek );
 }
}

//----------------------------------------------------------------------------------

StaticAutomat::StaticAutomat( int Handle )
{
 word 	j;
 Robot	far *Robotek;

 Load( Handle );

 // alocate lists for Accept
 Queue              =new List; if(!Queue) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 NextStep           =new List; if(!NextStep) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 StopNextStep       =new List; if(!StopNextStep) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 InputStatesPool    =new List; if(!InputStatesPool) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // create InputStatesPool for Accept
 for( j=0; j<NrOfInputItems; j++ )
 {
  Robotek=new Robot( InputStates[j] ); if(!Robotek) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
  InputStatesPool->Insert( Robotek );
 }
}

//-----------------------------------------------------------------------------

StaticAutomat::~StaticAutomat()
{
 word i;

 for( i=0; i<NrOfItems; i++ ) delete Engine[i]; // free jump tables

 farfree( Engine );

 delete InputStates;

 // delete accept lists
 delete Queue;
 delete NextStep;
 delete StopNextStep;

 // delete pool
 delete InputStatesPool;
}


//----------------------------------------------------------------------------------

#define ERROR_WHEN_READING \
 {                         \
  printf("\n ERROR reported from %s line %d:"                                     \
	 "\n  Error reading from the file with handle %u...", __FILE__, __LINE__, Handle );\
  return HI_ERROR;                                                                \
 }

word StaticAutomat::Load( char *FileName )
{
 #ifdef DEBUG
  printf("\n\n Loading static automat %Fp from file %s...", this, FileName );
 #endif

 int      Handle;
 word     i;
 StatJmpTable far *JumpTable;



 if((Handle = open( FileName, O_RDONLY | O_BINARY, S_IWRITE | S_IREAD)) == -1)
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Error opening file \"%s\"...", __FILE__, __LINE__, FileName );
  return HI_ERROR;
 }


 // NrOfStates
 if( read( Handle, &NrOfItems, 2u ) != 2u ) ERROR_WHEN_READING

 // alloc an array
 Engine=(StatJmpTable far **)farmalloc(sizeof(StatJmpTable far **)*NrOfItems); if(!Engine) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // NrOfInputStates
 if( read( Handle, &NrOfInputItems, 2u ) != 2u ) ERROR_WHEN_READING

 // alloc an array
 InputStates=(AutoItemID far *)new ( AutoItemID [NrOfInputItems]); if(!InputStates) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // input states self
 if( read( Handle, InputStates, NrOfInputItems*sizeof(word) ) != (NrOfInputItems*sizeof(word)) ) ERROR_WHEN_READING

 // states self
 for( i=0; i<NrOfItems; )
 {
  JumpTable = new StatJmpTable( Handle ); // load jmp table
	      if(!JumpTable) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  Engine[i]=JumpTable;

  #ifdef DEBUG
   Engine[i]->Print();
  #endif

  i++;

 }

 close( Handle );

 return HI_OK;
}

//-----------------------------------------------------------------------------

word StaticAutomat::Load( int Handle )
{
 #ifdef DEBUG
  printf("\n\n Loading static automat %Fp from file %u...", this, Handle );
 #endif

 word     i;
 StatJmpTable far *JumpTable;

 // NrOfStates
 if( read( Handle, &NrOfItems, 2u ) != 2u ) ERROR_WHEN_READING

 // alloc an array
 Engine=(StatJmpTable far **)farmalloc(sizeof(StatJmpTable far **)*NrOfItems); if(!Engine) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // NrOfInputStates
 if( read( Handle, &NrOfInputItems, 2u ) != 2u ) ERROR_WHEN_READING

 // alloc an array
 InputStates=(AutoItemID far *)new ( AutoItemID [NrOfInputItems]); if(!InputStates) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 // input states self
 if( read( Handle, InputStates, NrOfInputItems*sizeof(word) ) != (NrOfInputItems*sizeof(word)) ) ERROR_WHEN_READING

 // states self
 for( i=0; i<NrOfItems; )
 {
  JumpTable = new StatJmpTable( Handle ); // load jmp table
	      if(!JumpTable) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

  Engine[i]=JumpTable;

  #ifdef DEBUG
   Engine[i]->Print();
  #endif

  i++;

 }

 return HI_OK;
}

//-----------------------------------------------------------------------------

bool StaticAutomat::Accept( Buffer far  	*Buf,
			    dword       	BegOff,
			    dword       	&EndOff,
			    LexicatorType   	&EleID
			  )
{
 StatJmpTableItem 	far *EngineIDItems;
 List           	far *Help;
 Robot          	far *Robotek;
 ListItem       	far *Item;
 dword			HelpTo,
			GetCharSucc;
 word           	j;
 AutoItemID     	ID,
			HelpID;
 char           	C;
 bool           	FromToFit,
			StopInput,
			FirstCycle;

 AcceptItem     	AcceptedItem;



 StopInput       =FALSE;

 // init it to recognize that nothing was accepted
 AcceptedItem.To = BegOff; // not exist smallest token
 AcceptedItem.ID = INFINITY;

 // in first cycle is as input used InputStatePool
 // -> cannot be deleted so detect...
 FirstCycle      =TRUE;

 // lists are empty -> so no reason to make them empty
 #ifdef DEBUG
  printf("\n %u %u %u", Queue->NrOfItems, NextStep->NrOfItems, StopNextStep->NrOfItems );
 #endif

 if( !Buf->Seek( BegOff ) )
 {
  #ifdef DEBUG
   printf("\n Buffer seek maked...");
  #endif

  return FALSE;
 }

 // init Queue with InputStatesPool
 Help  = Queue;
 Queue = InputStatesPool;



 // make one step
 while( Queue->NrOfItems )
 {

 // take character from input if everybody wants it
 if( StopInput )
 {
  StopInput=FALSE;
 }
 else
 {
  GetCharSucc=Buf->GetChar( C );	// contains position moved see prec implementation

  if( !GetCharSucc )
  {
   #ifdef DEBUG
    printf("\n Buffer GetChar maked -> end of input...");
   #endif

   // Here is the end of file. For some elements is some whitespace
   // required to finish accepting -> simulate this whitespace after eof...
   // In GetCharSucc I have flag that I wasn't successful.

   C='\n';	// simulate whitespace, new line is the best

  }
 }

 // go through robot queue
 Item=Queue->Head->Next;	// direct access for speedup

 while( Item != Queue->Tail )	// for each robot
 {

  ID=((Robot far *)Item)->ID;

  FromToFit=FALSE;

  EngineIDItems=Engine[ID]->Items;	// for speedup

  for( j=0; j<Engine[ID]->NrOfItems; j++ )	// go through table
  {

   if(
       C>=EngineIDItems[j].From
	&&
       C<=EngineIDItems[j].To
	&&
       !(EngineIDItems[j].Flags&JMP_REST)
     )
   {

    FromToFit=TRUE;     // I am inside through from-to, rest cannot be used

    // ACCEPTING ITEM
    if( EngineIDItems[j].Flags&JMP_ACCEPT )
    {

     if( EngineIDItems[j].Flags&JMP_STOP ) // if stop last char was not accepted -> dec to in this case
      HelpTo=Buf->GetPos()-GetCharSucc;
     else
      HelpTo=Buf->GetPos();

     if( HelpTo >= AcceptedItem.To ) // token longest and with smallest ID must be taken
     {
      // search for element ID ( is smaller ? ), EVERYTIME IS THE LAST ITEM IN TABLE
      HelpID = EngineIDItems[ Engine[ID]->NrOfItems-1u ].JmpTo;

      if( HelpTo == AcceptedItem.To )
      {
       if( AcceptedItem.ID > HelpID  ) // jmp to contains ELEMENT ID
	AcceptedItem.ID = HelpID;
      }
      else
       AcceptedItem.ID = HelpID;

      AcceptedItem.To = HelpTo;

      #ifdef DEBUG
       if( EngineIDItems[j].Flags&JMP_STOP )
	printf("\n        step FROM-TO accept from %lu to %lu", BegOff, AcceptedItem.To );
       else
	printf("\n \'%c\' step FROM-TO accept from %lu to %lu", C, BegOff, AcceptedItem.To );
      #endif
     }
     #ifdef DEBUG
     else
     {
      if( EngineIDItems[j].Flags&JMP_STOP )
       printf("\n        step FROM-TO NOT accept from %lu to %lu", BegOff, HelpTo );
      else
       printf("\n \'%c\' step FROM-TO NOT accept from %lu to %lu", C, BegOff, HelpTo );
     }
     #endif

     continue; // to for
    }

    // CONTINUE THROUGH ITEM
    if(
       !(EngineIDItems[j].Flags&JMP_END)
      )
    {
     // continue through automat

     // was accepted
     #ifdef DEBUG
      printf("\n C \'%c\' FROM-TO -> to state %u, pos %lu", C, EngineIDItems[j].JmpTo, Buf->GetPos() );
      Engine[ID]->Print();
     #endif

     if( EngineIDItems[j].Flags&JMP_STOP )
     {
      // concentrate stop robots after head
      Robotek=new Robot( EngineIDItems[j].JmpTo, TRUE ); if(!Robotek) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
      StopNextStep->Insert( Robotek );
     }
     else
     {
      Robotek=new Robot( EngineIDItems[j].JmpTo ); if(!Robotek) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
      NextStep->Insert( Robotek );
     }
    }

    // ( next cases are ignored -> so killed when Queue destroyed )

   } // if
   else
   {

    if(
	FromToFit==FALSE
	&&
	EngineIDItems[j].Flags&JMP_REST
      )
    {


      // ACCEPTING ITEM
      if( EngineIDItems[j].Flags&JMP_ACCEPT )
      {

       if( EngineIDItems[j].Flags&JMP_STOP ) // if stop last char was not accepted -> dec to in this case
	HelpTo=Buf->GetPos()-GetCharSucc;
       else
	HelpTo=Buf->GetPos();

      if( HelpTo >= AcceptedItem.To ) // token longest and with smallest ID must be taken
      {
       // search for element ID ( is smaller ? ), EVERYTIME IS THE LAST ITEM IN TABLE
       HelpID = EngineIDItems[ Engine[ID]->NrOfItems-1u ].JmpTo;

       if( HelpTo == AcceptedItem.To )
       {
	if( AcceptedItem.ID > HelpID  ) // jmp to contains ELEMENT ID
	 AcceptedItem.ID = HelpID;
       }
       else
	AcceptedItem.ID = HelpID;

       AcceptedItem.To = HelpTo;

       #ifdef DEBUG
	if( EngineIDItems[j].Flags&JMP_STOP )
	 printf("\n        step REST accept from %lu to %lu", BegOff, AcceptedItem.To );
	else
	 printf("\n \'%c\' step REST accept from %lu to %lu", C, BegOff, AcceptedItem.To );
       #endif
      }
      else
      {
       #ifdef DEBUG
	if( EngineIDItems[j].Flags&JMP_STOP )
	 printf("\n        step REST NOT accept from %lu to %lu", BegOff, HelpTo );
	else
	 printf("\n \'%c\' step REST NOT accept from %lu to %lu", C, BegOff, HelpTo );
       #endif
      }

      continue; // to for
     }

     // CONTINUE THROUGH ITEM
     if(
	!(EngineIDItems[j].Flags&JMP_END)
       )
     {
      // continue through automat

      // char was accepted
      #ifdef DEBUG
       printf("\n C \'%c\' REST -> to state %u, pos %lu", C, EngineIDItems[j].JmpTo, Buf->GetPos() );
      #endif

      if( EngineIDItems[j].Flags&JMP_STOP )
      {
       Robotek=new Robot( EngineIDItems[j].JmpTo, TRUE ); if(!Robotek) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       StopNextStep->Insert( Robotek );
      }
      else
      {
       Robotek=new Robot( EngineIDItems[j].JmpTo ); if(!Robotek) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
       NextStep->Insert( Robotek );
      }
     }

     // ( next cases are ignored -> so killed when Queue destroyed )

    } // if
   } // else
  } // for

  Item = Item->Next;
 } // while Item



 if( FirstCycle )
 {
  Queue 	= Help;		// rescue InputStatesPool items
  FirstCycle 	= FALSE;
 }
 else
  Queue->Destroy();	// prepare for the next step

 // Queue is now empty

 if( !(StopNextStep->NrOfItems) ) // if( StopNextStep->Empty() )
 {
  // now Queue contains stop robots, and NextStepQueue is empty

  // Queue == StopNextStep == NULL

  Help=Queue;
  Queue=NextStep;
  NextStep=Help;

  StopInput=FALSE;
 }
 else
 {
  // continue of work on stop queue

  Help          =Queue;
  Queue         =StopNextStep;
  StopNextStep  =Help;

  // NextStep   stays the same

  StopInput=TRUE;
 }

 // if simulating whitespace on place of EOF break away...
 if( !GetCharSucc ) break;

 } // while of step



 // CHECK RESULTS



 if( AcceptedItem.ID != INFINITY )	// something was accepted
 {
  EndOff = AcceptedItem.To;
  EleID  = (LexicatorType)AcceptedItem.ID;

  #ifdef ACCEPTED_TO_FILE
   Buf->Append( BegOff, AcceptedItem.To );
  #endif

  #ifdef DEBUG
   printf("\n Accept RESULT from %lu to %lu ID %u", BegOff, AcceptedItem.To, AcceptedItem.ID );
  #endif

  return TRUE;
 }
 else
 {
  EndOff=BegOff;

  return FALSE;
 }
}

//-----------------------------------------------------------------------------

void StaticAutomat::Print( void )
{
 word i;

 #ifdef DEBUG
  printf("\n\n - STATIC PRINT ------------------------------------------------------------"
	 "\n StaticAutomat profile:"
	 "\n  NrOfStates:      %u"
         "\n  NrOfInputStates: %u"
         "\n  ListOfInputStates:"
         ,
         NrOfItems,
         NrOfInputItems
        );
 #endif

 // InputStates
 for( i=0; i<NrOfInputItems; i++ )
 {
  #ifdef DEBUG
   printf(" %u", InputStates[i] );
  #endif
  ;
 }

 // States
 for( i=0; i<NrOfItems; i++ )
 {
  #ifdef DEBUG
   printf("\n State %u:", i );
  #endif
  Engine[i]->Print();
 }

 #ifdef DEBUG
  printf("\n - STATIC PRINT ------------------------------------------------------------\n");
 #endif
}

//- EOF ----------------------------------------------------------------------------
