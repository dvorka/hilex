#ifndef __HILIST_H
 #define __HILIST_H

 #include <alloc.h>
 #include <stdio.h>
 #include <stdlib.h>

 #include "source\hierrors.h"
 #include "source\hitypes.h"

 //- struct ListItem --------------------------------------------------------

 typedef struct far _ListItem ListItem;

 struct far _ListItem
 {
  _ListItem far *Next,
	    far *Last;

  virtual ~_ListItem( void ) {};
 };

 //- class List -------------------------------------------------------------

 class far List
 {
  public:
   ListItem far *Head;
   ListItem far *Tail;	        	// service head and tail are empty
					// nodes
   word 	NrOfItems;		// data items ( Head, Tail not in )

   List( void );
    void inline  Insert( ListItem far *InsertedItem ) { InsertBeforeTail( InsertedItem ); }
    void         InsertAfterHead( ListItem far *InsertedItem  ); // InsertedItem is inserted => not copied!
    void         InsertBeforeTail( ListItem far *InsertedItem  ); // InsertedItem is inserted => not copied!
    void inline  Paste( ListItem far *InsHead, ListItem far *InsTail, word NrOfNewItems ) { PasteAtTail( InsHead, InsTail, NrOfNewItems ); };
    void         PasteAtTail( ListItem far *InsHead, ListItem far *InsTail, word NrOfNewItems );
    void         Delete( ListItem far *DelItem );           // DelItem points to node in queue which should be deleted
    void         Get( ListItem far *Item ); // take item out of list but DO NOT delete
    ListItem far *Next( ListItem far *Item );
    ListItem far *Last( ListItem far *Item );
    ListItem far *GetHead( void );
    ListItem far *GetTail( void );
    void         Destroy( void );       // destroys every data node
    void         LeaveContent( void );  // make list empty, nodes not destroyed
    bool	 Empty( void );

    void         Push( ListItem far *PushedItem ) { InsertBeforeTail( PushedItem ); }
    ListItem far *Pop( void );		// pop it but DO NOT delete item

   ~List();

 };

#endif