#ifndef __HITYPES_H
 #define __HITYPES_H

 //- Common types ------------------------------------------------------------------

 typedef unsigned char bool;
 typedef unsigned char byte;
 typedef unsigned int  word;
 typedef unsigned long dword;


 #define TRUE  1
 #define FALSE !TRUE

 //- Data types ----------------------------------------------------------------

 enum ItemType		// types of tree nodes and boxes
 {
  ANONYM=0,
  SWITCH,		// tree, box
  REPEATER,             // tree, box
  OR,                   // tree, box
  CONTINUE,	     	//       box
  SPAINBOOT,            // tree
  UFO
 };

 typedef unsigned int AutoItemID;
 typedef unsigned int ElementID;

 enum ListSignature
 {
  IN,
  OK,
  NOK,
  NOT_A_LIST
 };

 enum SyntactNodeType
 {
  SNODE_NULL,
  SNODE_STAR,
  SNODE_R,
  SNODE_R2,
  SNODE_R3,
  SNODE_T,
  SNODE_T4,
  SNODE_T5,
  SNODE_K,
  SNODE_S,
  SNODE_H,
  SNODE_LBRACE,
  SNODE_RBRACE,
  SNODE_LPARENT,
  SNODE_RPARENT,
  SNODE_EQU,
  SNODE_SEMICOLON,
  SNODE_k,
  SNODE_s,
  SNODE_h
 };

 enum TokenType
 {
  TOKEN_GHOST,

  TOKEN_ELEMENT		=1,
  TOKEN_DESCR,
  TOKEN_ACTION,
  TOKEN_RAW,
  TOKEN_RAWBEG,
  TOKEN_RAWEND,
  TOKEN_FONT,
  TOKEN_SIZE,
  TOKEN_COLOR,
  TOKEN_UNDERSCORE,
  TOKEN_ITALIC,
  TOKEN_BOLD,

  TOKEN_SETUP,
  TOKEN_OUTPUT,
  TOKEN_CRLF,
  TOKEN_HTMLOUTPUT,
  TOKEN_TITLE,
  TOKEN_FACEFONT,
  TOKEN_BASECOLOR,
  TOKEN_BASESIZE,
  TOKEN_BGCOLOR,
  TOKEN_BACKGROUND,
  TOKEN_WIDTH,
  TOKEN_INTABLE,
  TOKEN_OUTTABLE
 };


 // Keywords MUST HAVE THE SAME ID AS IN TokenType - see Lexicator::Pop
 // order is important! (is used when sorting )
 enum LexicatorType
 {
  // same as in TokenType
  LEXICATOR_GHOST,

  // keywords
  LEXICATOR_ELEMENT	=1,		// head keyword
  LEXICATOR_DESCR,
  LEXICATOR_ACTION,
  LEXICATOR_RAW,
  LEXICATOR_RAWBEG,
  LEXICATOR_RAWEND,
  LEXICATOR_FONT,
  LEXICATOR_SIZE,
  LEXICATOR_COLOR,
  LEXICATOR_UNDERSCORE,
  LEXICATOR_ITALIC,
  LEXICATOR_BOLD,

  LEXICATOR_SETUP,			// head keyword
  LEXICATOR_OUTPUT,
  LEXICATOR_CRLF,
  LEXICATOR_HTMLOUTPUT,
  LEXICATOR_TITLE,
  LEXICATOR_FACEFONT,
  LEXICATOR_BASECOLOR,
  LEXICATOR_BASESIZE,
  LEXICATOR_BGCOLOR,
  LEXICATOR_BACKGROUND,
  LEXICATOR_WIDTH,
  LEXICATOR_INTABLE,
  LEXICATOR_OUTTABLE,

  // rest of lexical elements brace {}, parent ()
  LEXICATOR_LBRACE,
  LEXICATOR_RBRACE,
  LEXICATOR_SEMICOLON,
  LEXICATOR_EQU,
  LEXICATOR_STRING,
  LEXICATOR_COMMENTS,
  LEXICATOR_LPARENT,
  LEXICATOR_RPARENT
 };


 enum RecordType
 {
  RECORD_SETUP_TRANSLATION,
  RECORD_SETUP_PAGE,
  RECORD_NORMAL,	// means that encapsulate and there is color/size/...
  RECORD_RAW,		// means that encapsulate & rawbeg/rawend
  RECORD_REPLACE,	// means that replace => raw...
  RECORD_REMOVE,	// means that remove
  RECORD_NO_ACTION,	// means that no action

  RECORD_EMPTY
 };

 enum OutputType
 {
  NO_ENCAPS,
  HTML_TABLE,
  HTML_PAGE
 };

 enum ActionType
 {
  ACTION_NO,        // ==NO action, encapsulate&&(RawBeg==RawEnd==NULL)
  ACTION_ENCAPSULATE,
  ACTION_REPLACE,
  ACTION_REMOVE
 };

 #define INFINITY 65535u

 //- EOF ---------------------------------------------------------------------

#endif

