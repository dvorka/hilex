#include "source\hilist.h"

// print info when hunting bugs
// #define DEBUG

//- class List methods -------------------------------------------------------

List::List( void )
{
 #ifdef DEBUG
  printf("\n\n List(): %luB",(unsigned long)farcoreleft() );
 #endif

 Head=new ListItem;
 Tail=new ListItem;

 if( Head==NULL || Tail==NULL )
 {
  printf("\n Error: not enough far memory in %s line %d...", __FILE__, __LINE__ );
  exit(0);
 }

 // init
 NrOfItems=0;
 Head->Last=Tail->Next=NULL;
 Head->Next=Tail;
 Tail->Last=Head;

 #ifdef DEBUG
  printf("->%luB, this 0x%Fp head 0x%Fp tail 0x%Fp", (unsigned long)farcoreleft(), this, Head, Tail );
 #endif
}

//----------------------------------------------------------------------------

void List::InsertAfterHead( ListItem far *InsertedItem )
// Inserted item is inserted not copied!
{
 if( InsertedItem )
 {
  Head->Next->Last=InsertedItem;

  InsertedItem->Next=Head->Next;
  InsertedItem->Last=Head;

  Head->Next=InsertedItem;

  NrOfItems++;

  #ifdef DEBUG
   printf("\n  Item %Fp inserted,", InsertedItem );
  #endif
 }

 #ifdef DEBUG
  printf(" far mem: %luB...",(unsigned long)farcoreleft() );
 #endif
}

//----------------------------------------------------------------------------

void List::InsertBeforeTail( ListItem far *InsertedItem )
// Inserted item is inserted not copied!
{
 if( InsertedItem )
 {
  InsertedItem->Next=Tail;
  InsertedItem->Last=Tail->Last;

  Tail->Last->Next=InsertedItem;
  Tail->Last=InsertedItem;

  NrOfItems++;

  #ifdef DEBUG
   printf("\n  Item %Fp inserted,", InsertedItem );
  #endif
 }

 #ifdef DEBUG
  printf(" far mem: %luB...",(unsigned long)farcoreleft() );
 #endif
}

//----------------------------------------------------------------------------

void List::PasteAtTail( ListItem far *InsHead, ListItem far *InsTail, word NrOfNewItems )
{
 Tail->Last->Next=InsHead;
 InsHead->Last=Tail->Last;

 InsTail->Next=Tail;
 Tail->Last=InsTail;

 NrOfItems+=NrOfNewItems;
}

//----------------------------------------------------------------------------

void List::Delete( ListItem far *DelItem )
// DelItem points to node in queue which should be deleted
{
 if( DelItem && DelItem!=Head && DelItem!=Tail )
 {
  DelItem->Last->Next=DelItem->Next;
  DelItem->Next->Last=DelItem->Last;

  // must be used delete, because program evides new instances
  // and if farfree( DelItem ); is used after return 0 of main
  // it says Null pointer assignment...
  delete DelItem;

  NrOfItems--;

  #ifdef DEBUG
   printf("\n  Item %Fp deleted,", DelItem );
  #endif
 }

 #ifdef DEBUG
  printf(" far mem: %luB...",(unsigned long)farcoreleft() );
 #endif
}

//----------------------------------------------------------------------------

void List::Get( ListItem far *Item )
// take Item out of list but DO NOT delete
{
 if( Item && Item!=Head && Item!=Tail )
 {
  Item->Last->Next=Item->Next;
  Item->Next->Last=Item->Last;

  NrOfItems--;

  #ifdef DEBUG
   printf("\n  Item %Fp taken out,", Item );
  #endif
 }

 #ifdef DEBUG
  printf(" far mem: %luB...",(unsigned long)farcoreleft() );
 #endif

}

//----------------------------------------------------------------------------

ListItem far *List::Next( ListItem far *Item )
// if next is tail returns NULL
{
 if( Item->Next==Tail )
  return NULL;
 else
  return
   Item->Next;
}

//----------------------------------------------------------------------------

ListItem far *List::Last( ListItem far *Item )
// if last is head returns NULL
{
 if( Item->Last==Head )
  return NULL;
 else
  return Item->Last;
}

//----------------------------------------------------------------------------

ListItem far *List::GetHead( void )
// get node head which contains data ( it's different from Head! )
// if no data node in queue => returns NULL
{
 if( Head->Next==Tail )
  return NULL;
 else
  return Head->Next;
}

//----------------------------------------------------------------------------

ListItem far *List::GetTail( void )
// if no data node in queue => returns NULL
{
 if( Head->Next==Tail )
  return NULL;
 else
  return Tail->Last;
}

//----------------------------------------------------------------------------

void List::Destroy( void )
{
 while( Head->Next!=Tail ) Delete( Head->Next );

 NrOfItems=0;

 #ifdef DEBUG
//  printf("\n  Destroy, far mem: %luB...",(unsigned long)farcoreleft() );
 #endif

 return;
}

//----------------------------------------------------------------------------

void List::LeaveContent( void )
{
 NrOfItems=0;

 Head->Last=Tail->Next=NULL;
 Head->Next=Tail;
 Tail->Last=Head;
}

//----------------------------------------------------------------------------

bool List::Empty( void )
{
 if( Head->Next==Tail )
  return TRUE;
 else
  return FALSE;
}

//----------------------------------------------------------------------------

ListItem far *List::Pop( void )
{
 ListItem far *PopItem;

 if( Head->Next != Tail )
 {
  PopItem=Tail->Last;

  // change pointers
  Tail->Last->Last->Next=Tail;
  Tail->Last		=Tail->Last->Last;


  #ifdef DEBUG
   printf("\n  POP item %Fp ( not deleted )", PopItem );
  #endif

  NrOfItems--;

  return PopItem;
 }
 else
 {
  #ifdef DEBUG
   printf("\n  stack empty");
  #endif

  return NULL;
 }
}

//----------------------------------------------------------------------------

List::~List( void )
{
 #ifdef DEBUG
  printf("\n ~List() this 0x%Fp, items %u", this, NrOfItems );
 #endif

 Destroy();

 delete Head;// is better than farfree(Head);
 delete Tail;// is better than farfree(Tail);

 #ifdef DEBUG
  printf(", far free: %luB...",(unsigned long)farcoreleft() );
 #endif
}

//- EOF ---------------------------------------------------------------------------

