#ifndef __COCON_H
 #define __COCON_H

 #include "source\hierrors.h"
 #include "source\hililex.h"
 #include "source\hilist.h"
 #include "source\hitypes.h"
 #include "source\static.h"

 //- class StaticTableItem --------------------------------------------------------------------------

 struct far StaticTableItem
 {
  ActionType    Action;

  char far 	*RawBeg,
		*RawEnd;

  public:

    void Load( int Handle );
   ~StaticTableItem( void );
 };

 //- struct Humanoid --------------------------------------------------------------------------

 struct far _Humanoid
 {
  ElementID ID;
  dword	    Beg,
	    End;
 };

 typedef _Humanoid Humanoid;

 //- class COCONator --------------------------------------------------------------------------

 class far _COCONator
 {
  private:

   StaticAutomat 	far *Coconizer;

   StaticTableItem 	far *CoconMucus;

   word			NrOfItems;

   // for GetHumanoid
   Buffer far 	*Buf;
   dword  	Beg,
		End,
		OldBeg;

  public:

   _COCONator( char *FileName );	// name of *.BCA file

    void Coconize( char *InFileName, char *OutFileName );
    void PrintCoconMucus( void );

   ~_COCONator();

  private:

   void Load( char *FileName );
   bool GetHumanoid( void );

 };

 typedef _COCONator COCONator;

#endif