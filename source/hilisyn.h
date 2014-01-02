#ifndef __HILISYN_H
 #define __HILISYN_H

 #include "source\hierrors.h"
 #include "source\hilist.h"
 #include "source\hitypes.h"
 #include "source\synode.h"

 //- Info ----------------------------------------------------------------------

 /*

  Syntax grammar of Hilex configuration file:

   *  ... initial symbol
   -| ... end of input

   * -> R -|                    // in LR analyzer this gright is NOT used
   R -> RR

   R -> H(S){T}
   T -> K=S;
   T -> TT

   S -> string                  // s
   K -> keyword                 // k
   H -> head                    // h

 */

 //- struct GrammarRight ---------------------------------------------------------

 #define GRAMMARRIGHT_MAXRIGHT          7

 struct far _GrammarRight
 {
  SyntactNodeType L;
  SyntactNodeType R[GRAMMARRIGHT_MAXRIGHT];
  word LngOfR;
 };

 typedef _GrammarRight GrammarRight;

 // list of rights is in HILISYN.CPP

 #include "source\hililex.h"

 //- class Syntactator ---------------------------------------------------------

 class far _Syntactator
 // right stack is input file
 // left stack is stack where I am reduce using grammar rights
 {
  private:

   List         far *LeftStack;         // left stack - list of syntact trees
   Lexicator    far *RightStack;        // input file divided by lexicator

  public:

   _Syntactator( char *InputFileName );

    void LRAnalyzer( SyntactNode far *&SyntactTree );
                                        // checks input and
                                        // builds syntact tree and returns
                                        // pointer on it

    void Cancel( void ) { RightStack->CancelPop(); };
                                        // if some error occure frees lstack
    ~_Syntactator( void );

  private:

    void ReduceLeftStack( void );       // reduces left stack using grights
    bool ReduceLeftStackStep( void );

 };

 typedef _Syntactator Syntactator;

#endif