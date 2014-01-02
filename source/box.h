#ifndef __BOX_H
 #define __BOX_H

 #include "source\hierrors.h"
 #include "source\hilist.h"
 #include "source\hitypes.h"

 class far _BoxStructure;

 typedef   _BoxStructure BoxStructure;

 //- class BoxListItem --------------------------------------------------------

 class far _BoxListItem : public ListItem
 {
  public:

   ListSignature List;
   void far 	 *Target;	// points to box or auto item

   _BoxListItem( void ) { Target=NULL; }

  public:
   void Print( void );

 };

 typedef _BoxListItem BoxListItem;

 //- class RegularBox --------------------------------------------------------

 class far _RegularBox
 {
  public:

   List *IN,    // in SWITCH box it is list of pointers to AutoItemID,
		// in other cases points to IN List of another box
		// it's list because of NDET
	*OK,   	// ... I am jump out of box when ACCEPT
		// points to IN List of another box
	*NOK;   // ... I am jump out of box when END
		// points to IN List of another box



   ItemType Type;		// type of box

  public:

   _RegularBox( ItemType iType, BoxStructure far *Boxes );
    void Print( void );
   ~_RegularBox();

 };

 typedef _RegularBox RegularBox;

 //- class RegistryItem --------------------------------------------------------

 struct far _RegistryItem : public ListItem
 {
  RegularBox far *Box;
 };

 typedef _RegistryItem RegistryItem;

 //- class BoxStructure --------------------------------------------------------

 class far _BoxStructure
 {
  private:
   List         far *Registry;	// for easy delete of box structure

  public:

   _BoxStructure( void );
    ListItem far *GetHead( void ) { return Registry->GetHead(); };
    ListItem far *Next( ListItem far *Item ) { return Registry->Next( Item ); };
    void	 Register( RegularBox far *Box );
   ~_BoxStructure();

 };


#endif

