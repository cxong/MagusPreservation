/*
------------------
  ITEMS.C
------------------
*/

#include <stdio.h>
#include "items.h"


struct objectInfo gObjectData[ MAXOBJECTS] =
      { { "Dummy", 0, 0, 0, 0, 0, 0 },
        { "Sword", O_CLASS_WEAPON, 4, O_REQ_ONEHAND, 76, 9, 20 },
        { "2H-Sword", O_CLASS_WEAPON, 8, O_REQ_TWOHANDS, 74, 15, 75 },
        { "Axe", O_CLASS_WEAPON, 5, O_REQ_ONEHAND, 85, 10, 20 },
        { "Dagger", O_CLASS_MIXEDWEAPON, 1, O_REQ_ONEHAND, 88, 6, 10 },
        { "Bow", O_CLASS_RANGEDWEAPON, 3, O_REQ_TWOHANDS, 83, 9, 20 },
        { "Crossbow", O_CLASS_RANGEDWEAPON, 4, O_REQ_TWOHANDS, 82, 12, 20 },
        { "Throwing star", O_CLASS_THROWINGWEAPON, 1, O_REQ_ONEHAND, 89, 9, 5 },
        { "Confusion", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 93, 5, 0 },
        { "Fireblade", O_CLASS_SPECIAL, 2, O_REQ_ONEHAND, 173, 0, 150 },
        { "Helmet", O_CLASS_ARMOR, 3, O_REQ_HEAD, 91, 2, 20 },
        { "Shield", O_CLASS_ARMOR, 3, O_REQ_ONEHAND, 78, 4, 20 },
        { "Leather", O_CLASS_ARMOR, 3, O_REQ_BODY, 79, 2, 20 },
        { "Armour", O_CLASS_ARMOR, 7, O_REQ_BODY, 80, 6, 75 },
        { "Emerald", O_CLASS_GADGET, 1, O_REQ_NECK, 81, 0, 100 },
        { "Emerald ring", O_CLASS_GADGET, 0, O_REQ_FINGER, 90, 0, 150 },
        { "Arrows", O_CLASS_ARROWS, 0, O_REQ_NOTHING, 84, 0, 10 },
        { "Studded leather", O_CLASS_ARMOR, 4, O_REQ_BODY, 77, 3, 20 },
        { "Chainmail", O_CLASS_ARMOR, 6, O_REQ_BODY, 75, 5, 60 },
        { "Lightning bolt", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 55, 10, 0 },
        { "FireBall", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 60, 5, 0 },
        { "Portal", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 56, 7, 0 },
        { "Air", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 72, 7, 0 },
        { "Fire", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 70, 10, 0 },
        { "Water", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 68, 8, 0 },
        { "Earth", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 67, 8, 0 },
        { "Skeleton", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 64, 15, 0 },
        { "Zombie", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 65, 15, 0 },
        { "Vision", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 69, 2, 0 },
        { "Phantom", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 73, 3, 0 },
        { "Sleep", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 59, 8, 0 },
        { "Slayer", O_CLASS_WEAPON, 8, O_REQ_TWOHANDS, 122, 17, 150 },
        { "FastFeet", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 54, 8, 0 },
        { "Heal", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 53, 5, 20 },
        { "StoneAxe", O_CLASS_WEAPON, 10, O_REQ_TWOHANDS, 107, 15, 0 },
        { "Gloves", O_CLASS_ARMOR, 1, O_REQ_HANDS, 58, 1, 20 },
        { "Darkness", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 57, 10, 0 },
        { "Sabre", O_CLASS_WEAPON, 7, O_REQ_ONEHAND, 86, 10, 0 },
        { "Staff", O_CLASS_WEAPON, 3, O_REQ_TWOHANDS, 87, 8, 20 },
        { "Chaos", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 66, 20, 0 },
        { "Demon", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 63, 25, 0 },
        { "Cloak", O_CLASS_ARMOR, 1, O_REQ_SHOULDERS, 94, 1, 20 },
        { "Big shield", O_CLASS_ARMOR, 6, O_REQ_ONEHAND, 110, 6, 20 },
        { "Big helmet", O_CLASS_ARMOR, 4, O_REQ_HEAD, 111, 3, 20 },
        { "Opal ring", O_CLASS_GADGET, 0, O_REQ_FINGER, 97, 0, 150 },
        { "Club", O_CLASS_WEAPON, 5, O_REQ_ONEHAND, 119, 11, 20 },
        { "Topaz", O_CLASS_GADGET, 1, O_REQ_NECK, 123, 0, 300 },
        { "Opal", O_CLASS_GADGET, 1, O_REQ_NECK, 124, 0, 150 },
        { "Stonefoot", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 129, 6, 0 },
        { "Lightning", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 71, 15, 0 },
        { "DragonTooth", O_CLASS_WEAPON, 4, O_REQ_ONEHAND, 132, 13, 150 },
        { "Sun's Edge", O_CLASS_WEAPON, 5, O_REQ_ONEHAND, 133, 13, 150 },
        { "SilverBow", O_CLASS_RANGEDWEAPON, 3, O_REQ_TWOHANDS, 134, 11, 100 },
        { "Bubble", O_CLASS_TRINKET, 0, O_REQ_NOTHING, 92, 0, 150 },
        { "Faithful", O_CLASS_ARROWS, 0, O_REQ_NOTHING, 142, 0, 50 },
        { "SunBow", O_CLASS_SPECIAL, 3, O_REQ_TWOHANDS, 150, 11, 150 },
        { "Chock", O_CLASS_SPECIAL, 4, O_REQ_ONEHAND, 143, 2, 150 },
        { "BloodTaste", O_CLASS_WEAPON, 9, O_REQ_TWOHANDS, 144, 19, 200 },
        { "SunArmour", O_CLASS_ARMOR, 4, O_REQ_BODY, 149, 6, 150 },
        { "ShadowCloak", O_CLASS_ARMOR, 3, O_REQ_BODY | O_REQ_SHOULDERS, 148, 7, 20 },
        { "Elven cloak", O_CLASS_ARMOR, 1, O_REQ_SHOULDERS, 147, 3, 50 },
        { "Focus", O_CLASS_GADGET, 1, O_REQ_HANDS, 146, 0, 150 },
        { "Terror", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 153, 7, 0 },
        { "Berzerk", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 152, 3, 0 },
        { "Protection", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 151, 2, 0 },
        { "Leadball", O_CLASS_THROWINGWEAPON, 1, O_REQ_ONEHAND, 145, 8, 10 },
        { "Wooden shield", O_CLASS_ARMOR, 2, O_REQ_ONEHAND, 163, 3, 10 },
        { "Wakizashi", O_CLASS_WEAPON, 2, O_REQ_ONEHAND, 164, 9, 20 },
        { "SunHelmet", O_CLASS_ARMOR, 2, O_REQ_HEAD, 157, 3, 70 },
        { "SunShield", O_CLASS_ARMOR, 3, O_REQ_ONEHAND, 158, 6, 70 },
        { "ThunderStorm", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 159, 25, 0 },
        { "HyperSpace", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 160, 20, 0 },
        { "Panic", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 161, 20, 0 },
        { "Shooting star", O_CLASS_SPECIAL, 0, O_REQ_ONEHAND, 162, 0, 300 },
        { "Inferno", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 165, 12, 0 },
        { "Negator", O_CLASS_TRINKET, 3, O_REQ_NOTHING, 114, 0, 150 },
        { "Chain'n'ball", O_CLASS_WEAPON, 5, O_REQ_TWOHANDS, 170, 10, 0 },
        { "SunGloves", O_CLASS_ARMOR, 1, O_REQ_HANDS, 171, 2, 20 },
        { "Delay", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 172, 2, 0 },
        { "Ghostblade", O_CLASS_SPECIAL, 4, O_REQ_ONEHAND, 140, 9, 150 },
        { "Enchant", O_CLASS_SPECIAL, 0, O_REQ_ONEHAND, 178, 0, 100 },
        { "Purify", O_CLASS_SPELL, 0, O_REQ_ONEHAND, 179, 20, 0 },
        { "Blue", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Green", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Yellow", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Brown", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Red", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Grey", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "White", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Purple", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 },
        { "Black", O_CLASS_POTION, 1, O_REQ_ONEHAND, 180, 0, 100 } };


void GetItemDescription( struct item *i, char *s )
{
  if (!i) return;

  if (i->id == O_ARROWS)
    sprintf( s, "%s [%d], weight %d",
    		    gObjectData[ i->id].name,
    		    i->modifier,
    		    i->modifier * gObjectData[ i->id].weight);
  else if (!i->modifier ||
  		   gObjectData[ i->id].kind == O_CLASS_POTION ||
  		   i->id == O_ENCHANTSPELL)
    sprintf( s, "%s, weight %d",
                gObjectData[ i->id].name,
      			gObjectData[ i->id].weight);
  else
    sprintf( s, "%s %+d, weight %d",
                gObjectData[ i->id].name,
      			i->modifier,
      			gObjectData[ i->id].weight);
}
