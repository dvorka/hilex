#include "source\prefix.h"

// print info when hunting bugs
// #define DEBUG

// #define DELETE_INFIX

#define EXIT_PROGRAM exit(0)

//- RegularString methods ----------------------------------------------------

RegularString::RegularString( char far *iInfix )
{
 Infix=iInfix;
 Prefix=NULL; // space allocated in time ToPrefix
}

//--------------------------------------------------------------------------------------

bool RegularString::SnortleBackShlash( char far *Where, word Off )
// Gets pointer to some char, counts \ sequence if there is
// if nr. of \ is odd => \ is valid                  => TRUE ... vycuchal jsem \
// if nr. of \ is even => \ not valid it's \ itself  => FALSE
// Examples when moving operation * where it should be:
//      a* 	... 	   *a
//     \a*      ... 	  *\a
//    \\a*	... 	 \\*a
//   \\\a*	... 	\\*\a
//  \\\\a*	...    \\\\*a
// \\\\\a*	...   \\\\*\a
{
 word RetVal=FALSE;

 if( Off==0xFFFF ) return FALSE;	// called with 0-1 parameter

 for( ; Off>0; Off-- )
 {
  if( Where[Off]=='\\' )
   RetVal=!RetVal;
  else
   break;
 }

 if( Where[Off]=='\\' )		// treats Off==0
  RetVal=!RetVal;

 return RetVal;
}

//--------------------------------------------------------------------------------------

word RegularString::BubbleLeft( word &Off, word Shift )
//
// function:
//  bubbles left
//  stops on corresponding '(' or '[' or 'A'
//  what's right is moved Shift positions right
//
// in Off returns the first free position from left in hole which was
// created ( in Prefix )
{


 word
      h=strlen(Prefix)-1,	// point to last char in Prefix
      p=0,
      Pars=0,
      PreLng;


 // if last char is parent and has valid \ before, its default
 // ( parent has common mean )

 if( SnortleBackShlash( Prefix, h-1 ) )
 {
  // before character is \ =>  do this:
  // \(NULL
  // ______\(NULL

  Prefix[h+1 +Shift]=0;  		// set NULL;
  Prefix[h   +Shift]=Prefix[h]; 	// move char
  Prefix[h-1 +Shift]='\\'; 		// move \

  Off=h-1;

  return HI_OK;
 };

 // now treat other cases...

 switch( Prefix[h] )
 {
  case ')':

	   // h points to valid )
	   Pars=1;

	   while( Pars )
	   {
	    if( h>0 )
	     h--;     	// move one char left
	    else
	    {
	     printf("\n ERROR reported from %s line %d:"
		    "\n  in RegExp \"%s\" missing (.", __FILE__, __LINE__, Infix );
	     EXIT_PROGRAM;
	    }

	    switch( Prefix[h] )
	    {
	     case ')':
		       if( !SnortleBackShlash( Prefix, h-1 ) ) // there's no valid \
			Pars++;

		       break;

	     case '(':
		       if( !SnortleBackShlash( Prefix, h-1 ) ) // there's no valid \
			Pars--;

		       break;
	    } // switch
	   }  // while

	   // now h points to left bracket => move this bracket and
	   // everything what's right Shift positions right
	   Off=h;

	   PreLng=strlen( Prefix ); 	// points to NULL because array counted from 0

	   for( p=PreLng+Shift; p>=h+Shift; p--) // move substring, NULL too
	    Prefix[p]=Prefix[p-Shift];

	   #ifdef DEBUG
	   // printf("\n  Bubble result: \"%s\" HoleBegAt: %u", Prefix, Off );
	   #endif

  return HI_OK;

  case ']':
	   // h points to valid ]
	   Pars=1;

	   while( Pars )
	   {
	    if( h>0 )
	     h--;     	// move one char left
	    else
	    {
	     printf("\n ERROR reported from %s line %d:"
		    "\n  in RegExp \"%s\" missing [.", __FILE__, __LINE__, Infix );
	     EXIT_PROGRAM;
	    }

	    if( Prefix[h] == '[' && !SnortleBackShlash( Prefix, h-1 ) )
	      Pars--;

	   }  // while

	   // now h points to left bracket => move this bracket and
	   // everything what's right Shift positions right
	   Off=h;

	   PreLng=strlen( Prefix ); 	// points to NULL because array counted from 0

	   for( p=PreLng+Shift; p>=h+Shift; p--) // move substring, NULL too
	    Prefix[p]=Prefix[p-Shift];

	   #ifdef DEBUG
	   // printf("\n  Bubble result: \"%s\" HoleBegAt: %u", Prefix, Off );
	   #endif

  return HI_OK;

  default: // some switch-character:  a*

	   // h points to last char in prefix => to char

	   if( !SnortleBackShlash( Prefix, h-1 ) ) // there's no \
	   {
	    Prefix[h+1 +Shift]=0;  		// set NULL;
	    Prefix[h   +Shift]=Prefix[h]; 	// move char

	    Off=h;
	   }
	   else
	   {
	    // before character is \ =>  do this:
	    // \aNULL
	    // ______\aNULL

	    Prefix[h+1 +Shift]=0;  		// set NULL;
	    Prefix[h   +Shift]=Prefix[h]; 	// move char
	    Prefix[h-1 +Shift]='\\'; 		// move \

	    Off=h-1;
	   }


	   #ifdef DEBUG
	   // printf("\n  Bubble result: \"%s\" HoleBegAt: %u", Prefix, Off );
	   #endif

  return HI_OK;

 } // switch

}

//--------------------------------------------------------------------------------------

word RegularString::SayRepeaterLng( char far *Where, word Off,
				    word &Lng,   word &Diff )
// gets offset of valid {, returns lng of repeater type {.,.}
// other repeaters lng is fixed, see TransformRepeater head comment
// difference is difference between original txt operation and new
{
 switch( Where[Off] )
 {
  case '?':
	   Lng=5;
	   Diff=4;
	   return HI_OK;
  case '+':
	   Lng=9;
	   Diff=8;
	   return HI_OK;
  case '*':
	   Lng=9;
	   Diff=8;
	   return HI_OK;
  case '{':
	   Off--;		// Off points to {, must be counted in
	   Lng=0;

	   do
	   {
	    ++Lng;
	   }
	   while( Where[Off+Lng]!='}' && Where[Off+Lng]!=0 );

	   if( Where[Off+Lng]!=0 )
	   {
	    Diff=Lng-1;
	    return Lng;
	   }
	   else
	   {
	    printf("\n ERROR reported from %s line %d:"
		   "\n  in RegExp \"%s\" missing }.", __FILE__, __LINE__, Infix );
	    EXIT_PROGRAM;
	   }

  default:
	  printf("\n ERROR reported from %s line %d:"
		 "\n  UFO repeater in RegExp \"%s\" off %u.", __FILE__, __LINE__, Infix, Off );
	  EXIT_PROGRAM;
 }
 return HI_OK;
}

//--------------------------------------------------------------------------------------

word RegularString::TransformRepeater( word SrcOff, word DstOff )
// Transforms repeaters to universal format - 6B wide. Text numbers are
// converted to binary form - word ( here in comments 15 means text number
//                                   and '15'is its binary form )
//
// Conversion is done this way:		( INFINITY==65535 )
// ?		...	{0,1}        	... 5
// +		...	{1,INFINITY}	... 9
// *		...	{0,INFINITY}	... 9
// {15}         ...     {15}
// {15,}        ...     {15,INFINITY}
// {14,15}      ...     {14,15
//
//
// SrcOff is offset of text form of repeater    ( in Infix  )
// DstOff is place where to store binary result ( in Prefix )
{
 word i;

 Prefix[DstOff  ]='{';

 switch( Infix[SrcOff] )
 {
  case '?':
	   Prefix[DstOff+1]='0';
	   Prefix[DstOff+2]=',';
	   Prefix[DstOff+3]='1';
	   Prefix[DstOff+4]='}';
	   break;

  case '+':
  case '*':
	   if( Infix[SrcOff]=='+')
	    Prefix[DstOff+1]='1';
	   else
	    Prefix[DstOff+1]='0';

	   Prefix[DstOff+2]=',';
	   Prefix[DstOff+3]='6';
	   Prefix[DstOff+4]='5';
	   Prefix[DstOff+5]='5';
	   Prefix[DstOff+6]='3';
	   Prefix[DstOff+7]='5';
	   Prefix[DstOff+8]='}';
	   break;

  case '{':
	   // errors are already fixed in SayRepeaterLng
	   i=0;

	   do
	   {
	    ++i;	// { already done, jump across
	    Prefix[DstOff+i]=Infix[SrcOff+i];
	   }
	   while( Infix[SrcOff+i]!='}' );

	   break;
 }

 return HI_OK;
}

//--------------------------------------------------------------------------------------

word RegularString::ToPrefix( void )
// goes through Infix, copies normal chars to Prefix. When some operation
// moves it to right place in order Prefix notation. Transforms repeaters
// to universal format
{
 #ifdef DEBUG
  printf("\n ToPrefix conversion:\n  \"%s\"", Infix );
 #endif

 // ------------------------------------------
 // - Allocating vector of the STATIC LENGHT -
 // ------------------------------------------

 Prefix=(char far *)farmalloc((dword)strlen(Infix)+3000lu); // after conversion realloc is made
			if( !Prefix ) { printf("\n ERROR in %s line %d: not enough FAR memory...", __FILE__, __LINE__ ); EXIT_PROGRAM; }
 #ifdef DEBUG
  else
   printf("\n Infix strlen %lu -> allocated bytes %lu...", (dword)strlen(Infix), (dword)strlen(Infix)+3000lu );
 #endif


 Prefix[0]=0;

 word i=0,      // offset in infix
      p=0,	// offset in prefix
      Off,
      Shift,
      Difference;

 while( Infix[i]!=0 )	// to the end of Infix
 {

  switch( Infix[i] )
  {
   case '\\': // tohle mi hlida \|, \* apod. => nemusim to testovat

	// move cross two chars - char after \ is ignored
	Prefix[p++]=Infix[i++];
	Prefix[p]=0;

	Prefix[p++]=Infix[i++];
	Prefix[p]=0;

	continue;

   case '|':

	if(i==0) // "|a" has no sense - missing left argument
	{
	 printf("\n ERROR reported from %s line %d:"
		"\n  In Descr=\"%s\"; missing left argument of OR operation.", __FILE__, __LINE__, Infix );
	 EXIT_PROGRAM;
	}

	BubbleLeft( Off, 1 );

	Prefix[Off]=Infix[i];		// sets |

	p++;   	// move right in Prefix
	i++;	// move right in Infix

	#ifdef DEBUG
	// printf("\n  ToPrefix step: \"%s\"", Prefix );
	#endif

	break;

   case '?':
   case '+':
   case '*':

	if(i==0)
	{
	 printf("\n ERROR reported from %s line %d:"
		"\n  In Descr=\"%s\"; missing left argument of repeater"
		"\n  -> there is nothing to repeate.", __FILE__, __LINE__, Infix );
	 EXIT_PROGRAM;
	}

   case '{':

	// make free space for prefix operation
	SayRepeaterLng( Infix, i, Shift, Difference );

	BubbleLeft( Off, Shift );

	TransformRepeater( i, Off ); // transform to universal format

	#ifdef DEBUG
	// printf("\n  ToPrefix step: \"%s\"", Prefix );
	#endif

	p+=Difference; // were add -> correcting difference
	p++;	       // move right in Prefix


	if( Infix[i]=='{') i+=Difference;
	i++; // move right in Infix

	break;

   case '}':
	if(i==0)
	{
	 printf("\n ERROR reported from %s line %d:"
		"\n  In Descr=\"%s\"; is \'}\' without \'{\'."
		, __FILE__, __LINE__, Infix );
	 EXIT_PROGRAM;
	}
   case ']':
	if(i==0)
	{
	 printf("\n ERROR reported from %s line %d:"
		"\n  In Descr=\"%s\"; is \']\' without \'[\'."
		, __FILE__, __LINE__, Infix );
	 EXIT_PROGRAM;
	}


   default:

	Prefix[p++]=Infix[i++];
	Prefix[p]=0;
  } // switch
 } // while

 #ifdef DEBUG
  printf("\n  -->\n  \"%s\"", Prefix );
 #endif

 // make realloc, if fails leave as it is
 char far *Ptr;
 Ptr = (char far *)farrealloc(Prefix,(dword)strlen(Prefix)+1lu);
 if( Ptr ) Prefix=Ptr;

 return HI_OK;
}

//--------------------------------------------------------------------------------------

RegularString::~RegularString()
{
 #ifdef DELETE_INFIX
  if( Infix )  farfree( Infix  );
 #endif

 if( Prefix ) farfree( Prefix );
}

//- EOF ------------------------------------------------------------------------


