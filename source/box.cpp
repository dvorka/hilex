#include "source\box.h"

// print info when hunting bugs
//#define DEBUG

//- class BoxListItem methods -----------------------------------------------------------------------------

void BoxListItem::Print( void )
{
 #ifdef DEBUG
  printf("\n       Sign %d target %Fp", List, Target );
 #endif
}

//- class RegularBox methods -----------------------------------------------------------------------------

RegularBox::RegularBox( ItemType iType, BoxStructure far *Boxes )
{
 IN=new List;
 OK=new List;
 NOK=new List;	if( !IN || !OK || !NOK ) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 Type=iType;

 // register itself
 Boxes->Register( this );

 #ifdef DEBUG
  printf("\n RegularBox 0x%Fp created, type %d", this, Type );
 #endif
}

//------------------------------------------------------------------------------

void RegularBox::Print( void )
{
 #ifdef DEBUG

  ListItem far *Item;

  printf("\n BOX PROFILE: %Fp type is %d", this, Type );
  printf("\n IN list:");
   Item=IN->GetHead();
   while( Item )
   {
    ((BoxListItem far *)Item)->Print();
    Item=IN->Next(Item);
   }

  printf("\n OK list:");
   Item=OK->GetHead();
   while( Item )
   {
    ((BoxListItem far *)Item)->Print();
    Item=OK->Next(Item);
   }

  printf("\n NOK list:");
   Item=NOK->GetHead();
   while( Item )
   {
    ((BoxListItem far *)Item)->Print();
    Item=NOK->Next(Item);
   }

 #endif
}

//------------------------------------------------------------------------------

RegularBox::~RegularBox()
{
 delete IN;
 delete OK;
 delete NOK;
}

//- class BoxStructure methods-----------------------------------------------------------------------------

BoxStructure::BoxStructure( void )
{
 Registry=new List; if(!Registry) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
}

//------------------------------------------------------------------------------

void BoxStructure::Register( RegularBox far *Box )
{
 RegistryItem far *Item=new RegistryItem; if(!Item) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 Item->Box=Box;

 Registry->Insert( Item );
}

//------------------------------------------------------------------------------

BoxStructure::~BoxStructure( void )
{
 // first delete boxes which are in registry
 ListItem far *Item;

 Item=Registry->GetHead();

 while( Item )
 {
  #ifdef DEBUG
   printf("\n DeletingBox 0x%Fp type %d", ((RegistryItem far *)Item)->Box, ((RegistryItem far *)Item)->Box->Type );
   ((RegistryItem far *)Item)->Box->Print();
  #endif

  delete ((RegistryItem far *)Item)->Box;

  #ifdef DEBUG
   printf("\n Box 0x%Fp deleted", ((RegistryItem far *)Item)->Box);
  #endif

  Item=Registry->Next(Item);
 }

 // then delete registry
 delete Registry;
}

//- EOF -----------------------------------------------------------------------------
