/*
---------------------------------------------------------------------
	MAGUS.C

Datum   Žndring
---------------------------------------------------------------------
911103  P†b”rjad
931113  ™versatt till engelska...
960812  Konverterad till Watcom C
---------------------------------------------------------------------
*/
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <sys\stat.h>
#include <conio.h>
#include <dos.h>

#include <graph.h>

#include "grafx.h"
#include "pics.h"
#include "digisnd.h"


typedef unsigned char byte;

#include "items.h"
#include "sounds.h"


/* DEBUGGING */

void DebugLog( char *s )
{
  FILE *f;

  f = fopen( "LOG.TMP", "a");
  if (f != NULL)
  {
    fprintf( f, "%s", s);
    fclose( f);
  }
}

static int Rand( int x )
{
	if (x <= 0)
		return 0;
	else
		return rand() % x;
}

unsigned short myMouse[37] =
{
  2, 2, 1, 0, 1,
  14336, 32256, 32640, 16320, 4080, 2044, 4094, 4094,
  4094, 8190, 8191, 8191, 4095, 1022, 252, 56,
  0, 14336, 15872, 3968, 960, 448, 1596, 1788,
  1524, 2508, 3642, 3966, 126, 220, 56, 0
};


// +--------------+
// |   Paletten   |
// +--------------+


/* F„rgerna i Magus palett */


#define BLACK       0
#define WHITE      15
#define BROWN       8
#define ORANGE      4
#define YELLOW      2
#define LIGHTGRAY   6
#define BLUE       12
#define GRAY       10
#define GREEN      14
#define DARKGREEN   1
#define DARKBROWN   9
#define RED         5
#define BEIGE       3
#define DARKBLUE    7
#define DARKRED    13
#define DARKGRAY   11


Palette gPalette, gOldPalette;


/*
---------------------------------------
	Diverse sm†tt & gott...
---------------------------------------
*/

char uString[ 255];

void ReliefTextXY( int x, int y, char *s, int light, int dark, int normal, int bg )
{
  if (bg >= 0)
  {
    SetColor( bg);
    FillRect( x, y, x + strlen( s)*8, y + 10);
  }
  SetTextMode( TEXT_TRANSPARENT);
  SetColor( dark);
  OutTextXY( x, y, s);
  SetColor( light);
  OutTextXY( x, y+2, s);
  SetColor( normal);
  OutTextXY( x, y+1, s);
  SetTextMode( TEXT_OVERWRITE);
}

void ShowText( int x, int y, char *s, int color, int bg )
{
  if (bg >= 0)
  {
    SetColor( bg);
    FillRect( x, y, x + strlen( s)*8, y + 10);
  }
  SetColor( color);
  OutTextXY( x, y+1, s);
}

void DrawStoneBox( int x, int y, int w, int h )
{
  SetColor( GRAY);
  FillRect( x, y, x+w, y+h);
  SetColor( LIGHTGRAY);
  Rectangle( x, y, x+w, y+h);
  Rectangle( x+1, y+1, x+w-1, y+h-1);
  Rectangle( x+2, y+2, x+w-2, y+h-2);
  SetColor( DARKGRAY);
  Line( x+w, y, x+w, y+h);
  Line( x+w, y+h, x+1, y+h);
  Line( x+w-1, y+1, x+w-1, y+h-1);
  Line( x+w-1, y+h-1, x+2, y+h-1);
  Line( x+w-2, y+2, x+w-2, y+h-2);
  Line( x+w-2, y+h-2, x+3, y+h-2);

  SetColor( LIGHTGRAY);
  SetLineStyle( 59144);
  Line( x+3, y+3, x+w-3, y+3);
  Line( x+3, y+3, x+3, y+h-3);
  SetColor( DARKGRAY);
  Line( x+3, y+h-3, x+w-3, y+h-3);
  Line( x+w-3, y+3, x+w-3, y+h-3);
  SetLineStyle( 0xFFFF);
}

/*
---------------------------------------
	Meddelanden till spelaren
---------------------------------------
*/

void DrawPlayArea( void )
{
  DrawStoneBox( 0, 0, 159, 410);
  DrawStoneBox( 0, 400, 620, 79);
  SetColor( GRAY);
  FillRect( 5, 400, 155, 410);
}

void Message( char *s )
{
  static char *x = NULL;

  if (!x)
    x = malloc( _imagesize( 10, 415, 399, 475));

  if (x)
  {
    _getimage( 10, 415, 399, 475, x);
    _putimage( 10, 405, x, _GPSET);
  }
  SetColor( GRAY);
  FillRect( 10, 465, 399, 475);
  ShowText( 10, 465, s, BLACK, -1);
}


/*
---------------------------------------
	Bilder - Blittning o dyl.
---------------------------------------
*/


#define MAXSPRITE 200
unsigned short *gSprites[ MAXSPRITE];


void Put3Bytes( int x, int y, unsigned short *addr )
{
  PutPic( x, y, addr, 0);
}



struct thing *gThings;


/*
---------------------------------------
	Character classes
---------------------------------------
*/

#define MAXCLASSES		43
#define SELECTABLECLASSES	 9
#define MAXINVENTORY		10

struct class
{
  char name[20];
  short speed,
	strength,
	wisdom,
	skill,
	health,
	power,
	canWalk,
	maleIcon,
	femaleIcon,
	lifeSpan,
	rank,
	inventory[ MAXINVENTORY];
};

#define MAXLEVEL 11

char *gLevelNames[ MAXLEVEL+1] =
{ "Nobody",
  "Amateur",
  "Novice",
  "Apprentice",
  "Trained",
  "Good",
  "Experienced",
  "Expert",
  "Elite",
  "Master",
  "ArchMaster",
  "Legend"
};

int gLevelLimits[ MAXLEVEL+1] =
{ 50,
  100,
  200,
  300,
  400,
  500,
  700,
  900,
  1200,
  1500,
  2000,
  2000
};

#define PERSISTENT 4711

struct class gClassData[ MAXCLASSES] =
{
  { "Shaman", 6, 9, 15, 7, 30, 40, 0x59DB, 26, 27, 0, PERSISTENT,
    { O_ELVENCLOAK, O_STAFF, O_HEALINGSPELL, O_PHANTOMSPELL, O_SPEEDSPELL, 0, 0, 0, 0, 0 } },
  { "Wizard", 4, 12, 12, 10, 50, 50, 0x51CB, 23, 31, 0, PERSISTENT,
    { O_CLOAK, O_DAGGER, O_FIREBALLSPELL, O_TELEPORTSPELL, O_VISIONSPELL, 0, 0, 0, 0, 0 } },
  { "Elf", 6, 14, 0, 15, 90, 0, 0x59DB, 116, 117, 0, PERSISTENT,
    { 60, 4, 5, 16, 0, 0, 0, 0, 0, 0 } },
  { "Barbarian", 4, 17, 0, 15, 120, 0, 0x51CB, 19, 35, 0, PERSISTENT,
    { 45, 17, 5, 16, 0, 0, 0, 0, 0, 0 } },
  { "Knight", 4, 24, 0, 11, 90, 0, 0x51CB, 25, 47, 0, PERSISTENT,
    { 1, 18, 11, 10, 0, 0, 0, 0, 0, 0 } },
  { "Duck", 5, 14, 0, 17, 100, 0, 0x51DF, 108, 108, 0, PERSISTENT,
    { O_MACE, O_LEATHER, O_GLOVES, O_BOW, O_ARROWS, 0, 0, 0, 0, 0 } },
  { "Dwarf", 4, 20, 0, 12, 120, 0, 0x51CB, 126, 174, 0, PERSISTENT,
    { 3, 17, 35, 0, 0, 0, 0, 0, 0, 0 } },
  { "Druid", 4, 14, 10, 10, 80, 20, 0x51CB, 127, 175, 0, PERSISTENT,
    { O_CLOAK, O_MACE, O_FIRESPELL, O_AIRSPELL, O_WATERSPELL, O_EARTHSPELL, 0, 0, 0, 0 } },
  { "Duck mage", 5, 10, 12, 9, 40, 60, 0x51DF, 141, 141, 0, PERSISTENT,
    { O_CLOAK, O_DAGGER, O_CONFUSIONSPELL, O_CHAOSSPELL, O_FREEZESPELL, 0, 0, 0, 0, 0 } },
  { "Orch", 4, 14, 0, 9, 50, 0, 0x51CB, 42, 42, 0, 40,
    { 12, 37, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Orch", 4, 15, 0, 13, 60, 0, 0x51CB, 43, 43, 0, 80,
    { 18, 3, 11, 0, 0, 0, 0, 0, 0, 0 } },
  { "Orch", 4, 12, 12, 5, 40, 20, 0x51CB, 44, 44, 0, 80,
    { 20, 30, 26, 27, 37, 0, 0, 0, 0, 0 } },
  { "ChaosKnight", 4, 22, 0, 16, 100, 0, 0x51CB, 36, 36, 0, 100,
    { 43, 13, 2, 35, 0, 0, 0, 0, 0, 0 } },
  { "Spirit", 5, 50, 15, 15, 40, 10, 0x7FFF, 24, 24, 0, 120,
    { 0x1000 | O_CHAOSSPELL, 0x1000 | O_GHOSTBLADE, 0x1000 | O_SLEEPSPELL,
      0, 0, 0, 0, 0, 0, 0 } },
  { "Ghost", 4, 10, 10, 15, 20, 10, 0x7FFF, 33, 33, 0, 100,
    { 0x1000 | O_CONFUSIONSPELL, 0x1000 | O_CHAINANDBALL, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "DeathKnight", 4, 40, 20, 20, 200, 20, 0x51CB, 37, 37, 0, 170,
    { 42, 3, 13, 10, 20, 35, 75, 0, 0, 0 } },
  { "Skeleton", 4, 7, 0, 7, 30, 0, 0x51CB, 32, 32, 0, 0,
    { O_CHAINANDBALL, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Zombie", 4, 12, 0, 10, 10, 0, 0x51CB, 20, 20, 0, 0,
    { O_CUTLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Djinn", 14, 1, 0, 0, 1, 0, 0x51DF, 18, 18, 8, PERSISTENT,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Fenix", 5, 10, 19, 17, 40, 5, 0x51C3, 34, 34, 2, PERSISTENT,
    { 0x1000 | O_FIREBLADE, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Undine", 6, 20, 0, 15, 30, 0, 0x51DF, 29, 29, 6, PERSISTENT,
    { 0x1000 | O_CUTLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Golem", 3, 60, 0, 10, 80, 0, 0x51CB, 40, 40, 5, PERSISTENT,
    { 0x1000 | O_STONEAXE, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Demon", 6, 50, 10, 17, 200, 20, 0x7FFF, 39, 39, 9, PERSISTENT,
    { 0x1000 | O_FIREBALLSPELL, 0x1000 | O_FIREBLADE, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Sorcerer", 5, 25, 20, 20, 150, 150, 0x51CB, 41, 41, 0, 120,
    { O_CHAINMAIL, O_GREENRING, O_FIRESPELL, O_DEMONSPELL, O_ENERGYSPELL,
      O_SLEEPSPELL, O_LIGHTNINGSPELL, O_TWOHANDEDSWORD, O_NEGATOR, O_FREEZESPELL } },
  { "Hobgoblin", 5, 10, 0, 9, 10, 0, 0x59DB, 115, 115, 0, 10,
    { 7, 4, 4, 41, 0, 0, 0, 0, 0, 0 } },
  { "Minotaur", 4, 20, 0, 12, 80, 0, 0x51CB, 109, 109, 0, 100,
    { 17, 45, 42, 0, 0, 0, 0, 0, 0, 0 } },
  { "Witchlord", 5, 20, 20, 20, 100, 100, 0x51CB, 118, 118, 0, 140,
    { O_LIGHTNINGSPELL, O_FIRESPELL, O_SHADOWCLOAK, O_FIREBLADE, O_FIREBALLSPELL,
      O_CONFUSIONSPELL, O_GREENRING, O_NEGATOR, 0, 0 } },
  { "Magician", 4, 12, 10, 10, 50, 30, 0x51CB, 22, 22, 0, 50,
    { 6, 36, 8, 41, 16, 0, 0, 0, 0, 0 } },
  { "Shadowbeast", 5, 20, 0, 10, 200, 0, 0x51DF, 28, 28, 3, PERSISTENT,
    { 0x1000 | O_MACE, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Lightning", 8, 4, 14, 0, 1, 10, 0x51DF, 21, 21, 1, PERSISTENT,
    { 0x1000 | O_CHOCKHAMMER, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Undead", 5, 17, 16, 20, 70, 15, 0x51CB, 38, 38, 0, 100,
    { 20, 27, 45, 17, 15, 26, 0, 0, 0, 0 } },
  { "Troll", 3, 20, 0, 7, 100, 0, 0x51CB, 17, 17, 0, 20,
    { O_STONEAXE, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Monk", 4, 14, 11, 16, 60, 20, 0x51CB, 125, 125, 0, 60,
    { 12, 38, 25, 24, 78, 23, 0, 0, 0, 0 } },
  { "Goblin", 4, 8, 0, 14, 30, 0, 0x51CB, 128, 128, 0, 10,
    { O_SWORD, O_LEATHER, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Warrior", 4, 12, 0, 14, 80, 0, 0x51CB, 135, 135, 0, 60,
    { O_MACE, O_CHAINMAIL, 7, 7, 7, 0, 0, 0, 0, 0 } },
  { "ChaosWarrior", 4, 30, 0, 19, 300, 0, 0x51CB, 136, 136, 0, 200,
    { O_LARGEHELMET, O_TWOHANDEDSWORD, O_GLOVES, O_ARMOR, O_CLOAK, 0, 0, 0, 0, 0 } },
  { "DemonPrince", 6, 30, 0, 20, 500, 0, 0x51CB, 137, 137, 0, PERSISTENT,
    { O_GOLDENARMOR, O_BLOODTASTEAXE, O_GOLDENHELMET, O_GOLDENGLOVES,
      O_CLOAK, O_NEGATOR, O_GREENRING, O_GHOSTBLADE, 0, 0 } },
  { "Orch", 4, 13, 0, 10, 50, 0, 0x51CB, 30, 30, 0, 40,
    { O_BOW, O_ARROWS, 0, 0, 0, 0, 0, 0, 0, 0 } },
  { "Guardian", 5, 22, 0, 19, 200, 0, 0x51CB, 155, 155, 0, 200,
    { O_SILVERBOW, O_ARMOR, O_LARGEHELMET, O_GLOVES, O_FAITHFULARROW,
      O_TWOHANDEDSWORD, 0, 0, 0, 0 } },
  { "Dragon", 8, 60, 25, 25, 600, 300, 0x51DF, 156, 156, 0, 250,
    { 0x1000 | O_INFERNOSPELL, 0x1000 | O_DRAGONSTOOTH, O_DRAGONSTOOTH,
      0x1000 | O_GOLDENARMOR, 0x1000 | O_NEGATOR, 0x1000 | O_GREENRING, 0, 0, 0, 0 } },
  { "DeathLord", 5, 25, 20, 20, 200, 100, 0x7FFF, 113, 113, 0, 230,
    { O_LARGEHELMET, O_ARMOR, O_GLOVES, O_LIGHTNINGSPELL, O_SLAYERSWORD,
      O_DEMONSPELL, 0, 0, 0, 0 } },
  { "The Dark One", 7, 80, 25, 25, 500, 1000, 0x7FFF, 105, 105, 0, PERSISTENT,
    { O_GOLDENARMOR, O_GOLDENHELMET, O_NEGATOR, O_BLOODTASTEAXE, O_ENERGYSPELL,
      O_GREENRING, O_STORMSPELL, O_DEMONSPELL, O_INFERNOSPELL, O_CHAOSSPELL } },
  { "Small one", 8, 5, 0, 0, 10, 0, 0x59DB, 45, 46, 0, 1200,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};


/*
---------------------------------------
	The Game-world
---------------------------------------
*/

#define WORLD_X_MAX 200
#define WORLD_Y_MAX 320

#define SHADOW_FLAG    0x2000
#define ITEM_FLAG      0x4000
#define CHARACTER_FLAG 0x8000
#define THE_VOID       (102 | (31 << 8))
#define DARKNESS       (103 | (31 << 8) | SHADOW_FLAG)

typedef short WorldColumn[ WORLD_Y_MAX];

WorldColumn *gWorld;

short gGateX, gGateY;

int InBox( int x, int y, int left, int top, int right, int bottom )
{
  if (x >= left && x < right &&
      y >= top  && y < bottom)
    return TRUE;
  else
    return FALSE;
}

int Treadable( int x, int y, long c )
{
  if (!InBox( x, y, 0, 0, WORLD_X_MAX, WORLD_Y_MAX)) return FALSE;

  if (gWorld[ x][ y] & CHARACTER_FLAG) return FALSE;

  if (c & (1 << ((gWorld[ x][ y] >> 8) & 31))) return TRUE;

  return FALSE;
}

/*
---------------------------------------
	Characters
---------------------------------------
*/

#define CHARACTER_MAXNAME 15

struct character
{
  char name[ CHARACTER_MAXNAME + 1];
  short class,
	x, y,
	strength,
	wisdom,
	skill,
	health,
	power,
	maxHealth,
	maxPower,
	canWalk;
  long  xp;
  short level;
  struct item *wearing, *wielding, *carrying;
  short lifeSpan,
	backGround,
	icon,
	speed,
	moves,
	killerClass,
	damageBonus,
	ward,
	confused,
	isPlayer,
	isFleeing,
	isNasty,
	isTracker,
	isFooled,
	gifts,
	slayer;
  struct character *opponent, *next;
};

typedef struct character Person;

Person *gCharacters, *gPC;
Person *gDarkMaster, *gLastDrawn;

void AddCharacter( Person *pc )
{
  Person **p;

  p = &gCharacters;
  while (*p)
    p = &((*p)->next);
  *p = pc;
  pc->next = NULL;
}

void DisposeItems( struct item *list )
{
  struct item *i;

  while (list)
  {
    i = list;
    list = list->next;
    free( i);
  }
}

void CancelOpponents( Person *pc )
{
  Person *p;

  p = gCharacters;
  while (p)
  {
    if (p->opponent == pc)
      p->opponent = NULL;
    p = p->next;
  }
}


void DisposeCharacter( Person *p )
{
  Person *t;

  if (p == gCharacters)
    gCharacters = p->next;
  else
  {
    t = gCharacters;
    while (t && t->next != p)
      t = t->next;
    if (t)
      t->next = p->next;
  }
  DisposeItems( p->wielding);
  DisposeItems( p->wearing);
  DisposeItems( p->carrying);
  CancelOpponents( p);
  free( p);
}

Person * FindCharacter( int x, int y )
{
  Person *p;

  p = gCharacters;
  while (p && (p->x != x || p->y != y || p->health <= 0))
    p = p->next;
  return p;
}

void AddItemToList( struct item *i, struct item **list )
{
  i->next = *list;
  *list = i;
}

void RemoveItemFromList( struct item *i, struct item **list )
{
  struct item *ii;

  if (i == *list)
    *list = i->next;
  else
  {
    ii = *list;
    while (ii && ii->next != i)
      ii = ii->next;

    if (ii)
      ii->next = i->next;
  }
}

int HowManyItems( struct item *list )
{
  int count = 0;

  while (list)
  {
    count++;
    list = list->next;
  }
  return count;
}

void AddThing( struct thing *t )
{
  t->next = gThings;
  gThings = t;
}

void DeleteThing( struct thing *t )
{
  struct thing *tt;
  Person *p;

  if (gWorld[ t->x][ t->y] & CHARACTER_FLAG)
  {
    p = FindCharacter( t->x, t->y);
    if (p)
      p->backGround = t->backGround;
    gWorld[ t->x][ t->y] &= ~ITEM_FLAG;
  }
  else
    gWorld[ t->x][ t->y] = t->backGround;

  if (t == gThings)
    gThings = t->next;
  else
  {
    tt = gThings;
    while (tt && tt->next != t)
      tt = tt->next;
    if (tt)
      tt->next = t->next;
  }
  DisposeItems( t->items);
  free( t);
}

void PlaceItem( struct item *item, int x, int y )
{
  struct thing *t;
  Person *p;

  t = gThings;
  while (t && (t->x != x || t->y != y))
    t = t->next;

  if (t)
    AddItemToList( item, &(t->items));
  else
  {
    t = malloc( sizeof( struct thing));
    t->x = x;
    t->y = y;
    if (gWorld[ x][ y] & CHARACTER_FLAG)
      t->backGround = FindCharacter( x, y)->backGround;
    else
      t->backGround = gWorld[ x][ y];
    t->items = NULL;
    AddItemToList( item, &(t->items));
    AddThing( t);
  }

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    p = FindCharacter( x, y);
    if (p)
      p->backGround = gObjectData[ item->id].icon |
				  (p->backGround & 0xFF00) |
				  ITEM_FLAG;
    gWorld[ x][ y] |= ITEM_FLAG;
  }
  else
    gWorld[ x][ y] = gObjectData[ item->id].icon |
				(gWorld[ x][ y] & 0xFF00) |
				ITEM_FLAG;
}

struct item *FindItem( int x, int y )
{
  struct thing *t;

  t = gThings;
  while (t && (t->x != x || t->y != y))
    t = t->next;
  if (t)
    return t->items;
  else
    return NULL;
}

void RemoveItem( struct item *i )
{
  struct thing *t;
  struct item *ip;
  Person *p;

  t = gThings;
  while (t)
  {
    ip = t->items;
    while (ip)
    {
      if (ip == i)
      {
	RemoveItemFromList( i, &(t->items));
	if (t->items == NULL)
	  DeleteThing( t);
	else
	{
	  if (gWorld[ t->x][ t->y] & CHARACTER_FLAG)
	  {
	    p = FindCharacter( t->x, t->y);
	    if (p)
	    p->backGround = gObjectData[ t->items->id].icon |
					(p->backGround & 0xFF00) |
					ITEM_FLAG;
	  }
	  else
	    gWorld[ t->x][ t->y] = gObjectData[ t->items->id].icon |
				(gWorld[ t->x][ t->y] & 0xFF00) |
					ITEM_FLAG;
	}
	return;
      }
      ip = ip->next;
    }
    t = t->next;
  }
}

int MayBeCombined( struct item *i )
{
  switch (gObjectData[ i->id].kind)
  {
    case O_CLASS_WEAPON:
    case O_CLASS_MIXEDWEAPON:
      return TRUE;
  }
  return FALSE;
}

int MayUse( Person *p, struct item *it )
{
  int holds, hands;
  struct item *i;

  holds = 0;
  hands = 2;
  i = p->wielding;
  while (i)
  {
    holds |= gObjectData[ i->id].requires;
    hands -= (gObjectData[ i->id].requires & 3);
    i = i->next;
  }
  i = p->wearing;
  while (i)
  {
    holds |= gObjectData[ i->id].requires;
    hands -= (gObjectData[ i->id].requires & 3);
    i = i->next;
  }

  if ((gObjectData[ it->id].requires & 3) > hands) return FALSE;

  return ((gObjectData[ it->id].requires & 0xFFFC & holds) == 0);
}

void WieldItem( Person *p, struct item *i )
{
  if (p->wielding)
    p->skill -= 5;
  RemoveItemFromList( i, &(p->carrying));
  AddItemToList( i, &(p->wielding));
}

void WearItem( Person *p, struct item *i )
{
  RemoveItemFromList( i, &(p->carrying));
  AddItemToList( i, &(p->wearing));

  switch (i->id)
  {
    case O_GREENAMULET:
      p->skill += 5 + i->modifier;
      break;

    case O_BLUEAMULET:
      p->wisdom += 5 + i->modifier;
      break;

    case O_YELLOWAMULET:
      p->speed += max( 0, 1 + i->modifier/2);
      break;

    case O_REDGLOVES:
      p->strength += 5 + max( 0, i->modifier);
      break;
  }
}

void UnWieldItem( Person *p, struct item *i )
{
  RemoveItemFromList( i, &(p->wielding));
  AddItemToList( i, &(p->carrying));
  if (p->wielding)
    p->skill += 5;
}

void Use( Person *p, struct item *i )
{
  if (i && p->wielding &&
      gObjectData[ i->id].kind == O_CLASS_SPELL &&
      gObjectData[ p->wielding->id].kind == O_CLASS_SPELL)
    UnWieldItem( p, p->wielding);

  if (!i || !MayUse( p, i)) return;

  switch (gObjectData[ i->id].kind)
  {
    case O_CLASS_WEAPON:
    case O_CLASS_THROWINGWEAPON:
    case O_CLASS_RANGEDWEAPON:
    case O_CLASS_SPELL:
    case O_CLASS_MIXEDWEAPON:
    case O_CLASS_SPECIAL:
    case O_CLASS_POTION:
      if (p->wielding)
      {
        if (p->wielding->next)
          return;
        if (!MayBeCombined( p->wielding) || !MayBeCombined( i))
          return;
      }
      WieldItem( p, i);
      break;

    case O_CLASS_ARMOR:
      if (p->moves >= 2)
      {
        WearItem( p, i);
        p->moves -= 2;
      }
      break;

    case O_CLASS_GADGET:
      if (p->moves)
      {
        WearItem( p, i);
        p->moves--;
      }
      break;
  }
}

void UnWearItem( Person *p, struct item *i )
{
  RemoveItemFromList( i, &(p->wearing));
  AddItemToList( i, &(p->carrying));

  switch (i->id)
  {
    case O_GREENAMULET:
      p->skill -= 5 + i->modifier;
      break;

    case O_BLUEAMULET:
      p->wisdom -= 5 + i->modifier;
      break;

    case O_YELLOWAMULET:
      p->speed -= max( 0, 1 + i->modifier/2);
      break;

    case O_REDGLOVES:
      p->strength -= 5 + max( 0, i->modifier);
      break;
  }
}

void StopUsing( Person *p, struct item *i )
{
  if (!i) return;

  switch (gObjectData[ i->id].kind)
  {
    case O_CLASS_SPELL:
    case O_CLASS_THROWINGWEAPON:
    case O_CLASS_MIXEDWEAPON:
      UnWieldItem( p, i);
      break;

    case O_CLASS_WEAPON:
    case O_CLASS_RANGEDWEAPON:
    case O_CLASS_SPECIAL:
    case O_CLASS_POTION:
      if (p->moves)
      {
	UnWieldItem( p, i);
	p->moves--;
      }
      break;

    case O_CLASS_ARMOR:
    case O_CLASS_GADGET:
      if (p->moves)
      {
	UnWearItem( p, i);
	p->moves--;
      }
      break;
  }
}

/*
void GarbageCollect( void );
{
  struct thing *t;
  Person *p;

  t = gThings;
  while (t)
  {
    p = GetClosestPlayer( t->x, t->y);
    if (!p || max( abs( p->x - t->x), abs( p->y - t->y)) > 25)
    {
      DeleteThing( t);
      return;
    }
    t = t->next;
  }
}
*/

struct item *FindType( short id, struct item *list )
{
  while (list && list->id != id)
    list = list->next;

  return list;
}

struct item *Carries( Person *p, short id )
{
  return FindType( id, p->carrying);
}

struct item *Uses( Person *p, short id )
{
  return FindType( id, p->wielding);
}

struct item *Wears( Person *p, short id )
{
  return FindType( id, p->wearing);
}

Person *MagicallyFindCharacter( int x, int y )
{
  Person *p;

  p = FindCharacter( x, y);
  if (p && !Carries( p, O_NEGATOR))
    return p;
  else
    return NULL;
}

Person *CreateCharacter( char *name, int class, int female, int isPlayer )
{
  Person *pc;
  int count;
  struct item *i;

  pc = malloc( sizeof( Person));
  if (!pc) return NULL;

  memset( pc, 0, sizeof( Person));
  AddCharacter( pc);
  strcpy( pc->name, name);
  pc->class = class;
  pc->lifeSpan = gClassData[ class].lifeSpan;
  pc->maxHealth = gClassData[ class].health;
  pc->health = pc->maxHealth;
  pc->maxPower = gClassData[ class].power;
  pc->power = pc->maxPower;
  pc->canWalk = gClassData[ class].canWalk;
  if (female)
    pc->icon = gClassData[ class].femaleIcon;
  else
    pc->icon = gClassData[ class].maleIcon;
  pc->speed = gClassData[ class].speed;
  pc->moves = pc->speed;
  pc->strength = gClassData[ class].strength;
  pc->skill = gClassData[ class].skill;
  pc->wisdom = gClassData[ class].wisdom;
  pc->isPlayer = isPlayer;

  for (count = 0; count < MAXINVENTORY; count++)
  {
    if (gClassData[ class].inventory[ count])
    {
      i = malloc( sizeof( struct item));
      i->id = gClassData[ class].inventory[ count] & 0xFF;
      if (gClassData[ class].inventory[ count] & 0xFF00)
        i->locked = TRUE;
      else
        i->locked = FALSE;
      if (i->id == O_ARROWS)
        i->modifier = 50;
      else if (!isPlayer)
        i->modifier = Rand( 7) - 3;
      else
        i->modifier = 0;
      i->next = NULL;
        AddItemToList( i, &(pc->carrying));
    }
  }

  return pc;
}

void PlaceCharacter( Person *p, int x, int y )
{
  p->x = x;
  p->y = y;
  p->backGround = gWorld[ x][ y];
  gWorld[ x][ y] = (gWorld[ x][ y] & 0xFF00) | p->icon | CHARACTER_FLAG;
}

void PlaceCharacterCloseTo( Person *p, int xc, int yc, int distance )
{
  int x, y;

  while (TRUE)
  {
    x = xc - distance;
    y = yc - distance;
    while (x <= xc + distance)
    {
      if (Treadable( x, y, p->canWalk))
      {
	PlaceCharacter( p, x, y);
	return;
      }
      x++;
    }
    x = xc - distance;
    y = yc + distance;
    while (x <= xc + distance)
    {
      if (Treadable( x, y, p->canWalk))
      {
	PlaceCharacter( p, x, y);
	return;
      }
      x++;
    }
    x = xc - distance;
    y = yc - distance + 1;
    while (y <= yc + distance - 1)
    {
      if (Treadable( x, y, p->canWalk))
      {
	PlaceCharacter( p, x, y);
	return;
      }
      y++;
    }
    x = xc + distance;
    y = yc - distance + 1;
    while (y <= yc + distance - 1)
    {
      if (Treadable( x, y, p->canWalk))
      {
	PlaceCharacter( p, x, y);
	return;
      }
      y++;
    }
    distance++;
  }
}

void EnterHallOfFame( Person *p );


#define RemoveCharacter(p) gWorld[ (p)->x][ (p)->y] = (p)->backGround
void Death( Person *p, Person *slayer )
{
  struct item *i;
  int count;

  RemoveCharacter( p);
  while (p->wielding)
    UnWieldItem( p, p->wielding);

  while (p->wearing)
    UnWearItem( p, p->wearing);

  if (p->isPlayer)
  {
    while (p->carrying)
    {
      i = p->carrying;
      RemoveItemFromList( i, &(p->carrying));
      if (!i->locked)
        PlaceItem( i, p->x, p->y);
      else
        free( i);
    }
  }
  else if (Rand( 10) < 2)
  {
    count = HowManyItems( p->carrying);
    count = Rand( count);
    i = p->carrying;
    while (i && count)
    {
      i = i->next;
      count--;
    }
    if (i)
    {
      RemoveItemFromList( i, &(p->carrying));
      if (!i->locked)
        PlaceItem( i, p->x, p->y);
      else
        free( i);
    }
  }
  p->health = 0;
  p->maxHealth = 0;
  p->moves = 0;

  if (slayer && !p->isPlayer && !p->isNasty)
  {
    Message( "That wasn't very nice!");
    i = malloc( sizeof( struct item));
    i->id = O_CHAINANDBALL;
    i->modifier = 0;
    i->locked = TRUE;
    i->next = NULL;
    AddItemToList( i, &(slayer->carrying));
  }
  if (p->isPlayer && !p->lifeSpan)
  {
    p->slayer = slayer->class;
    EnterHallOfFame( p);
  }
}

int Load( Person *p )
{
  int load;
  struct item *i;

  load = 0;
  i = p->wielding;
  while (i)
  {
    load += gObjectData[ i->id].weight;
    i = i->next;
  }
  i = p->wearing;
  while (i)
  {
    load += gObjectData[ i->id].weight;
    i = i->next;
  }
  i = p->carrying;
  while (i)
  {
    load += gObjectData[ i->id].weight;
    i = i->next;
  }
  return load;
}

int ActualLoad( Person *p )
{
  int load;
  struct item *i;

  load = Load( p);
  if ((i = Carries( p, O_BUBBLE)) != NULL)
    load -= 6 + i->modifier;
  return load;
}

int OverLoaded( Person *p )
{
  return (ActualLoad( p) > p->strength);
}

/*
  TRUE  -> character still in action
  FALSE -> character should be disposed of
*/

int UpdateCharacter( Person *p )
{
  struct item *i;
  int load;

  if (p->health <= 0) return FALSE;

  if (p->power < p->maxPower)
  {
    p->power += 2;
    p->power += min( p->moves, p->speed);

    if ((i = Wears( p, O_BLUERING)) != 0)
      p->power += 4 + i->modifier;
    if (p->power > p->maxPower)
      p->power = p->maxPower;
  }

  if (p->health < p->maxHealth)
  {
    p->health++;
    p->health += min( p->moves, p->speed);

    if ((i = Wears( p, O_GREENRING)) != 0)
      p->health += 2 + i->modifier;
    if (p->health > p->maxHealth)
      p->health = p->maxHealth;
  }
  p->canWalk = gClassData[ p->class].canWalk;
  p->moves = p->speed;

  load = ActualLoad( p);
  if (load > p->strength*2)
    p->moves = 0;
  else if (load > p->strength + p->strength/2)
    p->moves = p->speed/3;
  else if (load > p->strength)
    p->moves -= p->speed/3;

  if (p->damageBonus)
    p->damageBonus--;
  p->confused = 0;
  if (p->ward)
    p->ward--;

  if ((p->canWalk & (1 << ((p->backGround >> 8) & 31))) == 0)
  {
    p->health -= 10;
    if (p->health <= 0)
      Death( p, NULL);
  }

  if (p->lifeSpan)
  {
    p->lifeSpan--;
    if (!p->lifeSpan)
    {
      RemoveCharacter( p);
      return FALSE;
    }
  }

  return TRUE;
}

int IsWounded( Person *p )
{
  if (gClassData[ p->class].rank == 4711)
    return FALSE;

  if (p->health < p->maxHealth/3) return TRUE;

  if (p->health > (2*p->maxHealth)/3) return FALSE;

  return (int)p->isFleeing;
}

/*
---------------------------------------
	Play area
---------------------------------------
*/

#define BOARDSIZE		 19
#define BOARDCENTER		  9
#define BOARDLEFT		160
#define BOARDTOP		  0
#define ICONWIDTH		 24
#define ICONHEIGHT		 21

typedef short Board[ BOARDSIZE][ BOARDSIZE];
Board gBuffer, gPreviousBuffer;

int gBufferX, gBufferY;
int gMustDraw, gFollowDarkMoves;

#define MX_TO_BX(x) (((x) - BOARDLEFT) / ICONWIDTH - BOARDCENTER)
#define MY_TO_BY(y) ((y) / ICONHEIGHT - BOARDCENTER)
#define VALIDBXY( x, y) ((x) >= -BOARDCENTER && (x) <= BOARDCENTER && (y) >= -BOARDCENTER && (y) <= BOARDCENTER)

void PicDraw( int x, int y, int nr )
{
  int dx = x - gBufferX,
  	  dy = y - gBufferY;

  if (abs( dx) <= BOARDCENTER && abs( dy) <= BOARDCENTER)
    PutPic( (BOARDCENTER + dx)*ICONWIDTH + BOARDLEFT, (BOARDCENTER + dy)*ICONHEIGHT, gSprites[ nr], 0x03030303);
}

void PicRestore( int x, int y )
{
  int dx = x - gBufferX,
  	  dy = y - gBufferY;

  if (abs( dx) <= BOARDCENTER && abs( dy) <= BOARDCENTER)
  {
    if (gBuffer[ BOARDCENTER + dx][ BOARDCENTER + dy] != DARKNESS &&
        gBuffer[ BOARDCENTER + dx][ BOARDCENTER + dy] != THE_VOID)
      gBuffer[ BOARDCENTER + dx][ BOARDCENTER + dy] = gWorld[ x][ y];
    PicDraw( x, y, gBuffer[ BOARDCENTER + dx][ BOARDCENTER + dy] & 0xFF);
  }
}

void SetBuffer( int xc, int yc )
{
  short x, y;

  gBufferX = xc;
  gBufferY = yc;

  memcpy( gPreviousBuffer, gBuffer, sizeof( Board));

  for (x = -BOARDCENTER; x <= BOARDCENTER; x++)
    for (y = -BOARDCENTER; y <= BOARDCENTER; y++)
    {
      if (InBox( x+xc, y+yc, 0, 0, WORLD_X_MAX, WORLD_Y_MAX))
        gBuffer[ x + BOARDCENTER][ y + BOARDCENTER] = gWorld[ x+xc][ y+yc];
      else
        gBuffer[ x + BOARDCENTER][ y + BOARDCENTER] = THE_VOID;
    }
}

void Shade( void )
{
  int x, y;

  for (x = BOARDCENTER - 2; x >= 0; x--)
  {
    if (gBuffer[ x + 1][ BOARDCENTER] & SHADOW_FLAG)
    {
      gBuffer[ x][ BOARDCENTER  ] = DARKNESS;
      gBuffer[ x][ BOARDCENTER-1] = DARKNESS;
      gBuffer[ x][ BOARDCENTER+1] = DARKNESS;
    }
    if (gBuffer[ BOARDCENTER][ x + 1] & SHADOW_FLAG)
    {
      gBuffer[ BOARDCENTER  ][ x] = DARKNESS;
      gBuffer[ BOARDCENTER-1][ x] = DARKNESS;
      gBuffer[ BOARDCENTER+1][ x] = DARKNESS;
    }
  }
  for (x = BOARDCENTER+2; x < BOARDSIZE; x++)
  {
    if (gBuffer[ x - 1][ BOARDCENTER] & SHADOW_FLAG)
    {
      gBuffer[ x][ BOARDCENTER  ] = DARKNESS;
      gBuffer[ x][ BOARDCENTER-1] = DARKNESS;
      gBuffer[ x][ BOARDCENTER+1] = DARKNESS;
    }
    if (gBuffer[ BOARDCENTER][ x - 1] & SHADOW_FLAG)
    {
      gBuffer[ BOARDCENTER  ][ x] = DARKNESS;
      gBuffer[ BOARDCENTER-1][ x] = DARKNESS;
      gBuffer[ BOARDCENTER+1][ x] = DARKNESS;
    }
  }
  for (x = BOARDCENTER-2; x >= 0; x--)
  {
    for (y = BOARDCENTER-2; y >= 0; y--)
      if (gBuffer[ x + 1][ y + 1] & SHADOW_FLAG)
        gBuffer[ x][ y] = DARKNESS;
    for (y = BOARDCENTER+2; y < BOARDSIZE; y++)
      if (gBuffer[ x + 1][ y - 1] & SHADOW_FLAG)
        gBuffer[ x][ y] = DARKNESS;
  }
  for (x = BOARDCENTER+2; x < BOARDSIZE; x++)
  {
    for (y = BOARDCENTER-2; y >= 0; y--)
      if (gBuffer[ x - 1][ y + 1] & SHADOW_FLAG)
        gBuffer[ x][ y] = DARKNESS;
    for (y = BOARDCENTER+2; y < BOARDSIZE; y++)
      if (gBuffer[ x - 1][ y - 1] & SHADOW_FLAG)
        gBuffer[ x][ y] = DARKNESS;
  }
}

void BufferDump( void )
{
  short x, y;

  hidemouse();
  for (x = 0; x < BOARDSIZE; x++)
    for (y = 0; y < BOARDSIZE; y++)
      if ((gBuffer[ x][ y] & 0xFF) != (gPreviousBuffer[ x][ y] & 0xFF))
        Put3Bytes( x*ICONWIDTH + BOARDLEFT, y*ICONHEIGHT,
                   gSprites[ gBuffer[ x][ y] & 0xFF]);
  showmouse( NOFORCE);
}

void BufferDraw( void )
{
  short x, y;

  hidemouse();
  for (x = 0; x < BOARDSIZE; x++)
    for (y = 0; y < BOARDSIZE; y++)
    {
      Put3Bytes( x*ICONWIDTH + BOARDLEFT, y*ICONHEIGHT,
                 gSprites[ gBuffer[ x][ y] & 0xFF]);
    }
  showmouse( NOFORCE);
}

void AlertBadGuys( Person *p )
{
  short x, y;
  Person *b;

  for (x = 0; x < BOARDSIZE; x++)
    for (y = 0; y < BOARDSIZE; y++)
      if (gBuffer[ x][ y] & CHARACTER_FLAG)
      {
        b = FindCharacter( gBufferX - BOARDCENTER + x, gBufferY - BOARDCENTER + y);
        if (b && !b->isPlayer && !b->opponent)
          b->opponent = p;
      }
}


/*
------------------------------
	Dialog boxes etc...
------------------------------
*/

struct Button
{
  short left, top,
	right, bottom;
  long key;
  short index;
  char text[50];
  struct Button *next;
};

#define BUTTON_LIGHT  		ORANGE
#define BUTTON_NORMAL 		BROWN
#define BUTTON_DARK   		DARKBROWN
#define BUTTON_TEXT   		YELLOW
#define BUTTON_INACTIVE_TEXT	DARKBROWN

typedef struct Button *ButtonPtr;

void DisposeButtons( ButtonPtr *bHead )
{
  ButtonPtr b;

  while (*bHead)
  {
    b = *bHead;
    *bHead = (*bHead)->next;
    free( b);
  }
}

void AddButton( ButtonPtr *bHead,
		int left, int top, int right, int bottom,
		long key,
		int index,
		char *text )
{
  ButtonPtr b;

  b = malloc( sizeof( struct Button));
  b->next = *bHead;
  *bHead = b;
  b->left = left;
  b->right = right;
  b->top = top;
  b->bottom = bottom;
  b->key = key;
  b->index = index;
  if (text)
  {
    if (strlen( text) > 49)
      strcpy( b->text, "** Overflow **");
    else
      strcpy( b->text, text);
  }
  else
    *(b->text) = 0;
}

void DrawButton( ButtonPtr b, int active )
{
  int x, y;

  if (*(b->text))
  {
    hidemouse();
    SetColor( LIGHTGRAY);
    Line( b->right+1, b->top-1, b->right+1, b->bottom+1);
    Line( b->right+1, b->bottom+1, b->left, b->bottom+1);
    SetColor( DARKGRAY);
    Line( b->left-1, b->bottom+1, b->left-1, b->top-1);
    Line( b->left-1, b->top-1, b->right+1, b->top-1);
    SetColor( BLACK);
    Rectangle( b->left, b->top, b->right, b->bottom);
    SetColor( BUTTON_NORMAL);
    FillRect( b->left+1, b->top+1, b->right-1, b->bottom-1);
    SetColor( BUTTON_LIGHT);
    Rectangle( b->left+1, b->top+1, b->right-1, b->bottom-1);
    Rectangle( b->left+2, b->top+2, b->right-2, b->bottom-2);
    SetColor( BUTTON_DARK);
    Line( b->right-1, b->top+1, b->right-1, b->bottom-1);
    Line( b->right-1, b->bottom-1, b->left+2, b->bottom-1);
    Line( b->right-2, b->top+2, b->right-2, b->bottom-2);
    Line( b->right-2, b->bottom-2, b->left+3, b->bottom-2);

    x = b->left + (b->right-b->left-8*((int)strlen( b->text))) / 2;
    y = b->top - 4 + (b->bottom-b->top) / 2;
    SetBgColor( BUTTON_NORMAL);
    if (active)
      ReliefTextXY( x, y, b->text, BUTTON_LIGHT, BLACK, BUTTON_DARK, -1);
    else
      ReliefTextXY( x, y, b->text, BUTTON_LIGHT, BUTTON_DARK, BUTTON_NORMAL, -1);
    showmouse( NOFORCE);
  }
}

void DrawOneButton( ButtonPtr b, int index, int active )
{
  while (b && index != b->index)
    b = b->next;
  if (!b) return;

   DrawButton( b, active);
}

void DrawButtons( ButtonPtr b )
{
  while (b)
  {
    DrawButton( b, TRUE);
    b = b->next;
  }
}

int CheckButtons( ButtonPtr bHead,
		  short x, short y,
		  long key,
		  int *index )
{
  while (bHead)
  {
    if (InBox( x, y, bHead->left, bHead->top, bHead->right, bHead->bottom)
	||
	(key != 0 && key == bHead->key))
    {
      *index = bHead->index;
      return TRUE;
    }
    bHead = bHead->next;
  }
  return FALSE;
}

int UserDialog( ButtonPtr bHead )
{
  short mx, my, buttons;
  int i;
  long key;

  showmouse( FORCE);
  do
  {
    key = 0;
    do
    {
      Mouse( &mx, &my, &buttons);
      if (KEYPRESSED) key = KEYREAD;
    }
    while (key == 0 && buttons == 0);

    if (!buttons) mx = -1;
  }
  while (!CheckButtons( bHead, mx, my, key, &i));

  return i;
}

void FrameButton( ButtonPtr b, int index, int down )
{
  while (b && index != b->index)
    b = b->next;
  if (!b) return;

  hidemouse();
  if (down)
    SetColor( BUTTON_DARK);
  else
    SetColor( BUTTON_LIGHT);
  Rectangle( b->left+1, b->top+1, b->right-1, b->bottom-1);
  Rectangle( b->left+2, b->top+2, b->right-2, b->bottom-2);
  SetColor( BUTTON_DARK);
  Line( b->right-1, b->top+1, b->right-1, b->bottom-1);
  Line( b->right-1, b->bottom-1, b->left+2, b->bottom-1);
  Line( b->right-2, b->top+2, b->right-2, b->bottom-2);
  Line( b->right-2, b->bottom-2, b->left+3, b->bottom-2);
  showmouse( NOFORCE);
}

void TextEditFrame( int x, int y, int len )
{
  SetColor( LIGHTGRAY);
  Line( x-3, y+11, x+len*8+3, y+11);
  Line( x+len*8+3, y+11, x+len*8+3, y - 3);
  SetColor( DARKGRAY);
  Line( x-3, y-3, x+len*8+3, y-3);
  Line( x-3, y-3, x-3, y+11);
}

void TextEdit( int x, int y, int len, long key, char *s )
{
  unsigned char letter, *t;
  char buff[50];

  hidemouse();
  letter = key & 0xFF;
  if (key == 8 && *s)
  {
    s[ strlen( s) - 1] = 0;
    ShowText( x + strlen( s)*8, y, " ", BLACK, GRAY);
  }
  else if (key == 27)
  {
    *s = 0;
    memset( buff, ' ', len);
    buff[ len] = 0;
    len = 0;
    strncpy( buff, s, strlen(s));
    ShowText( x, y, buff, BLACK, GRAY);
  }
  else if (letter >= 32 && strlen( s) < len)
  {
    t = s + strlen( s);
    *t = letter;
    t++;
    *t = 0;
    t--;
    ShowText( x + strlen( s)*8 - 8, y, t, BLACK, GRAY);
  }

  Click();
  showmouse( NOFORCE);
}

int Question( char *s )
{
  int item;
  ButtonPtr b;

  hidemouse();
  FadeOut();
  ClrScr();
  gMustDraw = TRUE;

  b = NULL;
  DrawStoneBox( 150, 100, 300, 120);
  AddButton( &b, 240, 175, 290, 200, 13, 0, "Yes");
  AddButton( &b, 310, 175, 360, 200, 27, 1, "No");
  DrawButtons( b);

  SetColor( BLACK);
  SetBgColor( GRAY);
  ShowText( 300 - strlen( s)*4, 140, s, BLACK, -1);
  FadeIn( gPalette);
  showmouse( NOFORCE);

  item = UserDialog( b);

  FrameButton( b, item, 4);
  Click();
  while (MOUSEK);
  FrameButton( b, item, 0);
  DisposeButtons( &b);
  if (item == 0)
    return TRUE;
  else
    return FALSE;
}

void Confirm( char *s, char *buttonText )
{
  ButtonPtr b;

  hidemouse();
  FadeOut();
  ClrScr();
  gMustDraw = TRUE;

  b = NULL;
  DrawStoneBox( 150, 100, 300, 120);
  AddButton( &b, 275, 175, 325, 200, 13, 0, buttonText);
  DrawButtons( b);

  SetColor( BLACK);
  SetBgColor( GRAY);
  ShowText( 300 - strlen( s)*4, 140, s, BLACK, -1);
  FadeIn( gPalette);
  showmouse( NOFORCE);

  UserDialog( b);

  FrameButton( b, 0, 4);
  Click();
  while (MOUSEK);
  FrameButton( b, 0, 0);

  DisposeButtons( &b);
}

void MagusFace( int pic, char *s, char *buttonText )
{
  ButtonPtr b;

  hidemouse();
  FadeOut();
  ClrScr();
  gMustDraw = TRUE;

  b = NULL;
  DrawStoneBox( 200, 100, 220, 120);
  PutPic( 290, 125, gSprites[ pic], 0x03030303);
  AddButton( &b, 260, 175, 360, 200, 13, 0, buttonText);
  DrawButtons( b);

  SetBgColor( GRAY);
  SetColor( BLACK);
  ShowText( 220, 160, s, BLACK, -1);

  FadeIn( gPalette);
  showmouse( NOFORCE);

  UserDialog( b);

  FrameButton( b, 0, 4);
  Click();
  while (MOUSEK);
  FrameButton( b, 0, 0);

  DisposeButtons( &b);
}

void ModifyCharacter( Person *p )
{
  ButtonPtr b;
  int index;
  int points;

  hidemouse();
  FadeOut();
  ClrScr();
  
  DrawStoneBox( 200, 50, 200, 315);
  PutPic( 250, 75, gSprites[ p->icon], 0x03030303);
  SetColor( BLACK);
  SetBgColor( GRAY);
  ShowText( 225, 105, p->name, BLACK, -1);
  
  b = NULL;
  AddButton( &b, 320, 125, 345, 150, 0,  0, "-");
  AddButton( &b, 350, 125, 375, 150, 0,  1, "+");
  AddButton( &b, 320, 160, 345, 185, 0,  2, "-");
  AddButton( &b, 350, 160, 375, 185, 0,  3, "+");
  AddButton( &b, 320, 195, 345, 220, 0,  4, "-");
  AddButton( &b, 350, 195, 375, 220, 0,  5, "+");
  AddButton( &b, 320, 230, 345, 255, 0,  6, "-");
  AddButton( &b, 350, 230, 375, 255, 0,  7, "+");
  AddButton( &b, 320, 265, 345, 290, 0,  8, "-");
  AddButton( &b, 350, 265, 375, 290, 0,  9, "+");
  AddButton( &b, 225, 315, 375, 340, 13, 10, "OK");
  DrawButtons( b);

  points = 0;
  sprintf( uString, "Mana     %3d", p->maxPower);
  ShowText( 210, 130, uString, BLACK, GRAY);
  sprintf( uString, "Health   %3d", p->maxHealth);
  ShowText( 210, 165, uString, BLACK, GRAY);
  sprintf( uString, "Strength  %2d", p->strength);
  ShowText( 210, 200, uString, BLACK, GRAY);
  sprintf( uString, "Agility   %2d", p->skill);
  ShowText( 210, 235, uString, BLACK, GRAY);
  sprintf( uString, "Wisdom    %2d", p->wisdom);
  ShowText( 210, 270, uString, BLACK, GRAY);
  sprintf( uString, "%2d points left", points);
  ShowText( 210, 300, uString, BLACK, GRAY);
  FadeIn( gPalette);

  showmouse( NOFORCE);

  do
  {
    hidemouse();
    SetColor( BLACK);
    SetBgColor( GRAY);
    sprintf( uString, "%2d", points);
    ShowText( 210, 300, uString, BLACK, GRAY);
    showmouse( NOFORCE);

    index = UserDialog( b);

    FrameButton( b, index, TRUE);
    while (MOUSEK);

    switch (index)
    {
      case 0:
	if (p->maxPower)
	{
	  p->maxPower -= 10;
	  points++;
	  sprintf( uString, "%3d", p->maxPower);
	  ShowText( 282, 130, uString, BLACK, GRAY);
	}
	break;

      case 1:
	if (points)
	{
	  p->maxPower += 10;
	  points--;
	  sprintf( uString, "%3d", p->maxPower);
	  ShowText( 282, 130, uString, BLACK, GRAY);
	}
	break;

      case 2:
	if (p->maxHealth > 10)
	{
	  p->maxHealth -= 10;
	  points++;
	  sprintf( uString, "%3d", p->maxHealth);
	  ShowText( 282, 165, uString, BLACK, GRAY);
	}
	break;

      case 3:
	if (points)
	{
	  p->maxHealth += 10;
	  points--;
	  sprintf( uString, "%3d", p->maxHealth);
	  ShowText( 282, 165, uString, BLACK, GRAY);
	}
	break;

      case 4:
	if (p->strength > 1)
	{
	  p->strength--;
	  points++;
	  sprintf( uString, "%2d", p->strength);
	  ShowText( 291, 200, uString, BLACK, GRAY);
	}
	break;

      case 5:
	if (points && p->strength < 30)
	{
	  p->strength++;
	  points--;
	  sprintf( uString, "%2d", p->strength);
	  ShowText( 291, 200, uString, BLACK, GRAY);
	}
	break;

      case 6:
	if (p->skill > 1)
	{
	  p->skill--;
	  points++;
	  sprintf( uString, "%2d", p->skill);
	  ShowText( 291, 235, uString, BLACK, GRAY);
	}
	break;

      case 7:
	if (points && p->skill < 20)
	{
	  p->skill++;
	  points--;
	  sprintf( uString, "%2d", p->skill);
	  ShowText( 291, 235, uString, BLACK, GRAY);
	}
	break;

      case 8:
	if (p->wisdom)
	{
	  p->wisdom--;
	  points++;
	  sprintf( uString, "%2d", p->wisdom);
	  ShowText( 291, 270, uString, BLACK, GRAY);
	}
	break;

      case 9:
	if (points && p->wisdom < 20)
	{
	  p->wisdom++;
	  points--;
	  sprintf( uString, "%2d", p->wisdom);
	  ShowText( 291, 270, uString, BLACK, GRAY);
	}
	break;
    }
    FrameButton( b, index, FALSE);
  }
  while (index != 10);

  p->health = p->maxHealth;
  p->power = p->maxPower;

  DisposeButtons( &b);
}

void DrawClass( int class, int female )
{
  if (female)
    PutPic( 200, 125, gSprites[ gClassData[ class].femaleIcon], 0x3030303);
  else
    PutPic( 200, 125, gSprites[ gClassData[ class].maleIcon], 0x3030303);
  SetColor( GRAY);
  FillRect( 250, 140, 410, 150);
  ShowText( 250, 140, gClassData[ class].name, BLACK, -1);
}

void DrawSelectionScreen( int class, int female, char *name, ButtonPtr b )
{
  hidemouse();
  FadeOut();
  ClrScr();

  DrawStoneBox( 150, 100, 325, 120);
  DrawButtons( b);
  DrawClass( class, female);
  SetColor( BLACK);
  SetBgColor( GRAY);
  TextEditFrame( 250, 120, CHARACTER_MAXNAME);
  TextEdit( 250, 120, CHARACTER_MAXNAME, 0, name);
  FadeIn( gPalette);
  showmouse( NOFORCE);
}

void SelectCharacters( void )
{
  int female = 0,
      class = 0;
  short mx, my, mk;
  int item;
  long key;
  Person *pc;
  char name[ CHARACTER_MAXNAME + 1];
  ButtonPtr b;

  *name = 0;

  b = NULL;
  AddButton( &b, 175, 175, 225, 200, 0x4B00, 0, "<<");
  AddButton( &b, 250, 175, 300, 200, 0x4D00, 1, ">>");
  AddButton( &b, 200, 125, 225, 150, 0x5200, 2, NULL);
  AddButton( &b, 325, 175, 375, 200, 13, 3, "OK");
  AddButton( &b, 400, 175, 450, 200, 0, 4, "Done");

  DrawSelectionScreen( class, female, name, b);
  DrawOneButton( b, 3, FALSE);
  DrawOneButton( b, 4, FALSE);

  while (TRUE) /* Vi return:ar inifr†n loopen... */
  {
    key = 0;
    do
    {
      if (KEYPRESSED) key = KEYREAD;
      Mouse( &mx, &my, &mk);
    }
    while (key == 0 && mk == 0);

    if (mk == 0)
      mx = my = 0;

    if (!CheckButtons( b, mx, my, key, &item))
      item = -1;

    hidemouse();
    if (item >= 0 && item != 2)
      FrameButton( b, item, TRUE);
    while (MOUSEK);

    if (item == 0)
    {
      Click();
      Pause( 5);
      class--;
      if (class < 0)
	class = SELECTABLECLASSES - 1;
      DrawClass( class, female);
    }
    else if (item == 1)
    {
      Click();
      Pause( 5);
      class++;
      if (class > SELECTABLECLASSES - 1)
	class = 0;
      DrawClass( class, female);
    }
    else if (item == 2)
    {
      Click();
      female = !female;
      DrawClass( class, female);
    }
    else if (item == 4)
    {
      hidemouse();
      Click();
      Pause( 5);
      showmouse( NOFORCE);
      if (gCharacters)
      {
	DisposeButtons( &b);
	showmouse( NOFORCE);
	return;
      }
      else
	Ding();
    }
    else if (item == 3)
    {
      Click();
      Pause( 5);
      if (*name)
      {
	pc = CreateCharacter( name, class, female, TRUE);
	ModifyCharacter( pc);
	PlaceCharacterCloseTo( pc, gGateX, gGateY, 0);
	DrawSelectionScreen( class, female, name, b);
	/* Send an Escape keycode to TextEdit to clear the name */
	TextEdit( 250, 120, CHARACTER_MAXNAME, 27, name);
      }
    }
    else
    {
      TextEdit( 250, 120, CHARACTER_MAXNAME, key, name);
      if (!*name || !*(name+1))
	DrawOneButton( b, 3, *name);
    }

    if (item >= 0 && item != 2)
      FrameButton( b, item, FALSE);
    showmouse( NOFORCE);
  }
}

void DropObject( Person *p, struct item *i )
{
  if (!p || !i) return;

  if (i->locked)
  {
    Message( "Oh dear, this seems to be cursed...");
    return;
  }

  if (CANDROP_ITEMS & (1 << ((p->backGround >> 8) & 31)))
  {
    RemoveItemFromList( i, &(p->carrying));
    RemoveItemFromList( i, &(p->wearing));
    RemoveItemFromList( i, &(p->wielding));

    PlaceItem( i, p->x, p->y);
  }
  else
    Message( "You can't drop anything here");
}

void PickItemUp( Person *p, struct item *i )
{
  if (!p->moves)
  {
    sprintf( uString, "%s has no MP's left", p->name);
    Message( uString);
    return;
  }

  sprintf( uString, "%s took %s", p->name, gObjectData[ i->id].name);
  Message( uString);
  RemoveItem( i);
  AddItemToList( i, &(p->carrying));
  p->moves--;  
}

struct item *PreviousItem( struct item *i, struct item *list )
{
  while (list->next && list->next != i)
    list = list->next;
  return list;
}

void Inventory( Person *p )
{
  struct item *i;
  int count;
  int x, y;
  
  hidemouse();

  SetColor( GRAY);
  i = p->wielding;
  count = 0;
  while (count < 5)
  {
    x = 10 + count*(ICONWIDTH + 1);
    y = 182;
    if (i)
    {
      PutPic( x, y,
			  gSprites[ gObjectData[ i->id].icon], 0x03030303);
      i = i->next;
    }
    else
      FillRect( x, y, x + ICONWIDTH, y + ICONHEIGHT);
    count++;
  }

  i = p->wearing;
  count = 0;
  while (count < 10)
  {
    x = 10 + (count % 5)*(ICONWIDTH + 1);
    y = 217 + (count / 5)*(ICONHEIGHT + 1);
    if (i)
    {
      PutPic( x, y,
			  gSprites[ gObjectData[ i->id].icon], 0x03030303);
      i = i->next;
    }
    else
      FillRect( x, y, x + ICONWIDTH, y + ICONHEIGHT);
    count++;
  }


  i = p->carrying;
  count = 0;
  while (count < 25)
  {
    x = 10 + (count % 5)*(ICONWIDTH + 1);
    y = 277 + (count / 5)*(ICONHEIGHT + 1);
    if (i)
    {
      PutPic( x, y,
			  gSprites[ gObjectData[ i->id].icon], 0x03030303);
      i = i->next;
    }
    else
      FillRect( x, y, x + ICONWIDTH, y + ICONHEIGHT);
    count++;
  }
  showmouse( NOFORCE);
}

#define WIELDING 0
#define WEARING  1
#define INPACK	 2

struct item *GetClickedItem( Person *p, int x, int y, int *state )
{
  struct item *i;

  x -= 10;
  if (y >= 182 && y <= 182 + ICONHEIGHT)
  {
    x /= ICONWIDTH + 1;
    i = p->wielding;
    if (state) *state = WIELDING;
  }
  else if (y >= 217 && y <= 217 + 2*ICONHEIGHT)
  {
    x /= ICONWIDTH + 1;
    x += 5*((y - 217) / (ICONHEIGHT + 1));
    i = p->wearing;    
    if (state) *state = WEARING;
  }
  else if (y >= 277)
  {
    x /= ICONWIDTH + 1;
    x += 5*((y - 277) / (ICONHEIGHT + 1));
    i = p->carrying;
    if (state) *state = INPACK;
  }
  else
    i = NULL;
  
  while (i && x)
  {
    x--;
    i = i->next;
  }
  return i;
}


struct item *GetFromInventory( Person *p, int *state )
{
  ButtonPtr b;
  short x, y, button;
  struct item *i;
  int index;

  hidemouse();
  DrawStoneBox( 200, 100, 300, 150);
  b = NULL;
  AddButton( &b, 325, 200, 375, 225, 13, 0, "OK");
  DrawButtons( b);

  SetBgColor( GRAY);
  SetColor( BLACK);
  ShowText( 275, 120, "Pick an item", BLACK, -1);

  i = NULL;
  while (TRUE)
  {
    while (MOUSEK);
    showmouse( FORCE);
    do
    {
      Mouse( &x, &y, &button);
    }
    while (!button);
    hidemouse();

    if (CheckButtons( b, x, y, 0, &index))
    {
      FrameButton( b, 0, TRUE);
      while (MOUSEK);
      FrameButton( b, 0, FALSE);
      DisposeButtons( &b);
      BufferDraw();
      showmouse( NOFORCE);
      return i;
    }

    i = GetClickedItem( p, x, y, state);
    SetColor( GRAY);
    FillRect( 210, 150, 440, 190);
    if (i)
    {
      PutPic( 300, 150, gSprites[ gObjectData[ i->id].icon], 0x03030303);
      SetBgColor( GRAY);
      SetColor( BLACK);
      GetItemDescription( i, uString);
      ShowText( 350 - strlen( uString)*4, 180, uString, BLACK, -1);
    }
  }
}

void MovesLeft( Person *p )
{
  sprintf( uString, "%2d", p->moves);
  if (p->moves)
    ShowText( 10, 104, uString, BLACK, GRAY);
  else
    ShowText( 10, 104, uString, DARKGRAY, GRAY);
}

void StatusPanel( void )
{
  ShowText( 10,  92, "Xp",        BLACK, -1);
  ShowText( 10, 116, "Strength",  BLACK, -1);
  ShowText( 10, 128, "Agility",   BLACK, -1);
  ShowText( 10, 140, "Wisdom",    BLACK, -1);
  ShowText( 10, 152, "Load",      BLACK, -1);
  ShowText( 10, 170, "Using:",    BLACK, -1);
  ShowText( 10, 205, "Carrying:", BLACK, -1);
  ShowText( 10, 265, "In pack:",  BLACK, -1);
}

void Status( Person *p )
{
  hidemouse();
  PutPic( 10, 57, gSprites[ p->icon], 0x03030303);
  SetColor( GRAY);
  FillRect( 10, 46, 10 + CHARACTER_MAXNAME*8, 55);
  ShowText( 10, 46, p->name, BLACK, -1);

  sprintf( uString, "%-15s", gLevelNames[ p->level]);
  ShowText( 10, 80, uString, BLACK, GRAY);

  sprintf( uString, "%-4ld", p->xp);
  ShowText( 34, 92, uString, BLACK, GRAY);
  sprintf( uString, "%2d MP's", p->moves);
  ShowText( 10, 104, uString, BLACK, GRAY);
  sprintf( uString, "%2d", p->strength);
  ShowText( 90, 116, uString, BLACK, GRAY);
  sprintf( uString, "%2d", p->skill);
  ShowText( 90, 128, uString, BLACK, GRAY);
  sprintf( uString, "%2d", p->wisdom);
  ShowText( 90, 140, uString, BLACK, GRAY);
  sprintf( uString, "%2d", Load( p));
  if (OverLoaded( p))
    ShowText( 90, 152, uString, DARKRED, GRAY);
  else
    ShowText( 90, 152, uString, BLACK, GRAY);
  showmouse( NOFORCE);

  MovesLeft( p);
}

void Bars( Person *p )
{
  int x;

  SetColor( LIGHTGRAY);
  Rectangle( 56, 59, 82, 66);
  SetColor( DARKGRAY);
  Line( 56, 59, 82, 59);
  Line( 56, 59, 56, 66);

  if (p->maxPower == 0)
    x = 0;
  else
    x = (25*p->power)/p->maxPower;
  SetColor( BLUE);
  if (x > 0)
    FillRect( 57, 60, 56 + x, 65);
  if (x < 25)
  {
    SetColor( GRAY);
    FillRect( 57 + x, 60, 81, 65);
  }
  sprintf( uString, "%3d/%3d", max( 0, p->power), p->maxPower);
  ShowText( 90, 58, uString, BLACK, GRAY);

  SetColor( LIGHTGRAY);
  Rectangle( 56, 69, 82, 76);
  SetColor( DARKGRAY);
  Line( 56, 69, 82, 69);
  Line( 56, 69, 56, 76);

  if (p->maxHealth == 0)
    x = 0;
  else
    x = (25*p->health)/p->maxHealth;
  SetColor( GREEN);
  if (x > 0)
    FillRect( 57, 70, 56 + x, 75);
  if (x < 25)
  {
    SetColor( GRAY);
    FillRect( 57 + x, 70, 81, 75);
  }
  sprintf( uString, "%3d/%3d", p->health, p->maxHealth);
  ShowText( 90, 68, uString, BLACK, GRAY);
}


struct OverviewInfo
{
  Person *person;
  int    hp;
  int    mana;
  int    mp;
};

#define OVERVIEW_MAX    8
#define OVERVIEW_LEFT   400
#define OVERVIEW_RIGHT  550
#define OVERVIEW_TOP    405

static struct OverviewInfo overviewInfo[ OVERVIEW_MAX];


static void RatingLine( int x, int y, int color, int value, int max )
{
  if (value > max)
    value = max;

  SetColor( BLACK);
  Rectangle( x, y, x + max + 1, y + 3);
  if (value > 0)
  {
    SetColor( color);
    FillRect( x + 1, y + 1, x + value, y + 2);
  }
  if (value < max)
  {
    SetColor( GRAY);
    FillRect( x + value + 1, y + 1, x + max, y + 2);
  }
}

static void OverviewPanel( int forceDraw )
{
  Person *p = gCharacters;
  int count = 0;
  int x = OVERVIEW_LEFT, y = OVERVIEW_TOP;
  int hp, mp, mana;
  struct OverviewInfo *info;

  hidemouse();
  while (p && count < OVERVIEW_MAX)
  {
    if (p->isPlayer)
    {
      if (p->maxPower == 0)
        mana = 0;
      else
        mana = (22*p->power)/p->maxPower;

      if (p->maxHealth == 0)
        hp = 0;
      else
        hp = (22*p->health)/p->maxHealth;

      if (p->speed == 0)
        mp = 0;
      else
        mp = (22*p->moves)/p->speed;

      info = &overviewInfo[ count];
      if (forceDraw ||
          info->person != p ||
          info->hp != hp ||
          info->mana != mana ||
          info->mp != mp)
      {
        PutPic( x + 2, y, gSprites[ p->icon], 0x03030303);
        RatingLine( x + 2, y + 23, RED, mp, 22);
        RatingLine( x + 2, y + 26, GREEN, hp, 22);
        RatingLine( x + 2, y + 29, DARKBLUE, mana, 22);
        info->person = p;
        info->hp = hp;
        info->mana = mana;
        info->mp = mp;
      }
      count++;
      x += 30;
      if (x + 30 > OVERVIEW_RIGHT)
      {
        x = OVERVIEW_LEFT;
        y += 35;
      }
    }
    p = p->next;
  }

  SetColor( GRAY);
  while (count < OVERVIEW_MAX)
  {
    info = &overviewInfo[ count];
    if (forceDraw || info->person)
    {
      info->person = NULL;
      FillRect( x, y, x + 29, y + 34);
    }
    x += 30;
    if (x + 30 > OVERVIEW_RIGHT)
    {
      x = OVERVIEW_LEFT;
      y += 35;
    }
    count++;
  }
  showmouse( NOFORCE);
}

static int OverviewClick( int x, int y, Person **selected )
{
  int index;
  Person *p;

  if (x >= OVERVIEW_LEFT && x <= OVERVIEW_RIGHT &&
      y >= OVERVIEW_TOP)
  {
    index = (x - OVERVIEW_LEFT)/30 + 5*((y - OVERVIEW_TOP)/35);
    if (index < 0 || index >= OVERVIEW_MAX)
      return 0;

    p = gCharacters;
    while (p && index >= 0)
    {
      if (p->isPlayer)
      {
        if (!index)
        {
          *selected = p;
          return 1;
        }
        index--;
      }
      p = p->next;
    }
    return 0;
  }
  return 0;
}

void Experience( Person *p, long xp )
{
  p->xp += xp;
  
  if (p->xp > gLevelLimits[ p->level] &&
  	  p->level < MAXLEVEL)
  {
    p->maxHealth += 10;
    if (p->wisdom)
      p->maxPower += 10;
    p->level++;
  }
}

void PickUp( Person *p )
{
  struct item *list, *i;
  int button;
  ButtonPtr b;

  if (!(gWorld[ p->x][ p->y] & ITEM_FLAG))
  {
    Message( "Nothing here.");
    return;
  }

  hidemouse();
  b = NULL;
  DrawStoneBox( 200, 100, 300, 150);
  AddButton( &b, 225, 200, 275, 225, 0x4800, 0, "<<");
  AddButton( &b, 300, 200, 350, 225, 0x5000, 1, ">>");
  AddButton( &b, 425, 200, 475, 225, 27, 2, "Done");
  AddButton( &b, 425, 125, 475, 150, 13, 3, "Take");
  DrawButtons( b);

  showmouse( NOFORCE);
  i = list = FindItem( p->x, p->y);

  DrawOneButton( b, 0, i->next != NULL);
  DrawOneButton( b, 1, i->next != NULL);
  DrawOneButton( b, 3, p->moves);

  while (TRUE)
  {
    hidemouse();
    if (i)
      PutPic( 250, 125, gSprites[ gObjectData[ i->id].icon], 0x03030303);
    else
    {
      SetColor( GRAY);
      FillRect( 250, 125, 250 + ICONWIDTH, 125 + ICONHEIGHT);
      DrawOneButton( b, 0, FALSE);
      DrawOneButton( b, 1, FALSE);
      DrawOneButton( b, 3, FALSE);
    }

    SetColor( GRAY);
    FillRect( 225, 152, 425, 162);
    SetBgColor( GRAY);
    if (i)
    {
      SetColor( BLACK);
      GetItemDescription( i, uString);
      ShowText( 225, 152, uString, BLACK, -1);
    }

    sprintf( uString, "%2d items", HowManyItems( list));
    ShowText( 225, 162, uString, BLACK, GRAY);

    showmouse( NOFORCE);
    while (MOUSEK);
    button = UserDialog( b);

    FrameButton( b, button, TRUE);
    while (MOUSEK);

    if (button == 0)
    {
      if (i)
      {
	Click();
	i = PreviousItem( i, list);
      }
      else
	Ding();
    }
    else if (button == 1)
    {
      if (i)
      {
	Click();
	i = i->next;
	if (!i)
	  i = list;
      }
      else
	Ding();
    }
    else if (button == 2)
    {
      Click();
      FrameButton( b, 2, FALSE);
      DisposeButtons( &b);
      hidemouse();
      SetColor( 0);
      FillRect( 200, 100, 350, 250);
      BufferDraw();
      showmouse( NOFORCE);
      return;
    }
    else if (button == 3)
    {
      if (i)
      {
	Click();
	PickItemUp( p, i);
	i = list = FindItem( p->x, p->y);
	if (list && list->next == NULL)
	{
	  DrawOneButton( b, 0, i->next != NULL);
		  DrawOneButton( b, 1, i->next != NULL);
	}
	DrawOneButton( b, 3, list && p->moves);
	Inventory( p);
	Status( p);
      }
    }
    FrameButton( b, button, FALSE);
  }
}

void Funeral( Person *p )
{
  char s[100];

  SetColor( BLACK);
  FillRect( BOARDLEFT, 0, 639, 399);
  PutPic( 200, 80, gSprites[ 130], 0x03030303);
  SetColor( RED);
  SetBgColor( BLACK);
  sprintf( s, "%s is dead", p->name);
  OutTextXY( 225, 140, s);
  gMustDraw = TRUE;
}

void HighlightCenter()
{
  int x = BOARDCENTER*ICONWIDTH + BOARDLEFT;
  int y = BOARDCENTER*ICONHEIGHT;
  SetColor( YELLOW);
  Line( x, y, x + 2, y);
  Line( x, y, x, y + 2);
  x += ICONWIDTH-1;
  Line( x, y, x - 2, y);
  Line( x, y, x, y + 2);
  y += ICONHEIGHT-1;
  Line( x, y, x - 2, y);
  Line( x, y, x, y - 2);
  x -= ICONWIDTH-1;
  Line( x, y, x + 2, y);
  Line( x, y, x, y - 2);
}

void Screen( Person *p )
{
  hidemouse();
  Status( p);
  Bars( p);
  Inventory( p);
  OverviewPanel( gMustDraw);

  if (p->health <= 0)
    Funeral( p);
  else
  {
    SetBuffer( p->x, p->y);
    Shade();
    if (gMustDraw)
    {
      BufferDraw();
      gMustDraw = FALSE;
    }
    else
      BufferDump();
    HighlightCenter();
  }
  showmouse( NOFORCE);
}

void ShowItem( struct item *i )
{
  if (!i) return;

  GetItemDescription( i, uString);
  Message( uString);
}

void AllIsLost( void )
{
  MagusFace( 121, "The Dark One smiles...", "He-he-he");
}

void Victory( void )
{
  Person *p;

  p = gCharacters;
  while (p)
  {
    if (p->isPlayer && !p->lifeSpan)
    {
      Experience( p, 1000);
      EnterHallOfFame( p);
    }
    p = p->next;
  }

  MagusFace( 166, "Pure luck...", "Blah!");
  MagusFace( 167, "Look, guys...", "Please?");
  MagusFace( 168, "Nooooo", "Aaargh!");
  MagusFace( 120, "", "Horray!");
}

void Move( Person *p, int dx, int dy )
{
  int x, y;
  
  if (!p->moves) return;
  
  if (!p->confused || Rand( 10) >= 7)
  {
    dx = SGN( dx);
    dy = SGN( dy);
  }
  else
  {
    dx = Rand( 3) - 1;
    dy = Rand( 3) - 1;
  }
  x = p->x + dx;
  y = p->y + dy;

  if (Treadable( x, y, p->canWalk))
  {
    RemoveCharacter( p);
    PlaceCharacter( p, x, y);
    p->moves--;
  }
}

int ValidTarget( Person *p, int dx, int dy )
{
  int x, y;

  if (abs( dx) > BOARDSIZE/2 || abs( dy) > BOARDSIZE/2) return FALSE;
  
  x = p->x + dx;
  y = p->y + dy;
  
  if (!InBox( x, y, 0, 0, WORLD_X_MAX, WORLD_Y_MAX)) return FALSE;
  
  x += SGN( p->x - x);
  y += SGN( p->y - y);

  while (x != p->x || y != p->y)
  {
    if (gWorld[ x][ y] & SHADOW_FLAG) return FALSE;
    x += SGN( p->x - x);
    y += SGN( p->y - y);
  }
  return TRUE;
}

int WornArmor( Person *p )
{
  struct item *i;
  int armor;

  armor = 0;  
  i = p->wearing;
  while (i)
  {
    if (gObjectData[ i->id].kind == O_CLASS_ARMOR)
      armor += gObjectData[ i->id].value + i->modifier;
    i = i->next;
  }
  return armor;
}

void Damage( int x, int y, int damage, Person *p )
{
  Person *victim;

  victim = FindCharacter( x, y);
  if (!victim) return;
  
  victim->opponent = p;
  
  damage -= WornArmor( victim);
  if (victim->ward)
    damage >>= 1;

  if (damage > 0)
  {
    victim->health -= damage;

    if (victim->health <= 0)
    {
      if (victim->isPlayer && !victim->lifeSpan)
      {
        sprintf( uString, "%s has been killed", victim->name);
        Message( uString);
      }
      Death( victim, p);
      Experience( p, 1);
    }
    else if (victim->isPlayer && !victim->lifeSpan)
    {
      sprintf( uString, "%s takes %d points of damage", victim->name, damage);
      Message( uString);
    }
  }
}

#define FAILURE	0
#define SUCCESS	1
#define PERFECT 2

int SkillRoll( Person *p, struct item *i )
{
  int skill, die;

  skill = p->skill;
  if (i)
    skill += i->modifier;

  skill = min( skill, 19);

  die = Rand( 20) + 1;

  if (die <= skill)
  {
    if (die <= 3)
    {
      Experience( p, 2);
      return PERFECT;
    }
    Experience( p, 1);
    return SUCCESS;
  }
  return FAILURE;
}

int WisdomRoll( Person *p, struct item *i )
{
  int wisdom, die;

  wisdom = p->wisdom;
  wisdom += i->modifier;

  wisdom = min( wisdom, 19);

  die = Rand( 20) + 1;

  if (die <= wisdom)
  {
    if (die <= 3)
    {
      Experience( p, 2);
      return PERFECT;
    }
    Experience( p, 1);
    return SUCCESS;
  }
  return FAILURE;
}

int GetDamage( Person * p, struct item *i )
{
  int damage;

  if (!i) return 0;
  
  damage = gObjectData[ i->id].value;
  damage += i->modifier;
  /* damage += p->level >> 1; */
  damage += p->damageBonus;

  return damage;
}

void Violence( Person *p, int dx, int dy, int damage, int wasPerfect )
{
  int x, y;

  x = p->x + dx;
  y = p->y + dy;
  if (wasPerfect)
  {
    PicDraw( x, y, 50);
    Damage( x, y, damage + (damage/2), p);
  }
  else
  {
    PicDraw( x, y, 48);
    Damage( x, y, (damage/2) + Rand( (damage > 1 ? (damage/2) : 0)), p);
  }
  Hit();
  Pause( 10);
  PicRestore( x, y);
}

void Strike( Person *p, struct item *i, int dx, int dy )
{
  int damage, success;

  dx = SGN( dx);
  dy = SGN( dy);

  if (!p->moves) return;

  Swing();
  Pause( 15);

  p->moves--;
  if (!ValidTarget( p, dx, dy)) return;

  if (FindCharacter( p->x + dx, p->y + dy))
  {
    do
    {
      success = SkillRoll( p, i);
      if (success)
      {
        damage = GetDamage( p, i);
        damage++;
        damage += p->strength >> 2;
        Violence( p, dx, dy, damage, success == PERFECT);
      }
      if (i)
        i = i->next;
    }
    while (i);
  }
}

void Shoot( Person *p, struct item *i, int dx, int dy )
{
  int damage, success;
  struct item *arrows;

  if (!p->moves) return;

  if (abs( dx) <= 1 && abs( dy) <= 1) return;

  if (!ValidTarget( p, dx, dy)) return;

  if ((arrows = Carries( p, O_ARROWS)) != NULL)
  {
    arrows->modifier--;
    if (!arrows->modifier)
    {
      RemoveItemFromList( arrows, &(p->carrying));
      free( arrows);
    }
  }
  else if (!Carries( p, O_FAITHFULARROW)) return;

  Bow();
  p->moves--;

  if (FindCharacter( p->x + dx, p->y + dy))
  {
    success = SkillRoll( p, i);
    if (success)
    {
      damage = GetDamage( p, i);
      Violence( p, dx, dy, damage, success == PERFECT);
    }
  }
}

void Throw( Person *p, struct item *i, int dx, int dy )
{
  int damage, success;

  if (!p->moves) return;

  if (!ValidTarget( p, dx, dy)) return;

  p->moves--;
  Swisch();
  Pause( 15);

  UnWieldItem( p, i);
  RemoveItemFromList( i, &(p->carrying));
  if (gWorld[ p->x + dx][ p->y + dy] & SHADOW_FLAG)
  {
    PlaceItem( i, p->x + dx - SGN( dx), p->y + dy - SGN( dy));
    PicRestore( p->x + dx - SGN( dx), p->y + dy - SGN( dy));
  }
  else
  {
    PlaceItem( i, p->x + dx, p->y + dy);
    PicRestore( p->x +dx, p->y + dy);
  }

  if (FindCharacter( p->x + dx, p->y + dy))
  {
    success = SkillRoll( p, i);
    if (success)
    {
      damage = GetDamage( p, i);
      damage += p->skill >> 1;
      Violence( p, dx, dy, damage, success == PERFECT);
    }
  }
}

void Push( Person *pc, Person *target )
{
  int x, y;

  if (!pc->moves) return;
  
  Swisch();
  pc->moves--;

  x = (target->x << 1) - pc->x;
  y = (target->y << 1) - pc->y;
  if (pc->strength > 2 &&
      (pc->strength/2) + Rand( pc->strength/2) > target->strength &&
  	  Treadable( x, y, target->canWalk))
  {
    RemoveCharacter( target);
    PicRestore( target->x, target->y);
    PlaceCharacter( target, x, y);
    PicRestore( x, y);
  }
}

void Lightning( Person *p, struct item *spell, int x, int y )
{
  int damage;

  PicDraw( x, y, 52);
  Zzap();
  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    damage = 24 + spell->modifier;
    Damage( x, y, damage, p);
  }
  Pause( 15);
  PicRestore( x, y);
}

void FireBall( Person *p, struct item *spell, int x, int y )
{
  int damage;

  PicDraw( x, y, 49);
  Swoop();
  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    damage = 17 + spell->modifier;
    Damage( x, y, damage, p);
  }
  Pause( 15);
  PicRestore( x, y);
}

void Teleport( Person *p, int x, int y )
{
  Person *t;
  short nx, ny, mk;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    t = MagicallyFindCharacter( x, y);
    if (t)
    {
      Boing();
      Message( "Pick a destination...");
      while (MOUSEK);
      showmouse( FORCE);
      do
      {
        Mouse( &nx, &ny, &mk);
        nx = MX_TO_BX( nx);
        ny = MY_TO_BY( ny);
      }
      while (mk == 0 || !VALIDBXY( nx, ny));
      nx += p->x;
      ny += p->y;
      hidemouse();
      if (Treadable( nx, ny, t->canWalk))
      {
        PicDraw( x, y, 95);
        PicDraw( nx, ny, 95);
        Boomm();
        RemoveCharacter( t);
        PlaceCharacter( t, nx, ny);

        Pause( 15);
        if (t == p)
        {
          PicRestore( nx, ny);
          PicRestore( x, y);
          Screen( p);
          AlertBadGuys( p);
        }
        else
        {
          PicRestore( x, y);
          PicRestore( nx, ny);
        }
      }
      else
        Message( "Unable to teleport there.");
      showmouse( NOFORCE);
    }
    else
    {
      PicDraw( x, y, 176);
      Boing();
      Pause( 15);
      PicRestore( x, y);
    }
  }
  else
    Ding();
}

void Create( Person *p, int dx, int dy, int class )
{
  Person *creature;

  dx = SGN( dx);
  dy = SGN( dy);
  if (Treadable( p->x + dx, p->y + dy, gClassData[ class].canWalk))
  {
    PicDraw( p->x + dx, p->y + dy, 51);
    Summon();
    creature = CreateCharacter( "Servant", class, TRUE, p->isPlayer);
    creature->isTracker = TRUE;
    creature->isNasty = p->isNasty;
    PlaceCharacter( creature, p->x + dx, p->y + dy);
    Pause( 15);
    PicRestore( p->x + dx, p->y + dy);
  }
  else
    Ding();
}

void Vision( Person *p, int x, int y )
{
  hidemouse();
  SetBuffer( x, y);
  BufferDraw();
  while (MOUSEK);
  while (!MOUSEK);
  Screen( p);
  BufferDump();
  while (MOUSEK);
  showmouse( NOFORCE);
}

void Sleep( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 106);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->moves = min( 1, t->moves);
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Confuse( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 154);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->confused = TRUE;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void FastFeet( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 61);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->moves += 3;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Cure( Person *p, int x, int y, int perfect )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 61);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
    {
      if (perfect)
        t->health = t->maxHealth;
      else
        t->health = min( t->maxHealth, t->health + 25);
      t->confused = FALSE;
    }
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void FlingItems( int x, int y, struct item **list )
{
  struct item *i;
  i = *list;
  while (i)
  {
    if (!i->locked)
    {
      RemoveItemFromList( i, list);
      PlaceItem( i, x, y);
      i = *list;
    }
    else
      i = i->next;
  }
}

void Chaos( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    if ((t = MagicallyFindCharacter( x, y)) != NULL)
    {
      PicDraw( x, y, 154);
      Boomm();
      FlingItems( x, y, &(t->carrying));
    }
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Phantom( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 61);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->canWalk = 0x7FFF;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Glue( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 106);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->canWalk = 0;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

#define SCARED 17

void Terror( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 106);
    Bong();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->isFleeing = SCARED;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Berzerk( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 61);
    Boing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->damageBonus += 8;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Ward( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 61);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
      t->ward += 2;
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Storm( Person *p, struct item *spell, int x, int y )
{
  int count, tx, ty;

  for (count = 1; count <= 20; count++)
  {
    tx = x + Rand( 3) - 1;
    ty = y + Rand( 3) - 1;
    if (tx != p->x || ty != p->y)
      if (InBox( tx, ty, 0, 0, WORLD_X_MAX, WORLD_Y_MAX))
        Lightning( p, spell, tx, ty);
  }
}

void HyperSpace( Person *p )
{
  int x, y;

  do
  {
    x = Rand( WORLD_X_MAX);
    y = Rand( WORLD_Y_MAX);
  }
  while (!Treadable( x, y, p->canWalk));
          
  PicDraw( p->x, p->y, 95);
  PicDraw( x, y, 95);
  Boomm();
  RemoveCharacter( p);
  PlaceCharacter( p, x, y);
  Pause( 15);
  Screen( p);
  PicRestore( x, y);
}

void Panic( Person *p )
{
  Person *t;
  
  t = gCharacters;
  while (t)
  {
    if (!t->isPlayer &&
        max( abs( t->x - p->x), abs( t->y - p->y)) < BOARDSIZE/2 + 1)
      Terror( p, t->x, t->y);
    t = t->next;
  }
}

void Inferno( Person *p, int x, int y )
{
  int tx, ty;

  Swoop();
  tx = p->x;
  ty = p->y;
  while (tx != x || ty != y)
  {
    if (abs( x - tx) >= abs( y - ty))
    {
      if (abs( y - ty) >= abs( x - tx))
        ty += SGN( y - ty);
      tx += SGN( x - tx);
    }
    else if (abs( y - ty) >= abs( x - tx))
      ty += SGN( y - ty);
    PicDraw( tx, ty, 49);
    Damage( tx, ty, 24, p);
    Pause( 15);
  }
  tx = p->x;
  ty = p->y;
  while (tx != x || ty != y)
  {
    if (abs( x - tx) >= abs( y - ty))
    {
      if (abs( y - ty) >= abs( x - tx))
        ty += SGN( y - ty);
      tx += SGN( x - tx);
    }
    else if (abs( y - ty) >= abs( x - tx))
      ty += SGN( y - ty);
    PicRestore( tx, ty);
    Pause( 15);
  }
  Swoop();
  for (tx = x - 1; tx <= x + 1; tx++)
    for (ty = y - 1; ty <= y + 1; ty++)
    {
      PicDraw( tx, ty, 49);
      Damage( tx, ty, 24, p);
    }
  Pause( 15);
  for (tx = x - 1; tx <= x + 1; tx++)
    for (ty = y - 1; ty <= y + 1; ty++)
      PicRestore( tx, ty);
}

void Slow( Person *p, int x, int y )
{
  Person *t;

  if (gWorld[ x][ y] & CHARACTER_FLAG)
  {
    PicDraw( x, y, 106);
    Ploing();
    t = MagicallyFindCharacter( x, y);
    if (t)
    {
      if (t->moves > 1) t->moves >>= 1;
    }
    else
    {
      PicDraw( x, y, 176);
      Boing();
    }
    Pause( 15);
    PicRestore( x, y);
  }
}

void Purify( Person *pc )
{
  Person *p;

  p = gCharacters;
  while (p)
  {
    if (p != pc && p->lifeSpan != 0 && p->health > 0)
    {
      PicDraw( p->x, p->y, 51);
      Boomm();
      Pause( 15);
      Death( p, pc);
      PicRestore( p->x, p->y);
    }
    p = p->next;
  }
}

void CastSpell( Person *p, struct item *spell, int dx, int dy )
{
  int success, drain, x, y;
  
  if (!ValidTarget( p, dx, dy)) return;
  
  if (!p->moves) return;
  p->moves--;
        
  if (p->power < max( 1, gObjectData[ spell->id].value + spell->modifier))
  {
    if (p->isPlayer)
    {
      sprintf( uString, "%s hasn't got the mana for %s",
      					p->name, gObjectData[ spell->id].name);
      Message( uString);
    }
    return;
  }
  
  success = WisdomRoll( p, spell);
  if (success)
  {
    drain = gObjectData[ spell->id].value;
    drain -= spell->modifier;
    if (success == PERFECT)
      drain = min( 1, drain);
    drain = max( 1, drain);
    
    p->power -= drain;  
    
    x = p->x + dx;
    y = p->y + dy;
    switch (spell->id)
    {
      case O_CONFUSIONSPELL:
        Confuse( p, x, y);
        break;
        
      case O_LIGHTNINGSPELL:
        Lightning( p, spell, x, y);
        break;
        
      case O_FIREBALLSPELL:
        FireBall( p, spell, x, y);
        break;
        
      case O_TELEPORTSPELL:
        Teleport( p, x, y);
        break;
          
      case O_AIRSPELL:
        Create( p, dx, dy, 18);
        break;
        
      case O_FIRESPELL:
        Create( p, dx, dy, 19);
        break;
        
      case O_WATERSPELL:
        Create( p, dx, dy, 20);
        break;

      case O_EARTHSPELL:
        Create( p, dx, dy, 21);
        break;
        
      case O_SKELETONSPELL:
        Create( p, dx, dy, 16);
        break;
       
      case O_ZOMBIESPELL:     
        Create( p, dx, dy, 17);
        break;
          
      case O_VISIONSPELL:
        Vision( p, x, y);
        break;
        
      case O_PHANTOMSPELL:
        Phantom( p, x, y);
        break;
        
      case O_SLEEPSPELL:
        Sleep( p, x, y);
        break;
        
      case O_SPEEDSPELL:
        FastFeet( p, x, y);
        break;

      case O_HEALINGSPELL:
        Cure( p, x, y, success == PERFECT);
        break;
        
      case O_DARKNESSSPELL:
        Create( p, dx, dy, 28);
        break;
        
      case O_CHAOSSPELL:
        Chaos( p, x, y);
        break;
        
      case O_DEMONSPELL:
        Create( p, dx, dy, 22);
        break;
          
      case O_FREEZESPELL:
        Glue( p, x, y);
        break;
        
      case O_ENERGYSPELL:
        Create( p, dx, dy, 29);
        break;
        
      case O_TERRORSPELL:
        Terror( p, x, y);
        break;
        
      case O_BERZERKSPELL:
        Berzerk( p, x, y);
        break;
        
      case O_WARDSPELL:
        Ward( p, x, y);
        break;
        
      case O_STORMSPELL:
        Storm( p, spell, x, y);
        break;
        
      case O_HYPERSPACESPELL:
        HyperSpace( p);
        break;
        
      case O_PANICSPELL:
        Panic( p);
        break;
        
      case O_INFERNOSPELL:
        Inferno( p, x, y);
        break;
        
      case O_SLOWSPELL:
        Slow( p, x, y);
        break;
      
      case O_PURIFYSPELL:
        Purify( p);
        break;
    }
  }
  else
  {
    if (p->isPlayer)
    {
      Message( "The spell failed.");
      Ding();
    }
  }
}

void FireBlade( Person *p, struct item *i, int dx, int dy )
{
  dx = SGN( dx);
  dy = SGN( dy);
  Swing();
  if (SkillRoll( p, i))
    FireBall( p, i, p->x + dx, p->y + dy);
}

void Chock( Person *p, struct item *i, int dx, int dy )
{
  dx = SGN( dx);
  dy = SGN( dy);
  Swing();
  if (WisdomRoll( p, i))
    Lightning( p, i, p->x + dx, p->y + dy);
}

void SunBow( Person *p, struct item *i, int dx, int dy )
{
  int damage, success;

  if (abs( dx) > 1 || abs( dy) > 1)
  {
    Bow();
    if (FindCharacter( p->x + dx, p->y + dy))
    {
      success = SkillRoll( p, i);
      if (success)
      {
        damage = GetDamage( p, i);
        Violence( p, dx, dy, damage, success == PERFECT);
      }
    }
  }
  else
    p->moves++;
}

void GhostBlade( Person *p, struct item *i, int dx, int dy )
{
  int damage, success;

  Swing();
  if (gWorld[ p->x + dx][ p->y + dy] & CHARACTER_FLAG)
  {
    success = SkillRoll( p, i);
    if (success)
    {
      damage = GetDamage( p, i);
      if (max( abs( dx), abs( dy)) <= 1)
        damage += p->skill >> 2;
      Violence( p, dx, dy, damage, success == PERFECT);
    }
  }
}

void Wishing( Person *p )
{
  short mx, my, mk;
  long key;
  char id[ 20];
  struct item *i, *wished;
  int index;
  ButtonPtr b;
  int done;

  hidemouse();

  DrawStoneBox( 200, 100, 225, 120);
  b = NULL;
  AddButton( &b, 225, 175, 275, 200, 13, 0, "OK");
  AddButton( &b, 300, 175, 350, 200, 27, 1, "Cancel");
  DrawButtons( b);
  DrawOneButton( b, 0, FALSE);

  SetBgColor( GRAY);
  SetColor( BLACK);
  OutTextXY( 250, 125, "Wish for...?");
  *id = 0;
  TextEditFrame( 225, 150, 20);
  TextEdit( 225, 150, 20, 0, id);
  showmouse( FORCE);

  done = FALSE;
  while (!done)
  {
    key = 0;
    do
    {
      if (KEYPRESSED) key = KEYREAD;
      Mouse( &mx, &my, &mk);
    }
    while (key == 0 && mk == 0);

    if (!mk)
      mx = my = 0;

    if (CheckButtons( b, mx, my, key, &index))
    {
      FrameButton( b, index, TRUE);
      while (MOUSEK);

      if (index == 0)
      {
        Click();
        if (!*id)
          Ding();
        else
          done = TRUE;
      }
      else if (index == 1)
      {
        Click();
        done = TRUE;
      }
      FrameButton( b, index, FALSE);
    }
    else
    {
      TextEdit( 225, 150, 20, key, id);
      if (!*id || !*(id+1))
      DrawOneButton( b, 0, *id);
    }
  }
  DisposeButtons( &b);

  if (index == 0)
  {
    wished = NULL;
    for (index = 1; index < MAXOBJECTS; index++)
      if (!stricmp( gObjectData[ index].name, id))
      {
        wished = malloc( sizeof( struct item));
        memset( wished, 0, sizeof( struct item));
        wished->id = index;
        break;
      }

    if (wished)
    {
      Summon();
      i = p->wielding;
      RemoveItemFromList( i, &(p->wielding));

      if (wished->id == O_ARROWS)
        wished->modifier = 150;
      else if (wished->id == O_ENCHANTSPELL)
        wished->modifier = 8;
      else
        wished->modifier = i->modifier;
      AddItemToList( wished, &(p->carrying));
      Inventory( p);
      free( i);
    }
    else
    {
      Ding();
      Message( "Unknown item");
    }
  }
  SetColor( BLACK);
  FillRect( 200, 100, 425, 220);
  BufferDraw();
}

void Enchant( Person *p )
{
  struct item *i;
  int state;

  showmouse( NOFORCE);
  i = GetFromInventory( p, &state);
  hidemouse();
  if (!i) return;

  Ploing();
  switch (state)
  {
    case WIELDING:
      UnWieldItem( p, i);
      i->modifier++;
      WieldItem( p, i);
      break;

    case WEARING:
      UnWearItem( p, i);
      i->modifier++;
      WearItem( p, i);
      break;

    case INPACK:
      i->modifier++;
      break;
  }
  i = p->wielding;
  i->modifier--;
  if (i->modifier <= 0)
  {
    UnWieldItem( p, i);
    RemoveItemFromList( i, &(p->carrying));
  }
}

void Specials( Person *p, struct item *i, int dx, int dy )
{
  if (!ValidTarget( p, dx, dy)) return;

  if (!p->moves) return;
  p->moves--;

  switch (i->id)
  {
    case O_SUNBOW:
      SunBow( p, i, dx, dy);
      break;

    case O_GHOSTBLADE:
      GhostBlade( p, i, dx, dy);
      break;

    case O_FIREBLADE:
      FireBlade( p, i, dx, dy);
      break;

    case O_CHOCKHAMMER:
      Chock( p, i, dx, dy);
      break;

    case O_WISHINGSPELL:
      Wishing( p);
      break;

    case O_ENCHANTSPELL:
      Enchant( p);
      break;
  }
}

void Potions( Person *p )
{
  struct item *i;

  if (!p->moves) return;

  p->moves--;
  i = p->wielding;
  RemoveItemFromList( i, &(p->wielding));
  Ploing();
  PicDraw( p->x, p->y, 61);

  switch (i->id)
  {
    case O_BLUEPOTION:
      p->maxPower += 25;
      break;

    case O_GREENPOTION:
      p->maxHealth += 25;
      break;

    case O_CLEARPOTION:
      if (p->speed < 7)
	p->speed++;
      break;

    case O_YELLOWPOTION:
      p->skill++;
      break;

    case O_REDPOTION:
      p->strength++;
      break;

    case O_GRAYPOTION:
      Experience( p, Rand( 200));
      break;

    case O_WHITEPOTION:
      p->wisdom++;
      if (p->skill > 0)
	p->skill--;
      break;

    case O_PURPLEPOTION:
      p->wisdom++;
      break;

    case O_DARKPOTION:
      p->strength++;
      p->skill++;
      if (p->wisdom > 0)
	p->wisdom--;
      break;
  }
  Pause( 15);
  PicRestore( p->x, p->y);
  free( i);
}

void Action( Person *p, int dx, int dy )
{
  struct item *i;

  hidemouse();

  if (p->confused)
  {
    dx = Rand( BOARDSIZE) - BOARDCENTER;
    dy = Rand( BOARDSIZE) - BOARDCENTER;
  }

  i = p->wielding;
  if (i)
  {
    switch (gObjectData[ i->id].kind)
    {
      case O_CLASS_WEAPON:
	Strike( p, i, dx, dy);
	break;

      case O_CLASS_THROWINGWEAPON:
	Throw( p, i, dx, dy);
	break;

      case O_CLASS_RANGEDWEAPON:
	Shoot( p, i, dx, dy);
	break;

      case O_CLASS_SPELL:
	CastSpell( p, i, dx, dy);
	break;

      case O_CLASS_MIXEDWEAPON:
	if (abs( dx) < 2 && abs( dy) < 2)
	  Strike( p, i, dx, dy);
	else
	  Throw( p, i, dx, dy);
	break;

      case O_CLASS_SPECIAL:
	Specials( p, i, dx, dy);
	break;

      case O_CLASS_POTION:
	if (dx == 0 && dy == 0)
	  Potions( p);
	break;
    }
  }
  else
    Strike( p, NULL, dx, dy);
  showmouse( NOFORCE);
}

/*
---------------------------------------
	Diverse character-manipulering
---------------------------------------
*/


/*
---------------------------------------
	Datorns drag
---------------------------------------
*/

void WearArmor( Person *pc )
{
  struct item *i;

  i = pc->carrying;
  while (i)
  {
    if (gObjectData[ i->id].kind == O_CLASS_ARMOR 
    	|| 
    	gObjectData[ i->id].kind == O_CLASS_GADGET)
      Use( pc, i);
    i = i->next;
  }
}

int ChooseSpell( Person *pc, Person *target, struct item *spell )
{
  int x, y, value;
  struct item *negator;

  x = pc->x + SGN( target->x - pc->x);
  y = pc->y + SGN( target->y - pc->y);
  
  negator = Carries( target, O_NEGATOR);
  
  value = -1;
  if (pc->power >= gObjectData[ spell->id].value)
  {
    switch (spell->id)
    {
      case O_CONFUSIONSPELL:
        if (target->confused == 0 && !negator)
          value = Rand( 20);
        break;

      case O_LIGHTNINGSPELL:
        value = Rand( 50);
        break;
        
      case O_FIREBALLSPELL:
        value = Rand( 40);
        break;

      case O_SLEEPSPELL:
        if (target->moves > 1 && !negator)
          value = Rand( 30);
        break;
     
      case O_FREEZESPELL:
        if (target->canWalk != 0 && !negator)
          value = Rand( 20);
        break;
        
      case O_STORMSPELL:
        value = Rand( 30);
        break;
        
      case O_INFERNOSPELL:
        value = Rand( 30);
        break;
        
      case O_SLOWSPELL:
        if (target->moves > 1)
          value = Rand( 20);

      case O_FIRESPELL:
        if (Treadable( x, y, gClassData[ 19].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_WATERSPELL:
        if (Treadable( x, y, gClassData[ 20].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_EARTHSPELL:
        if (Treadable( x, y, gClassData[ 21].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_SKELETONSPELL:
        if (Treadable( x, y, gClassData[ 16].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_ZOMBIESPELL:
        if (Treadable( x, y, gClassData[ 17].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_DEMONSPELL:
        if (Treadable( x, y, gClassData[ 22].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_ENERGYSPELL:
        if (Treadable( x, y, gClassData[ 29].canWalk))
          value = 20 + Rand( 10);
        break;
        
      case O_DARKNESSSPELL:
        if (Treadable( x, y, gClassData[ 28].canWalk))
          value = 20 + Rand( 10);
        break;        
       
      case O_CHAOSSPELL:
        if (target->carrying && !negator)
          value = Rand( 40);
        break;
    }
  }
  return value;
}

int ChooseSpecial( struct item *item, int distance )
{
  int value = -1;
  
  switch (item->id)
  {
    case O_FIREBLADE:
      if (!distance)
        value = 17;
      break;
  
    case O_CHOCKHAMMER:
      if (!distance)
        value = 24;
      break;
      
    case O_GHOSTBLADE:
      value = 20;
      break;
  }
  return value;
}

void TestBest( struct item *item, int v,
               struct item **best, int *high )
{
  if (v > *high || (v == *high && Rand(2) == 0))
  {
    *best = item;
    *high = v;
  }
}

void WieldWeapon( Person *pc, Person *target, int distance )
{
  struct item *item, *best;
  int value, highest;
  
  best = NULL;
  highest = 0;
  
  while (pc->wielding)
    UnWieldItem( pc, pc->wielding);
  
  item = pc->carrying;
  while (item)
  {
    switch (gObjectData[ item->id].kind)
    {
      case O_CLASS_WEAPON:
        value = GetDamage( pc, item);
        value += 1 + pc->strength / 4;
        if (!distance)
          TestBest( item, value, &best, &highest);
        break;

      case O_CLASS_THROWINGWEAPON:
        value = GetDamage( pc, item);
        value += 1 + pc->skill / 4;
        if (distance)
          TestBest( item, value, &best, &highest);
        break;
        
      case O_CLASS_RANGEDWEAPON:
        if (distance && 
        	(Carries( pc, O_ARROWS) || Carries( pc, O_FAITHFULARROW)))
        {
          value = GetDamage( pc, item);
          value += 1 + pc->skill / 4;
          TestBest( item, value, &best, &highest);
        }
        break;
        
      case O_CLASS_SPELL:
        value = ChooseSpell( pc, target, item);
        value += Rand( 10) - 20;
        TestBest( item, value, &best, &highest);
        break;
        
      case O_CLASS_MIXEDWEAPON:
        value = GetDamage( pc, item);
        if (distance)
          value += 1 + pc->skill / 4;
        else
          value += 1 + pc->strength / 4;
        TestBest( item, value, &best, &highest);
        break;
        
      case O_CLASS_SPECIAL:
        value = ChooseSpecial( item, distance);
        TestBest( item, value, &best, &highest);
        break;
    }
    item = item->next;
  }
  if (best)
    Use( pc, best);
}

int Range( Person *p1, Person *p2 )
{
  if (p1 && p2)
    return max( abs( p1->x - p2->x), abs( p1->y - p2->y));
  else
    return WORLD_Y_MAX;
}

Person *GetClosestPlayer( Person *pc, int bySmell )
{
  Person *closest, *player;
  int r, range;
  
  range = WORLD_X_MAX;
  closest = NULL;
  
  player = gCharacters;
  while (player)
  {
    if (player->isPlayer && player->health > 0)
    {
      if (bySmell || 
      	  ValidTarget( player, pc->x - player->x, pc->y - player->y))
      {
        r = Range( player, pc);
        if (r < range || (r == range && Rand( 2) == 0))
        {
          range = r;
          closest = player;
        }
      }
    }
    player = player->next;
  }
  return closest;
}

#define cMaxStep   12
#define cAreaSize  20

struct AreaPos
{
  int step;
  int path;
};

struct AreaPos area[cAreaSize][cAreaSize];


static void InitArea( Person *pc, int cx, int cy )
{
  int x, y;

  memset( area, 0, sizeof( area));
  for (x = 0; x < cAreaSize; x++)
    for (y = 0; y < cAreaSize; y++)
      if (Treadable( cx + x, cy + y, pc->canWalk))
      {
        area[x][y].step = 1;
        area[x][y].path = 10000;
      }
}

static void WalkPath( int x, int y, int step, int cost )
{
  struct AreaPos *p;

  if (step &&
      x >= 0 && x < cAreaSize &&
      y >= 0 && y < cAreaSize)
  {
    p = &area[ x][ y];
    if (step == cMaxStep || (p->step && p->path > p->step + cost))
    {
      cost += p->step;
      p->path = cost;
      step--;
      WalkPath( x+1, y,   step, cost);
      WalkPath( x+1, y+1, step, cost);
      WalkPath( x,   y+1, step, cost);
      WalkPath( x-1, y+1, step, cost);
      WalkPath( x-1, y,   step, cost);
      WalkPath( x-1, y-1, step, cost);
      WalkPath( x,   y-1, step, cost);
      WalkPath( x+1, y-1, step, cost);
    }
  }
}

static int FindDirection( Person *pc, int *dx, int *dy )
{
  if (Treadable( pc->x + *dx, pc->y + *dy, pc->canWalk))
    return TRUE;
  else if (dx == 0)
  {
    if (Treadable( pc->x -1, pc->y + *dy, pc->canWalk))
      *dx = -1;
    else if (Treadable( pc->x + 1, pc->y + *dy, pc->canWalk))
      *dx = 1;
    else if (Treadable( pc->x - 1, pc->y, pc->canWalk))
    { *dx = -1; *dy = 0; }
    else if (Treadable( pc->x + 1, pc->y, pc->canWalk))
    { *dx = 1; *dx = 0; }
    else
      return FALSE;
  }
  else if (*dy == 0)
  {
    if (Treadable( pc->x + *dx, pc->y - 1, pc->canWalk))
      *dy = -1;
    else if (Treadable( pc->x + *dx, pc->y + 1, pc->canWalk))
      *dy = 1;
    else if (Treadable( pc->x, pc->y - 1, pc->canWalk))
    { *dx = 0; *dy = -1; }
    else if (Treadable( pc->x, pc->y + 1, pc->canWalk))
    { *dx = 0; *dy = 1; }
    else
      return FALSE;
  }
  else
  {
    if (Treadable( pc->x, pc->y + *dy, pc->canWalk))
      *dx = 0;
    else if (Treadable( pc->x + *dx, pc->y, pc->canWalk))
      *dy = 0;
    else if (Treadable( pc->x - *dx, pc->y + *dy, pc->canWalk))
      *dx = -*dx;
    else if (Treadable( pc->x + *dx, pc->y + -*dy, pc->canWalk))
      *dy = -*dy;
    else
      return FALSE;
  }
  return TRUE;
}

static int FindPath( Person *pc, int tx, int ty, int *dx, int *dy )
{
  int x, y, min = 0, cost;

  if (max( abs( tx - pc->x), abs( ty - pc->y)) > cMaxStep) return FALSE;

  x = (pc->x + tx)/2;
  y = (pc->y + ty)/2;
  x -= cAreaSize/2;
  y -= cAreaSize/2;
  InitArea( pc, x, y);
  WalkPath( tx - x, ty - y, cMaxStep, 0);

  x = pc->x - x;
  y = pc->y - y;
  cost = area[ x+1][ y].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = 1; *dy = 0;
  }
  cost = area[ x+1][ y+1].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = 1; *dy = 1;
  }
  cost = area[ x][ y+1].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = 0; *dy = 1;
  }
  cost = area[ x-1][ y+1].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = -1; *dy = 1;
  }
  cost = area[ x-1][ y].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = -1; *dy = 0;
  }
  cost = area[ x-1][ y-1].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = -1; *dy = -1;
  }
  cost = area[ x][ y-1].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = 0; *dy = -1;
  }
  cost = area[ x+1][ y-1].path;
  if (cost && (!min || cost < min))
  {
    min = cost;
    *dx = 1; *dy = -1;
  }
  return (min != 0);
}

static int GoTowards( Person *pc, int x, int y )
{
  int dx, dy;

  dx = SGN( x - pc->x);
  dy = SGN( y - pc->y);
  if (FindPath( pc, x, y, &dx, &dy) ||
      FindDirection( pc, &dx, &dy))
  {
    Move( pc, dx, dy);
    return TRUE;
  }
  return FALSE;
}

static int RunFrom( Person *pc, int x, int y )
{
  int dx, dy;

  dx = SGN( pc->x - x);
  dy = SGN( pc->y - y);
  if (FindDirection( pc, &dx, &dy))
  {
    Move( pc, dx, dy);
    return TRUE;
  }
  return FALSE;
}

/*
int ThinkNWalk( Person *pc, int dx, int dy )
{
  if (Treadable( pc->x + dx, pc->y + dy, pc->canWalk))
    Move( pc, dx, dy);
  else if (dx == 0)
  {
    if (Treadable( pc->x - 1, pc->y + dy, pc->canWalk))
      Move( pc, -1, dy);
    else if (Treadable( pc->x + 1, pc->y + dy, pc->canWalk))
      Move( pc, 1, dy);
    else if (Treadable( pc->x - 1, pc->y, pc->canWalk))
      Move( pc, -1, 0);
    else if (Treadable( pc->x + 1, pc->y, pc->canWalk))
      Move( pc, 1, 0);
    else
      return FALSE;
  }
  else if (dy == 0)
  {
    if (Treadable( pc->x + dx, pc->y - 1, pc->canWalk))
      Move( pc, dx, -1);
    else if (Treadable( pc->x + dx, pc->y + 1, pc->canWalk))
      Move( pc, dx, 1);
    else if (Treadable( pc->x, pc->y - 1, pc->canWalk))
      Move( pc, 0, -1);
    else if (Treadable( pc->x, pc->y + 1, pc->canWalk))
      Move( pc, 0, 1);
    else
      return FALSE;
  }
  else
  {
    if (Treadable( pc->x, pc->y + dy, pc->canWalk))
      Move( pc, 0, dy);
    else if (Treadable( pc->x + dx, pc->y, pc->canWalk))
      Move( pc, dx, 0);
    else if (Treadable( pc->x - dx, pc->y + dy, pc->canWalk))
      Move( pc, -dx, dy);
    else if (Treadable( pc->x + dx, pc->y - dy, pc->canWalk))
      Move( pc, dx, -dy);
    else
      return FALSE;
  }
  return TRUE;
}
*/

void UpdateScreen( Person *pc, int moved )
{
  if (moved)
  {
    SetBuffer( pc->x, pc->y);
    Shade();
    if (gMustDraw)
    {
      BufferDraw();
      gMustDraw = FALSE;
    }
    else
      BufferDump();
  }
  Status( pc);
  Bars( pc);
  OverviewPanel( FALSE);
  if (gFollowDarkMoves)
    Inventory( pc);
  delay( 100);
}

Person *PickAPlayer( void )
{
  Person *p;
  int count;
  
  count = Rand( 16) + 1;
  p = gCharacters;
  while (p && (count > 0 || !p->isPlayer))
  {
    if (p->isPlayer) count--;
    p = p->next;
    if (!p)
    {
      p = gCharacters;
      count--;
    }
  }
  return p;
}

int PickABadGuy( int x, int y )
{
  int distance, count, class;

  distance = max( abs( x - gGateX), abs( y - gGateY));
  for (count = 0; count < 30; count++)
  {
    class = Rand( MAXCLASSES - SELECTABLECLASSES) + SELECTABLECLASSES;
    if (gClassData[ class].rank <= distance &&
    	gClassData[ class].rank + 50 >= distance)
      break;
  }
  if (gClassData[ class].rank > distance)
    class = 33; /* V„tte */
    
  return class;
}

void PickPosition( int x, int y, int *bx, int *by )
{
  *bx = Rand( 3) - 1;
  if (*bx == 0)
  {
    *by = (Rand( 2)*2 - 1)*12;
    *bx = Rand( 25) - 12;
  }
  else
  {
    *bx *= 12;
    *by = Rand( 25) - 12;
  }
  
  *bx += x;
  *by += y;
  
  if (*bx < 0)
    *bx = x + 12;
  else if (*bx >= WORLD_X_MAX)
    *bx = x - 12;
  if (*by < 0)
    *by = y + 12;
  else if (*by >= WORLD_Y_MAX)
    *by = y - 12;
}

int CheckPosition( int x, int y, int class )
{
  Person *p;
  
  if (!Treadable( x, y, gClassData[ class].canWalk))
    return FALSE;
  else
  {
    p = gCharacters;
    while (p)
    {
      if (p->isPlayer && max( abs( p->x - x), abs( p->y - y)) <= 9)
        return FALSE;
      p = p->next;
    }
    return TRUE;
  }
}

#define EVIL		TRUE
#define FRIENDLY	FALSE

Person *MakeNPC( int nasty )
{
  Person *p;
  int count, x, y, class;
  
  p = PickAPlayer();
  if (p)
  {
    if (nasty)
      class = PickABadGuy( p->x, p->y);
    else
      class = 42;
    for (count = 0; count < 10; count++)
    {
      PickPosition( p->x, p->y, &x, &y);
      if (CheckPosition( x, y, class))
        break;
    }
    if (count < 10)
    {
      if (nasty)
        p = CreateCharacter( "Nasty", class, FALSE, FALSE);
      else
        p = CreateCharacter( "Nicey", class, FALSE, FALSE);
      p->isNasty = nasty;
      PlaceCharacter( p, x, y);
      return p;
    }
  }
  return NULL;
}

void CreateHordes( void )
{
  int count;
  Person *p;

  for (count = 0; count < 3; count++)
  {
    p = CreateCharacter( "Tetris", 36, FALSE, FALSE);
    p->isNasty = TRUE;
    PlaceCharacterCloseTo( p, 100, 312, 2);
  }

  gDarkMaster = CreateCharacter( "Magus", 41, FALSE, FALSE);
  gDarkMaster->isNasty = TRUE;
  PlaceCharacter( gDarkMaster, 100, 316);
}

void MaintainBalance( void )
{
  int count, players, enemies;
  Person *p;
  static int tracker = 0;
  
  p = gCharacters;
  players = enemies = 0;
  while (p)
  {
    if (p->isPlayer)
      players++;
    else
      enemies++;
    p = p->next;
  }
  
  for (count = enemies - 4; count <= players*2; count++)
  {
    p = MakeNPC( EVIL);
    if (p)
    {
      p->isTracker = tracker;
      tracker = !tracker;
    }
  }
  
  if (Rand( 20) == 1)
  {
    p = MakeNPC( FRIENDLY);
    if (p)
      p->isTracker = TRUE;
  }
}

#define CLOSE_ENOUGH 15

void AtariActions( Person *pc )
{
  int count, range;
  Person *target;
  int moved, acted, drawn;
  int dx, dy;
  
  if (pc->isFleeing == SCARED) 
    pc->isFleeing = TRUE;
  else
    pc->isFleeing = IsWounded( pc);

  if (!pc->wearing)
    WearArmor( pc);
  
  target = GetClosestPlayer( pc, pc->isTracker);
  if (target == NULL && pc->opponent != NULL)
    target = pc->opponent;
  range = Range( pc, target);

  if (range > CLOSE_ENOUGH) target = NULL;
  
  if (target)
  {
    if (pc->isNasty)
      pc->opponent = target;
    else
    {
      dx = Rand( 2*pc->speed) - pc->speed;
      dy = Rand( 2*pc->speed) - pc->speed;
    }
    drawn = FALSE;
    if (gFollowDarkMoves)
      Screen( pc);

    acted = TRUE;

    for (count = 0; count < pc->speed; count++)
    {
      if (pc->moves == 0 || !acted) return;

      if (!target->isPlayer || target->health <= 0)
      {
        target = GetClosestPlayer( pc, pc->isTracker);
        if (!target)
          target = pc->opponent;
        if (!target || target->health <= 0)
          return;
        drawn = FALSE;
      }

      range = Range( pc, target);

      acted = FALSE;
      if (pc->isNasty && !pc->isFleeing)
      {
        if (ValidTarget( target, pc->x - target->x, pc->y - target->y)
            &&
            ValidTarget( pc, target->x - pc->x, target->y - pc->y))
        {
          WieldWeapon( pc, target, range > 1);
          if (gFollowDarkMoves)
            Inventory( pc);
          else if (!drawn && target->isPlayer)
          {
            if (target != gLastDrawn)
            {
              Screen( target);
              gLastDrawn = target;
            }
            else
              UpdateScreen( target, TRUE);
            drawn = TRUE;
          }

          if (target->isPlayer && (pc->wielding || range == 1))
          {
            Action( pc, target->x - pc->x, target->y - pc->y);
            acted = TRUE;
            moved = FALSE;
          }
        }
      }

      if (!acted)
      {
        if (pc->isFleeing)
          acted = moved = RunFrom( pc, target->x, target->y);
        else if (pc->isNasty)
        {
          dx = SGN( target->x - pc->x);
          dy = SGN( target->y - pc->y);
          acted = moved = GoTowards( pc, target->x, target->y);
        }
        else
          acted = moved = GoTowards( pc, dx, dy);
      }

      if (acted)
      {
        if (gFollowDarkMoves)
          UpdateScreen( pc, moved);
        else if (target->isPlayer && target->health > 0)
        {
          if (ValidTarget( target, pc->x - target->x, pc->y - target->y))
          {
            if (!drawn)
            {
              if (target != gLastDrawn)
                Screen( gLastDrawn = target);
              else
                UpdateScreen( target, TRUE);
            }
            else
              UpdateScreen( target, moved);
          }
        }
      }
    }
  }
  target = GetClosestPlayer( pc, TRUE);
  range = Range( pc, target);
  if (target && range > CLOSE_ENOUGH &&
      gClassData[ pc->class].rank != PERSISTENT)
  {
    pc->health = 0;
    RemoveCharacter( pc);
  }
}

int Darkness( void )
{
  Person *p, *p2;

  p = gCharacters;
  while (p && (!p->isPlayer || p->health <= 0))
    p = p->next;
    
  if (!p)
  {
    AllIsLost();
    return TRUE;
  }
  else if (gDarkMaster->health <= 0)
  {
    Victory();
    return TRUE;
  }
  else
  {
    hidemouse();

    /*
    DrawStoneBox( BOARDLEFT, BOARDTOP, 620-BOARDLEFT, 399);
    PutPic( 300, 125, gSprites[ 138], 0x03030303);
    */

    gLastDrawn = NULL;
    p = gCharacters;
    while (p)
    {
      if (!p->isPlayer && p->health > 0)
        AtariActions( p);
      p = p->next;
    }

    /* En andra chans - f”r eventuella elementarer & dylikt... */
    p = gCharacters;
    while (p)
    {
      if (!p->isPlayer && p->health > 0)
        AtariActions( p);
      p = p->next;
    }

    p = gCharacters;
    do
    {
      while (p && p->isPlayer)
        p = p->next;
      if (p)
      {
        if (UpdateCharacter( p))
          p = p->next;
        else
        {
          p2 = p;
          p = p->next;
          DisposeCharacter( p2);
        }
      }
    }
    while (p);

    MaintainBalance();

    /*
    SetColor( BLACK);
    FillRect( BOARDLEFT, BOARDTOP, 620, 399);
    */

    showmouse( NOFORCE);
    return FALSE;
  }
}


void InventoryClick( Person *p, int x, int y, int button )
{
  struct item *i;
  int status;
  int change;

  i = GetClickedItem( p, x, y, &status);
  if (!i) return;

  ShowItem( i);
  change = TRUE;
  switch (status)
  {
    case WIELDING:
      if (button == 2)
        StopUsing( p, i);
      else if (Kbshift( -1)&3)
      {
        UnWieldItem( p, i);
        DropObject( p, i);
      }
      else
        change = FALSE;
      break;

    case WEARING:
      if (button == 2)
        StopUsing( p, i);
      else if (p->moves && (Kbshift( -1)&3))
      {
        UnWearItem( p, i);
        DropObject( p, i);
        p->moves--;
      }
      else
        change = FALSE;
      break;

    case INPACK:
      if (button == 2)
        Use( p, i);
      else if (Kbshift( -1)&3)
        DropObject( p, i);
      else
        change = FALSE;
      break;
  }
  if (change)
  {
    Status( p);
    Inventory( p);
  }
}

void Drop( Person *pc )
{
  struct item *i;
  int state;

  i = GetFromInventory( pc, &state);

  if (!i || i->locked) return;

  if (state == WIELDING)
    UnWieldItem( pc, i);
  else if (state == WEARING)
  {
    if (pc->moves < 1) return;
    UnWearItem( pc, i);
    pc->moves--;
  }

  DropObject( pc, i);
  Status( pc);
  Inventory( pc);
}

void Give( Person *pc, Person *p )
{
  struct item *i;
  int state;

  i = GetFromInventory( pc, &state);

  if (!i || i->locked) return;

  if (state == WIELDING)
    UnWieldItem( pc, i);
  else if (state == WEARING)
  {
    if (pc->moves < 2) return;
    UnWearItem( pc, i);
    pc->moves--;
  }

  RemoveItemFromList( i, &(pc->carrying));
  AddItemToList( i, &(p->carrying));
  pc->moves--;

  if (!p->isPlayer)
  {
    if (gObjectData[ i->id].price)
    {
      pc->gifts += gObjectData[ i->id].price;
      if (pc->gifts > 150)
      {
        i = malloc( sizeof( struct item));
        if (i)
        {
          pc->gifts -= 150;
          switch (Rand( 11))
          {
            case 0:
              i->id = O_ENCHANTSPELL;
              i->modifier = 3 + Rand( 5);
              break;

            case 1:
              i->id = O_BLUEPOTION;
              break;

            case 2:
              i->id = O_GREENPOTION;
              break;

            case 3:
              i->id = O_CLEARPOTION;
              break;

            case 4:
              i->id = O_REDPOTION;
              break;

            case 5:
              i->id = O_WHITEPOTION;
              break;

            case 6:
              i->id = O_YELLOWPOTION;
              break;

            case 7:
              i->id = O_PURPLEPOTION;
              break;

            case 8:
              i->id = O_DARKPOTION;
              break;

            case 9:
              i->id = O_GRAYPOTION;
              break;

            case 10:
              i->id = O_WISHINGSPELL;
              break;
          }
          i->modifier = 0;
          i->locked = FALSE;
          i->next = NULL;
          AddItemToList( i, &(pc->carrying));
          Ploing();
        }
      }
      Message( "- Thank you!");
    }
    else
    {
      Ding();
      Message( "- Nope. Don't want it.");
      RemoveItemFromList( i, &(p->carrying));
      AddItemToList( i, &(pc->carrying));
    }
  }
  Inventory( pc);
}

void FriendlyInterAction( Person *pc, Person *p )
{
  ButtonPtr b;
  int index;
  
  DrawStoneBox( 200, 50, 200, 225);
  b = NULL;
  AddButton( &b, 225, 125, 375, 150, 0, 0, "Push");
  AddButton( &b, 225, 175, 375, 200, 0, 1, "Give...");
  AddButton( &b, 225, 225, 375, 250, 0, 2, "Cancel");
  DrawButtons( b);
  DrawOneButton( b, 1, pc->wielding || pc->wearing || pc->carrying);

  PutPic( 250, 75, gSprites[ p->icon], 0x03030303);
  SetBgColor( GRAY);
  SetColor( BLACK);
  OutTextXY( 225, 100, p->name);
  
  index = UserDialog( b);
  
  FrameButton( b, index, TRUE);
  while (MOUSEK);
  FrameButton( b, index, FALSE);
  
  DisposeButtons( &b);
  BufferDraw();
  
  if (index == 0)
    Push( pc, p);
  else if (index == 1)
    Give( pc, p);
}

void HostileInterAction( Person *pc, Person *p )
{
  ButtonPtr b;
  int index;
  
  DrawStoneBox( 200, 50, 200, 275);
  b = NULL;
  AddButton( &b, 225, 125, 375, 150, 0, 0, "Push");
  AddButton( &b, 225, 175, 375, 200, 0, 1, "\"Booh!\"");
  AddButton( &b, 225, 225, 375, 250, 0, 2, "\"!#$%&?*!!\"");
  AddButton( &b, 225, 275, 375, 300, 0, 3, "Cancel");
  DrawButtons( b);
  
  PutPic( 250, 75, gSprites[ p->icon], 0x03030303);
  SetBgColor( GRAY);
  SetColor( BLACK);
  OutTextXY( 225, 100, p->name);
  
  index = UserDialog( b);
  
  FrameButton( b, index, TRUE);
  while (MOUSEK);
  FrameButton( b, index, FALSE);
  
  DisposeButtons( &b);
  BufferDraw();
  
  switch (index)
  {
    case 0:
      Push( pc, p);
      break;
      
    case 1:
      pc->moves--;
      if (!p->isFooled && Rand( 10) < 2)
      {
        p->isFleeing = SCARED;
        sprintf( uString, "%s looks scared.", gClassData[ p->class].name);
        Message( uString);
      }
      else
        Message( "Ha ha ha!");
      p->isFooled = TRUE;
      break;

    case 2:
      pc->moves--;
      if (!p->isFooled && Rand( 10) < 2)
      {
        p->confused = TRUE;
        sprintf( uString, "%s scratches its head", gClassData[ p->class].name);
        Message( uString);
      }
      else
      {
        sprintf( uString, "%s: #$%&?*! yourself!", gClassData[ p->class].name);
        Message( uString);
      }
      p->isFooled = TRUE;
      break;
  }
}

void InterAct( Person *pc, Person *p )
{
  if (!pc->moves) return;

  hidemouse();
  while (MOUSEK);

  if (!p->isNasty)
    FriendlyInterAction( pc, p);
  else
    HostileInterAction( pc, p);
  MovesLeft( pc);
}


/*
---------------------------------------
	Disk
---------------------------------------
*/

void SaveItems( int f, struct item *i )
{
  while (i)
  {
    write( f, i, sizeof( struct item));
    i = i->next;
  }
}

void SaveGame( void )
{
  int f;
  Person *p;
  struct thing *t;

  f = open( "SAVED.DAT",
            O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
            S_IRUSR | S_IWUSR);
  if (f >= 0)
  {
    p = gCharacters;
    while (p)
    {
      write( f, p, sizeof( Person));
      SaveItems( f, p->wielding);
      SaveItems( f, p->wearing);
      SaveItems( f, p->carrying);
      p = p->next;
    }
    t = gThings;
    while (t)
    {
      write( f, &(t->x), sizeof( short));
      write( f, &(t->y), sizeof( short));
      SaveItems( f, t->items);
      t = t->next;
    }
    close( f);
  }
}

void CleanUpTheWorld( void )
{
  while (gCharacters)
  {
    RemoveCharacter( gCharacters);
    DisposeCharacter( gCharacters);
  }
  
  while (gThings)
    DeleteThing( gThings);
}


void LoadItems( int f, struct item **i )
{
  while (*i)
  {
    *i = malloc( sizeof( struct item));
    read( f, *i, sizeof( struct item));
    i = &((*i)->next);
  }
}

int RestoreGame( void )
{
  int f;
  Person *p, *nextPerson;
  struct item *i, *nextItem;
  short x, y;
  size_t bytesRead;

  f = open( "SAVED.DAT", O_RDONLY | O_BINARY);
  if (f >= 0)
  {
    CleanUpTheWorld();
    do
    {
      p = malloc( sizeof( Person));
      read( f, p, sizeof( Person));
      LoadItems( f, &(p->wielding));
      LoadItems( f, &(p->wearing));
      LoadItems( f, &(p->carrying));
      nextPerson = p->next;
      p->next = NULL;
      p->opponent = NULL;
      if (p->class == 41)
        gDarkMaster = p;
      AddCharacter( p);
      if (p->health > 0)
        PlaceCharacter( p, p->x, p->y);
    }
    while (nextPerson);
    do
    {
      bytesRead = read( f, &x, sizeof( short));
      if (bytesRead > 0)
      {
        read( f, &y, sizeof( short));
        do
        {
          i = malloc( sizeof( struct item));
          read( f, i, sizeof( struct item));
          nextItem = i->next;
          i->next = NULL;
          PlaceItem( i, x, y);
        }
        while (nextItem);
      }
    }
    while (bytesRead > 0);
    close( f);
    unlink( "SAVED.DAT");
    return TRUE;
  }
  else
    return FALSE;
}

/*
---------------------------------------
	Huvud-loopen
---------------------------------------
*/

static int PrevPlayer( Person **player )
{
  Person *p = gCharacters, *candidate = NULL;

  while (p && p != *player)
  {
    if (p->isPlayer)
      candidate = p;
    p = p->next;
  }

  if (p && !candidate)
  {
    p = p->next;
    while (p)
    {
      if (p->isPlayer)
        candidate = p;
      p = p->next;
    }
  }

  if (candidate)
  {
    *player = candidate;
    return 1;
  }
  return 0;
}


void Events( void )
{
  short mx, my, mk;
  int x, y;
  long key;
  int done, item;
  Person *pc, *p;
  ButtonPtr b;

  b = NULL;
  AddButton( &b,  58,   7, 102,  40, 32, 0, ">");
  AddButton( &b,  10,   7,  54,  40,  0, 4, "<");
  AddButton( &b, 106,   7, 150,  40, 13, 1, "End");
  AddButton( &b, 560, 445, 610, 470, 27, 2, "Quit");
  AddButton( &b, 100, 165, 150, 180,  0, 3, "Drop");
  DrawPlayArea();
  StatusPanel();
  DrawButtons( b);
  Screen( gCharacters);
  FadeIn( gPalette);
  done = 0;
  pc = gCharacters;
  while (!done)
  {
    showmouse( FORCE);
    key = 0;
    do
    {
      if (KEYPRESSED) key = KEYREAD;
      Mouse( &mx, &my, &mk);
    }
    while (key == 0 && mk == 0);

    if (!mk)
      mx = my = -1;
    if (CheckButtons( b, mx, my, key, &item))
    {
      FrameButton( b, item, TRUE);
      while (MOUSEK);
      FrameButton( b, item, FALSE);
    }
    else
      item = -1;

    if (item == 0)
    {
      pc = pc->next;
      while (pc && !pc->isPlayer)
        pc = pc->next;
      if (!pc)
        pc = gCharacters;
      while (pc && !pc->isPlayer)
        pc = pc->next;
      if (pc)
      {
        Screen( pc);
        AlertBadGuys( pc);
      }
      else
      {
        AllIsLost();
        done = TRUE;
      }
    }
    else if (item == 4)
    {
      if (PrevPlayer( &pc))
      {
        Screen( pc);
        AlertBadGuys( pc);
      }
    }
    else if (item == 1)
    {
      pc = gCharacters;
      while (pc)
      {
        if (pc->isPlayer)
        {
          if (!UpdateCharacter( pc))
          {
            p = pc;
            pc = pc->next;
            DisposeCharacter( p);
          }
          else
            pc = pc->next;
        }
        else
          pc = pc->next;
      }

      gMustDraw = TRUE;
      done = Darkness();

      if (!done)
      {
        gMustDraw = TRUE;
        pc = gCharacters;
        while (pc && !pc->isPlayer)
          pc = pc->next;
        if (pc)
        {
          Screen( pc);
          AlertBadGuys( pc);
        }
        else
        {
          AllIsLost();
          done = TRUE;
        }
      }
    }
    else if (item == 2)
    {
      done = Question( "Abort current game?");
      if (!done)
      {
        FadeOut();
        gMustDraw = TRUE;
        DrawPlayArea();
        StatusPanel();
        DrawButtons( b);
        Screen( pc);
        FadeIn( gPalette);
      }
      else if (Question( "Save game?"))
        SaveGame();
    }
    else if (item == 3)
    {
      if (pc->health > 0)
        Drop( pc);
    }
    else if (OverviewClick( mx, my, &pc))
    {
      Screen( pc);
      AlertBadGuys( pc);
    }
    else if (mx >= BOARDLEFT && my < OVERVIEW_TOP)
    {
      /* Mus-klick i spelplanen */

      if (pc->health > 0)
      {
        x = MX_TO_BX( mx);
        y = MY_TO_BY( my);
        if (mk & 2)
        {
          /* H”ger knapp */

          if (VALIDBXY( x, y))
          {
            hidemouse();
            Action( pc, x, y);
            Status( pc);
            Bars( pc);
            Inventory( pc);
            OverviewPanel( FALSE);
            showmouse( NOFORCE);
          }
        }
        else
        {
          /* V„nster knapp */

          if (x || y)
          {
            p = FindCharacter( pc->x + SGN(x), pc->y + SGN(y));
            if (!p)
            {
              Move( pc, x, y);
              SetBuffer( pc->x, pc->y);
              Shade();
              BufferDump();
              MovesLeft( pc);
              AlertBadGuys( pc);
              OverviewPanel( FALSE);
            }
            else
            {
              InterAct( pc, p);
              Status( pc);
              Inventory( pc);
              OverviewPanel( FALSE);
            }
          }
          else
          {
            PickUp( pc);
            OverviewPanel( FALSE);
          }
        }
        HighlightCenter();
      }
    }
    else
    {
      InventoryClick( pc, mx, my, mk);
      OverviewPanel( FALSE);
    }

    while (MOUSEK);
  }
  DisposeButtons( &b);
}

void CenterText( int y, char *s, int c )
{
  ShowText( 320 - 4*strlen( s), y, s, c, -1);
}

void Titles( void )
{
  ClrScr();
  PutPic( 290, 140, gSprites[ 131], 0x03030303);

  CenterText( 280, "MAGUS", WHITE);
  CenterText( 300, "3rd edition", LIGHTGRAY);
  CenterText( 310, "Code & Graphics by Ronny Wester, ronny@rat.se", GRAY);

  CenterText( 330, "Thanx to", GRAY);
  CenterText( 340, "Anders T”rlind", GRAY);
  CenterText( 350, "Jay Stelly", GRAY);
  CenterText( 360, "Christian Wagner", GRAY);
  CenterText( 370, "Gary Shaw", GRAY);
  CenterText( 380, "Mikael Arle", GRAY);
}

void PressAnyKey( void )
{
  CenterText( 460, "Press any key", LIGHTGRAY);
  getch();
}

void ShortSwap( short *i )
{
  *i = (*i >> 8) | ((*i & 0xFF) << 8);
}

void ReadWorld( void )
{
  int f, x, y;
  WorldColumn tmp;

  f = open( "WORLD.MGS", O_RDONLY | O_BINARY);
  if (f >= 0)
  {
    for (x = 0; x < WORLD_X_MAX; x++)
    {
      read( f, tmp, sizeof( WorldColumn));
      for (y = 0; y < WORLD_Y_MAX; y++)
      {
        ShortSwap( &(tmp[ y]));
        gWorld[ x][ y] = tmp[ y];
      }
    }
    read( f, &gGateX, sizeof( gGateX));
    ShortSwap( &gGateX);
    read( f, &gGateY, sizeof( gGateY));
    ShortSwap( &gGateY);
    close( f);
  }
}

void SpreadItems( void )
{
  struct item *i;
  int x, y, count;

  FadeOut();
  hidemouse();
  ClrScr();
  DrawStoneBox( 78, 53, 154, 60);
  SetColor( BLACK);
  SetBgColor( GRAY);
  ShowText( 120, 75, "Thinking...", BLACK, -1);
  FadeIn( gPalette);

  for (count = 0; count < 5000; count++)
  {
    x = Rand( WORLD_X_MAX);
    y = Rand( WORLD_Y_MAX);
    if (((gWorld[ x][ y] >> 8) & 31) == 8) /* Tr„golv  stengolv */
    {
      i = malloc( sizeof( struct item));
      i->id = 1 + Rand( MAXOBJECTS-1);
      if (i->id == O_ARROWS)
        i->modifier = 50 + Rand( 25);
      else if (i->id == O_ENCHANTSPELL)
        i->modifier = Rand( 4) + 3;
      else
        i->modifier = Rand( 7) - 3;
      i->locked = FALSE;
      i->next = NULL;
      PlaceItem( i, x, y);
    }
  }
  showmouse( NOFORCE);
}

/*
---------------------------------------
	Hi-Scores
---------------------------------------
*/

struct HallEntry
{
  char  name[20];
  int	level;
  long	xp;
  int   slayer;
};

#define MAXENTRY	15
struct HallEntry gHallOfFame[ MAXENTRY];
int uHallHasChanged;

void HallInit( void )
{
  int f;

  memset( gHallOfFame, 0, sizeof( gHallOfFame));
  f = open( "FAMOUS.MGS", O_RDONLY | O_BINARY);
  if (f >= 0)
  {
    read( f, gHallOfFame, sizeof( gHallOfFame));
    close( f);
  }
  uHallHasChanged = FALSE;
}

void HallSave( void )
{
  int f;

  if (!uHallHasChanged) return;

  f = open( "FAMOUS.MGS",
            O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
            S_IRUSR | S_IWUSR);
  if (f >= 0)
  {
    write( f, gHallOfFame, sizeof( gHallOfFame));
    close( f);
  }
  uHallHasChanged = FALSE;
}

void EnterHallOfFame( Person *p )
{
  int place, i;

  place = 0;
  while (place < MAXENTRY && gHallOfFame[ place].xp >= p->xp)
    place++;
    
  if (place < MAXENTRY)
  {
    uHallHasChanged = TRUE;
    for (i = MAXENTRY-1; i > place; i--)
      gHallOfFame[ i] = gHallOfFame[ i-1];
    strcpy( gHallOfFame[ place].name, p->name);
    gHallOfFame[ place].level = p->level;
    gHallOfFame[ place].xp = p->xp;
    gHallOfFame[ place].slayer = p->slayer;
  }
}
  
void TheHallOfFame( void )
{
  ButtonPtr b;
  int i;
  struct HallEntry *h;
  
  FadeOut();
  hidemouse();
  ClrScr();

  b = NULL;
  DrawStoneBox( 100, 0, 400, 480);
  AddButton( &b, 250, 440, 350, 465, 0x1C, 0, "OK");
  DrawButtons( b);

  PutPic( 150, 10, gSprites[ 139], 0x03030303);
  ShowText( 200, 20, "Brave heroes", BLACK, -1);

  for (i = 0; i < MAXENTRY; i++)
  {
    h = &gHallOfFame[i];
    if (*(h->name))
    {
      ShowText( 150, 50 + i*25, h->name, BLACK, -1);
      sprintf( uString, "%s/%ld", gLevelNames[ h->level], h->xp);
      ShowText( 350, 50 + i*25, uString, BLACK, -1);
      if (h->slayer)
      {
        sprintf( uString, "slain by %s.", gClassData[ h->slayer].name);
        ShowText( 150, 60 + i*25, uString, BLACK, -1);
      }
      else
        ShowText( 150, 60 + i*25, "Retired", BLACK, -1);
    }
  }

  FadeIn( gPalette);
  showmouse( NOFORCE);

  UserDialog( b);

  FrameButton( b, 0, TRUE);
  while (MOUSEK);
  FrameButton( b, 0, FALSE);

  DisposeButtons( &b);
}


/*
---------------------------------------
	Initialize & CleanUp
---------------------------------------
*/

void Initialize( void )
{
  SetFont( FONT_8X8);

  hidemouse();
  GetPalette( gOldPalette);
  gWorld = (WorldColumn *) malloc( (unsigned long)WORLD_X_MAX * (unsigned long)sizeof( WorldColumn));
  memset( gSprites, 0, sizeof( gSprites));
  memset( gBuffer, 0, sizeof( Board));
  memset( gPreviousBuffer, 0, sizeof( Board));
  gCharacters = NULL;
  gThings = NULL;
  ClrScr();
  SetMouseShape( myMouse);
  srand( (unsigned int) time( NULL));
  gFollowDarkMoves = FALSE;

  showmouse( FORCE);
}

void CleanUp( void )
{
  ErasePics( gSprites, MAXSPRITE);
  SetMouseArrow();
  // SetPalette( gOldPalette);
  free( gWorld);
}


/*
---------------------------------------
	Huvud-programmet
---------------------------------------
*/

void main( int argc, char *argv[] )
{
  appl_init();

  if (argc <= 1)
  {
    if (!InitializeSound())
    {
      printf( "Sound initialization failed!\n");
      getch();
    }
    PlaySong( NULL); // to setup channels...
    printf( "Sound initialization completed\n");
  }

  Initialize();
  hidemouse();
  ReadPics( "MAGUS.ART", gSprites, MAXSPRITE, gPalette);
  SetPalette( gPalette);
  Titles();
  ReadWorld();
  HallInit();
  PressAnyKey();
  showmouse( NOFORCE);
  do
  {
    if (!RestoreGame())
    {
      SelectCharacters();
      SpreadItems();
      CreateHordes();
    }

    FadeOut();
    ClrScr();
    gMustDraw = TRUE;
    Events();
    CleanUpTheWorld();

    TheHallOfFame();
  }
  while (Question( "Play again?"));
  printf( "Exiting main loop...\n");

  HallSave();
  CleanUp();

  printf( "Shutting down sound...");
  ShutDownSound();
  printf( "OK\n");
  printf( "going to text mode...\n");

  appl_exit();
}
