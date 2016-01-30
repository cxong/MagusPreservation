![logo](https://raw.githubusercontent.com/cxong/MagusPreservation/master/logo_small.png)

Magus Preservation
=================

Magus is an obscure DOS rogue-like released in the mid-90's, by Ronny Wester, better known for creating Cyberdogs and [C-Dogs](https://en.wikipedia.org/wiki/C-Dogs).

![magus](https://raw.githubusercontent.com/cxong/MagusPreservation/master/magus.png)

This project is dedicated to documenting and reverse engineering the game.

## [Source Code](https://github.com/cxong/MagusPreservation/tree/master/source)

Original source code saved from link rot by SG ["Stats Guy"]; look out for an SDL port some time...

## File Listing
**DSETUP32.EXE (3rd ed)**
Digital Sound and Music Interface setup for RAW sound files.

**DOS4GW.EXE (3rd ed)**
DOS/4GW was a free limited edition of DOS/4G and was included with the Watcom C compiler with a commercial re-distribution license. It was made widely popular by computer games like Doom.

**EGAVGA.BGI**
EGA/VGA driver, from Borland. No interesting strings.

**FAMOUS.MGS**
High scores file

**MAGUS.ART**
Images, in paletted planar format (see [RonnyWesterPics](https://github.com/cxong/RonnyWesterPics))

**MAGUS.EXE**
Game executable, contains all code plus game information.

**SDUSB32.DSD (3rd ed)**
Direct-Stream Digital (DSD) is the trademark name used by Sony and Philips for their system of digitally recreating audible signals for the Super Audio CD (SACD).

**SOUND.CFG (3rd ed)**
Sound configuration file.

**WORLD.MGS**
World map, 2-byte tiles. First byte is flags, second byte an image index (MAGUS.ART). Total dimensions: 200*320

##World Map (click for full size)
[![map](https://raw.githubusercontent.com/cxong/MagusPreservation/master/map_small.png)](https://raw.githubusercontent.com/cxong/MagusPreservation/master/map.png)

##Archive web content
**[Looking for a game [=Magus]](https://raw.githubusercontent.com/cxong/MagusPreservation/master/archive%20web%20content/Looking%20for%20a%20game%20%5B=Magus%5D.html)**
Ronny Wester's forum comment about his games in Feb 13, 2012.

**[Magus on Haba's Pages](https://raw.githubusercontent.com/cxong/MagusPreservation/master/archive%20web%20content/magus.htm)**
A description and some useful hex codes found by a game collector.

**[Ronny's other stuff](https://raw.githubusercontent.com/cxong/MagusPreservation/master/archive%20web%20content/OtherStuff.html)**
From Ronny's homepage, archived by the web.archive.org. Some other stuff up for grabs...including another fun freeware game, Magus, and some source. It says the C source for Magus is available, but sadly the link is broken.

**[Magus readme on Rogue Class Linux wiki](https://raw.githubusercontent.com/cxong/MagusPreservation/master/archive%20web%20content/Rogue%20Class%20Linux%20%20Artifact%20Content.html)**
A fan-made readme on from the Rogue Class Linux wiki.

**[Magus description on Rogue Class Linux wiki](https://raw.githubusercontent.com/cxong/MagusPreservation/master/archive%20web%20content/Rogue%20Class%20Linux%20%20magus.html)**
Some basic info and link about Magus, from the Rogue Class Linux wiki.

**[Magus on DOSGames.com](https://raw.githubusercontent.com/cxong/MagusPreservation/master/archive%20web%20content/Role%20Playing%20Games.html)**
A nice description and a working download link.

## [Classes](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L272)
Class | Speed | Strength | Wisdom | Skill | Health | Power | Inventory
------|-------|----------|--------|-------|--------|-------|------
Shaman | 6 | 9 | 15 | 7 | 30 | 40 | elven cloak, staff, healing spell, phantom spell, speed spell
Wizard | 4 | 12 | 12 | 10 | 50 | 50 | cloak, dagger, fireball spell, teleport spell, vision spell
Elf | 6 | 14 | 0 | 15 | 90 | 0 | elven cloak, dagger, bow, arrows
Barbarian | 4 | 17 | 0 | 15 | 120 | 0 | mace, studded leather, bow, arrows
Knight | 4 | 24 | 0 | 11 | 90 | 0 | sword, chainmail, shield, helmet
Duck | 5 | 14 | 0 | 17 | 100 | 0 | mace, leather, gloves, bow, arrows
Dwarf | 4 | 20 | 0 | 12 | 120 | 0 | axe, studded leather, gloves
Druid | 4 | 14 | 10 | 10 | 80 | 60 | cloak, dagger, confusion spell, chaos spell, freeze spell

- Power is mana pool

## [Enemies](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L292)

*The table is sorted by rank; it appears in code in a different order*

Name | Speed | Strength | Wisdom | Skill | Health | Power | Rank | Inventory
-----|-------|----------|--------|-------|--------|-------|------|----------
Skeleton | 4 | 7 | 0 | 7 | 30 | 0 | 0 | chain and ball
Zombie | 4 | 12 | 0 | 10 | 10 | 0 | 0 | cutlass
Hobgoblin | 5 | 10 | 0 | 9 | 10 | 0 | 10 | throwing star, dagger, dagger, cloak
Goblin | 4 | 8 | 0 | 14 | 30 | 0 | 10 | sword, leather
Troll | 3 | 20 | 0 | 7 | 100 | 0 | 20 | stone axe
Orch (A) | 4 | 14 | 0 | 9 | 50 | 0 | 40 | leather, cutlass
Orch (D) | 4 | 13 | 0 | 10 | 50 | 0 | 40 | bow, arrows
Magician | 4 | 12 | 10 | 10 | 50 | 30 | 50 | crossbow, darkness spell, confusion spell, cloak, arrows
Monk | 4 | 14 | 11 | 16 | 60 | 20 | 60 | leather, staff, earth spell, water spell, slow spell, fire spell
Warrior | 4 | 12 | 0 | 14 | 80 | 0 | 60 | mace, chainmail, throwing star, throwing star, throwing star
Orch (B) | 4 | 15 | 0 | 13 | 60 | 0 | 80 | chainmail, axe, shield
Orch (C) | 4 | 12 | 12 | 5 | 40 | 20 | 80 | fireball spell, sleep spell, skeleton spell, zombie spell, cutlass
ChaosKnight | 4 | 22 | 0 | 16 | 100 | 0 | 100 | large helmet, armor, two handed sword, gloves
Ghost | 4 | 10 | 10 | 15 | 20 | 10 | 100 | confusion spell\*, chain and ball\*
Minotaur | 4 | 20 | 0 | 12 | 80 | 0 | 100 | studded leather, mace, large shield
Undead | 5 | 17 | 16 | 20 | 70 | 15 | 100 | fireball spell, zombie spell, mace, studded leather, green ring, skeleton spell
Spirit | 5 | 50 | 15 | 15 | 40 | 10 | 120 | chaos spell\*, ghostblade\*, sleep spell
Sorceror | 5 | 25 | 20 | 20 | 150 | 150 | 120 | chainmail, green ring, fire spell, demon spell, energy spell, sleep spell, lightning spell, two handed sword, negator, freeze spell
Witchlord | 5 | 20 | 20 | 20 | 100 | 100 | 140 | lightning spell, fire spell, shadow cloak, fireblade, fireball spell, confusion spell, green ring, negator
DeathKnight | 4 | 40 | 20 | 20 | 200 | 20 | 170 | large shield, axe, armor, helmet, fireball spell, gloves, negator
ChaosWarrior | 4 | 30 | 0 | 19 | 300 | 0 | 200 | large helmet, two handed sword, gloves, armor, cloak
Guardian | 5 | 22 | 0 | 19 | 200 | 0 | 200 | silver bow, armor, large helmet, gloves, faithful arrow, two handed sword
DeathLord | 5 | 25 | 20 | 20 | 200 | 100 | 230 | large helmet, armor, gloves, lightning spell, slayer sword, demon spell
Dragon | 8 | 60 | 25 | 25 | 600 | 300 | 250 | inferno spell\*, dragons tooth\*, dragons tooth, golden armor\*, negator\*, green ring\*
Small one | 8 | 5 | 0 | 0 | 10 | 0 | 1200 |
Djinn | 14 | 1 | 0 | 0 | 1 | 0 | persistent |
Fenix | 5 | 10 | 19 | 17 | 40 | 5 | persistent | fireblade\*
Undine | 6 | 20 | 0 | 15 | 30 | 0 | persistent | cutlass\*
Golem | 3 | 60 | 0 | 10 | 80 | 0 | persistent | stone axe\*
Demon | 6 | 50 | 10 | 17 | 200 | 20 | persistent | fireball spell\*, fireblade\*
Shadowbeast | 5 | 20 | 0 | 10 | 200 | 0 | persistent | mace\*
Lightning | 8 | 4 | 14 | 0 | 1 | 10 | persistent | chock hammer\*
DemonPrince | 6 | 30 | 0 | 20 | 500 | 0 | persistent | golden armor, blood taste axe, golden helmet, golden gloves, cloak, negator, green ring, ghost blade
The Dark One | 7 | 80 | 25 | 25 | 500 | 1000 | persistent | golden armor, golden helmet, negator, blood taste axe, energy spell, green ring, storm spell, demon spell, inferno spell, chaos spell

[Note: it appears rank is used to decide which enemies to spawn; further from starting gate -> spawn higher rank enemies](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L4587)

"persistent" rank probably means they are ignored by the random generator, they could be spawned on special occasions or summoned

[Skill determines how likely they'll hit](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L2809)

Note: some enemy items are OR'd with 0x1000; this probably means those items can't be dropped?

## [Player Levels](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L240)
At the end of a game, the player gets a ranking e.g. "Nobody" or "Apprentice". This is based on the player's level. There is a level cap.

[At every level up, the player gains 10 health and 10 power (if they have wisdom).](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L2481)

Level | Name | XP
------|------|---
0 | Nobody | 50
1 | Amateur | 100
2 | Novice | 200
3 | Apprentice | 300
4 | Trained | 400
5 | Good | 500
6 | Experienced | 700
7 | Expert | 900
8 | Elite | 1200
9 | Master | 1500
10 | ArchMaster | 2000
11 | Legend | 2000

## [Items](https://github.com/cxong/MagusPreservation/blob/master/source/ITEMS.C#L11)

This table is sorted by: type, equips, value; it is ordered differently in the source code

Name | Type | Weight | Equips | Value | Price
-----|------|--------|--------|-------|------
Sword | weapon | 4 | one hand | 9 | 20
Wakizashi | weapon | 2 | one hand | 9 | 20
Axe | weapon | 5 | one hand | 10 | 20
Sabre | weapon | 7 | one hand | 10 | 0
Club | weapon | 5 | one hand | 11 | 20
DragonTooth | weapon | 4 | one hand | 13 | 150
Sun's Edge | weapon | 5 | one hand | 13 | 150
Staff | weapon | 3 | two hands | 10 | 0
Chain'n'ball | weapon | 5 | two hands | 10 | 0
2H-Sword | weapon | 8 | two hands | 15 | 75
StoneAxe | weapon | 10 | two hands | 15 | 0
Slayer | weapon | 8 | two hands | 17 | 150
BloodTaste | weapon | 9 | two hands | 19 | 200
Dagger | mixed weapon | 1 | one hand | 6 | 10
Bow | ranged weapon | 3 | two hands | 9 | 20
SilverBow | ranged weapon | 3 | two hands | 11 | 100
Crossbow | ranged weapon | 4 | two hands | 12 | 20
Leadball | throwing weapon | 1 | one hand | 8 | 10
Throwing star | throwing weapon | 1 | one hand | 9 | 5
Vision | spell | 0 | one hand | 2 | 0
Protection | spell | 0 | one hand | 2 | 0
Delay | spell | 0 | one hand | 2 | 0
Phantom | spell | 0 | one hand | 3 | 0
Berzerk | spell | 0 | one hand | 3 | 0
Confusion | spell | 0 | one hand | 5 | 0
FireBall | spell | 0 | one hand | 5 | 0
Heal | spell | 0 | one hand | 5 | 20
Stonefoot | spell | 0 | one hand | 6 | 0
Portal | spell | 0 | one hand | 7 | 0
Air | spell | 0 | one hand | 7 | 0
Terror | spell | 0 | one hand | 7 | 0
Water | spell | 0 | one hand | 8 | 0
Earth | spell | 0 | one hand | 8 | 0
Sleep | spell | 0 | one hand | 8 | 0
FastFeet | spell | 0 | one hand | 8 | 0
Lightning bolt | spell | 0 | one hand | 10 | 0
Fire | spell | 0 | one hand | 10 | 0
Darkness | spell | 0 | one hand | 10 | 0
Inferno | spell | 0 | one hand | 12 | 0
Skeleton | spell | 0 | one hand | 15 | 0
Zombie | spell | 0 | one hand | 15 | 0
Lightning | spell | 0 | one hand | 15 | 0
Chaos | spell | 0 | one hand | 20 | 0
HyperSpace | spell | 0 | one hand | 20 | 0
Panic | spell | 0 | one hand | 20 | 0
Purify | spell | 0 | one hand | 20 | 0
Demon | spell | 0 | one hand | 25 | 0
ThunderStorm | spell | 0 | one hand | 25 | 0
Helmet | armor | 3 | head | 2 | 20
Big helmet | armor | 4 | head | 3 | 20
SunHelmet | armor | 2 | head | 3 | 70
Wooden shield | armor | 2 | one hand | 3 | 10
Shield | armor | 3 | one hand | 4 | 20
Big shield | armor | 6 | one hand | 6 | 20
SunShield | armor | 3 | one hand | 6 | 70
Leather | armor | 3 | body | 2 | 20
Studded leather | armor | 4 | body | 3 | 20
Chainmail | armor | 6 | body | 5 | 60
Armour | armor | 7 | body | 6 | 75
SunArmour | armor | 4 | body | 6 | 150
Gloves | armor | 1 | hands | 1 | 20
SunGloves | armor | 1 | hands | 2 | 20
Cloak | armor | 1 | shoulders | 1 | 20
Elven cloak | armor | 1 | shoulders | 3 | 50
ShadowCloak | armor | 3 | body, shoulders | 7 | 20
Emerald | gadget | 1 | neck | 0 | 100
Topaz | gadget | 1 | neck | 0 | 300
Opal | gadget | 1 | neck | 0 | 150
Emerald ring | gadget | 0 | finger | 0 | 150
Opal ring | gadget | 0 | finger | 0 | 150
Focus | gadget | 1 | hands | 0 | 150
Arrows | arrows | 0 | | 0 | 10
Faithful | arrows | 0 | | 0 | 50
Bubble | trinket | 0 | | 0 | 150
Negator | trinket | 0 | | 0 | 150
Blue | potion | 1 | one hand | 0 | 100
Green | potion | 1 | one hand | 0 | 100
Yellow | potion | 1 | one hand | 0 | 100
Brown | potion | 1 | one hand | 0 | 100
Red | potion | 1 | one hand | 0 | 100
Grey | potion | 1 | one hand | 0 | 100
White | potion | 1 | one hand | 0 | 100
Purple | potion | 1 | one hand | 0 | 100
Black | potion | 1 | one hand | 0 | 100
Fireblade | special | 2 | one hand | 0 | 150
Shooting star | special | 0 | one hand | 0 | 300
Enchant | special | 0 | one hand | 0 | 100
Chock | special | 4 | one hand | 2 | 150
SunBow | special | 3 | two hands | 11 | 150
Ghostblade | special | 4 | one hand | 9 | 150

- The value attribute takes on different meanings depending on the item type:
  - [for armor, it's the defensive value](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L2754)
  - [for weapons, it's the base damage](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L2858)
  - [for spells, it's the mana required](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L3524)
  - [special items have fixed damage output](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L4168)
