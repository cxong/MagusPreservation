#define TRUE	1
#define FALSE	0

#define SGN(x)	((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

#define KEYPRESSED kbhit()
#define KEYREAD ReadKey()

#define FORCE   0
#define NOFORCE 1

#define MOUSEK	MouseButtons()

#define TEXT_TRANSPARENT	1
#define TEXT_OVERWRITE		7

typedef unsigned short word;
typedef word Palette[16];

#define FONT_6X6	0
#define FONT_8X8	1
#define FONT_8X16	2

#define FontHeight(x)	textheight( "Mg")
#define FontWidth(x)	textwidth( "M");

#define hidemouse HideMouse
#define showmouse(x) ShowMouse(x)

void HideMouse( void );
void ShowMouse( int force );
int  MouseButtons( void );
void SetMouseShape( word *shape );
void SetMouseArrow( void );
void Mouse( short *x, short *y, short *button );
void Pause( int ticks );

void ClrScr( void );
void SetFont( int fontIndex );
int  GetFont( void );
void SetColor( word c );
word GetColor( void );
void SetBgColor( word c );
word GetBgColor( void );
void SetLineStyle( word c );
word GetLineStyle( void );
void SetWriteMode( word c );
word GetWriteMode( void );
void SetTextStyle( word c );
word GetTextStyle( void );
void SetTextMode( word c );
word GetTextMode( void );

void Line( int x1, int y1, int x2, int y2 );
void Rectangle( int x1, int y1, int x2, int y2 );
void FillRect( int x1, int y1, int x2, int y2 );
void OutTextXY( int x, int y, char *s );

void GetPalette( word *p );
void SetPalette( word *p );
void FadeOut( void );
void FadeIn( word *p );

void appl_init( void );
void appl_exit( void );
int  Kbshift( int x );
int  ReadKey( void );
