//
//  Hooks.hpp
//  EIOSTest
//
//  Created by Brandon on 2019-12-05.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef Hooks_hpp
#define Hooks_hpp

#include <string>
#include "ReflectionHook.hxx"

struct Hook
{
	std::string cls;
	std::string field;
	std::string desc;
	std::int64_t multiplier;
	
	Hook(std::string cls, std::string field = "", std::string desc = "", std::int64_t multiplier = 1)
	{
		this->cls = cls;
		this->field = field;
		this->desc = desc;
		this->multiplier = multiplier;
	}
	
	ReflectionHook hook()
	{
		return ReflectionHook(nullptr, cls, field, desc);
	}
	
	ReflectionHook hook() const
	{
		return ReflectionHook(nullptr, cls, field, desc);
	}
};

/**  Node  **/
extern Hook HOOK_NODE_CLASS;
extern Hook HOOK_NODE_UID;
extern Hook HOOK_NODE_PREV;
extern Hook HOOK_NODE_NEXT;


/**  NodeDeque  **/
extern Hook HOOK_NODEDEQUE_CLASS;
extern Hook HOOK_NODEDEQUE_HEAD;
extern Hook HOOK_NODEDEQUE_CURRENT;


/**  CacheableNode  **/
extern Hook HOOK_CACHEABLENODE_CLASS;
extern Hook HOOK_CACHEABLENODE_NEXT;
extern Hook HOOK_CACHEABLENODE_PREV;


/**  LinkedList  **/
extern Hook HOOK_LINKEDLIST_CLASS;
extern Hook HOOK_LINKEDLIST_HEAD;
extern Hook HOOK_LINKEDLIST_CURRENT;


/**  HashTable  **/
extern Hook HOOK_HASHTABLE_CLASS;
extern Hook HOOK_HASHTABLE_HEAD;
extern Hook HOOK_HASHTABLE_TAIL;
extern Hook HOOK_HASHTABLE_CACHE_BUCKETS;
extern Hook HOOK_HASHTABLE_INDEX;
extern Hook HOOK_HASHTABLE_CAPACITY;


/**  Queue  **/
extern Hook HOOK_QUEUE_CLASS;
extern Hook HOOK_QUEUE_HEAD;


/**  Cache  **/
extern Hook HOOK_CACHE_CLASS;
extern Hook HOOK_CACHE_HASHTABLE;
extern Hook HOOK_CACHE_QUEUE;
extern Hook HOOK_CACHE_REMAINING;
extern Hook HOOK_CACHE_CAPACITY;


/**  ClassData  **/
extern Hook HOOK_CLASSDATA_CLASS;
extern Hook HOOK_CLASSDATA_BYTES;
extern Hook HOOK_CLASSDATA_METHODS;
extern Hook HOOK_CLASSDATA_FIELDS;


/**  Rasteriser  **/
extern Hook HOOK_RASTERISER_CLASS;
extern Hook HOOK_RASTERISER_PIXELS;
extern Hook HOOK_RASTERISER_WIDTH;
extern Hook HOOK_RASTERISER_HEIGHT;


/**  Rasteriser3D  **/
extern Hook HOOK_RASTERISER3D_CLASS;
extern Hook HOOK_RASTERISER3D_SHADOWDECAY;
extern Hook HOOK_RASTERISER3D_SINETABLE;
extern Hook HOOK_RASTERISER3D_COSINETABLE;


/**  Typeface  **/
extern Hook HOOK_TYPEFACE_CLASS;
extern Hook HOOK_TYPEFACE_CHARACTERPIXELS;


/**  IndexedRGB  **/
extern Hook HOOK_INDEXEDRGB_CLASS;
extern Hook HOOK_INDEXEDRGB_PIXELS;
extern Hook HOOK_INDEXEDRGB_PALETTE;


/**  ImageRGB  **/
extern Hook HOOK_IMAGERGB_CLASS;
extern Hook HOOK_IMAGERGB_PIXELS;
extern Hook HOOK_IMAGERGB_WIDTH;
extern Hook HOOK_IMAGERGB_HEIGHT;
extern Hook HOOK_IMAGERGB_MAXWIDTH;
extern Hook HOOK_IMAGERGB_MAXHEIGHT;


/**  Keyboard  **/
extern Hook HOOK_KEYBOARD_CLASS;


/**  GameShell  **/
extern Hook HOOK_GAMESHELL_CLASS;


/**  Stream  **/
extern Hook HOOK_STREAM_CLASS;
extern Hook HOOK_STREAM_PAYLOAD;
extern Hook HOOK_STREAM_CRC;


/**  BufferedConnection  **/
extern Hook HOOK_BUFFEREDCONNECTION_CLASS;
extern Hook HOOK_BUFFEREDCONNECTION_INPUTSTREAM;
extern Hook HOOK_BUFFEREDCONNECTION_OUTPUTSTREAM;
extern Hook HOOK_BUFFEREDCONNECTION_SOCKET;
extern Hook HOOK_BUFFEREDCONNECTION_PAYLOAD;
extern Hook HOOK_BUFFEREDCONNECTION_ISCLOSED;


/**  CollisionMap  **/
extern Hook HOOK_COLLISIONMAP_CLASS;
extern Hook HOOK_COLLISIONMAP_WIDTH;
extern Hook HOOK_COLLISIONMAP_HEIGHT;
extern Hook HOOK_COLLISIONMAP_ADJACENCY;


/**  NameInfo  **/
extern Hook HOOK_NAMEINFO_CLASS;
extern Hook HOOK_NAMEINFO_NAME;
extern Hook HOOK_NAMEINFO_DECODEDNAME;


/**  Animable  **/
extern Hook HOOK_ANIMABLE_CLASS;
extern Hook HOOK_ANIMABLE_MODELHEIGHT;


/**  Region  **/
extern Hook HOOK_REGION_CLASS;
extern Hook HOOK_REGION_TILES;
extern Hook HOOK_REGION_INTERACTABLEOBJECTS;


/**  AnimableNode  **/
extern Hook HOOK_ANIMABLENODE_CLASS;
extern Hook HOOK_ANIMABLENODE_ID;
extern Hook HOOK_ANIMABLENODE_ANIMATION;
extern Hook HOOK_ANIMABLENODE_FLAGS;
extern Hook HOOK_ANIMABLENODE_ORIENTATION;
extern Hook HOOK_ANIMABLENODE_PLANE;
extern Hook HOOK_ANIMABLENODE_X;
extern Hook HOOK_ANIMABLENODE_Y;


/**  Boundary  **/
extern Hook HOOK_BOUNDARY_CLASS;
extern Hook HOOK_BOUNDARY_ID;
extern Hook HOOK_BOUNDARY_FLAGS;
extern Hook HOOK_BOUNDARY_PLANE;
extern Hook HOOK_BOUNDARY_HEIGHT;
extern Hook HOOK_BOUNDARY_X;
extern Hook HOOK_BOUNDARY_Y;
extern Hook HOOK_BOUNDARY_ORIENTATION;
extern Hook HOOK_BOUNDARY_RENDERABLE;
extern Hook HOOK_BOUNDARY_OLDRENDERABLE;


/**  WallDecoration  **/
extern Hook HOOK_WALLDECORATION_CLASS;
extern Hook HOOK_WALLDECORATION_ID;
extern Hook HOOK_WALLDECORATION_FLAGS;
extern Hook HOOK_WALLDECORATION_PLANE;
extern Hook HOOK_WALLDECORATION_HEIGHT;
extern Hook HOOK_WALLDECORATION_X;
extern Hook HOOK_WALLDECORATION_Y;
extern Hook HOOK_WALLDECORATION_RELATIVEX;
extern Hook HOOK_WALLDECORATION_RELATIVEY;
extern Hook HOOK_WALLDECORATION_ORIENTATION;
extern Hook HOOK_WALLDECORATION_RENDERABLE;
extern Hook HOOK_WALLDECORATION_OLDRENDERABLE;


/**  GroundDecoration  **/
extern Hook HOOK_GROUNDDECORATION_CLASS;
extern Hook HOOK_GROUNDDECORATION_ID;
extern Hook HOOK_GROUNDDECORATION_FLAGS;
extern Hook HOOK_GROUNDDECORATION_X;
extern Hook HOOK_GROUNDDECORATION_Y;
extern Hook HOOK_GROUNDDECORATION_PLANE;
extern Hook HOOK_GROUNDDECORATION_RENDERABLE;


/**  Interactable  **/
extern Hook HOOK_INTERACTABLE_CLASS;
extern Hook HOOK_INTERACTABLE_RENDERABLE;
extern Hook HOOK_INTERACTABLE_ID;
extern Hook HOOK_INTERACTABLE_FLAGS;
extern Hook HOOK_INTERACTABLE_ORIENTATION;
extern Hook HOOK_INTERACTABLE_PLANE;
extern Hook HOOK_INTERACTABLE_HEIGHT;
extern Hook HOOK_INTERACTABLE_X;
extern Hook HOOK_INTERACTABLE_Y;
extern Hook HOOK_INTERACTABLE_RELATIVEX;
extern Hook HOOK_INTERACTABLE_RELATIVEY;
extern Hook HOOK_INTERACTABLE_SIZEX;
extern Hook HOOK_INTERACTABLE_SIZEY;


/**  SceneTile  **/
extern Hook HOOK_SCENETILE_CLASS;
extern Hook HOOK_SCENETILE_BOUNDARY;
extern Hook HOOK_SCENETILE_SCENETILE;
extern Hook HOOK_SCENETILE_INTERACTABLES;
extern Hook HOOK_SCENETILE_WALLDECORATION;
extern Hook HOOK_SCENETILE_GROUNDDECORATION;
extern Hook HOOK_SCENETILE_X;
extern Hook HOOK_SCENETILE_Y;
extern Hook HOOK_SCENETILE_PLANE;


/**  TradingPost  **/
extern Hook HOOK_TRADINGPOST_CLASS;
extern Hook HOOK_TRADINGPOST_STATUS;
extern Hook HOOK_TRADINGPOST_ITEMID;
extern Hook HOOK_TRADINGPOST_PRICE;
extern Hook HOOK_TRADINGPOST_QUANTITY;
extern Hook HOOK_TRADINGPOST_TRANSFERRED;
extern Hook HOOK_TRADINGPOST_SPENT;
extern Hook HOOK_TRADINGPOST_QUERYIDS;


/**  Model  **/
extern Hook HOOK_MODEL_CLASS;
extern Hook HOOK_MODEL_INDICESX;
extern Hook HOOK_MODEL_INDICESY;
extern Hook HOOK_MODEL_INDICESZ;
extern Hook HOOK_MODEL_INDICESLENGTH;
extern Hook HOOK_MODEL_VERTICESX;
extern Hook HOOK_MODEL_VERTICESY;
extern Hook HOOK_MODEL_VERTICESZ;
extern Hook HOOK_MODEL_VERTICESLENGTH;
extern Hook HOOK_MODEL_TEXINDICESX;
extern Hook HOOK_MODEL_TEXINDICESY;
extern Hook HOOK_MODEL_TEXINDICESZ;
extern Hook HOOK_MODEL_TEXVERTICESX;
extern Hook HOOK_MODEL_TEXVERTICESY;
extern Hook HOOK_MODEL_TEXVERTICESZ;
extern Hook HOOK_MODEL_TEXVERTICESLENGTH;
extern Hook HOOK_MODEL_SHADOWINTENSITY;
extern Hook HOOK_MODEL_FITSSINGLETILE;


/**  AnimationSequence  **/
extern Hook HOOK_ANIMATIONSEQUENCE_CLASS;
extern Hook HOOK_ANIMATIONSEQUENCE_CONTROLFLOW;


/**  CombatInfo1  **/
extern Hook HOOK_COMBATINFO1_CLASS;
extern Hook HOOK_COMBATINFO1_HEALTH;
extern Hook HOOK_COMBATINFO1_HEALTHRATIO;


/**  CombatInfo2  **/
extern Hook HOOK_COMBATINFO2_CLASS;
extern Hook HOOK_COMBATINFO2_HEALTHSCALE;


/**  CombatInfoList  **/
extern Hook HOOK_COMBATINFOLIST_CLASS;
extern Hook HOOK_COMBATINFOLIST_HEAD;
extern Hook HOOK_COMBATINFOLIST_CURRENT;


/**  CombatInfoHolder  **/
extern Hook HOOK_COMBATINFOHOLDER_CLASS;
extern Hook HOOK_COMBATINFOHOLDER_COMBATINFOLIST;
extern Hook HOOK_COMBATINFOHOLDER_COMBATINFO2;


/**  Entity  **/
extern Hook HOOK_ENTITY_CLASS;
extern Hook HOOK_ENTITY_ANIMATIONID;
extern Hook HOOK_ENTITY_ANIMATIONDELAY;
extern Hook HOOK_ENTITY_SPOKENTEXT;
extern Hook HOOK_ENTITY_HITDAMAGES;
extern Hook HOOK_ENTITY_HITTYPES;
extern Hook HOOK_ENTITY_HITCYCLE;
extern Hook HOOK_ENTITY_QUEUEX;
extern Hook HOOK_ENTITY_QUEUEY;
extern Hook HOOK_ENTITY_QUEUETRAVERSED;
extern Hook HOOK_ENTITY_QUEUELENGTH;
extern Hook HOOK_ENTITY_LOCALX;
extern Hook HOOK_ENTITY_LOCALY;
extern Hook HOOK_ENTITY_ISANIMATING;
extern Hook HOOK_ENTITY_COMBATCYCLE;
extern Hook HOOK_ENTITY_INTERACTINGINDEX;
extern Hook HOOK_ENTITY_ORIENTATION;
extern Hook HOOK_ENTITY_COMBATINFOLIST;
extern Hook HOOK_ENTITY_HEIGHT;


/**  NPCDefinition  **/
extern Hook HOOK_NPCDEFINITION_CLASS;
extern Hook HOOK_NPCDEFINITION_ID;
extern Hook HOOK_NPCDEFINITION_NAME;
extern Hook HOOK_NPCDEFINITION_ACTIONS;
extern Hook HOOK_NPCDEFINITION_MODELIDS;
extern Hook HOOK_NPCDEFINITION_COMBATLEVEL;


/**  NPC  **/
extern Hook HOOK_NPC_CLASS;
extern Hook HOOK_NPC_DEFINITION;


/**  PlayerDefinition  **/
extern Hook HOOK_PLAYERDEFINITION_CLASS;
extern Hook HOOK_PLAYERDEFINITION_ID;
extern Hook HOOK_PLAYERDEFINITION_ISFEMALE;
extern Hook HOOK_PLAYERDEFINITION_ANIMATEDMODELID;
extern Hook HOOK_PLAYERDEFINITION_MODELID;
extern Hook HOOK_PLAYERDEFINITION_EQUIPMENT;
extern Hook HOOK_PLAYERDEFINITION_MODELCACHE;


/**  Player  **/
extern Hook HOOK_PLAYER_CLASS;
extern Hook HOOK_PLAYER_NAME;
extern Hook HOOK_PLAYER_MODEL;
extern Hook HOOK_PLAYER_VISIBLE;
extern Hook HOOK_PLAYER_DEFINITION;
extern Hook HOOK_PLAYER_COMBATLEVEL;


/**  WidgetNode  **/
extern Hook HOOK_WIDGETNODE_CLASS;
extern Hook HOOK_WIDGETNODE_ID;


/**  Widget  **/
extern Hook HOOK_WIDGET_CLASS;
extern Hook HOOK_WIDGET_NAME;
extern Hook HOOK_WIDGET_TEXT;
extern Hook HOOK_WIDGET_ID;
extern Hook HOOK_WIDGET_PARENTID;
extern Hook HOOK_WIDGET_ITEMID;
extern Hook HOOK_WIDGET_ITEMS;
extern Hook HOOK_WIDGET_ITEMSTACKSIZES;
extern Hook HOOK_WIDGET_ITEMAMOUNT;
extern Hook HOOK_WIDGET_ACTIONS;
extern Hook HOOK_WIDGET_ACTIONTYPE;
extern Hook HOOK_WIDGET_TYPE;
extern Hook HOOK_WIDGET_ISHIDDEN;
extern Hook HOOK_WIDGET_ABSOLUTEX;
extern Hook HOOK_WIDGET_ABSOLUTEY;
extern Hook HOOK_WIDGET_RELATIVEX;
extern Hook HOOK_WIDGET_RELATIVEY;
extern Hook HOOK_WIDGET_SCROLLX;
extern Hook HOOK_WIDGET_SCROLLY;
extern Hook HOOK_WIDGET_WIDTH;
extern Hook HOOK_WIDGET_HEIGHT;
extern Hook HOOK_WIDGET_CHILDREN;
extern Hook HOOK_WIDGET_BOUNDSINDEX;


/**  ItemDefinition  **/
extern Hook HOOK_ITEMDEFINITION_CLASS;
extern Hook HOOK_ITEMDEFINITION_ID;
extern Hook HOOK_ITEMDEFINITION_NAME;
extern Hook HOOK_ITEMDEFINITION_ISMEMBERS;
extern Hook HOOK_ITEMDEFINITION_ACTIONS;
extern Hook HOOK_ITEMDEFINITION_GROUNDACTIONS;


/**  Item  **/
extern Hook HOOK_ITEM_CLASS;
extern Hook HOOK_ITEM_ID;
extern Hook HOOK_ITEM_QUANTITY;


/**  Client  **/
extern Hook HOOK_CLIENT_CLASS;
extern Hook HOOK_CLIENT_REVISION;
extern Hook HOOK_CLIENT_CLIENT;
extern Hook HOOK_CLIENT_LOCALNPCS;
extern Hook HOOK_CLIENT_NPCINDICES;
extern Hook HOOK_CLIENT_LOCALPLAYERS;
extern Hook HOOK_CLIENT_PLAYERINDICES;
extern Hook HOOK_CLIENT_LOCALPLAYER;
extern Hook HOOK_CLIENT_PLAYERINDEX;
extern Hook HOOK_CLIENT_GAMECYCLE;
extern Hook HOOK_CLIENT_LOGINSTATE;
extern Hook HOOK_CLIENT_GAMESTATE;
extern Hook HOOK_CLIENT_CROSSHAIRCOLOUR;
extern Hook HOOK_CLIENT_GROUNDITEMS;
extern Hook HOOK_CLIENT_COLLISIONMAP;
extern Hook HOOK_CLIENT_TRADINGPOSTOFFERS;
extern Hook HOOK_CLIENT_CAMERAX;
extern Hook HOOK_CLIENT_CAMERAY;
extern Hook HOOK_CLIENT_CAMERAZ;
extern Hook HOOK_CLIENT_CAMERAPITCH;
extern Hook HOOK_CLIENT_CAMERAYAW;
extern Hook HOOK_CLIENT_REGION;
extern Hook HOOK_CLIENT_PLANE;
extern Hook HOOK_CLIENT_BASEX;
extern Hook HOOK_CLIENT_BASEY;
extern Hook HOOK_CLIENT_DESTX;
extern Hook HOOK_CLIENT_DESTY;
extern Hook HOOK_CLIENT_SINETABLE;
extern Hook HOOK_CLIENT_COSINETABLE;
extern Hook HOOK_CLIENT_TILEHEIGHTS;
extern Hook HOOK_CLIENT_TILESETTINGS;
extern Hook HOOK_CLIENT_WIDGETS;
extern Hook HOOK_CLIENT_WIDGETSETTINGS;
extern Hook HOOK_CLIENT_WIDGETNODECACHE;
extern Hook HOOK_CLIENT_WIDGETPOSITIONSX;
extern Hook HOOK_CLIENT_WIDGETPOSITIONSY;
extern Hook HOOK_CLIENT_WIDGETWIDTHS;
extern Hook HOOK_CLIENT_WIDGETHEIGHTS;
extern Hook HOOK_CLIENT_VIEWPORTWIDTH;
extern Hook HOOK_CLIENT_VIEWPORTHEIGHT;
extern Hook HOOK_CLIENT_VIEWPORTSCALE;
extern Hook HOOK_CLIENT_MAPANGLE;
extern Hook HOOK_CLIENT_MAPSCALE;
extern Hook HOOK_CLIENT_MAPOFFSET;
extern Hook HOOK_CLIENT_MENUCOUNT;
extern Hook HOOK_CLIENT_MENUACTIONS;
extern Hook HOOK_CLIENT_MENUOPTIONS;
extern Hook HOOK_CLIENT_ISMENUOPEN;
extern Hook HOOK_CLIENT_MENUX;
extern Hook HOOK_CLIENT_MENUY;
extern Hook HOOK_CLIENT_MENUWIDTH;
extern Hook HOOK_CLIENT_MENUHEIGHT;
extern Hook HOOK_CLIENT_CURRENTLEVELS;
extern Hook HOOK_CLIENT_REALLEVELS;
extern Hook HOOK_CLIENT_EXPERIENCES;
extern Hook HOOK_CLIENT_CURRENTWORLD;
extern Hook HOOK_CLIENT_ENERGYLEVEL;
extern Hook HOOK_CLIENT_PLAYERWEIGHT;

#endif /* Hooks_hpp */