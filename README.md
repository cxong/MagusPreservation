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

## Enemies (stats TBA)
- Orch (A, B, C)
- ChaosKnight
- Spirit
- Ghost
- DeathKnight
- Skeleton
- Zombie
- Djinn
- Fenix
- Undine
- Golem
- Demon
- Sorceror
- Hobgoblin
- Minotaur
- Witchlord
- Magician
- Shadowbeast
- Lightning
- Undead
- Troll
- Monk
- Goblin
- Warrior
- ChaosWarrior
- DemonPrince
- Orch (D)
- Guardian
- Dragon
- DeathLord
- The Dark One
- Small one

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
