#ifndef __PREFIX_H
 #define __PREFIX_H


 #include <alloc.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>

 #include "source\hierrors.h"
 #include "source\hitypes.h"


 class far RegularString
 {

  public:

   RegularString( char far *iInfix );
    word ToPrefix( void );
   ~RegularString();

   char far *Infix, // infix not deleted -> use it when reporting error
	far *Prefix;

  private:
   bool SnortleBackShlash( char far *Where, word Off );
   word BubbleLeft( word &Off, word Shift );
   word SayRepeaterLng( char far *Where, word Off, word &Lng, word &Diff );
   word TransformRepeater( word SrcOff, word DstOff );
 };

#endif
