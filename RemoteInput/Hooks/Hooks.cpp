//
//  Hooks.cpp
//  EIOSTest
//
//  Created by Brandon on 2019-12-05.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "Hooks.hpp"

/**  Node  **/
Hook HOOK_NODE_CLASS = {"fb"};
Hook HOOK_NODE_UID = {"fb", "cc", "J"};
Hook HOOK_NODE_PREV = {"fb", "ce", "Lfb;"};
Hook HOOK_NODE_NEXT = {"fb", "cp", "Lfb;"};


/**  NodeDeque  **/
Hook HOOK_NODEDEQUE_CLASS = {"jm"};
Hook HOOK_NODEDEQUE_HEAD = {"jm", "u", "Lfb;"};
Hook HOOK_NODEDEQUE_CURRENT = {"jm", "f", "Lfb;"};


/**  CacheableNode  **/
Hook HOOK_CACHEABLENODE_CLASS = {"fn"};
Hook HOOK_CACHEABLENODE_NEXT = {"fn", "cs", "Lfn;"};
Hook HOOK_CACHEABLENODE_PREV = {"fn", "cu", "Lfn;"};


/**  LinkedList  **/
Hook HOOK_LINKEDLIST_CLASS = {"jm"};
Hook HOOK_LINKEDLIST_HEAD = {"jm", "u", "Lfb;"};
Hook HOOK_LINKEDLIST_CURRENT = {"jm", "f", "Lfb;"};


/**  HashTable  **/
Hook HOOK_HASHTABLE_CLASS = {"lb"};
Hook HOOK_HASHTABLE_HEAD = {"lb", "b", "Lfb;"};
Hook HOOK_HASHTABLE_TAIL = {"lb", "g", "Lfb;"};
Hook HOOK_HASHTABLE_CACHE_BUCKETS = {"lb", "f", "[Lfb;"};
Hook HOOK_HASHTABLE_INDEX = {"lb", "z", "I"};
Hook HOOK_HASHTABLE_CAPACITY = {"lb", "u", "I"};


/**  Queue  **/
Hook HOOK_QUEUE_CLASS = {"jz"};
Hook HOOK_QUEUE_HEAD = {"jz", "u", "Lfn;"};


/**  Cache  **/
Hook HOOK_CACHE_CLASS = {"ef"};
Hook HOOK_CACHE_HASHTABLE = {"ef", "g", "Llb;"};
Hook HOOK_CACHE_QUEUE = {"ef", "z", "Ljz;"};
Hook HOOK_CACHE_REMAINING = {"ef", "b", "I"};
Hook HOOK_CACHE_CAPACITY = {"ef", "f", "I"};


/**  ClassData  **/
Hook HOOK_CLASSDATA_CLASS = {"ce"};
Hook HOOK_CLASSDATA_BYTES = {"ce", "y", "[[[B"};
Hook HOOK_CLASSDATA_METHODS = {"ce", "h", "[Ljava/lang/reflect/Method;"};
Hook HOOK_CLASSDATA_FIELDS = {"ce", "p", "[Ljava/lang/reflect/Field;"};


/**  Rasteriser  **/
Hook HOOK_RASTERISER_CLASS = {"ly"};
Hook HOOK_RASTERISER_PIXELS = {"ly", "ad", "[I"};
Hook HOOK_RASTERISER_WIDTH = {"ly", "am", "I"};
Hook HOOK_RASTERISER_HEIGHT = {"ly", "ai", "I"};


/**  Rasteriser3D  **/
Hook HOOK_RASTERISER3D_CLASS = {"eg"};
Hook HOOK_RASTERISER3D_SHADOWDECAY = {"eg", "q", "[I"};
Hook HOOK_RASTERISER3D_SINETABLE = {"eg", "j", "[I"};
Hook HOOK_RASTERISER3D_COSINETABLE = {"eg", "af", "[I"};


/**  Typeface  **/
Hook HOOK_TYPEFACE_CLASS = {"kr"};
Hook HOOK_TYPEFACE_CHARACTERPIXELS = {"kr", "u", "[[B"};


/**  IndexedRGB  **/
Hook HOOK_INDEXEDRGB_CLASS = {"lm"};
Hook HOOK_INDEXEDRGB_PIXELS = {"lm", "f", "[I"};
Hook HOOK_INDEXEDRGB_PALETTE = {"lm", "u", "[B"};


/**  ImageRGB  **/
Hook HOOK_IMAGERGB_CLASS = {"ln"};
Hook HOOK_IMAGERGB_PIXELS = {"ln", "u", "[I"};
Hook HOOK_IMAGERGB_WIDTH = {"ln", "f", "I"};
Hook HOOK_IMAGERGB_HEIGHT = {"ln", "b", "I"};
Hook HOOK_IMAGERGB_MAXWIDTH = {"ln", "p", "I"};
Hook HOOK_IMAGERGB_MAXHEIGHT = {"ln", "h", "I"};


/**  Keyboard  **/
Hook HOOK_KEYBOARD_CLASS = {"an"};


/**  GameShell  **/
Hook HOOK_GAMESHELL_CLASS = {"bo"};


/**  Stream  **/
Hook HOOK_STREAM_CLASS = {"kg"};
Hook HOOK_STREAM_PAYLOAD = {"kg", "p", "[B"};
Hook HOOK_STREAM_CRC = {"kg", "y", "[I"};


/**  BufferedConnection  **/
Hook HOOK_BUFFEREDCONNECTION_CLASS = {"fq"};
Hook HOOK_BUFFEREDCONNECTION_INPUTSTREAM = {"fq", "u", "Ljava/io/InputStream;"};
Hook HOOK_BUFFEREDCONNECTION_OUTPUTSTREAM = {"fq", "f", "Ljava/io/OutputStream;"};
Hook HOOK_BUFFEREDCONNECTION_SOCKET = {"fq", "b", "Ljava/net/Socket;"};
Hook HOOK_BUFFEREDCONNECTION_PAYLOAD = {"fq", "h", "[B"};
Hook HOOK_BUFFEREDCONNECTION_ISCLOSED = {"fq", "g", "Z"};


/**  CollisionMap  **/
Hook HOOK_COLLISIONMAP_CLASS = {"fh"};
Hook HOOK_COLLISIONMAP_WIDTH = {"fh", "ab", "I", -860400911};
Hook HOOK_COLLISIONMAP_HEIGHT = {"fh", "ar", "I", 1540424195};
Hook HOOK_COLLISIONMAP_ADJACENCY = {"fh", "av", "[[I"};


/**  NameInfo  **/
Hook HOOK_NAMEINFO_CLASS = {"jb"};
Hook HOOK_NAMEINFO_NAME = {"jb", "u", "Ljava/lang/String;"};
Hook HOOK_NAMEINFO_DECODEDNAME = {"jb", "f", "Ljava/lang/String;"};


/**  Animable  **/
Hook HOOK_ANIMABLE_CLASS = {"ee"};
Hook HOOK_ANIMABLE_MODELHEIGHT = {"ee", "cq", "I", -254761411};


/**  Region  **/
Hook HOOK_REGION_CLASS = {"el"};
Hook HOOK_REGION_TILES = {"el", "p", "[[[Ldm;"};
Hook HOOK_REGION_INTERACTABLEOBJECTS = {"el", "w", "[Leh;"};


/**  AnimableNode  **/
Hook HOOK_ANIMABLENODE_CLASS = {"cu"};
Hook HOOK_ANIMABLENODE_ID = {"cu", "u", "I"};
Hook HOOK_ANIMABLENODE_ANIMATION = {"cu", "h", "Liy;"};
Hook HOOK_ANIMABLENODE_FLAGS = {"cu", "f", "I"};
Hook HOOK_ANIMABLENODE_ORIENTATION = {"cu", "b", "I"};
Hook HOOK_ANIMABLENODE_PLANE = {"cu", "g", "I"};
Hook HOOK_ANIMABLENODE_X = {"cu", "z", "I"};
Hook HOOK_ANIMABLENODE_Y = {"cu", "p", "I"};


/**  Boundary  **/
Hook HOOK_BOUNDARY_CLASS = {"eu"};
Hook HOOK_BOUNDARY_ID = {"eu", "y", "J", 667705109801803207};
Hook HOOK_BOUNDARY_FLAGS = {"eu", "w", "I", 1624595735};
Hook HOOK_BOUNDARY_PLANE = {"eu", "u", "I", 1092947403};
Hook HOOK_BOUNDARY_HEIGHT = {"eu", "z", "I", 113063737};
Hook HOOK_BOUNDARY_X = {"eu", "f", "I", -1863121231};
Hook HOOK_BOUNDARY_Y = {"eu", "b", "I", 523556867};
Hook HOOK_BOUNDARY_ORIENTATION = {"eu", "g", "I", -470947197};
Hook HOOK_BOUNDARY_RENDERABLE = {"eu", "p", "Lee;"};
Hook HOOK_BOUNDARY_OLDRENDERABLE = {"eu", "h", "Lee;"};


/**  WallDecoration  **/
Hook HOOK_WALLDECORATION_CLASS = {"et"};
Hook HOOK_WALLDECORATION_ID = {"et", "i", "J", -1256134443736608047};
Hook HOOK_WALLDECORATION_FLAGS = {"et", "k", "I", 1975621391};
Hook HOOK_WALLDECORATION_PLANE = {"et", "u", "I", 1205113251};
Hook HOOK_WALLDECORATION_HEIGHT = {"et", "z", "I", 1125268543};
Hook HOOK_WALLDECORATION_X = {"et", "f", "I", -271474909};
Hook HOOK_WALLDECORATION_Y = {"et", "b", "I", -1267605537};
Hook HOOK_WALLDECORATION_RELATIVEX = {"et", "p", "I", -1924743317};
Hook HOOK_WALLDECORATION_RELATIVEY = {"et", "h", "I", -2072940953};
Hook HOOK_WALLDECORATION_ORIENTATION = {"et", "g", "I", -1727501807};
Hook HOOK_WALLDECORATION_RENDERABLE = {"et", "y", "Lee;"};
Hook HOOK_WALLDECORATION_OLDRENDERABLE = {"et", "w", "Lee;"};


/**  GroundDecoration  **/
Hook HOOK_GROUNDDECORATION_CLASS = {"du"};
Hook HOOK_GROUNDDECORATION_ID = {"du", "z", "J", -2591467341780492125};
Hook HOOK_GROUNDDECORATION_FLAGS = {"du", "p", "I", 424120209};
Hook HOOK_GROUNDDECORATION_X = {"du", "f", "I", -2093523795};
Hook HOOK_GROUNDDECORATION_Y = {"du", "b", "I", -512662577};
Hook HOOK_GROUNDDECORATION_PLANE = {"du", "u", "I", 1000348855};
Hook HOOK_GROUNDDECORATION_RENDERABLE = {"du", "g", "Lee;"};


/**  Interactable  **/
Hook HOOK_INTERACTABLE_CLASS = {"eh"};
Hook HOOK_INTERACTABLE_RENDERABLE = {"eh", "z", "Lee;"};
Hook HOOK_INTERACTABLE_ID = {"eh", "o", "J", 9016646142835721523};
Hook HOOK_INTERACTABLE_FLAGS = {"eh", "e", "I", -2013432545};
Hook HOOK_INTERACTABLE_ORIENTATION = {"eh", "p", "I", -1884988183};
Hook HOOK_INTERACTABLE_PLANE = {"eh", "u", "I", 602271545};
Hook HOOK_INTERACTABLE_HEIGHT = {"eh", "f", "I", 996548305};
Hook HOOK_INTERACTABLE_X = {"eh", "b", "I", -28565159};
Hook HOOK_INTERACTABLE_Y = {"eh", "g", "I", -213801155};
Hook HOOK_INTERACTABLE_RELATIVEX = {"eh", "h", "I", -422485367};
Hook HOOK_INTERACTABLE_RELATIVEY = {"eh", "w", "I", -1986114597};
Hook HOOK_INTERACTABLE_SIZEX = {"eh", "y", "I", 640777493};
Hook HOOK_INTERACTABLE_SIZEY = {"eh", "i", "I", 1415770979};


/**  SceneTile  **/
Hook HOOK_SCENETILE_CLASS = {"dm"};
Hook HOOK_SCENETILE_BOUNDARY = {"dm", "h", "Leu;"};
Hook HOOK_SCENETILE_SCENETILE = {"dm", "v", "Ldm;"};
Hook HOOK_SCENETILE_INTERACTABLES = {"dm", "x", "[Leh;"};
Hook HOOK_SCENETILE_WALLDECORATION = {"dm", "y", "Let;"};
Hook HOOK_SCENETILE_GROUNDDECORATION = {"dm", "w", "Ldu;"};
Hook HOOK_SCENETILE_X = {"dm", "f", "I", 1934572627};
Hook HOOK_SCENETILE_Y = {"dm", "b", "I", 1222614809};
Hook HOOK_SCENETILE_PLANE = {"dm", "g", "I", -1168297541};


/**  TradingPost  **/
Hook HOOK_TRADINGPOST_CLASS = {"i"};
Hook HOOK_TRADINGPOST_STATUS = {"i", "u", "B"};
Hook HOOK_TRADINGPOST_ITEMID = {"i", "f", "I", -1162503983};
Hook HOOK_TRADINGPOST_PRICE = {"i", "b", "I", 501595023};
Hook HOOK_TRADINGPOST_QUANTITY = {"i", "g", "I", -483385453};
Hook HOOK_TRADINGPOST_TRANSFERRED = {"i", "z", "I"};
Hook HOOK_TRADINGPOST_SPENT = {"i", "p", "I"};
Hook HOOK_TRADINGPOST_QUERYIDS = {"i", "N/A", "N/A"};


/**  Model  **/
Hook HOOK_MODEL_CLASS = {"df"};
Hook HOOK_MODEL_INDICESX = {"df", "i", "[I"};
Hook HOOK_MODEL_INDICESY = {"df", "k", "[I"};
Hook HOOK_MODEL_INDICESZ = {"df", "x", "[I"};
Hook HOOK_MODEL_INDICESLENGTH = {"df", "w", "I"};
Hook HOOK_MODEL_VERTICESX = {"df", "p", "[I"};
Hook HOOK_MODEL_VERTICESY = {"df", "h", "[I"};
Hook HOOK_MODEL_VERTICESZ = {"df", "y", "[I"};
Hook HOOK_MODEL_VERTICESLENGTH = {"df", "z", "I"};
Hook HOOK_MODEL_TEXINDICESX = {"df", "o", "[I"};
Hook HOOK_MODEL_TEXINDICESY = {"df", "e", "[I"};
Hook HOOK_MODEL_TEXINDICESZ = {"df", "n", "[I"};
Hook HOOK_MODEL_TEXVERTICESX = {"df", "m", "[I"};
Hook HOOK_MODEL_TEXVERTICESY = {"df", "v", "[I"};
Hook HOOK_MODEL_TEXVERTICESZ = {"df", "q", "[I"};
Hook HOOK_MODEL_TEXVERTICESLENGTH = {"df", "t", "I"};
Hook HOOK_MODEL_SHADOWINTENSITY = {"df", "ai", "I"};
Hook HOOK_MODEL_FITSSINGLETILE = {"df", "af", "Z"};


/**  AnimationSequence  **/
Hook HOOK_ANIMATIONSEQUENCE_CLASS = {"iy"};
Hook HOOK_ANIMATIONSEQUENCE_CONTROLFLOW = {"iy", "k", "[I"};


/**  CombatInfo1  **/
Hook HOOK_COMBATINFO1_CLASS = {"bl"};
Hook HOOK_COMBATINFO1_HEALTH = {"bl", "b", "I", -529706747};
Hook HOOK_COMBATINFO1_HEALTHRATIO = {"bl", "f", "I", 1785180099};


/**  CombatInfo2  **/
Hook HOOK_COMBATINFO2_CLASS = {"iz"};
Hook HOOK_COMBATINFO2_HEALTHSCALE = {"iz", "n", "I", -1572537397};


/**  CombatInfoList  **/
Hook HOOK_COMBATINFOLIST_CLASS = {"jm"};
Hook HOOK_COMBATINFOLIST_HEAD = {"jm", "u", "Lfb;"};
Hook HOOK_COMBATINFOLIST_CURRENT = {"jm", "f", "Lfb;"};


/**  CombatInfoHolder  **/
Hook HOOK_COMBATINFOHOLDER_CLASS = {"cm"};
Hook HOOK_COMBATINFOHOLDER_COMBATINFOLIST = {"cm", "g", "Ljm;"};
Hook HOOK_COMBATINFOHOLDER_COMBATINFO2 = {"cm", "b", "Liz;"};


/**  Entity  **/
Hook HOOK_ENTITY_CLASS = {"bt"};
Hook HOOK_ENTITY_ANIMATIONID = {"bt", "br", "I", 980221};
Hook HOOK_ENTITY_ANIMATIONDELAY = {"bt", "c", "I", 613007297};
Hook HOOK_ENTITY_SPOKENTEXT = {"bt", "aj", "Ljava/lang/String;"};
Hook HOOK_ENTITY_HITDAMAGES = {"bt", "ah", "[I"};
Hook HOOK_ENTITY_HITTYPES = {"bt", "bi", "[I"};
Hook HOOK_ENTITY_HITCYCLE = {"bt", "bo", "[I"};
Hook HOOK_ENTITY_QUEUEX = {"bt", "ck", "[I"};
Hook HOOK_ENTITY_QUEUEY = {"bt", "cb", "[I"};
Hook HOOK_ENTITY_QUEUETRAVERSED = {"bt", "cy", "[B"};
Hook HOOK_ENTITY_QUEUELENGTH = {"bt", "cr", "I", 469579589};
Hook HOOK_ENTITY_LOCALX = {"bt", "ai", "I", 1951571129};
Hook HOOK_ENTITY_LOCALY = {"bt", "ag", "I", -1702454117};
Hook HOOK_ENTITY_ISANIMATING = {"bt", "ap", "Z"};
Hook HOOK_ENTITY_COMBATCYCLE = {"bt", "N/A", "N/A"};
Hook HOOK_ENTITY_INTERACTINGINDEX = {"bt", "bn", "I", 2135432751};
Hook HOOK_ENTITY_ORIENTATION = {"bt", "cg", "I", -27617279};
Hook HOOK_ENTITY_COMBATINFOLIST = {"bt", "bd", "Ljm;"};
Hook HOOK_ENTITY_HEIGHT = {"bt", "cl", "I", 1455392247};


/**  NPCDefinition  **/
Hook HOOK_NPCDEFINITION_CLASS = {"ih"};
Hook HOOK_NPCDEFINITION_ID = {"ih", "z", "I", 164501927};
Hook HOOK_NPCDEFINITION_NAME = {"ih", "p", "Ljava/lang/String;"};
Hook HOOK_NPCDEFINITION_ACTIONS = {"ih", "t", "[Ljava/lang/String;"};
Hook HOOK_NPCDEFINITION_MODELIDS = {"ih", "y", "[I"};
Hook HOOK_NPCDEFINITION_COMBATLEVEL = {"ih", "v", "I", 578089375};


/**  NPC  **/
Hook HOOK_NPC_CLASS = {"cd"};
Hook HOOK_NPC_DEFINITION = {"cd", "u", "Lih;"};


/**  PlayerDefinition  **/
Hook HOOK_PLAYERDEFINITION_CLASS = {"hd"};
Hook HOOK_PLAYERDEFINITION_ID = {"hd", "g", "I", -1659743431};
Hook HOOK_PLAYERDEFINITION_ISFEMALE = {"hd", "b", "Z"};
Hook HOOK_PLAYERDEFINITION_ANIMATEDMODELID = {"hd", "z", "J", 8570520165784208047};
Hook HOOK_PLAYERDEFINITION_MODELID = {"hd", "p", "J", -3088053679670010611};
Hook HOOK_PLAYERDEFINITION_EQUIPMENT = {"hd", "u", "[I"};
Hook HOOK_PLAYERDEFINITION_MODELCACHE = {"hd", "x", "Lef;"};


/**  Player  **/
Hook HOOK_PLAYER_CLASS = {"bg"};
Hook HOOK_PLAYER_NAME = {"bg", "u", "Ljb;"};
Hook HOOK_PLAYER_MODEL = {"bg", "n", "Ldf;"};
Hook HOOK_PLAYER_VISIBLE = {"bg", "s", "Z"};
Hook HOOK_PLAYER_DEFINITION = {"bg", "f", "Lhd;"};
Hook HOOK_PLAYER_COMBATLEVEL = {"bg", "h", "I", 274943013};


/**  WidgetNode  **/
Hook HOOK_WIDGETNODE_CLASS = {"bf"};
Hook HOOK_WIDGETNODE_ID = {"bf", "u", "I", -827675327};


/**  Widget  **/
Hook HOOK_WIDGET_CLASS = {"hl"};
Hook HOOK_WIDGET_NAME = {"hl", "dw", "Ljava/lang/String;"};
Hook HOOK_WIDGET_TEXT = {"hl", "cd", "Ljava/lang/String;"};
Hook HOOK_WIDGET_ID = {"hl", "d", "I", 1682461819};
Hook HOOK_WIDGET_PARENTID = {"hl", "av", "I", -603142629};
Hook HOOK_WIDGET_ITEMID = {"hl", "ep", "Ljava/lang/String;"};
Hook HOOK_WIDGET_ITEMS = {"hl", "eo", "[I"};
Hook HOOK_WIDGET_ITEMSTACKSIZES = {"hl", "ei", "[I"};
Hook HOOK_WIDGET_ITEMAMOUNT = {"hl", "ev", "Ljava/lang/String;"};
Hook HOOK_WIDGET_ACTIONS = {"hl", "cj", "[Ljava/lang/String;"};
Hook HOOK_WIDGET_ACTIONTYPE = {"hl", "m", "I", -1207146159};
Hook HOOK_WIDGET_TYPE = {"hl", "t", "I", -1464178541};
Hook HOOK_WIDGET_ISHIDDEN = {"hl", "aq", "Z"};
Hook HOOK_WIDGET_ABSOLUTEX = {"hl", "ad", "I", 1387465487};
Hook HOOK_WIDGET_ABSOLUTEY = {"hl", "am", "I", 69728839};
Hook HOOK_WIDGET_RELATIVEX = {"hl", "ao", "I", -1601823443};
Hook HOOK_WIDGET_RELATIVEY = {"hl", "aw", "I", -543253817};
Hook HOOK_WIDGET_SCROLLX = {"hl", "ac", "I", 559295079};
Hook HOOK_WIDGET_SCROLLY = {"hl", "az", "I", -1519015545};
Hook HOOK_WIDGET_WIDTH = {"hl", "ak", "I", 201590315};
Hook HOOK_WIDGET_HEIGHT = {"hl", "aa", "I", -1083073471};
Hook HOOK_WIDGET_CHILDREN = {"hl", "eq", "[Lhl;"};
Hook HOOK_WIDGET_BOUNDSINDEX = {"hl", "fc", "I", 405715237};


/**  ItemDefinition  **/
Hook HOOK_ITEMDEFINITION_CLASS = {"iu"};
Hook HOOK_ITEMDEFINITION_ID = {"iu", "e", "I", -1707758529};
Hook HOOK_ITEMDEFINITION_NAME = {"iu", "r", "Ljava/lang/String;"};
Hook HOOK_ITEMDEFINITION_ISMEMBERS = {"iu", "am", "Z"};
Hook HOOK_ITEMDEFINITION_ACTIONS = {"iu", "ai", "[Ljava/lang/String;"};
Hook HOOK_ITEMDEFINITION_GROUNDACTIONS = {"iu", "ag", "[Ljava/lang/String;"};


/**  Item  **/
Hook HOOK_ITEM_CLASS = {"cb"};
Hook HOOK_ITEM_ID = {"cb", "u", "I", -1378813027};
Hook HOOK_ITEM_QUANTITY = {"cb", "f", "I", -744056543};


/**  Client  **/
Hook HOOK_CLIENT_CLASS = {"client"};
Hook HOOK_CLIENT_REVISION = {"client", "186", "I"};
Hook HOOK_CLIENT_CLIENT = {"client", "as", "Lclient;"};
Hook HOOK_CLIENT_LOCALNPCS = {"client", "ed", "[Lcd;"};
Hook HOOK_CLIENT_NPCINDICES = {"client", "fe", "[I"};
Hook HOOK_CLIENT_LOCALPLAYERS = {"client", "ja", "[Lbg;"};
Hook HOOK_CLIENT_PLAYERINDICES = {"cn", "h", "[I"};
Hook HOOK_CLIENT_LOCALPLAYER = {"hc", "jg", "Lbg;"};
Hook HOOK_CLIENT_PLAYERINDEX = {"client", "jb", "I", 241706055};
Hook HOOK_CLIENT_GAMECYCLE = {"client", "cr", "I", -283063249};
Hook HOOK_CLIENT_LOGINSTATE = {"client", "bg", "I", -782138248};
Hook HOOK_CLIENT_GAMESTATE = {"cr", "ar", "I", -748182157};
Hook HOOK_CLIENT_CROSSHAIRCOLOUR = {"client", "jw", "I", -1916688599};
Hook HOOK_CLIENT_GROUNDITEMS = {"client", "kg", "[[[Ljd;"};
Hook HOOK_CLIENT_COLLISIONMAP = {"client", "w", "[Lfh;"};
Hook HOOK_CLIENT_TRADINGPOSTOFFERS = {"client", "sv", "[Li;"};
Hook HOOK_CLIENT_CAMERAX = {"ag", "hv", "I", -378303491};
Hook HOOK_CLIENT_CAMERAY = {"du", "hj", "I", -787534095};
Hook HOOK_CLIENT_CAMERAZ = {"eh", "hl", "I", 143211467};
Hook HOOK_CLIENT_CAMERAPITCH = {"bm", "hs", "I", 778957861};
Hook HOOK_CLIENT_CAMERAYAW = {"dn", "hh", "I", -1979391885};
Hook HOOK_CLIENT_REGION = {"b", "gc", "Lel;"};
Hook HOOK_CLIENT_PLANE = {"gj", "jh", "I", 222528841};
Hook HOOK_CLIENT_BASEX = {"ah", "fn", "I", -838256623};
Hook HOOK_CLIENT_BASEY = {"ia", "fa", "I", -1499248159};
Hook HOOK_CLIENT_DESTX = {"client", "px", "I", -731611885};
Hook HOOK_CLIENT_DESTY = {"client", "pn", "I", 16541371};
Hook HOOK_CLIENT_SINETABLE = {"eg", "j", "[I"};
Hook HOOK_CLIENT_COSINETABLE = {"eg", "af", "[I"};
Hook HOOK_CLIENT_TILEHEIGHTS = {"bu", "u", "[[[I"};
Hook HOOK_CLIENT_TILESETTINGS = {"bu", "f", "[[[B"};
Hook HOOK_CLIENT_WIDGETS = {"du", "h", "[[Lhl;"};
Hook HOOK_CLIENT_WIDGETSETTINGS = {"hm", "b", "[I"};
Hook HOOK_CLIENT_WIDGETNODECACHE = {"client", "mq", "Lle;"};
Hook HOOK_CLIENT_WIDGETPOSITIONSX = {"client", "on", "[I"};
Hook HOOK_CLIENT_WIDGETPOSITIONSY = {"client", "ol", "[I"};
Hook HOOK_CLIENT_WIDGETWIDTHS = {"client", "ow", "[I"};
Hook HOOK_CLIENT_WIDGETHEIGHTS = {"client", "oh", "[I"};
Hook HOOK_CLIENT_VIEWPORTWIDTH = {"client", "ro", "I", 1417358417};
Hook HOOK_CLIENT_VIEWPORTHEIGHT = {"client", "rj", "I", -1082228789};
Hook HOOK_CLIENT_VIEWPORTSCALE = {"client", "rr", "I", -766521235};
Hook HOOK_CLIENT_MAPANGLE = {"client", "hp", "I", -1970077779};
Hook HOOK_CLIENT_MAPSCALE = {"client", "N/A", "N/A"};
Hook HOOK_CLIENT_MAPOFFSET = {"client", "N/A", "N/A"};
Hook HOOK_CLIENT_MENUCOUNT = {"client", "lu", "I", 1033223389};
Hook HOOK_CLIENT_MENUACTIONS = {"client", "lw", "[Ljava/lang/String;"};
Hook HOOK_CLIENT_MENUOPTIONS = {"client", "ls", "[Ljava/lang/String;"};
Hook HOOK_CLIENT_ISMENUOPEN = {"client", "ke", "Z"};
Hook HOOK_CLIENT_MENUX = {"cm", "kt", "I", 751391879};
Hook HOOK_CLIENT_MENUY = {"gw", "kc", "I", 124291179};
Hook HOOK_CLIENT_MENUWIDTH = {"da", "kd", "I", -495854777};
Hook HOOK_CLIENT_MENUHEIGHT = {"dc", "ku", "I", -73888697};
Hook HOOK_CLIENT_CURRENTLEVELS = {"client", "kq", "[I"};
Hook HOOK_CLIENT_REALLEVELS = {"client", "kn", "[I"};
Hook HOOK_CLIENT_EXPERIENCES = {"client", "ki", "[I"};
Hook HOOK_CLIENT_CURRENTWORLD = {"client", "bi", "I", 664513407};
Hook HOOK_CLIENT_ENERGYLEVEL = {"client", "mv", "I", -660424691};
Hook HOOK_CLIENT_PLAYERWEIGHT = {"client", "my", "I", 557453795};
