#include "source\synode.h"

// print info when hunting bugs
// #define DEBUG

//- class SyntactNode methods ----------------------------------------------------------------------------

void SyntactNode::DeleteNodeElements( void )
// - goes through the tree and frees s.Element and i.Element
//   this function is used for explicit deleting of this elements
//   In normal case are deleted in semantic analyzer
{
 switch( Type )
 {
  case SNODE_R2:
	 Data.NeterminalR2.Rl->DeleteNodeElements();
	 Data.NeterminalR2.Rr->DeleteNodeElements();
	return;
  case SNODE_R3:
	 Data.NeterminalR3.H->DeleteNodeElements();
	 Data.NeterminalR3.S->DeleteNodeElements();
	 Data.NeterminalR3.T->DeleteNodeElements();
	return;
  case SNODE_T4:
	 Data.NeterminalT4.K->DeleteNodeElements();
	 Data.NeterminalT4.S->DeleteNodeElements();
	return;
  case SNODE_T5:
	 Data.NeterminalT5.Tl->DeleteNodeElements();
	 Data.NeterminalT5.Tr->DeleteNodeElements();
	return;
  case SNODE_K:
	 Data.NeterminalK.k->DeleteNodeElements();
	return;
  case SNODE_S:
	 Data.NeterminalS.s->DeleteNodeElements();
	return;
  case SNODE_H:
	 Data.NeterminalH.h->DeleteNodeElements();
	return;
  case SNODE_k: // leaf
	 // contains nothing dynamic -> return back
	return;
  case SNODE_s: // leaf
	 // delete element
	 if( Data.s.Element != NULL )
	  delete Data.s.Element;
	return;
  case SNODE_h: // leaf
	 // contains nothing dynamic -> return back
	return;

  case SNODE_LBRACE:
  case SNODE_RBRACE:
  case SNODE_LPARENT:
  case SNODE_RPARENT:
  case SNODE_SEMICOLON:
  case SNODE_EQU:
  case SNODE_NULL:
  case SNODE_STAR:
  case SNODE_R:
  case SNODE_T:

	 printf("\n ERROR reported from %s line %d:"
		"\n  Usually not created node %d in DeleteNodeElements!",
		__FILE__,
		__LINE__,
		 Type
	       );
	 exit(0);

  default:

	 // unknown node -> probably error
	 printf("\n ERROR reported from %s line %d:"
		"\n  Unknown node %d probably error!",
		__FILE__,
		__LINE__,
		Type
	       );
	 exit(0);
 }
}

//-----------------------------------------------------------------------------

void SyntactNode::Print( void )
// backtrack destructor
{
 switch( Type )
 {
  case SNODE_R2:
	 if( Data.NeterminalR2.Rl ) Data.NeterminalR2.Rl->Print();
	 if( Data.NeterminalR2.Rr ) Data.NeterminalR2.Rr->Print();
	return;
  case SNODE_R3:
	 if( Data.NeterminalR3.H ) Data.NeterminalR3.H->Print();
	 if( Data.NeterminalR3.S ) Data.NeterminalR3.S->Print();
	 if( Data.NeterminalR3.T ) Data.NeterminalR3.T->Print();
	return;
  case SNODE_T4:
	 if( Data.NeterminalT4.K ) Data.NeterminalT4.K->Print();
	 printf("=");
	 if( Data.NeterminalT4.S ) Data.NeterminalT4.S->Print();
	 printf(";\n");
	return;
  case SNODE_T5:
	 if( Data.NeterminalT5.Tl ) Data.NeterminalT5.Tl->Print();
	 if( Data.NeterminalT5.Tr ) Data.NeterminalT5.Tr->Print();
	return;
  case SNODE_K:
	 if( Data.NeterminalK.k ) Data.NeterminalK.k->Print();
	return;
  case SNODE_S:
	 if( Data.NeterminalS.s ) Data.NeterminalS.s->Print();
	return;
  case SNODE_H:
	 if( Data.NeterminalH.h ) Data.NeterminalH.h->Print();
	return;
  case SNODE_k: // leaf
	 // keywords must be printed
	 switch( Data.k.Key )
	 {
	  case LEXICATOR_DESCR:       // keywords
		printf(" Descr");
	       return;
	  case LEXICATOR_RAWBEG:
		printf(" Raw_beg");
	       return;
	  case LEXICATOR_RAWEND:
		printf(" Raw_end");
	       return;
	  case LEXICATOR_FONT:
		printf(" Font");
	       return;
	  case LEXICATOR_SIZE:
		printf(" Size");
	       return;
	  case LEXICATOR_COLOR:
		printf(" Color");
	       return;
	  case LEXICATOR_UNDERSCORE:
		printf(" Underscore");
	       return;
	  case LEXICATOR_ITALIC:
		printf(" Italic");
	       return;
	  case LEXICATOR_BOLD:
		printf(" Bold");
	       return;
	  case LEXICATOR_OUTPUT:
		printf(" Output");
	       return;
	  case LEXICATOR_CRLF:
		printf(" CRLF");
	       return;
	  case LEXICATOR_HTMLOUTPUT:
		printf(" HTMLOut");
	       return;
	  case LEXICATOR_TITLE:
		printf(" Title");
	       return;
	  case LEXICATOR_FACEFONT:
		printf(" Face_font");
	       return;
	  case LEXICATOR_BASECOLOR:
		printf(" BaseColor");
	       return;
	  case LEXICATOR_BASESIZE:
		printf(" BaseSize");
	       return;
	  case LEXICATOR_BGCOLOR:
		printf(" BgColor");
	       return;
	  case LEXICATOR_BACKGROUND:
		printf(" Background");
	       return;
	  case LEXICATOR_WIDTH:
		printf(" Width");
	       return;
	  default:
	       return;
	 }
  case SNODE_s: // leaf
	  printf("%s", Data.s.Element );
	return;
  case SNODE_h: // leaf
	 // head keywords must be printed
	 switch( Data.h.Key )
	 {
	  case LEXICATOR_SETUP:       	// head keywords
		printf(" Setup");
	       return;
	  case LEXICATOR_ELEMENT:       // head keywords
		printf(" Element");
	       return;
	  default:
	       return;
	 }
  case SNODE_LBRACE:
	  printf("\n{\n");
	return;
  case SNODE_RBRACE:
	  printf("\n}\n");
	return;
  case SNODE_LPARENT:
	  printf("(");
	return;
  case SNODE_RPARENT:
	  printf(")");
	return;
  case SNODE_SEMICOLON:
	  printf(";\n");
	return;
  case SNODE_EQU:
	  printf("=");
	return;
  case SNODE_NULL:
	return;
  case SNODE_STAR:
  case SNODE_R:
  case SNODE_T:
	 return;

  default:

	 // unknown node -> probably error
	 printf("\n ERROR reported from %s line %d:"
		"\n  Unknown node %d probably error!",
		__FILE__,
		__LINE__,
		Type
	       );
	 exit(0);
 }
}

//-----------------------------------------------------------------------------

SyntactNode::~SyntactNode( void )
// backtrack destructor
{
 switch( Type )
 {
  case SNODE_R2:
	 #ifdef DEBUG
	  printf("\n v Rl %d", Type );
	 #endif
	 if( Data.NeterminalR2.Rl ) delete Data.NeterminalR2.Rl;
	 #ifdef DEBUG
	  printf("\n v Rr %d", Type );
	 #endif
	 if( Data.NeterminalR2.Rr ) delete Data.NeterminalR2.Rr;
	return;
  case SNODE_R3:
	 #ifdef DEBUG
	  printf("\n v H %d", Type );
	 #endif
	 if( Data.NeterminalR3.H ) delete Data.NeterminalR3.H;
	 #ifdef DEBUG
	  printf("\n v S %d", Type );
	 #endif
	 if( Data.NeterminalR3.S ) delete Data.NeterminalR3.S;
	 #ifdef DEBUG
	  printf("\n v T %d", Type );
	 #endif
	 if( Data.NeterminalR3.T ) delete Data.NeterminalR3.T;
	return;
  case SNODE_T4:
	 #ifdef DEBUG
	  printf("\n v I %d", Type );
	 #endif
	 if( Data.NeterminalT4.K ) delete Data.NeterminalT4.K;
	 #ifdef DEBUG
	  printf("\n v S %d", Type );
	 #endif
	 if( Data.NeterminalT4.S ) delete Data.NeterminalT4.S;
	return;
  case SNODE_T5:
	 #ifdef DEBUG
	  printf("\n v Tl %d", Type );
	 #endif
	 if( Data.NeterminalT5.Tl ) delete Data.NeterminalT5.Tl;
	 #ifdef DEBUG
	  printf("\n v Tr %d", Type );
	 #endif
	 if( Data.NeterminalT5.Tr ) delete Data.NeterminalT5.Tr;
	return;
  case SNODE_K:
	 #ifdef DEBUG
	  printf("\n v i %d", Type );
	 #endif
	 if( Data.NeterminalK.k ) delete Data.NeterminalK.k;
	return;
  case SNODE_S:
	 #ifdef DEBUG
	  printf("\n v s %d", Type );
	 #endif
	 if( Data.NeterminalS.s ) delete Data.NeterminalS.s;
	return;
  case SNODE_H:
	 #ifdef DEBUG
	  printf("\n v h %d", Type );
	 #endif
	 if( Data.NeterminalH.h ) delete Data.NeterminalH.h;
	return;
  case SNODE_k: // leaf
	 // DO NOT delete element ( deleted in semantic )
	 // deepest point of backtrack
	 #ifdef DEBUG
	  printf("\n \"%s\" bottom -> returning back", Data.i.Element );
	 #endif
	return;
  case SNODE_s: // leaf
	 // DO NOT delete element ( deleted in semantic )
	 // deepest point of backtrack
	 #ifdef DEBUG
	  printf("\n \"%s\" bottom -> returning back", Data.s.Element );
	 #endif
	return;
  case SNODE_h: // leaf
	 // DO NOT delete element ( deleted in semantic )
	 // deepest point of backtrack
	 #ifdef DEBUG
	  printf("\n \"%s\" bottom -> returning back", Data.h.Element );
	 #endif
	return;

  case SNODE_LBRACE:
  case SNODE_RBRACE:
  case SNODE_LPARENT:
  case SNODE_RPARENT:
  case SNODE_SEMICOLON:
  case SNODE_EQU:
  case SNODE_NULL:
  case SNODE_STAR:
  case SNODE_R:
  case SNODE_T:

	 // do not delete nothing AND do not dive
	 #ifdef DEBUG
	  printf("\n deleting node %d", Type );
	 #endif
	 return;

  default:

	 // unknown node -> probably error
	 printf("\n ERROR reported from %s line %d:"
		"\n  Unknown node %d probably error!",
		__FILE__,
		__LINE__,
		Type
	       );
	 exit(0);
 }
}

//- EOF ----------------------------------------------------------------------------
