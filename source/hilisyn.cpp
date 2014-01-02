#include "source\hilisyn.h"

// print info when hunting bugs
// #define DEBUG

#define EXIT_PROGRAM exit(0)

//- Grammar Rights in in binary form ----------------------------------------------------------------------

 #define NR_OF_GRAMMAR_RIGHTS   8

 // GRights are checked from end to beg => valid items are moved to the end
 // in front of grights are 0

 GrammarRight
  // * -> R
  GRight1={ SNODE_STAR, { SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_R, },		1 },
  // R -> RR
  GRight2={ SNODE_R, 	{ SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_R,
			  SNODE_R }, 	    	2 },
  // R -> H(S){T}
  GRight3={ SNODE_R, 	{ SNODE_H,
			  SNODE_LPARENT,
			  SNODE_S,
			  SNODE_RPARENT,
			  SNODE_LBRACE,
			  SNODE_T,
			  SNODE_RBRACE }, 	7 },
  // T -> K=S;
  GRight4={ SNODE_T, 	{
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_K,
			  SNODE_EQU,
			  SNODE_S,
			  SNODE_SEMICOLON }, 	4 },
  // T -> TT
  GRight5={ SNODE_T, 	{ SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_T,
			  SNODE_T }, 		2 },
  // K -> k
  GRight6={ SNODE_K, 	{ SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_k }, 		1 },
  // S -> s
  GRight7={ SNODE_S, 	{ SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_s }, 		1 },
  // H -> h
  GRight8={ SNODE_H, 	{ SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_NULL,
			  SNODE_h }, 		1 };

//- class Syntactator methods ----------------------------------------------------------------------------

Syntactator::Syntactator( char *InputFileName )
{
 LeftStack=new List; if(!LeftStack) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
 RightStack=new Lexicator; if(!RightStack) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

 RightStack->PrepareForPop( InputFileName );
}

//-----------------------------------------------------------------------------

Syntactator::~Syntactator( void )
{
 delete LeftStack;
 delete RightStack;
}

//-----------------------------------------------------------------------------


void Syntactator::LRAnalyzer( SyntactNode far *&SyntactTree )
{
 SyntactNode far *Element=NULL;

 while( RightStack->Pop( Element ) )
 {
  if( Element )
  {
   LeftStack->Push( Element ); // NODE

   ReduceLeftStack();
  }
  else
  {
    break;
  }

  #ifdef DEBUG
   printf("\n-------> After POP and REDUCE, LeftStackItems %d <-------", LeftStack->NrOfItems);
  #endif
 }

 // print content of LeftStack
 #ifdef DEBUG
  printf("\n-------> State of LeftStack after SYNTACTATOR <-------");

  Item=LeftStack->GetHead();

  while( Item )
  {
   printf("\n Type: %d", ((SyntactNode far *)Item)->Type );

   Item=LeftStack->Next( Item );
  }

  printf("\n-------> State of LeftStack after SYNTACTATOR <-------");
 #endif

 // --- CHECK FOR ERRORS ---

 ListItem far *Item;

 // check if input worked out
 dword ErrPos;

 if( !RightStack->CorrectFinish( ErrPos ) )
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Syntact error at offset %lu in HLX cfg file."
	 , __FILE__, __LINE__, ErrPos );

  // search for error
  // R ... ok records
  // something else -> not complete record -> error in it
  // => go through stack roots while R
  // => when not R print while roots not R
  // => then exit

  // jump cross R
  Item=LeftStack->GetHead();

  if( Item==NULL )
  {
   printf("\n  Error is in the first element in cfg file...");
   exit(0);
  }
  else
  {
   printf(
	  "\n  Unknown element, invalid keyword or missing \'\"\' block"
	  "\n  is influenced by mistake which is right behind inside..."
	  "\n\n"
	 );
  }

  while( Item )
  {
   switch( ((SyntactNode far *)Item)->Type )
   {
    case SNODE_STAR:
    case SNODE_R:
    case SNODE_R2:
    case SNODE_R3:
		Item=LeftStack->Next( Item );
		continue;
    default:
		break;
   }
   break;
  }

  // print !R(s)
  while( Item )
  {
   switch( ((SyntactNode far *)Item)->Type )
   {
    case SNODE_STAR:
    case SNODE_R:
    case SNODE_R2:
    case SNODE_R3:
		break;
    default:
		((SyntactNode far *)Item)->Print(); // print content of tree
		Item=LeftStack->Next( Item );
		continue;
   }
   break;
  }

  exit(0);
 }



 // check the state of stack, there must be ONE item - R2 or R3 -
 // ( because I DO NOT use gright 1 ) neterminal R2/R3 is not
 // reduced to initial symbol *

 if( LeftStack->NrOfItems!=1 )	// on left stack must be right one item
 {
  printf("\n ERROR reported from %s line %d:"
	 "\n  Syntact error in HLX file - LR analyzer not accepts it..."
	 "\n"
	 "\n  Error was detected in this block:"
	 "\n"
	 , __FILE__, __LINE__  );

  // search for error
  // R ... ok records
  // something else -> not complete record -> error in it
  // => go through stack roots while R
  // => when not R print while roots not R
  // => then exit

  // jump cross R
  Item=LeftStack->GetHead();

  if( Item==NULL )
  {
   printf("\n  Input file empty?");
   exit(0);
  }

  while( Item )
  {
   switch( ((SyntactNode far *)Item)->Type )
   {
    case SNODE_STAR:
    case SNODE_R:
    case SNODE_R2:
    case SNODE_R3:
		Item=LeftStack->Next( Item );
		continue;
    default:
		break;
   }
   break;
  }

  // print !R(s)
  while( Item )
  {
   switch( ((SyntactNode far *)Item)->Type )
   {
    case SNODE_STAR:
    case SNODE_R:
    case SNODE_R2:
    case SNODE_R3:
		break;

    case SNODE_RPARENT:
		((SyntactNode far *)Item)->Print(); // print content of tree

		if( Item->Next != LeftStack->Tail
		     &&
		    ((SyntactNode far *)Item->Next)->Type!=SNODE_LBRACE
		  )
		 {
		  printf("\n\n  Error: Missing \'{\'.");
		  exit(0);
		 }
		else
		{
		 Item=LeftStack->Next( Item );
		 continue;
		}
    case SNODE_H:
		((SyntactNode far *)Item)->Print(); // print content of tree

		if( Item->Next != LeftStack->Tail
		     &&
		    ((SyntactNode far *)Item->Next)->Type!=SNODE_LPARENT
		  )
		 {
		  printf("\n\n  Error: Missing \'(\'.");
		  exit(0);
		 }
		else
		{
		 Item=LeftStack->Next( Item );
		 continue;
		}

    case SNODE_K:
		((SyntactNode far *)Item)->Print(); // print content of tree

		if( Item->Next != LeftStack->Tail
		     &&
		    ((SyntactNode far *)Item->Next)->Type!=SNODE_EQU
		  )
		 {
		  printf("\n\n  Error: Missing \'=\'.");
		  exit(0);
		 }
		else
		{
		 Item=LeftStack->Next( Item );
		 continue;
		}

    case SNODE_S:
		((SyntactNode far *)Item)->Print(); // print content of tree

		if( Item->Next != LeftStack->Tail
		     &&
		    ((SyntactNode far *)Item->Next)->Type!=SNODE_SEMICOLON
		     &&
		    ((SyntactNode far *)Item->Next)->Type!=SNODE_RPARENT
		  )
		 {
		  // missing ) or ;
		  // recognize what's before:
		  //  H(S   ->  )
		  //  K=S   ->  ;

		  if( ((SyntactNode far *)Item->Last)->Type==SNODE_LPARENT )
		   printf("\n\n  Error: Missing \')\'.");
		  else
		   printf("\n\n  Error: Missing \';\'.");

		  exit(0);
		 }
		else
		{
		 Item=LeftStack->Next( Item );
		 continue;
		}

    default:
		((SyntactNode far *)Item)->Print(); // print content of tree
		Item=LeftStack->Next( Item );
		continue;
   }
   break;
  }

  printf("\n\n  Error: Missing \'}\'.");

  exit(0);
 }

 // NO ERRORS -> SUCCESS

 // pop OK syntact tree which will be returned
 SyntactTree=((SyntactNode far *)(LeftStack->Pop()));

 return;
}

//-----------------------------------------------------------------------------

void Syntactator::ReduceLeftStack( void )
{

 while( ReduceLeftStackStep() ) ;

}

//-----------------------------------------------------------------------------

bool Syntactator::ReduceLeftStackStep( void )
{
 ListItem far 	 *Item;
 SyntactNode far *SNode;
 SyntactNode far *TempNode;
 SyntactNodeType TopOfStack[GRAMMARRIGHT_MAXRIGHT];
 word		 r;
 int		 i,
		 FromBack,
		 Height=0;	// TopOfStack height <0,MAXRIGHT>
 GrammarRight    *Right;
 bool		 Success;

 // create top of stack in string form

 FromBack    	= GRAMMARRIGHT_MAXRIGHT-1;
 Item 		= LeftStack->GetTail();

 // null TopOfStack
 for( i=0; i<GRAMMARRIGHT_MAXRIGHT; i++ ) TopOfStack[i]=SNODE_NULL;

 while( Item )
 {
  switch( (( SyntactNode far *)Item)->Type )
  {
   case SNODE_R2:
   case SNODE_R3:
		TopOfStack[ FromBack-- ] = SNODE_R;
		break;
   case SNODE_T4:
   case SNODE_T5:
		TopOfStack[ FromBack-- ] = SNODE_T;
		break;
   default:
		TopOfStack[ FromBack-- ] = (( SyntactNode far *)Item)->Type;
  }

  // inc Height of TopOfStack
  if( ++Height>=GRAMMARRIGHT_MAXRIGHT ) break; // its enough, I do not need more than MAXRIGHT

  Item=LeftStack->Last( Item);
 }


 // check what's on the top of stack with grammar rights
 // DO NOT USE RIGHT 1 -> IS NOT IMPORTANT, REDUCE TO ONE "R"
 for( r=2; r<=NR_OF_GRAMMAR_RIGHTS; r++ )
 {
  switch( r )
  {
   case 2:
	  Right=&GRight2;
	  break;
   case 3:
	  Right=&GRight3;
	  break;
   case 4:
	  Right=&GRight4;
	  break;
   case 5:
	  Right=&GRight5;
	  break;
   case 6:
	  Right=&GRight6;
	  break;
   case 7:
	  Right=&GRight7;
	  break;
   case 8:
	  Right=&GRight8;
	  break;
   default:
	  EXIT_PROGRAM;
  }

  // roll through the right part of the gright ( from end to front )

  // can not reduce, top is shorter than the right part of the gright
  if( Right->LngOfR > Height )
   continue;	// take the next gright

  Success=TRUE;

  for( i =  GRAMMARRIGHT_MAXRIGHT - 1;
       i >= (int)(GRAMMARRIGHT_MAXRIGHT - Right->LngOfR);
       i--
     )
  {
   if( Right->R[i] != TopOfStack[i] )
   {
    Success=FALSE;
    break;
   }
  }

  // top of stack OK ( can be reduced )?
  if( Success )
  {
   // REDUCE ( pop from stack from BACK TO FRONT )
   switch( r )
   {
    case 2:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_R2;
	   SNode->Data.NeterminalR2.Rr =((SyntactNode far *)(LeftStack->Pop()));
	   SNode->Data.NeterminalR2.Rl =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
    case 3:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_R3;
	   TempNode		       =((SyntactNode far *)(LeftStack->Pop()));
	    delete TempNode; // its "}" no longer needed
	   SNode->Data.NeterminalR3.T =((SyntactNode far *)(LeftStack->Pop()));
	   TempNode		       =((SyntactNode far *)(LeftStack->Pop()));
	    delete TempNode; // its "{" no longer needed
	   TempNode		       =((SyntactNode far *)(LeftStack->Pop()));
	    delete TempNode; // its ")" no longer needed
	   SNode->Data.NeterminalR3.S =((SyntactNode far *)(LeftStack->Pop()));
	   TempNode		       =((SyntactNode far *)(LeftStack->Pop()));
	    delete TempNode; // its "(" no longer needed
	   SNode->Data.NeterminalR3.H =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
    case 4:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_T4;
	   TempNode		       =((SyntactNode far *)(LeftStack->Pop()));
	    delete TempNode; // its ";" no longer needed
	   SNode->Data.NeterminalT4.S =((SyntactNode far *)(LeftStack->Pop()));
	   TempNode		       =((SyntactNode far *)(LeftStack->Pop()));
	    delete TempNode; // its "=" no longer needed
	   SNode->Data.NeterminalT4.K =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
    case 5:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_T5;
	   SNode->Data.NeterminalT5.Tr =((SyntactNode far *)(LeftStack->Pop()));
	   SNode->Data.NeterminalT5.Tl =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
    case 6:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_K;
	   SNode->Data.NeterminalK.k =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
    case 7:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_S;
	   SNode->Data.NeterminalS.s =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
    case 8:
	  SNode = new SyntactNode; if(!SNode) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	   SNode->Type 	 	       =SNODE_H;
	   SNode->Data.NeterminalH.h =((SyntactNode far *)(LeftStack->Pop()));

	   LeftStack->Push( SNode );
	  break;
   }



   return TRUE;
  }

  // else try the next gright
 }

 return FALSE;
}

//- EOF ----------------------------------------------------------------------

