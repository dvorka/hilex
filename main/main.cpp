//-----------------------------------------------------------------------------
//
//                                 HILEX
//
//                                by Dvorka
//
//                                 1998
//
//-----------------------------------------------------------------------------



#include <alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "source\buffer.h"
#include "source\dynauto.h"
#include "source\hitypes.h"
#include "source\hililex.h"
#include "source\hilisyn.h"
#include "source\hilisem.h"
#include "source\prefix.h"
#include "source\regtree.h"
#include "source\support.h"
#include "source\cocon.h"

// print info when hunting bugs
// #define DEBUG

// #define CHECK_NR_OF_ARGS

extern unsigned _stklen = 30000u;

bool            Verbose    = TRUE, 	// write info
                OutToStd   = FALSE,
                HTMLOutput = TRUE,
                CRLF       = FALSE; // output not to file but to stdout

int main( int argc, char *argv[] )
{

 #ifdef DEBUG
 dword FarFreeMemoryAtStart=(unsigned long)farcoreleft();

 printf("\n\n                               * HILEX *"
          "\n                                  by"
          "\n                                Dvorka"
          "\n"
          "\n                 Compiled at %s, %s\n\n", __DATE__, __TIME__
       );
 printf(  "                  Starting with far heap: %luB \n", FarFreeMemoryAtStart );

 #endif

 randomize();







 //----------------------------------------------------------------

 Syntactator far *SYNTactator;
 SyntactNode far *SyntactTree;
 Semantator  far *SEMantator;
 COCONator   far *Coconator;
 time_t          StartTime,
                 FinishTime;
 word            i;
 char            WorkType;

 if( argc >= 2 )
 {
  // analyze switch
  if( argv[1][0]!='-' )
  {
   printf("\n ERROR reported from %s line %d:"
          "\n  Argument \"%s\" is not a switch...", __FILE__, __LINE__, argv[1] );
   return 0;
  }

  if( argv[1][1]==0 )
  {
   printf("\n ERROR reported from %s line %d:"
          "\n  Switch \"%s\" is empty...", __FILE__, __LINE__, argv[1] );
   return 0;
  }

  // analyze the rest of switch
  i=1;
  WorkType=0;

  while( argv[1][i]!=0 )
  {
   switch( argv[1][i] )
   {
    case 'a':
    case 't':
    case 'c':
    case 'h':
             if( WorkType == 0 )
              WorkType=argv[1][i];
             else
             {
              printf("\n ERROR reported from %s line %d:"
                     "\n  Only one of a/t/c on command line can be set...", __FILE__, __LINE__ );
              return 0;
             }

             break;

    case 'o':
              OutToStd=TRUE;
              Verbose=FALSE;
             break;
    case 'v':
              Verbose=FALSE;
             break;
   }

   i++;
  }
 } // if


 if( argc>=3 )
 {
  switch( WorkType )
  {
   case 'h':
   case 'H':
   case '?':
             break;

   case 'c':    // compile HLX to BCA
   case 'C':
	     #ifdef CHECK_NR_OF_ARGS
	     if( argc!=3 )
	     {
	      printf("\n ERROR reported from %s line %d:"
		     "\n  Bad number of arguments...", __FILE__, __LINE__ );
	      return 0;
	     }
	     #endif

             // argv[2] is file which should be compiled
             if( !strstr( argv[2], ".hlx" ) )
             {
              printf("\n ERROR reported from %s line %d:"
                     "\n  Input file must be *.HLX file, %s is not...", __FILE__, __LINE__, argv[2] );
              return 0;
             }

             if( Verbose ) printf("\n Compiling %s to binary form\n   ", argv[2] );

	     SYNTactator=new Syntactator( strlwr(argv[2]) ); if(!SYNTactator) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
	     SyntactTree=NULL;

	     SYNTactator->LRAnalyzer( SyntactTree );

	     delete SYNTactator;



	     argv[2][strlen(argv[2])-3]=0;
	     strcat( argv[2], "bca" );



	     SEMantator=new Semantator( SyntactTree ); if(!SEMantator) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

             SEMantator->SyntactTreeToDynamicTable();

             SEMantator->Save( argv[2] );
             if( Verbose ) printf("->\n compiled to %s...", argv[2] );

             delete SEMantator;

             if( Verbose ) printf("\n\n Bye!\n\n");

             #if !defined(DEBUG)
              return 0;
             #else
              break;
             #endif

   case 't':    // translate using BCA
   case 'T':
	     #ifdef CHECK_NR_OF_ARGS
	     if( (!OutToStd&&argc!=5) || (OutToStd&&argc!=4))
	     {
	      printf("\n ERROR reported from %s line %d:"
		     "\n  Bad number of arguments...", __FILE__, __LINE__ );
	      return 0;
	     }
	     #endif

             // argv[2] is file which should be compiled
             if( !strstr( argv[2], ".bca" ) )
             {
              printf("\n ERROR reported from %s line %d:"
                     "\n  Hilex binary file must be *.bca file, %s is not...", __FILE__, __LINE__, argv[2] );
              return 0;
             }

	     Coconator=new COCONator( strlwr(argv[2]) ); if(!Coconator) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

             StartTime = time(NULL);
             if( Verbose ) printf("\n Coconizing\n   ");

              if( OutToStd )
               Coconator->Coconize( strlwr(argv[3]), "hilex.tmp" );
              else
               Coconator->Coconize( strlwr(argv[3]), strlwr(argv[4]) );

             FinishTime = time(NULL);

             delete Coconator;

             if( Verbose ) printf("->\n coconized to %s in %ld sec(s)..."
                                  "\n\n Bye!\n\n"
                                  , argv[4], FinishTime-StartTime+1l
                                 );

             if( OutToStd )
              unlink( "hilex.tmp" );

             #if !defined(DEBUG)
              return 0;
             #else
              break;
             #endif

   case 'a':    // compile hlx to bca and then translate
   case 'A':
	     #ifdef CHECK_NR_OF_ARGS
	     if( (!OutToStd&&argc!=5) || (OutToStd&&argc!=4))
	     {
	      printf("\n ERROR reported from %s line %d:"
		     "\n  Bad number of arguments...", __FILE__, __LINE__ );
	      return 0;
	     }
	     #endif

             // argv[2] is file which should be compiled
             if( !strstr( argv[2], ".hlx" ) )
             {
              printf("\n ERROR reported from %s line %d:"
                     "\n  Compilation input file must be *.HLX file, %s is not...", __FILE__, __LINE__, argv[2] );
              return 0;
             }

             if( Verbose ) printf("\n Compiling %s to binary form\n   ", argv[2] );
	     SYNTactator=new Syntactator( strlwr(argv[2]) ); if(!SYNTactator) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }
             SyntactTree=NULL;

              SYNTactator->LRAnalyzer( SyntactTree );

             delete SYNTactator;



             argv[2][strlen(argv[2])-3]=0;
             strcat( argv[2], "bca" );



	     SEMantator=new Semantator( SyntactTree ); if(!SEMantator) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

             SEMantator->SyntactTreeToDynamicTable();

             SEMantator->Save( argv[2] );
             if( Verbose ) printf("->\n compiled to %s...", argv[2] );

             delete SEMantator;



	     Coconator=new COCONator( strlwr(argv[2]) ); if(!Coconator) { printf("\n ERROR reported from %s line %d:\n Not enough far memory...", __FILE__, __LINE__ ); exit(0); }

             StartTime = time(NULL);
             if( Verbose ) printf("\n Coconizing\n   ");

              if( OutToStd )
               Coconator->Coconize( strlwr(argv[3]), "hilex.tmp" );
              else
               Coconator->Coconize( strlwr(argv[3]), strlwr(argv[4]) );

             FinishTime = time(NULL);

             delete Coconator;

             if( Verbose ) printf("->\n coconized to %s in %ld sec(s)..."
                                  "\n\n Bye!\n\n"
                                  , argv[4], FinishTime-StartTime+1l
                                 );

             if( OutToStd )
              unlink( "hilex.tmp" );

             #if !defined(DEBUG)
              return 0;
             #else
              break;
             #endif

   default:
             printf("\n ERROR reported from %s line %d:"
                    "\n  Bad switch %s...", __FILE__, __LINE__, argv[1] );

             #if !defined(DEBUG)
              return 0;
             #endif

  }
 } // if

 #ifndef DEBUG

 printf("\n"
	"\n   HILEX by Dvorka --> Version 1.0173 compiled at %s, %s"
	"\n                      EMail: UltraDvorka@post.cz"
	"\n Command line help:"
	"\n   hilex <-[c|h|H|?|t|a][o][v]> <HLX/BCA> <infile> <outfile>"
        "\n Usage:"
	"\n   hilex -h or hilex -H or hilex -?"
	"\n      ... for help"
        "\n   hilex -c <*.HLX>"
        "\n      ... compiles HLX file to BCA (binary form)"
        "\n   hilex -t <*.BCA> <infile> <outfile>"
        "\n      ... translates infile to outfile using binary BCA"
        "\n   hilex -a <*.HLX> <infile> <outfile>"
        "\n      ... compiles HLX file to BCA and then translates as in -t"
        "\n   hilex -cv <*.HLX>"
        "\n      ... add \'v\' to switch to run program in \"off verbose\" mode"
        "\n          ( example for -c )"
        "\n   hilex -ot <*.BCA>  <infile>"
        "\n      ... add \'o\' to a/t in switch to redirect output to STDOUT"
        "\n          specify ONLY input file, verbose off is set by default"
        "\n          ( example for -t )"
        "\n"
	,
        __DATE__,
        __TIME__
       );

  return 0;
 #endif

 //----------------------------------------------------------------





 #ifdef DEBUG
  printf("\n\n Memory statistic:");

 if( FarFreeMemoryAtStart!=((unsigned long) farcoreleft()))
 {
   printf("\n  Deallocation ERROR:");
  Beep(); Beep(); Beep();
 }
 else
  Beep();

 printf(
        "\n  Far free at start: %lu"
        "\n  and now          : %lu"
        "\n"
        ,
        FarFreeMemoryAtStart,
        ((unsigned long) farcoreleft())
       );

 printf("\n Bye! \n\n");

  return 0;
 #endif

}

//- EOF -----------------------------------------------------------------------

