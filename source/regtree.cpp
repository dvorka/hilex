#include "source\regtree.h"

// print info when hunting bugs
// #define DEBUG

// print pointers in CreateSubtree
// #define SHOWPTRS

#define REMOVESPAINBOOTS

char far *ErrReport=NULL;	// for reporting errors

//- class RegularNode methods -----------------------------------------------------------------------------

RegularNode::RegularNode( ItemType iType, word &Width, char far *iExpr=NULL)
{
 word Error;

 Continue=NULL;
 Type=iType;

 switch( Type )
 {
  case SWITCH:
	      // alloc and copy space for switch
	      Error=CopySwitch( iExpr, Data.Switch.Expr ); // "a", "[c-f]"...

	      if( Error )
	      {
	       printf("\n ERROR reported from %s line %d:"
		      "\n  Failed when building tree for \"%s\" (CopySwitch)...", __FILE__, __LINE__, ErrReport );
	       exit(0);
	      }

	      #ifdef DEBUG
	       printf("%s", Data.Switch.Expr );
	      #endif

	      Width=strlen( Data.Switch.Expr );
	      return;

  case REPEATER:
	      Error=
	      RepeaterStringToBinary( iExpr,
				      Data.Repeater.From,
				      Data.Repeater.To,
				      Width
				    );

	      if( Error )
	      {
	       printf("\n ERROR reported from %s line %d:"
		      "\n  Failed when building tree for \"%s\" (RepeaterStringToBinary)...", __FILE__, __LINE__, ErrReport );
	       exit(0);
	      }

	      Data.Repeater.Son=NULL;

	      #ifdef DEBUG
	       printf("{%u,%u}", Data.Repeater.From, Data.Repeater.To );
	      #endif
	      return;
  case OR:
	      Data.Or.LeftSon=NULL;
	      Data.Or.RightSon=NULL;

	      #ifdef DEBUG
	       printf("|");
	      #endif

	      Width=1;
	      return;
  case SPAINBOOT:
	      Data.SpainBoot.Son=NULL;

	      #ifdef DEBUG
	       printf("()");
	      #endif

	      Width=1;
	      return;
  default:
	      break;

 }
}

//------------------------------------------------------------------------------

word RegularNode::CopySwitch( char far *Src, char far *&Dest )
// accepts and copies switch
// types:
//              a
//       	\a
//              \\
//              [...]
//
// wrong input: (a), \\\, ...
{
  word e,
       SrcLng;
  Dest=NULL;
  char far *Ptr;

  switch( Src[0] )
  {
   case '\\':	// type 2., type 3.

	Dest=(char far *)farmalloc(2lu+1lu);
				if( !Dest ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	Dest[0]='\\';
	Dest[1]=Src[1];
	Dest[2]=0;

	return HI_OK;

   case '[':	// type 4. ( errors already fixed )

	Dest=(char far *)farmalloc( (dword)strlen(Src)+1lu );
				if( !Dest ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	SrcLng=strlen(Src);

	e=0;

	do
	{
	 if( SrcLng<=e )
	 {
	  printf("\n ERROR reported from %s line %d:"
		 "\n  When building tree - missing \']\' in \"%s\"", __FILE__, __LINE__, ErrReport );
	  exit(0);
	 }

	 if( Src[e]=='\\' )	// treat [...\]...]
	 {
	  Dest[e]=Src[e];
	  e++;
	  Dest[e]=Src[e];
	 }
	 else
	 {
	  Dest[e]=Src[e];
	 }
	}
	while( Src[++e]!=']' );

	Dest[e]=Src[e];	// copy [
	e++;
	Dest[e]=NULL;	// make string

	// make realloc, if fails leave as it is
	Ptr = (char far *)farrealloc( Dest,
				      (dword)strlen(Dest)+1lu
				    );
	if( Ptr )
	 Dest=Ptr;
	else
	{
	  printf("\n ERROR reported from %s line %d:"
		 "\n  When building tree for \"%s\" realloc fails...", __FILE__, __LINE__, ErrReport );
	  exit(0);
	}

	return HI_OK;

   case '|':
   case '{':
   case '}':
   case '(':
   case ')':
   case ']':
	printf("\n ERROR reported from %s line %d:"
	       "\n  It's operation or parent not a switch: \'%c\',"
	       "\n  when building tree for \"%s\"...", __FILE__, __LINE__, Src[0], ErrReport );
	exit(0);

   default:     // type 1.

	Dest=(char far *)farmalloc(1lu+1lu);
				if( !Dest ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	Dest[0]=Src[0];
	Dest[1]=0;

	return HI_OK;

  } // switch
}

//------------------------------------------------------------------------------

word RegularNode::RepeaterStringToBinary( char far *Expr,
					  word &From, word &To,
					  word &Width )
{
 From=To=0;

 if( !Expr )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  repeater string Expr is NULL - when building tree for \"%s\"...", __FILE__, __LINE__, ErrReport );
  exit(0);
 };

 if( Expr[0]!='{' )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  It's NOT repeater string: \"%s\" - when building tree for \"%s\"...", __FILE__, __LINE__, Expr, ErrReport );
  exit(0);
 }

 // now try to transform it
 word e=1, 	// pointer in Expr point after {
      t=0;      // pointer in Temp			// ptrs point to free
 char Temp[10];

 // copy text number
 while( Expr[e]>='0' && Expr[e]<='9' )
 {
  Temp[t++]=Expr[e++];

  if( t>5 )  	// infinity: 65535
  {
   Temp[t]=0;	// t points to free -> OK
   printf("\n ERROR reported from %s line %d:"
	  "\n  too many repeater iterations: \"%s\" - when building tree for \"%s\"...", __FILE__, __LINE__, Temp, ErrReport );
   exit(0);
  }
 }

 Temp[t]=0;	// make string

 long LongNumber;

 LongNumber=atol(Temp);
 From=(word)LongNumber;	// From converted

 if( Expr[e]=='}') 	// {15}
 {
  To=From;

  Width=e+1;

  return HI_OK;
 }

 // else there is ,
 e++;

 // and now here is } (means infinity)
 if( Expr[e]=='}') 	// {15,}
 {
  To=0xFFFF;

  Width=e+1;

  return HI_OK;
 }

 // or number => make binary To
 t=0;

 while( Expr[e]>='0' && Expr[e]<='9' )
 {
  Temp[t++]=Expr[e++];

  if( t>5 )  	// infinity: 65535
  {
   Temp[t]=0;	// t points to free -> OK
   printf("\n ERROR reported from %s line %d:"
	  "\n  too many repeater iterations: \"%s\" - when building tree for \"%s\"...", __FILE__, __LINE__, Temp, ErrReport );
   exit(0);
  }
 }

 Temp[t]=0;	// make string

 LongNumber=atol(Temp);
 To=(word)LongNumber;	// To converted

 Width=e+1;

 return HI_OK;
}

//------------------------------------------------------------------------------

void RegularNode::Print( void )
// prints info about node, used for debug
{
 switch( Type )
 {
  case SWITCH:
	      #ifdef DEBUG
	       printf("%s", Data.Switch.Expr );
	       #ifdef SHOWPTRS
		printf("\n 	SWITCH    %Fp : Continue %Fp\n", this, Continue );
	       #endif
	      #endif
	      return;
  case REPEATER:

	      #ifdef DEBUG
	       printf("{From,To}"); // only for debug, so shema is enought
	       #ifdef SHOWPTRS
		printf("\n 	REPEATER  %Fp : Continue %Fp Son %Fp\n", this, Continue, Data.Repeater.Son );
	       #endif
	      #endif
	      return;
  case OR:
	      #ifdef DEBUG
	       printf("|");
	       #ifdef SHOWPTRS
		printf("\n 	OR        %Fp : Continue %Fp LSon %Fp RSon %Fp\n", this, Continue, Data.Or.LeftSon, Data.Or.RightSon );
	       #endif
	      #endif
	      return;
  case SPAINBOOT:
	      #ifdef DEBUG
	       printf("()");
	       #ifdef SHOWPTRS
		printf("\n 	SPAINBOOT %Fp : Continue %Fp Son %Fp\n", this, Continue, Data.SpainBoot.Son );
	       #endif
	      #endif
	      return;
 }
}

//------------------------------------------------------------------------------

RegularNode::~RegularNode()
{
 switch( Type )
 {
  case SWITCH:
	      if( Data.Switch.Expr )
	       farfree( Data.Switch.Expr );
	      break;
  case REPEATER:
  case OR:
  default:
	      break;
 }
}

//- class RegularTree methods -----------------------------------------------------------------------------

RegularTree::RegularTree()
{
 Root=NULL;
}

//------------------------------------------------------------------------------

word RegularTree::PrefixToTree( char far *iPrefix )
// builds tree from regular expression in prefix notation
{
 #ifdef DEBUG
  printf("\n\n PrefixToTree conversion, building...\n %s\n  ", iPrefix );
 #endif

 ErrReport=iPrefix;	// init global

 Prefix   =iPrefix;
 Off=0;

 CreateSubtree( Root );		// build tree

 #ifdef REMOVESPAINBOOTS
  RemoveSpainBoots();		// optimize it
 #endif

 return HI_OK;
}

//------------------------------------------------------------------------------

word RegularTree::CreateSubtree( RegularNode far *&RetNode,
				 ItemType WhoCalls
			       )
// Goes from left of prefix string to right
// Calls itself recursively and all the time moving Off right,
// each call returns new Off ( kolik toho zpracoval )
//
// Off points to free == nezpracovane
//
// -> MOVE CROSS ) RIGHT IN STRING CAN ONLY SPAINBOOT NODE, OTHERS CAN NOT <-
// so therefore its done inside its section
//
// -> REPEATER vaze ONLY NEIGHBOUR SWITCH OR () nic vic
//    ( only son subtree is repeated ) <-
// so therefore parameter WhoCalls==REPEATER in this case, using this
// avoiding Continue in son and forsing continuing from repeater
//
// -> "OR" IS PROXY OF OR PARENT, SO DISTRIBUTES WhoCalls TO SONS -<
//
// in other cases WhoCalls==FALSE
{
 word 		Width;
 RegularNode 	far *NewNode;

 switch( Prefix[Off] )
 {
  case NULL:			// return back from backtrack - end of string
	     NewNode=NULL;

	     RetNode=NewNode;

	     return HI_OK;

  case '|': 	// Or

	    NewNode=new RegularNode( OR, Width );
				if( !NewNode ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	    Off++;

	    CreateSubtree( NewNode->Data.Or.LeftSon, REPEATER );
	    CreateSubtree( NewNode->Data.Or.RightSon, REPEATER);

	    if( WhoCalls!=REPEATER )
	     CreateSubtree( NewNode->Continue );
	    else
	     NewNode->Continue=NULL;

	    RetNode=NewNode;

	    #ifdef SHOWPTRS
	     printf("\n OR        %Fp : Continue %Fp LSon %Fp RSon %Fp", NewNode, NewNode->Continue, NewNode->Data.Or.LeftSon, NewNode->Data.Or.RightSon );
	    #endif

	    return HI_OK;

  case '{':	// Repeater

	    NewNode=new RegularNode( REPEATER, Width, Prefix+Off );
				if( !NewNode ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	    // roll right cross repeater

	    Off+=Width;		// now points to free

	    // now construct what will be repeated

	    CreateSubtree( NewNode->Data.Repeater.Son, REPEATER );
	    CreateSubtree( NewNode->Continue );

	    RetNode=NewNode;

	    #ifdef SHOWPTRS
	     printf("\n REPEATER  %Fp : Continue %Fp Son %Fp", NewNode, NewNode->Continue, NewNode->Data.Repeater.Son );
	    #endif

	    return HI_OK;

  case '(':	// subexpression => SpainBoot

	    NewNode=new RegularNode( SPAINBOOT, Width );
				if( !NewNode ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	    Off++;	// now Off points behind (

	    CreateSubtree( NewNode->Data.SpainBoot.Son );

	    // now Off points to ), nobody can move across )
	    // only ( section can do this ( here )
	    // => section ) is leave of backtrack, from this section
	    // I am moving up

	    Off++;	// now Off points behind )

	    if( WhoCalls!=REPEATER )
	     CreateSubtree( NewNode->Continue );
	    else
	     NewNode->Continue=NULL;

	    RetNode=NewNode;

	    #ifdef SHOWPTRS
	     printf("\n SPAINBOOT %Fp : Continue %Fp Son %Fp ", NewNode, NewNode->Continue, NewNode->Data.SpainBoot.Son );
	    #endif

	    return HI_OK;

  case ')': 	// end of subexpression

	    NewNode=NULL;	// return back from backtrack
				// cross ) I am moving only in ( section

	    RetNode=NewNode;

	    return HI_OK;

  default:	// Switch ... '\a', '\\', '[', 'a'

	    #ifdef DEBUG
	     printf("\n CREATE SUBTREE Width %u Prefix+Off %s...\n", Width, Prefix+Off );
	    #endif

	    NewNode=new RegularNode( SWITCH, Width, Prefix+Off );
				if( !NewNode ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); exit(0); }

	    Off+=Width;       	// now points to free

	    if( WhoCalls!=REPEATER )
	     CreateSubtree( NewNode->Continue );
	    else
	     NewNode->Continue=NULL;

	    RetNode=NewNode;

	    #ifdef SHOWPTRS
	     printf("\n SWITCH    %Fp : Continue %Fp", NewNode, NewNode->Continue );
	    #endif

	    return HI_OK;
 }
}

//------------------------------------------------------------------------------

void RegularTree::RemoveSpainBoots( void )
// optimizes tree: removes SpainBoots: () nodes from tree
{
 bool 		AddrChanged;
 RegularNode 	far *NewAddr;

 #ifdef DEBUG
  printf("\n\n Remove SPAINBOOTs - preorder before:\n");
  #ifndef SHOWPTRS
  printf("   ");
  #endif

  PreorderPrint( Root );
 #endif

 RemoveSpainBootsStep( Root, AddrChanged, NewAddr );

 if( AddrChanged ) Root=NewAddr;

 #ifdef DEBUG
  printf("\n\n Remove SPAINBOOTs - preorder after:\n");
  #ifndef SHOWPTRS
  printf("   ");
  #endif

  PreorderPrint( Root );
 #endif


 if( AddrChanged ) Root=NewAddr;

 return;
}

//------------------------------------------------------------------------------

void RegularTree::RemoveSpainBootsStep( RegularNode far *Node,
					bool &AddrChanged,
					RegularNode far *&NewAddr
				      )
// backtrack, postorder
// if node is SpainBoot, commits suicide and returns addr of its son
{
 bool 		Changed;
 RegularNode 	far *Addr,
		far *RightestBrother;


 if( Node!=NULL )
 {

  switch( Node->Type )
  {
   case SWITCH:
			RemoveSpainBootsStep( Node->Continue,
					      Changed,
					      Addr );
			if( Changed ) Node->Continue=Addr;

			AddrChanged=FALSE;
			return;
   case REPEATER:
			RemoveSpainBootsStep( Node->Data.Repeater.Son,
					      Changed,
					      Addr );
			if( Changed ) Node->Data.Repeater.Son=Addr;

			RemoveSpainBootsStep( Node->Continue,
					      Changed,
					      Addr );
			if( Changed ) Node->Continue=Addr;

			AddrChanged=FALSE;
			return;
   case OR:
			RemoveSpainBootsStep( Node->Data.Or.LeftSon,
					      Changed,
					      Addr );
			if( Changed ) Node->Data.Or.LeftSon=Addr;

			RemoveSpainBootsStep( Node->Data.Or.RightSon,
					      Changed,
					      Addr );
			if( Changed ) Node->Data.Or.RightSon=Addr;

			RemoveSpainBootsStep( Node->Continue,
					      Changed,
					      Addr );
			if( Changed ) Node->Continue=Addr;

			AddrChanged=FALSE;
			return;
   case SPAINBOOT:
			RemoveSpainBootsStep( Node->Data.SpainBoot.Son,
					      Changed,
					      Addr );
			if( Changed ) Node->Data.SpainBoot.Son=Addr;

			RemoveSpainBootsStep( Node->Continue,
					      Changed,
					      Addr );
			if( Changed ) Node->Continue=Addr;


			// parent (A) Continue must be redirected to son (B),
			// 		A.Continue=B;
			//
			// now find end of string in son level, and redirect
			// it:
			//	B.RightestBrother.Continue=SpainBoot.Continue

			//     Before:              After:
			//
			// A->SpainBoot->E      A->B->C->D->E
			//     |
			//     |
			//     B->C->D->Null


			// find the rightest Brother of SpainBoot son
			RightestBrother=Node->Data.SpainBoot.Son->Continue;

			while( RightestBrother !=NULL
			       &&
			       RightestBrother->Continue!=NULL
			     )
			{
			 RightestBrother=RightestBrother->Continue;
			}

			if( RightestBrother!=NULL )
			 RightestBrother->Continue=Node->Continue;
			else // node is null
			 Node->Data.SpainBoot.Son->Continue=Node->Continue;

			// move up son ( Parent.Continue=Son; )
			AddrChanged=TRUE;
			NewAddr=Node->Data.SpainBoot.Son;

			// now can be SpainBoot deleted
			delete Node;

			return;
  } // switch
 } // if
 else
 {
  // node is NULL

  AddrChanged=FALSE;

 }
}

//------------------------------------------------------------------------------

void RegularTree::RootOfDeath( RegularNode far *Node )
// kills tree by postorder with node deletion
// ( preorder print is done in time )
{

 if( Node!=NULL )
 {

  switch( Node->Type )
  {
   case SWITCH:
			Node->Print();
			RootOfDeath( Node->Continue );
			delete Node;

			return;
   case REPEATER:
			Node->Print();
			RootOfDeath( Node->Data.Repeater.Son );
			RootOfDeath( Node->Continue );
			delete Node;

			return;
   case OR:
			Node->Print();
			RootOfDeath( Node->Data.Or.LeftSon );
			RootOfDeath( Node->Data.Or.RightSon );
			RootOfDeath( Node->Continue );
			delete Node;

			return;
   case SPAINBOOT:
			Node->Print();
			RootOfDeath( Node->Data.SpainBoot.Son );
			RootOfDeath( Node->Continue );
			delete Node;

			return;
  } // switch
 } // if
}

//------------------------------------------------------------------------------

void RegularTree::PreorderPrint( RegularNode far *Node )
{
 if( Node!=NULL )
 {

  switch( Node->Type )
  {
   case SWITCH:
			Node->Print();	// for debug
			PreorderPrint( Node->Continue );
			return;
   case REPEATER:
			Node->Print();	// for debug
			PreorderPrint( Node->Data.Repeater.Son );
			PreorderPrint( Node->Continue );
			return;
   case OR:
			Node->Print();	// for debug
			PreorderPrint( Node->Data.Or.LeftSon );
			PreorderPrint( Node->Data.Or.RightSon );
			PreorderPrint( Node->Continue );
			return;
   case SPAINBOOT:
			Node->Print();	// for debug
			PreorderPrint( Node->Data.SpainBoot.Son );
			PreorderPrint( Node->Continue );
			return;
  } // switch
 } // if
}

//------------------------------------------------------------------------------

RegularTree::~RegularTree()
{
 #ifdef DEBUG
  printf("\n\n RegularTree destructor...");
 #endif

 #ifdef DEBUG
  printf("\n  RootOfDeath:"
	 "\n   "
	);
 #endif

 RootOfDeath( Root );

 #ifdef DEBUG
  printf("\n RegularTree destructed...");
 #endif
}

//- EOF -----------------------------------------------------------------------------

