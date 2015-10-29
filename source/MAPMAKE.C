/*
 Magus v„rldstillverkare
*/
#include <fcntl.h>
#include <string.h>
#include <dos.h>
#include <linea.h>
#include <aes.h>

#define XMAX 200
#define YMAX 320

short far gWorld[XMAX][YMAX];
short gateX, gateY;

short terrain[23] =
{
  0x0001, 0x0006, 0x0003, 0x0800, 0x0109, 0x0102, 0x0164, 0x0205,
  0x0363, 0x0462, 0x250C, 0x2565, 0x060A, 0x270F, 0x0804, 0x000D,
  0x2907, 0x2908, 0x2A10, 0x2B0E, 0x2D0B, 0x2C70, 0x0E3E
};

void SaveMap( void )
{
  int f;
  char directory[ FMSIZE];
  
  getcd( 0, directory);
  strcat( directory, "\\WORLD.MGS");
  f = creat( directory, S_IWRITE | S_IREAD | O_RAW);
  if (f >= 0)
  {
    write( f, gWorld, sizeof( gWorld));
    write( f, &gateX, sizeof( gateX));
    write( f, &gateY, sizeof( gateY));
    close( f);
  }
}


int ReadScreen( void )
{
  char name[FNSIZE], directory[FMSIZE];
  short button;
  int f, status;
  	  
  status = 0;
  getcd( 0, directory);
  strcat( directory, "\\*.NEO");
  *name = 0;
  fsel_exinput( directory, name, &button, "L„s NEO-sk„rm");
  if (button == 1)
  {
  	*(strrchr( directory, '\\')+1) = 0;
    strcat( directory, name);
    f = open( directory, O_RDONLY | O_RAW, 0);
    if (f >= 0)
    {
      if (filelength( f) == 32128)
      {
        read( f, Physbase(), 128);
        read( f, Physbase(), 32000);
        status = 1;
      }
      else
        form_alert( 1, "[3][Ej NeoChrome fil][Ok]");
      close( f);
    }
    else
      form_alert( 1, "[3][Filen finns inte][Ok]");
  }
  return status;
}

#define BLACK		 0
#define	WHITE		15
#define	BROWN		 1
#define	ORANGE		 2
#define	YELLOW		 4
#define	LIGHTGRAY	 6
#define	BLUE		 3
#define	GRAY		 5
#define	GREEN		 7
#define	DARKGREEN	 8
#define	DARKBROWN	 9
#define	RED			10
#define	BEIGE		12
#define	DARKBLUE	14
#define	DARKRED		11
#define	DARKGRAY	13

#define ABS(x)	((x) > 0 ? (x) : -(x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int adjacent( int y, int x, int color )
{
  int cnt;

  cnt = 0;
  if (x > 0 && getpixel( x-1, y) == color)
    cnt++;
  if (y > 0 && getpixel( x, y-1) == color)
    cnt++;
  if (x < YMAX-1 && getpixel( x+1, y) == color)
    cnt++;
  if (y < XMAX-1 && getpixel( x, y+1) == color)
    cnt++;
  return cnt;
}

void MakeMap( void )
{
  int x, y, color;

  for (y = 0; y < YMAX; y++)
    for (x = 0; x < XMAX; x++)
    {
      color = getpixel( y, x);
      switch (color)
      {
        case BLACK:
          if (adjacent( x, y, BROWN) > 1)
            gWorld[ x][ y] = terrain[ 21]; /* D”rr */
          else
            gWorld[ x][ y] = terrain[ 13]; /* Port */
          break;

        case DARKGREEN:
          gWorld[ x][ y] = terrain[ 4]; /* Gr„s */
          break;
          
        case DARKBLUE:
          gWorld[ x][ y] = terrain[ 7]; /* H2O */
          break;
          
        case BLUE:
          if (adjacent( x, y, DARKBLUE) > 0)
            gWorld[ x][ y] = terrain[ 8]; /* Stenar i vatten */
          else
            gWorld[ x][ y] = terrain[ 1]; /* Stenplatta */
          break;

        case DARKBROWN:
          gWorld[ x][ y] = terrain[ 0]; /* Jord */
          break;
          
        case RED:
          if (adjacent( x, y, YELLOW) != 0 || adjacent( x, y, DARKGREEN) != 0)
            gWorld[ x][ y] = terrain[ 6]; /* R”d blomma */
          else
            gWorld[ x][ y] = terrain[ 14]; /* Tr„golv */
          break;
          
        case YELLOW:
          if (adjacent( x, y, YELLOW) < 2 || adjacent( x, y, RED) > 1)
            gWorld[ x][ y] = terrain[ 5]; /* Gul blomma */
          else
            gWorld[ x][ y] = terrain[ 12]; /* Sand */
          break;
          
        case BEIGE:
          gWorld[ x][ y] = terrain[ 19]; /* Buske */
          break;
          
        case GREEN:
          if (adjacent( x, y, YELLOW) > 2)
            gWorld[ x][ y] = terrain[ 11]; /* Palm */
          else if (adjacent( x, y, DARKBLUE) > 1)
            gWorld[ x][ y] = terrain[ 9]; /* Sankmark */
          else
            gWorld[ x][ y] = terrain[ 10]; /* Tr„d */
          break;
          
        case WHITE:
          if (adjacent( x, y, WHITE) != 0)
            gWorld[ x][ y] = terrain[ 16]; /* Stenmur */
          else
          {
            gWorld[ x][ y] = terrain[ 22]; /* Portal */
            if (MAX( ABS( x - 100), y) < MAX( ABS( gateX - 100), gateY))
            {
              gateX = x;
              gateY = y;
            }
          }
          break;
          
        case LIGHTGRAY:
          gWorld[ x][ y] = terrain[ 17]; /* Tegelv„gg */
          break;
          
        case DARKGRAY:
          gWorld[ x][ y] = terrain[ 18]; /* Berg */
          break;
          
        case DARKRED:
          gWorld[ x][ y] = terrain[ 2]; /* Grus */
          break;
          
        case ORANGE:
          gWorld[ x][ y] = terrain[ 15]; /* Plankor */
          break;
          
        case BROWN:
          gWorld[ x][ y] = terrain[ 20]; /* Tr„v„gg */
          break;
          
        case GRAY:
          gWorld[ x][ y] = terrain[ 3]; /* Stenplatta */
          break;
      }
    }
}

void main( void )
{
  appl_init();
  linea0();
  if (ReadScreen())
  {
    MakeMap();
    SaveMap();
  }
  appl_exit();
}
