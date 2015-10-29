/*
---------------------------------------
	GRAFX.C
	
	Diverse sm†tt & gott...
---------------------------------------
*/
#include <graph.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <i86.h>
#include "grafx.h"


static int mouseStatus = -1;

void InitMouse( void )
{
  union REGS in, out;

  in.w.ax = 0;
  int386( 0x33, &in, &out);
  mouseStatus = -1;
  ShowMouse(FORCE);
}

void ShowMouse( int force )
{
  union REGS in, out;

  if (mouseStatus >= 0)
    return;

  do
  {
    in.w.ax = 1;
    int386( 0x33, &in, &out);
    mouseStatus++;
  }
  while (force != NOFORCE && mouseStatus < 0);
}

void HideMouse( void )
{
  union REGS in, out;

  in.w.ax = 2;
  int386( 0x33, &in, &out);
  mouseStatus--;
}

void Mouse( short *x, short *y, short *button )
{
  union REGS in, out;

  in.w.ax = 3;
  int386( 0x33, &in, &out);
  *x = out.w.cx;
  *y = out.w.dx;
  *button = out.w.bx;
}

int MouseButtons( void )
{
  union REGS in, out;

  in.w.ax = 3;
  int386( 0x33, &in, &out);
  return out.w.bx;
}

void SetMouseShape( word *shape )
{
  if (shape) shape = NULL;
}

void SetMouseArrow( void )
{
}

void Pause( int ticks )
{
  delay( ticks);
}

void ClrScr( void )
{
  _clearscreen( _GCLEARSCREEN);
//  clearviewport();
}

void SetFont( int fontIndex )
{
  if (fontIndex) fontIndex = 0;
}

int GetFont( void )
{
  return 0;
}

void SetColor( word c )
{
  _setcolor( c);
//  _setfillmask( SOLID_FILL, c);
}

word GetColor( void )
{
  return _getcolor();
}

void SetBgColor( word c )
{
  if (c) c = 0;
//  setfillstyle( SOLID_FILL, c);
}

word GetBgColor( void )
{
  return 0;
}

void SetLineStyle( word c )
{
  _setlinestyle( c);
}

word GetLineStyle( void )
{
  return 0;
}

void SetWriteMode( word c )
{
  if (c) c = 0;
}

word GetWriteMode( void )
{
  return 0;
}

void SetTextStyle( word c )
{
  if (c) c = 0;
}

word GetTextStyle( void )
{
  return 0;
}

void SetTextMode( word c )
{
  if (c) c = 1;
}

word GetTextMode( void )
{
  return 0;
}

void SetFillStyle( word *c )
{
  if (c) c = 0;
}

word *GetFillStyle( void )
{
  return NULL;
}

void Line( int x1, int y1, int x2, int y2 )
{
  _moveto( x1, y1);
  _lineto( x2, y2);
}

void Rectangle( int x1, int y1, int x2, int y2 )
{
  _rectangle( _GBORDER, x1, y1, x2, y2);
}

void FillRect( int x1, int y1, int x2, int y2 )
{
  _rectangle( _GFILLINTERIOR, x1, y1, x2, y2);
}


void OutTextXY( int x, int y, char *s )
{
  _moveto( x, y);
  _outgtext( s);
}

Palette uPalette =
{ 0x000, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF,
  0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF };

void GetPalette( word *p )
{
  int count;

  for (count = 0; count < 16; count++)
    p[ count] = uPalette[ count];
}

word Swap( word w )
{
  return (w >> 8) | ((w & 0xFF) << 8);
}

word Reverse( word i )
{
  return ((i & 8) >> 3) + ((i & 4) >> 1) + ((i & 2) << 1) + ((i & 1) << 3);
}

void SetPalette( word *p )
{
  int i;
  long r, g, b;
  word x;

  for (i = 0; i < 16; i++)
  {
    uPalette[i] = p[i];
    x = Swap( p[i]);
    b = (x & 7) << 3;
    g = ((x >> 4) & 7) << 3;
    r = ((x >> 8) & 7) << 3;
    _remappalette( Reverse( i), (b << 16) + (g << 8) + r);
  }
}

void FadeOut( void )
{
  /*
  short turns, i;
  Palette temporary;

  GetPalette( temporary);
  for (turns = 0; turns < 7; turns++)
  {
    for (i = 0; i < 16; i++)
    {
      if ((temporary[ i] & 0x0F00) != 0)
	temporary[i] -= 0x0100;
      if ((temporary[ i] & 0x00F0) != 0)
	temporary[i] -= 0x0010;
      if ((temporary[ i] & 0x000F) != 0)
	temporary[i] -= 0x0001;
    }
    SetPalette( temporary);
  }
  */
}

void FadeIn( word *p )
{
//  short turns, i;
//  Palette temporary;

  SetPalette( p);
  /*
  memset( temporary, 0, sizeof( Palette));
  for (turns = 7; turns > 0; turns--)
  {
    for (i = 0; i < 16; i++)
    {
      if (((p[i] >> 8) & 0x000F) >= turns)
	temporary[i] += 0x0100;
      if (((p[i] >> 4) & 0x000F) >= turns)
	temporary[i] += 0x0010;
      if ((p[i] & 0x000F) >= turns)
	temporary[i] += 0x0001;
    }
    SetPalette( temporary);
  }
  */
}

void appl_init( void )
{
  _setvideomode( _VRES16COLOR);
  InitMouse();
}

void appl_exit( void )
{
  union REGS regs;

  regs.w.ax = 3;
  int386( 0x10, &regs, &regs);
//  _setvideomode( _DEFAULTMODE);
}

int Kbshift( int x )
{
  union REGS in, out;

  if (x) x = 0;
  in.h.ah = 2;
  int386( 0x16, &in, &out);
  return out.h.al;
}

int ReadKey( void )
{
  int c = getch();
  if (c == 0)
    return getch() << 8;
  else
    return c;
}
