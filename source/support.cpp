#include "source\support.h"

// print info when hunting bugs
// #define DEBUG

//- Beep -----------------------------------------------------------------------

void Beep( void )
{
 sound(200);
 delay(300);
 nosound();
 delay(150);
}

//- KeybWait -----------------------------------------------------------------------

void KeybWait( void )
{
 while( kbhit() ); getch();
 getch();
}

//- KeybClear -----------------------------------------------------------------------

void KeybClear( void )
{
 while( kbhit() ) getch();
}

//- EOF -----------------------------------------------------------------------




