#ifndef __STATIC_H
 #define __STATIC_H

 #include "io.h"
 #include "fcntl.h"
 #include "sys\stat.h"
 #include "source\buffer.h"
 #include "source\dynauto.h"
 #include "source\hierrors.h"
 #include "source\hitypes.h"
 #include "source\hilist.h"

 //- class StatJmpTable --------------------------------------------------------

 class far StatJmpTable;

 //- class Robot ----------------------------------------------------------------

 class far _Robot : public ListItem
 {
  public:

   AutoItemID ID;
   bool       Stop;	// true if new input character NOT wanted

  public:

   _Robot( AutoItemID StartHere, bool iStop=FALSE ) { ID=StartHere; Stop=iStop; };

 };

 typedef _Robot Robot;

 //- struct AcceptItem --------------------------------------------------------

 struct _AcceptItem
 {
  dword     To;

  ElementID ID;	// elementID

  void Print( void );
 };

 typedef _AcceptItem AcceptItem;

 //- class StaticAutomat --------------------------------------------------------

 struct far StatJmpTableItem;

 class far _StaticAutomat
 {
  private:

   AutoItemID       NrOfItems;
   word             NrOfInputItems;

   StatJmpTable     far **Engine;
   AutoItemID       far *InputStates;

   List       	    far *InputStatesPool; // dynamic list of input states

  public:

   _StaticAutomat( char *FileName );
   _StaticAutomat( int Handle );
    bool Accept( Buffer far    *Buf,
		 dword         BegOff, 	dword &EndOff,
		 LexicatorType &EleID
	       );
    void Print( void );
   ~_StaticAutomat();

  private:
   word Load( char *FileName );
   word Load( int  Handle );

   // Accept lists, declared here for Accept speedup
   List	far *Queue,
	far *NextStep,
	far *StopNextStep;

 };

 typedef _StaticAutomat StaticAutomat;

 //- class StatJmpTableItem --------------------------------------------------------

 struct far StatJmpTableItem
 {
   char         From,
		To;

   AutoItemID   JmpTo;

   byte         Flags;
 };

 //- class StatJmpTable --------------------------------------------------------

 class far StatJmpTable
 {
  private:
   StatJmpTableItem far *Items;
   word		    NrOfItems;

  public:

   StatJmpTable( int Handle);
    friend bool StaticAutomat::Accept( Buffer far *Buf, dword BegOff, dword &EndOff, LexicatorType &EleID );
    void Print( void );
   ~StatJmpTable( void );

  private:
   word SwitchToStatJmpTable( char far *Expr );
 };

#endif
