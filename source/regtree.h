#ifndef __REGTREE_H
 #define __REGTREE_H

 #include <alloc.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

 #include "source\hierrors.h"
 #include "source\hitypes.h"

 //- class RegularNode --------------------------------------------------------

 class far _RegularNode
 {
  public:

   _RegularNode( ItemType iType, word &Width, char far *iExpr );
    void Print( void );
   ~_RegularNode();

   ItemType    Type;
   _RegularNode far *Continue;

   union	// specific node data
   {
    struct
    {
     _RegularNode far *LeftSon;
     _RegularNode far *RightSon;
    }
    Or;

    struct
    {
     _RegularNode far *Son;	// what's inside ()
    }
    SpainBoot;

    struct
    {
     char far *Expr;
    }
    Switch;

    struct
    {
     word From;
     word To;			// INFINITY == 65535
     _RegularNode far *Son;
    }
    Repeater;


   }
   Data; 	// union


  private:
   word RepeaterStringToBinary( char far *Expr, word &From, word &To,
				word &Width );
   word CopySwitch( char far *Src, char far *&Dest );

 };

 typedef _RegularNode RegularNode;

 //- class RegularTree --------------------------------------------------------

 class far _RegularTree
 {
  public:

   _RegularTree( void );
    word PrefixToTree( char far *iPrefix );
    void PreorderPrint( RegularNode far *Node );
   ~_RegularTree();



   RegularNode far *Root;

  private:

   char far *Prefix;	// Used by CreateSubtree, filled by PrefixToTree
   word     Off;	// Used by CreateSubtree, filled by PrefixToTree

   word CreateSubtree( RegularNode far *&NewNode, ItemType WhoCalls=ANONYM );
   void RemoveSpainBoots( void );
   void RemoveSpainBootsStep( RegularNode far *Node,
			      bool &AddrChanged, RegularNode far *&NewAddr );
   void RootOfDeath( RegularNode far *Node );

   // char far *ErrReport; this string is global initialized in regular
   // tree constructor

 };

 typedef _RegularTree RegularTree;

#endif

