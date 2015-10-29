#include <dos.h>
#include <stdlib.h>
#include "digisnd.h"


#define DELAY 200


void Dosound( void )
{
  PlaySound( SND_MACHINEGUN, 0, 255);
  delay( DELAY);
}

void Ding( void )
{
  PlaySound( SND_KILL2, 0, 255);
  delay( DELAY);
}

void Swing( void )
{
  PlaySound( SND_LAUNCH, 0, 255);
  delay( DELAY);
}

void Swisch( void )
{
  PlaySound( SND_LAUNCH, 0, 255);
  delay( DELAY);
}

void Boing( void )
{
  PlaySound( SND_LASER, 0, 255);
  delay( DELAY);
}

void Click( void )
{
  PlaySound( SND_DOOR, 0, 255);
}

void Hit( void )
{
  PlaySound( SND_THUD, 0, 255);
  delay( DELAY);
}

void Bow( void )
{
  PlaySound( SND_LAUNCH, 0, 255);
  delay( DELAY);
}

void Summon( void )
{
  PlaySound( SND_DONE, 0, 255);
  delay( DELAY);
}

void Zzap( void )
{
  PlaySound( SND_SHRINKER, 0, 255);
  delay( DELAY);
}

void Boomm( void )
{
  PlaySound( SND_BANG, 0, 255);
  delay( DELAY);
}

void Ploing( void )
{
  PlaySound( SND_POWERGUN, 0, 255);
  delay( DELAY);
}

void Bong( void )
{
  PlaySound( SND_HAHAHA, 0, 255);
  delay( DELAY);
}

void Swoop( void )
{
  PlaySound( SND_FLAMER, 0, 255);
  delay( DELAY);
}
