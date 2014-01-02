#ifndef __HILISEM_H
 #define __HILISEM_H

 #include "source\dynauto.h"
 #include "source\hierrors.h"
 #include "source\hitypes.h"
 #include "source\synode.h"
 #include "string.h"

 // finishes translation from hili config file to binary

 //---------------------------------------------------------------------------------

 void QuotesAway( char far *String );

 //- class DynaCfgFile ---------------------------------------------------------

 class far DynaCfgFile
 {
  public:

   List far *Records; // List of DynaCfgRecordLists then list of DynaCfgRecords

  public:

   DynaCfgFile( void );
    void inline  Insert( ListItem far *InsertedItem ) { Records->Insert( InsertedItem ); }
    void PrintTokens( void );
   ~DynaCfgFile();

 };

 //- class DynaCfgRecordList ---------------------------------------------------------

 class far DynaCfgRecordList : public ListItem
 {
  // record list contains tokens

  public:

   List far *Tokens;

  public:

   DynaCfgRecordList( void );
    void Print( void );
    void inline  Insert( ListItem far *InsertedItem ) { Tokens->Insert( InsertedItem ); }
   ~DynaCfgRecordList();

 };

 //- struct Token ---------------------------------------------------------------

 struct far DynaCfgToken : public ListItem
 {
  // Idf=String;

  // string is transformed to number: TokenType
  TokenType Idf;	// Element/
			// Descr/
			// Color/Size/Font/
			// RawBeg/RawEnd/
			// Setup/
			// Face_font/Title/BaseColor/BaseSize/
			// BgColor/Background
  char far  *String;

  void Print( void );
  virtual ~DynaCfgToken( void );
 };

 //- struct DynaCfgRecord ---------------------------------------------------------

 struct far DynaCfgRecord : public ListItem
 {

  RecordType	Type;	// Normal ...  Element/Descr/Action/Color/Size/Font
			//  ||
			// Raw    ...  Element/Descr/Action/RawBeg/RawEnd
			//  ||
			// Replace ... Element/Descr/Action/Raw
			//  ||
			// Remove ...  Element/Descr/Action
			//  ||
			// Setup_translation  ... Output/CRLF/HTMLOutput
			//  ||
			// Setup_encapsulation  ... Face_font/Title/
			//			    BaseColor/BaseSize/
			//		            BgColor/Background



			// for details see hitypes.h
  union
  {

   struct
   {
    char
	 far 	*Descr,
	 far 	*Font,
	 far 	*Size,
	 far 	*Color;
    bool 	Underscore,
		Italic,
		Bold;
   }
   Normal;

   struct
   {
    char
	 far 	*Descr,
	 far 	*RawBeg,
	 far 	*RawEnd;
   }
   Raw;

   struct
   {
    char
	 far 	*Descr,
	 far 	*Raw;
   }
   Replace;

   struct
   {
    char
	 far 	*Descr;
   }
   Remove;

   struct
   {
    char
	 far 	*Descr;
   }
   No;

   struct
   {
    OutputType  OutType;		// translation block
    bool 	CRLF,                   // translation block
		HTMLOutput;             // translation block

    char
	 far 	*Title,			// page
	 far 	*FaceFont,              // page
	 far 	*BaseColor,             // page
	 far 	*BaseSize,              // page
	 far 	*BgColor,               // page, table
	 far 	*Background,            // page
	 far    *Width;                 //       table
   }
   Setup;

  }
  Data;

 public:

  void Print( void );
  virtual ~DynaCfgRecord( void );

 };

 //- struct DynaBinaryCfgTableItem ---------------------------------------------------------

 struct far DynaBinaryCfgTableItem : public ListItem
 {

  // ID which automat returns when this element is accepted
  ElementID     ID;	// - if ID==0 this struct contains beg and end
			//   of html page or table which is encapsulated
			//   using hilex
			// - else contains element things,
			//   later will be element encapsulated
			//   this way: "RawBeg" Element "RawEnd"

  ActionType    Action; // ACTION_ENCAPSULATE, _REPLACE, _REMOVE, _NO

  char 		far *RawBeg,	// when Replace used as Raw
		far *RawEnd;    // when Encapsulate are used both


  public:

   void Print( void );
   void Save( int Handle );
  ~DynaBinaryCfgTableItem( void );

 };

 //- class Semantator ---------------------------------------------------------

 class far _Semantator
 {

  private:

   SyntactNode    far *Root;	// root of syntact tree

   List	       	  far *Table;   // list of DynaBinaryCfgTableItems -> cfg file in binary form
   DynamicAutomat far *Auto;    // automat constructed using cfg file

  public:

   _Semantator( SyntactNode far *SyntactRoot );

    void SyntactTreeToDynamicTable( void );
    void PrintTable( void );
    void PrintListOfRecords( void );
    void Save( char *FileName ); // saves table and automat to *.BCA

   ~_Semantator( void );

  private:

   bool DiveForR3s( SyntactNode far *Node );
   void DiveForT4s( SyntactNode 	far *Node,
		    DynaCfgRecordList 	far *RecordList
		  );
   void WorkOnR3Subtree( SyntactNode far *Node );
   void DynaCfgRecordListToDynaCfgRecord( DynaCfgRecordList far *CfgRecordList,
					  DynaCfgRecord     far *&Record
					);
   void RecordsToDynaTable( void );

  private:

   DynaCfgFile far *File;
   List	       far *ListOfRecords;
 };

 typedef _Semantator Semantator;

 //- EOF ----------------------------------------------------------------------

#endif



