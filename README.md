Magus Preservation
=================

Magus is an obscure DOS rogue-like released in the mid-90's, by Ronny Wester, better known for creating Cyberdogs and [C-Dogs](https://en.wikipedia.org/wiki/C-Dogs).

![magus](https://raw.githubusercontent.com/cxong/MagusPreservation/master/magus.png)

This project is dedicated to documenting and reverse engineering the game.

##File Listing (2nd edition)
**EGAVGA.BGI**
EGA/VGA driver, from Borland. No interesting strings.

**FAMOUS.MGS**
High scores file

**MAGUS.ART**
Most likely images, in paletted format

**MAGUS.EXE**
Game executable, contains all code plus game information.

**WORLD.MGS**
World map, 2-byte tiles. Unknown which bits determine tile type vs modifiers. Total dimensions: 200*320

##File Listing (3rd edition)
**DOS4GW.EXE**
DOS/4GW was a free limited edition of DOS/4G and was included with the Watcom C compiler with a commercial re-distribution license. It was made widely popular by computer games like Doom.

**DSETUP32.EXE**
Digital Sound and Music Interface setup for RAW sound files.

**EGAVGA.BGI**
EGA/VGA driver, from Borland. No interesting strings.

**FAMOUS.MGS**
High scores file

**MAGUS.ART**
Most likely images, in paletted format

**MAGUS.EXE**
Game executable, contains all code plus game information.

**SDUSB32.DSD**
Direct-Stream Digital (DSD) is the trademark name used by Sony and Philips for their system of digitally recreating audible signals for the Super Audio CD (SACD).

**SOUND.CFG**
Sound configuration file.

**WORLD.MGS**
World map, 2-byte tiles. Unknown which bits determine tile type vs modifiers. Total dimensions: 200*320

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

## [Enemies](https://github.com/cxong/MagusPreservation/blob/master/source/MAGUS.C#L292)

*The table is sorted by rank; it appears in code in a different order*

Name | Speed | Strength | Wisdom | Skill | Health | Power | Rank | Inventory
-----|-------|----------|--------|-------|--------|-------|------|----------
Skeleton | 4 | 7 | 0 | 7 | 30 | 0 | 0 | chain and ball
Zombie | 4 | 12 | 0 | 10 | 10 | 0 | 0 | cutlass
Hobgoblin | 5 | 10 | 0 | 9 | 10 | 0 | 10 | 7, 4, 4, 41
Goblin | 4 | 8 | 0 | 14 | 30 | 0 | 10 | sword, leather
Troll | 3 | 20 | 0 | 7 | 100 | 0 | 20 | stone axe
Orch (A) | 4 | 14 | 0 | 9 | 50 | 0 | 40 | 12, 37
Orch (D) | 4 | 13 | 0 | 10 | 50 | 0 | 40 | bow, arrows
Magician | 4 | 12 | 10 | 10 | 50 | 30 | 50 | 6, 36, 8, 41, 16
Monk | 4 | 14 | 11 | 16 | 60 | 20 | 60 | 12, 38, 25, 24, 78, 23
Warrior | 4 | 12 | 0 | 14 | 80 | 0 | 60 | mace, chainmail, 7, 7, 7
Orch (B) | 4 | 15 | 0 | 13 | 60 | 0 | 80 | 18, 3, 11
Orch (C) | 4 | 12 | 12 | 5 | 40 | 20 | 80 | 20, 30, 26, 27, 37
ChaosKnight | 4 | 22 | 0 | 16 | 100 | 0 | 100 | 43, 13, 2, 35
Ghost | 4 | 10 | 10 | 15 | 20 | 10 | 100 | confusion spell\*, chain and ball\*
Minotaur | 4 | 20 | 0 | 12 | 80 | 0 | 100 | 17, 45, 42
Undead | 5 | 17 | 16 | 20 | 70 | 15 | 100 | 20, 27, 45, 17, 15, 26
Spirit | 5 | 50 | 15 | 15 | 40 | 10 | 120 | chaos spell\*, ghostblade\*, sleep spell
Sorceror | 5 | 25 | 20 | 20 | 150 | 150 | 120 | chainmail, green ring, fire spell, demon spell, energy spell, sleep spell, lightning spell, two handed sword, negator, freeze spell
Witchlord | 5 | 20 | 20 | 20 | 100 | 100 | 140 | lightning spell, fire spell, shadow cloak, fireblade, fireball spell, confusion spell, green rign, negator
DeathKnight | 4 | 40 | 20 | 20 | 200 | 20 | 170 | 42, 3, 13, 10, 20, 35, 75
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

Note: some enemy items are OR'd with 0x1000; this probably means those items can't be dropped?

## Ranks
- Nobody
- Amateur
- Novice
- Apprentice
- Trained
- Good
- Experienced
- Expert
- Elite
- Master
- ArchMaster
- Legend

## [Items](https://github.com/cxong/MagusPreservation/blob/master/source/ITEMS.C#L11)
- Sword
- 2H-Sword
- Dagger
- Crossbow
- Throwing star
- Confusion
- Fireblade
- Helmet
- Shield
- Leather
- Armour
- Emerald
- Emerald ring
- Arrows
- Studded leather
- Chainmail
- Lightning bolt
- FireBall
- Portal
- Fire
- Water
- Earth
- Skeleton
- Zombie
- Vision
- Phantom
- Sleep
- Slayer
- FastFeet
- Heal
- StoneAxe
- Gloves
- Darkness
- Sabre
- Staff
- Chaos
- Demon
- Cloak
- Big shield
- Big helmet
- Opal ring
- Club
- Topaz
- Opal
- Stonefoot
- Lightning
- DragonTooth
- Sun's Edge
- SilverBow
- Bubble
- Faithful
- SunBow
- Chock
- BloodTaste
- SunArmour
- ShadowCloak
- Elven cloak
- Focus
- Terror
- Berzerk
- Protection
- Leadball
- Wooden shield
- Wakizashi
- SunHelmet
- SunShield
- ThunderStorm
- HyperSpace
- Panic
- Shooting star
- Inferno
- Negator
- Chain'n'ball
- SunGloves
- Delay
- Ghostblade
- Enchant
- Purify
