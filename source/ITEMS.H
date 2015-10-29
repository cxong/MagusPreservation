/* Item definitions for Magus */

#define CANDROP_ITEMS 0x51C3

/* v„rde: Vapen - Skada, Pansar - Skydd, Besv„rjelser - "Drain" */

struct objectInfo
{
  char name[20];
  short kind,
        weight,
        requires,
        icon,
        value,
        price;
};

#define MAXOBJECTS 91

#define O_CLASS_WEAPON				1
#define O_CLASS_THROWINGWEAPON		2
#define O_CLASS_RANGEDWEAPON		3
#define O_CLASS_ARMOR				4
#define O_CLASS_SPELL				5
#define O_CLASS_MIXEDWEAPON			6
#define O_CLASS_GADGET				7
#define O_CLASS_TRINKET				8
#define O_CLASS_ARROWS				9
#define O_CLASS_SPECIAL			   10
#define O_CLASS_POTION			   11

#define O_REQ_NOTHING				0  
#define O_REQ_ONEHAND				1
#define O_REQ_TWOHANDS				2
#define O_REQ_HEAD					4
#define O_REQ_BODY					8
#define O_REQ_HANDS				   16
#define O_REQ_NECK				   32
#define O_REQ_FINGER			   64
#define O_REQ_SHOULDERS			  128
        
#define O_SWORD            1
#define O_TWOHANDEDSWORD   2
#define O_AXE              3
#define O_DAGGER           4
#define O_BOW              5
#define O_CROSSBOW         6
#define O_THROWINGSTAR     7
#define O_CONFUSIONSPELL   8
#define O_FIREBLADE        9
#define O_HELMET           10
#define O_SHIELD           11
#define O_LEATHER          12
#define O_ARMOR            13
#define O_GREENAMULET      14
#define O_GREENRING        15
#define O_ARROWS           16
#define O_STUDDEDLEATHER   17
#define O_CHAINMAIL        18
#define O_LIGHTNINGSPELL   19
#define O_FIREBALLSPELL    20
#define O_TELEPORTSPELL    21
#define O_AIRSPELL         22
#define O_FIRESPELL        23
#define O_WATERSPELL       24
#define O_EARTHSPELL       25
#define O_SKELETONSPELL    26
#define O_ZOMBIESPELL      27
#define O_VISIONSPELL      28
#define O_PHANTOMSPELL     29
#define O_SLEEPSPELL       30
#define O_SLAYERSWORD      31
#define O_SPEEDSPELL       32
#define O_HEALINGSPELL     33
#define O_STONEAXE         34
#define O_GLOVES           35
#define O_DARKNESSSPELL    36
#define O_CUTLASS          37
#define O_STAFF            38
#define O_CHAOSSPELL       39
#define O_DEMONSPELL       40
#define O_CLOAK            41
#define O_LARGESHIELD      42
#define O_LARGEHELMET      43
#define O_BLUERING         44
#define O_MACE             45
#define O_YELLOWAMULET     46
#define O_BLUEAMULET       47
#define O_FREEZESPELL      48
#define O_ENERGYSPELL      49
#define O_DRAGONSTOOTH     50
#define O_FIREAXE          51
#define O_SILVERBOW        52
#define O_BUBBLE           53
#define O_FAITHFULARROW    54
#define O_SUNBOW           55
#define O_CHOCKHAMMER      56
#define O_BLOODTASTEAXE    57
#define O_GOLDENARMOR      58
#define O_SHADOWCLOAK      59
#define O_ELVENCLOAK       60
#define O_REDGLOVES        61
#define O_TERRORSPELL      62
#define O_BERZERKSPELL     63
#define O_WARDSPELL        64
#define O_LEADBALL         65
#define O_WOODENSHIELD     66
#define O_WAKIZASHI        67
#define O_GOLDENHELMET     68
#define O_GOLDENSHIELD     69
#define O_STORMSPELL       70
#define O_HYPERSPACESPELL  71
#define O_PANICSPELL       72
#define O_WISHINGSPELL     73
#define O_INFERNOSPELL     74
#define O_NEGATOR          75
#define O_CHAINANDBALL     76
#define O_GOLDENGLOVES     77
#define O_SLOWSPELL        78
#define O_GHOSTBLADE       79
#define O_ENCHANTSPELL     80
#define O_PURIFYSPELL      81
#define O_BLUEPOTION       82
#define O_GREENPOTION      83
#define O_CLEARPOTION      84
#define O_YELLOWPOTION     85
#define O_REDPOTION        86
#define O_GRAYPOTION       87
#define O_WHITEPOTION      88
#define O_PURPLEPOTION     89
#define O_DARKPOTION       90
              
struct item
{
  short id, modifier;
  char locked;
  struct item *next;
};      
     
struct thing
{
  short x, y;
  struct item *items;
  short backGround;
  struct thing *next;
}; 

extern struct objectInfo gObjectData[ MAXOBJECTS];
void GetItemDescription( struct item *i, char *s );
