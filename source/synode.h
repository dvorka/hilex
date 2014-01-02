#ifndef __SYNODE_H
 #define __SYNODE_H

 #include "source\hierrors.h"
 #include "source\hilist.h"
 #include "source\hitypes.h"

 //- struct SyntactNode ---------------------------------------------------------

 struct far _SyntactNode : public ListItem
 {

  SyntactNodeType Type;

  // Union type:
  //  SNODE_NULL,
  //  SNODE_STAR,
  //  SNODE_R,
  //  SNODE_R2,
  //  SNODE_R3,
  //  SNODE_T,
  //  SNODE_T4,
  //  SNODE_T5,
  //  SNODE_K,
  //  SNODE_S,
  //  SNODE_H,
  //  SNODE_LBRACE,
  //  SNODE_RBRACE,
  //  SNODE_LPARENT,
  //  SNODE_RPARENT,
  //  SNODE_EQU,
  //  SNODE_SEMICOLON,
  //  SNODE_k,
  //  SNODE_s,
  //  SNODE_h

  // constant nodes {}();= are not constructed -> SAVING MEM

  union
  {

   struct
   {
    _SyntactNode far *R;
   }
   Star;

   struct
   {
    _SyntactNode far *Rl;
    _SyntactNode far *Rr;
   }
   NeterminalR2;

   struct
   {
    _SyntactNode far *H;
    //_SyntactNode far *Lp;		// constant node (
    _SyntactNode far *S;
    //_SyntactNode far *Rp;		// constant node )
    //_SyntactNode far *Lb;		// constant node {
    _SyntactNode far *T;
    //_SyntactNode far *Rb;		// constant node }
   }
   NeterminalR3;

   struct
   {
    _SyntactNode far *K;
    //_SyntactNode far *Equ;		// constant node =
    _SyntactNode far *S;
    //_SyntactNode far *Semicolon;	// constant node ;
   }
   NeterminalT4;

   struct
   {
    _SyntactNode far *Tl;
    _SyntactNode far *Tr;
   }
   NeterminalT5;

   struct
   {
    _SyntactNode far *k;	// point to LexicalElement containing ID of keyword
   }
   NeterminalK;

   struct
   {
    _SyntactNode far *s;	// point to LexicalElement containing string
   }
   NeterminalS;

   struct
   {
    _SyntactNode far *h;	// point to LexicalElement containing ID of head keyword
   }
   NeterminalH;

   struct
   {
    ElementID 	ID;		// contains SNODE_s
    dword	Beg;
    dword	End;
    char far	*Element;
   }
   s;				// string

   struct
   {
    ElementID 	ID;
    TokenType   Key;	        // contains enum ID of keyword
				// DESCR, RAW_BEG, ...
				// ( see  hitypes.h )
   }
   k;				// keyword

   struct
   {
    ElementID 	ID;
    TokenType   Key;	        // contains enum ID of head keywords
				// ELEMENT or SETUP
				// ( see  hitypes.h )
   }
   h;				// head keyword

  }
  Data;



  public:

    void DeleteNodeElements( void );
    void Print( void ); // for printing error - not debuged
   ~_SyntactNode( void );

 };

 typedef _SyntactNode SyntactNode;

#endif
