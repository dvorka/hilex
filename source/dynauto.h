#ifndef __DYNAUTO_H
 #define __DYNAUTO_H

 #include "io.h"
 #include "fcntl.h"
 #include "sys\stat.h"
 #include "source\box.h"
 #include "source\hierrors.h"
 #include "source\hilist.h"
 #include "source\hitypes.h"
 #include "source\prefix.h"
 #include "source\regtree.h"
 #include "source\static.h"

 //- struct InBagItem ---------------------------------------------------------------

 struct far InBagItem : public ListItem
 {
  RegularBox far *Box;
 };

 //- class DynaJmpTableItem --------------------------------------------------------

 //- JmpTableItemFlags -
 #define JMP_OK         1
 #define JMP_NOK        2
 #define JMP_ACCEPT     4
 #define JMP_END        8
 #define JMP_REST       16
 #define JMP_NOT_SET    32
 #define JMP_ELEMENT_ID 64
 #define JMP_STOP       128

 class far DynaJmpTableItem : public ListItem
 {
  public:
   char         From,
		To;

   AutoItemID   JmpTo;	// USED FOR ELEMENT ID IF FLAF IS SET

   byte         Flags;

  public:
   DynaJmpTableItem(void ) { From=To=JmpTo=Flags=0; };
   void Print( void );
   virtual ~DynaJmpTableItem() {};

 };

 //- class DynaJmpTable --------------------------------------------------------

 class far DynaJmpTable
 {
  public:
   List         far *Items;
   bool         JumpsAlreadySet;

  public:

   DynaJmpTable( char far *Expr );
   DynaJmpTable( DynaJmpTableItem far *TableItem ); // for green ghosts
    word AddItem( DynaJmpTableItem far *Item );
    void SetJmpInOKs( AutoItemID ID, ElementID EleID=0 );
    void SetJmpInNOKs( AutoItemID ID );
    void SetJmpInOKsAndMuteToNOKs( AutoItemID ID );
    void SetJmpInNOKsAndMuteToOKs( AutoItemID ID, ElementID EleID=0 );
    void Print( void );
    void Shift( word ShiftNum );
    word Save( int Handle, AutoItemID ID );
   ~DynaJmpTable( void );

  private:
   word SwitchToDynaJmpTable( char far *Expr );
 };

 //- class DynaAutoItem --------------------------------------------------------

 class far _DynaAutoItem : public ListItem
 {
  public:

   DynaJmpTable far *JmpTable;
   AutoItemID	ID;

  public:

   void Print( void );
 };

 typedef _DynaAutoItem DynaAutoItem;

 //- class DynamicAutomat --------------------------------------------------------

 class far _DynamicAutomat
 {
  private:

   AutoItemID   LastID;         	// points to the first free item
   List         far *Items;
   BoxStructure far *Registry;		// for easy delete of box structure
   List         far *InputStates;	// list of inuput states (NDET)
   ElementID    TempID;

  public:

   _DynamicAutomat( void );
    word AddItem( AutoItemID &ID, DynaAutoItem far *&Item );
    word StringToDynaAuto( char far *Infix, ElementID ID );
    word TreeToDynaAuto( RegularTree far *Tree );
    word GetNrOfItems( void ) { return LastID; };
    word Wampirator( _DynamicAutomat &Blood );
    word Cut( ListItem far *&ItHead, ListItem far *&ItTail, word &ItItems,
	      ListItem far *&InHead, ListItem far *&InTail, word &InItems );
    void Print( void );
    word Save( char *FileName );
    word Append( int Handle );
   ~_DynamicAutomat();

  private:

   void JoinLists( List far *FromList, void far *ToBox, ListSignature ToList );
   word TreeToDynaAutoLevel( RegularNode far *Node, RegularBox *&Box );
   word SubtreeToBox( RegularNode far *Node, RegularBox far *&CreatedBox );
   word FillJmpTables( RegularBox far *Box );
   word DiveForSwitch( BoxListItem  far *BLItem,
		       word         &Flag,
		       word         &TypeFlag,
		       List         far *Bag
		     );
   void FinishDiveForOK ( BoxListItem far *BLItem, List far *Bag );
   void FinishDiveForNOK( BoxListItem far *BLItem, List far *Bag );
   word ProcessSwitch( RegularBox far *SwitchBox );

   void ReduceNrOfInputStates( void );
   word RdcAreSame( DynaAutoItem far *State1, DynaAutoItem far *State2 );


 };

 typedef _DynamicAutomat DynamicAutomat;

 //- struct InputStatesItem --------------------------------------------------------

 struct far InputStatesItem : public ListItem
 {
  DynaAutoItem far *AutoItem;
 };

#endif

