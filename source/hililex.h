#ifndef __HILILEX_H
 #define __HILILEX_H

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
 #include "source\synode.h"

 //---------------------------------------------------------------------------

 bool IsWhiteSpace( char C );

 //- class Lexicator ---------------------------------------------------------

 class far _Lexicator
 {
  private:

   StaticAutomat far *Akceptor;

   // for POP
   Buffer far 	*Buf;
   dword  	Beg,
		End,
		OldBeg;

  public:

   _Lexicator( void );
    word SniftSnift( char *InFileName, char *OutFileName );
    void PrepareForPop( char *InFileName ); // automaticly cleans mem after the last pop
    void CancelPop( void );	// call only if you want to finish before input is worked out
    bool Pop( SyntactNode *&Element );
    bool CorrectFinish( dword &ErrPos );
   ~_Lexicator();

  private:

   word ConstructBuildInHiliLex( void );

   bool Correct; // to recognize, that there is not finished input
 };

 typedef _Lexicator Lexicator;

#endif