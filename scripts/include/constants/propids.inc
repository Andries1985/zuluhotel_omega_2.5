/////////////////////////////////////////////////////////////////////////////
// Propnames.inc
//
// Constants for all the possible prop names
//
//	TODO:	Add types of props other than item, mobile && corpses.
//		Like, weapon/armor/account/equipment && so on to make
//		the database more convenient.
//		
/////////////////////////////////////////////////////////////////////////////



// PropIDs for accounts
const PROPID_ACCOUNT_RELEASE_TIME			:= "Release";


// PropIDs for auction (those are item cprops, but only used by auction)
const PROPID_AUCTION_BUYER				    := "Buyer";
const PROPID_AUCTION_DESCRIPTION			:= "Description";
const PROPID_AUCTION_LAST_BID				:= "LastBid";
const PROPID_AUCTION_SELL_AT				:= "SellAt";
const PROPID_AUCTION_SELLER				    := "AuctionSeller";
const PROPID_AUCTION_STARTING_BID			:= "StartingBid";


// PropIDs for bankboxs
const PROPID_BANKBOX_AREA				    := "StorageArea";
const PROPID_BANKBOX_AUCTION				:= "AuctionBankbox";
const PROPID_BANKBOX_BANK_OF				:= "BankboxOf";


// PropIDs for sys books
const PROPID_BOOK_AUTHOR			    	:= "author";
const PROPID_BOOK_BOOKID			    	:= "bookid";
const PROPID_BOOK_CONTENTS			    	:= "contents";
const PROPID_BOOK_MAX_PAGES		    		:= "maxpages";
const PROPID_BOOK_NO_WRITE		    		:= "nowrite";
const PROPID_BOOK_TITLE				    	:= "title";


// PropIDs for book cases
const PROPID_BOOKCASE_ALWAYS_SPAWN			:= "AlwaysSpawn";
const PROPID_BOOKCASE_CAN_BE_DESTROYED		:= "CanBeDestroyed";
const PROPID_BOOKCASE_NEVER_SPAWN			:= "NeverSpawn";
const PROPID_BOOKCASE_NO_REMOVE				:= "NoRemove";
const PROPID_BOOKCASE_PROTECTED				:= "ProtectedCase";


// PropIDs for burners
const PROPID_BURNER_LAST_POTION_MADE		:= "lastmade";


// PropIDs for chests
const PROPID_CHEST_ALWAYS_SPAWN				:= "AlwaysSpawn";
const PROPID_CHEST_CAN_BE_DESTROYED			:= "CanBeDestroyed";
const PROPID_CHEST_HOUSE_SERIAL				:= "HouseSerial";
const PROPID_CHEST_LOCK_DIFFICULTY			:= "LockDifficulty";
const PROPID_CHEST_NEVER_SPAWN				:= "NeverSpawn";
const PROPID_CHEST_SPAWN_LEVEL				:= "SpawnChestLevel";
const PROPID_CHEST_PERMANENTLY_TRAPPED		:= "PermanentlyTrapped";
const PROPID_CHEST_TRAP_STRENGTH			:= "Trap_Strength";
const PROPID_CHEST_TRAP_TYPE				:= "Trap_Type";
const PROPID_CHEST_TRAPPED_BY				:= "Trapped_By";
const PROPID_CHEST_TREASURE_CHEST			:= "TreasureChest";
const PROPID_CHEST_SPAWNPOINT_CHEST			:= "SpawnPointChest";


// PropIDs for corpses
const PROPID_CORPSE_CHOPPED			    	:= "ChoppedAlready";
const PROPID_CORPSE_CHOPPED_BY				:= "ChoppedBy";
const PROPID_CORPSE_CORPSE_OF				:= "whose";
const PROPID_CORPSE_IS_BLUE			    	:= "IsBlue";
const PROPID_CORPSE_LOOTED_BY				:= "LootedBy";
const PROPID_CORPSE_NO_DECAY				:= "NoDecay";
const PROPID_CORPSE_WAS_CUT				    := "Cut";
const PROPID_CORPSE_IS_RPER                 := "IsRPer";


// PropIDs for datastore
const PROPID_DATASTORE_BOOK_AUTHOR		    := "author";
const PROPID_DATASTORE_BOOK_CONTENTS		:= "contents";
const PROPID_DATASTORE_BOOK_TITLE		    := "title";

// PropIDs for carpentry deeds
const PROPID_DEED_BASE_NAME			    	:= "ItemBaseName";
const PROPID_DEED_EXCEPTIONAL				:= "ItemIsExceptional";
const PROPID_DEED_LOCKABLE				    := "ItemIsLockable";
const PROPID_DEED_QUALITY				    := "ItemQuality";


// PropIDs for house deeds
const PROPID_DEED_BUILT_SERIAL				:= "builtserial";


// PropIDs for fields
const PROPID_FIELD_CREATOR				    := "Creator";


// PropIDs for moongates
const PROPID_GATE_DEST_X			    	:= "GateDestX";
const PROPID_GATE_DEST_Y			    	:= "GateDestY";
const PROPID_GATE_DEST_Z			    	:= "GateDestZ";


// PropIDs that can be found on anything
const PROPID_GENERAL_MOBILE_SAVED_DATA		:= "SavedMobileData";


// PropIDs for global properties
const PROPID_GLOBAL_AREAS_SAFEAREA			:= "SafeArea";
const PROPID_GLOBAL_AREAS_NOPK			    := "NOPK";
const PROPID_GLOBAL_AREAS_RPAREA			:= "RPAREA";
const PROPID_GLOBAL_AREAS_NOMARKING			:= "DisabledMarkingAreas";
const PROPID_GLOBAL_AREAS_ANTIMAGIC			:= "AntiMagicAreas";
const PROPID_GLOBAL_AREAS_FORBIDDEN			:= "ForbiddenAreas";
const PROPID_GLOBAL_AREAS_GUARDED			:= "GuardedAreas";
const PROPID_GLOBAL_AREAS_NOLOOTING			:= "AntiLootAreas";
const PROPID_GLOBAL_AREAS_NORECALL			:= "DisabledRecallingAreas";
const PROPID_GLOBAL_AREAS_NORECALLFROM	    := "DisabledRecallingFromAreas";
const PROPID_GLOBAL_AUCTION_LOCK			:= "#AuctionLock";
const PROPID_GLOBAL_AUCTION_LOCK_NPC		:= "#AuctionLockNpc";
const PROPID_GLOBAL_CTF_PLAYERS				:= "#ctfplyr";
const PROPID_GLOBAL_DISABLE_NPC_LOOTING		:= "#DisableNpcLooting";
const PROPID_GLOBAL_KARMASYS_FAME_ARRAY		:= "#FameArray";
const PROPID_GLOBAL_KARMASYS_KARMA_ARRAY	:= "#KarmaArray";
const PROPID_GLOBAL_LAST_SAVE				:= "#LastSave";
const PROPID_GLOBAL_LISTS_MANAGER			:= "#ListsManagerPID";
const PROPID_GLOBAL_NEXT_LOCKID				:= "nextlockid";
const PROPID_GLOBAL_RUNNING_QUEST			:= "RunningQuest";
const PROPID_GLOBAL_SKILL_MULTIPLIERS		:= "SkillPointsMultipliers";
const PROPID_GLOBAL_SPAWNBOOKCASES_MANAGER	:= "#SpawnBookCasesManagerPID";
const PROPID_GLOBAL_SPAWNCHESTS_MANAGER		:= "#SpawnChestsManagerPID";
const PROPID_GLOBAL_SPAWNPOINTS_FORBIDDEN_TEMPLATES	:= "ForbiddenTemplates";
const PROPID_GLOBAL_SPAWNPOINTS_MANAGER		:= "#SpawnPointManagerPID";
const PROPID_GLOBAL_STAFF_PAGES				:= "gmpages";


// PropIDs for heads
const PROPID_HEAD_OF				    	:= "HeadOf";


// PropIDs for items
const PROPID_ITEM_ALREADY_EQUIPPED			:= "EquippedAlready";
const PROPID_ITEM_ASTRAL			    	:= "Astral";
const PROPID_ITEM_BASE_NAME			    	:= "BaseName";
const PROPID_ITEM_CANT_BE_LOOTED			:= "CantBeLooted";
const PROPID_ITEM_CAST_AS_CIRCLE			:= "CastAsCircle";
const PROPID_ITEM_CHARGES			    	:= "charges";
const PROPID_ITEM_CHARGES_COST				:= "CostPerCharge";
const PROPID_ITEM_CHARGES_MAX				:= "maxcharges";
const PROPID_ITEM_CHANCE_OF_EFFECT			:= "ChanceOfEffect";
const PROPID_ITEM_CONSECRATED				:= "Consecrated";
const PROPID_ITEM_CONVERTED_TO_POL94		:= "ConvertedPol094";
const PROPID_ITEM_CURSED				    := "Cursed";
const PROPID_ITEM_DAMAGED_LOCK				:= "#DamagedLock";
const PROPID_ITEM_DEFAULT_DEX_MOD			:= "DefaultDex";
const PROPID_ITEM_DESTROY_IT				:= "#DestroyIt";
const PROPID_ITEM_DURATION				    := "Duration";
const PROPID_ITEM_DURATION_PID				:= "DurationPID";
const PROPID_ITEM_EFFECTIVE_CIRCLE			:= "EffectCircle";
const PROPID_ITEM_EMPTY_NAME				:= "EmptyName";
const PROPID_ITEM_ENCHANT_COLOR				:= "EnchantColor";
const PROPID_ITEM_ENCHANT_LEVEL				:= "EnchantLevel";
const PROPID_ITEM_ENCHANT_NAME				:= "Enchanted";
const PROPID_ITEM_ENCHANT_WITH				:= "EnchantedWith";
const PROPID_ITEM_FOOD_VALUE				:= "foodvalue";
const PROPID_ITEM_FORGED_WITH				:= "ForgedWith";
const PROPID_ITEM_FROM_LOOT				    := "FromLoot";
const PROPID_ITEM_GIVE_ARMOR_BONUS			:= "ArBonus";
const PROPID_ITEM_GIVE_DEXTERITY			:= "dex";
const PROPID_ITEM_GIVE_IMMUNITY_MAGIC			:= "MagicImmunity";
const PROPID_ITEM_GIVE_IMMUNITY_POISON			:= "PoisonImmunity";
const PROPID_ITEM_GIVE_IMMUNITY_SPELL_REFLECTION	:= "SpellReflect";
const PROPID_ITEM_GIVE_INTELLIGENCE			:= "int";
const PROPID_ITEM_GIVE_LUMBERJACKING_BONUS		:= "LumberjackingBonus";
const PROPID_ITEM_GIVE_MINING_BONUS			:= "MiningBonus";
const PROPID_ITEM_GIVE_PARTIAL_PROT_ACID		:= "PartialAcidProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_AIR			:= "PartialAirProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_ASTRAL		:= "PartialAstralProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_EARTH		:= "PartialEarthProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_FIRE		:= "PartialFireProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_FREE_ACTION		:= "PartialFreeAction";
const PROPID_ITEM_GIVE_PARTIAL_PROT_HEALING		:= "PartialHealingIncrease";
const PROPID_ITEM_GIVE_PARTIAL_PROT_HOLY		:= "PartialHolyProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_MAGICAL		:= "PartialMagicalProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_MAGIC_IMMUNITY	:= "PartialPermMagicImmunity";
const PROPID_ITEM_GIVE_PARTIAL_PROT_NECRO		:= "PartialNecroProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_PHYSICAL		:= "PartialPhysicalProtection";
const PROPID_ITEM_GIVE_PARTIAL_PROT_POISON_IMMUNITY	:= "PartialPermPoisonImmunity";
const PROPID_ITEM_GIVE_PARTIAL_PROT_SPELL_REFLECTION	:= "PartialPermMr";
const PROPID_ITEM_GIVE_PARTIAL_PROT_WATER		:= "PartialWaterProtection";
const PROPID_ITEM_GIVE_PROTECTION_ACID			:= "AcidProtection";
const PROPID_ITEM_GIVE_PROTECTION_AIR			:= "AirProtection";
const PROPID_ITEM_GIVE_PROTECTION_ASTRAL		:= "AstralProtection";
const PROPID_ITEM_GIVE_PROTECTION_EARTH			:= "EarthProtection";
const PROPID_ITEM_GIVE_PROTECTION_FIRE			:= "FireProtection";
const PROPID_ITEM_GIVE_PROTECTION_FREE_ACTION		:= "FreeAction";
const PROPID_ITEM_GIVE_PROTECTION_HEALING		:= "HealingIncrease";
const PROPID_ITEM_GIVE_PROTECTION_HOLY			:= "HolyProtection";
const PROPID_ITEM_GIVE_PROTECTION_MAGICAL		:= "MagicalProtection";
const PROPID_ITEM_GIVE_PROTECTION_MAGIC_IMMUNITY	:= "PermMagicImmunity";
const PROPID_ITEM_GIVE_PROTECTION_NECRO			:= "NecroProtection";
const PROPID_ITEM_GIVE_PROTECTION_PHYSICAL		:= "PhysicalProtection";
const PROPID_ITEM_GIVE_PROTECTION_POISON_IMMUNITY	:= "PermPoisonImmunity";
const PROPID_ITEM_GIVE_PROTECTION_SPELL_REFLECTION	:= "PermMr";
const PROPID_ITEM_GIVE_PROTECTION_WATER			:= "WaterProtection";
const PROPID_ITEM_GIVE_REGENERATION_LIFE		:= "HpRegen";
const PROPID_ITEM_GIVE_REGENERATION_MANA		:= "ManaRegen";
const PROPID_ITEM_GIVE_REGENERATION_STAMINA		:= "StamRegen";
const PROPID_ITEM_GIVE_SKILL_AMOUNT			:= "skilladvamount";
const PROPID_ITEM_GIVE_SKILL_AVAILABLE_SKILLID	:= "AvailSkills";
const PROPID_ITEM_GIVE_SKILL_GIVEN			:= "#SkillGiven";
const PROPID_ITEM_GIVE_SKILL_SKILLID		:= "skilladv";
const PROPID_ITEM_GIVE_STRENGTH				:= "str";
const PROPID_ITEM_GM_ITEM				    := "IsGMItem";
const PROPID_ITEM_HITSCRIPT_NUMBER			:= "HitscriptNumber";
const PROPID_ITEM_HIT_WITH_SPELL			:= "HitWithSpell";
const PROPID_ITEM_HOUSE_SERIAL				:= "HouseSerial";
const PROPID_ITEM_IDED				    	:= "IDed";
const PROPID_ITEM_IGNORE_IT			    	:= "#IgnoreIt";
const PROPID_ITEM_INDESTRUCTIBLE			:= "Indestructible";
const PROPID_ITEM_IS_EXCEPTIONAL			:= "IsExceptional";
const PROPID_ITEM_ITEM_TYPE				    := "itemtype";
const PROPID_ITEM_LAST_MADE			    	:= "lastmade";
const PROPID_ITEM_LAST_USED_TIMER			:= "#LastUse";
const PROPID_ITEM_LOCKABLE				    := "lockable";
const PROPID_ITEM_LOCKED_DOWN				:= "lockeddown";
const PROPID_ITEM_LOCKID				    := "lockid";
const PROPID_ITEM_LOCKPICKED_BY				:= "LockPickedBy";
const PROPID_ITEM_MADE_BY				    := "MadeBy";
const PROPID_ITEM_MADE_BY_SPECIALIST		:= "MadeBySpecialist";
const PROPID_ITEM_MAGICAL_LEVEL				:= "MagicalLevel";
const PROPID_ITEM_MAGIC_PENALTY				:= "MagicPenalty";
const PROPID_ITEM_NAME				    	:= "name";
const PROPID_ITEM_NOT_IN_LOOT				:= "NotInLoot";
const PROPID_ITEM_OF_BLACKROCK				:= "OfBlackrock";
const PROPID_ITEM_ON_HIT_SCRIPT				:= "OnHitScript";
const PROPID_ITEM_ON_HIT_SCRIPT_NUMBER		:= "OnHitscriptNumber";
const PROPID_ITEM_OWNER				    	:= "ownerserial";
const PROPID_ITEM_OWNER_NAME				:= "ownername";
const PROPID_ITEM_PLAY_ANIM			    	:= "PlayAnim";
const PROPID_ITEM_POISONED_BY				:= "PoisonedBy";
const PROPID_ITEM_POISONED_FROM_SKILL		:= "SkillPoisoned";
const PROPID_ITEM_POISON_LEVEL_FOR_HITSCRIPT	:= "Poisonlvl";
const PROPID_ITEM_POISON_LEVEL_FOR_POTION	:= "Poisoned";
const PROPID_ITEM_POTION_TYPE				:= "itemtype";
const PROPID_ITEM_POWER_LEVEL				:= "Powerlevel";
const PROPID_ITEM_PROTECTED_FROM_CREATURE_TYPE	:= "ProtectedType";
const PROPID_ITEM_RECALL_X			    	:= "x";
const PROPID_ITEM_RECALL_Y			    	:= "y";
const PROPID_ITEM_RECALL_Z			    	:= "z";
const PROPID_ITEM_RECHARGEABLE				:= "Rechargeable";
const PROPID_ITEM_REEQUIP_ON_UNPOSSESS		:= "ReEquipOnUnPossess";
const PROPID_ITEM_REQUIRE_NO_CHARGES		:= "RequireNoCharges";
const PROPID_ITEM_REQUIREMENTS				:= "ToUseRequirements";
const PROPID_ITEM_RETURN_FOR_OBJTYPE		:= "ReturnForObjtype";
const PROPID_ITEM_RETURN_FOR_SERIAL			:= "ReturnForSerial";
const PROPID_ITEM_RETURN_FOR_WORD			:= "ReturnForWord";
const PROPID_ITEM_RUNES_DEFINITION			:= "RuneDefs";
const PROPID_ITEM_SECURE_CONTAINER			:= "SecureContainer";
const PROPID_ITEM_SLAY_CREATURE_TYPE		:= "SlayType";
const PROPID_ITEM_SPELL_DELAY_MS			:= "SpellDelayMs";
const PROPID_ITEM_SPELLID				    := "SpellID";
const PROPID_ITEM_STATIC_COLOR				:= "StaticColor";
const PROPID_ITEM_STATIC_NAME				:= "StaticName";
const PROPID_ITEM_STATIC_NAME_CURSED		:= "StaticCursedName";
const PROPID_ITEM_STAY_EQUIPPED				:= "StayEquipped";
const PROPID_ITEM_SYSTEM_DESTROY			:= "DestroySystemItem";
const PROPID_ITEM_TO_USE_REQUIREMENTS		:= "ToUseRequirements";
const PROPID_ITEM_TRUE_NAME				    := "tname";
const PROPID_ITEM_UNDYEABLE				    := "Undyeable";
const PROPID_ITEM_UNIDED_MAGICAL_ITEM		:= "UnIDedMagicalItem";
const PROPID_ITEM_USE_SPECIAL_MULTIPLIER	:= "SpecMult";
const PROPID_ITEM_USE_WEAPON_ADVANCEMENT	:= "WeapAdv";
const PROPID_ITEM_WANTED_BY			    	:= "WantedBy";
const PROPID_ITEM_WAS_CURSED				:= "WasCursed";


// PopIDs for mana crystal
const PROPID_MANACRYSTAL_MANA_LEVEL			:= "ManaLevel";


// PropIDs for mobiles
const PROPID_MOBILE_ANCHOR			    	:= "Anchor";
const PROPID_MOBILE_ANIMATED				:= "animated";
const PROPID_MOBILE_AREAS_ALLOWED_ANTIMAGIC	:= "AllowedAntiMagicAreas";
const PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN	:= "AllowedForbiddenAreas";
const PROPID_MOBILE_AREAS_ALLOWED_LOOTING	:= "AllowedLootingAreas";
const PROPID_MOBILE_AREAS_ALLOWED_RECALLING	:= "AllowedRecallingAreas";
const PROPID_MOBILE_AREAS_BANNED			:= "BannedAreas";
const PROPID_MOBILE_ARMOR_BONUS_AWARDED		:= "ArBonus";
const PROPID_MOBILE_ATTACHED_RUNNING_PID	:= "#AttachedRunningPid";
const PROPID_MOBILE_ATTACKED_SELF			:= "#AttackedSelf";
const PROPID_MOBILE_ATTACK_TYPE_IMMUNITIES	:= "AttackTypeImmunities";
const PROPID_MOBILE_AUCTION_ON				:= "AuctionOn";
const PROPID_MOBILE_AUTOLOOP_AMOUNT			:= "autoloop";
const PROPID_MOBILE_BLINDED			    	:= "#Blinded";
const PROPID_MOBILE_CANT_BE_LOOTED			:= "CantBeLooted";
const PROPID_MOBILE_CASTING_OPTIONS_PERMANENT	:= "PermCastingOpts";
const PROPID_MOBILE_CASTING_OPTIONS_TEMPORARY	:= "#CastingOpts";
const PROPID_MOBILE_CHARMED			    	:= "charmed";
const PROPID_MOBILE_CLASSE			    	:= "ClasseData";
const PROPID_MOBILE_CREATURE_TYPE			:= "Type";
const PROPID_MOBILE_CTF_DEATHS				:= "ctfdeaths";
const PROPID_MOBILE_CTF_KILLS				:= "ctfkills";
const PROPID_MOBILE_CTF_LOCATION			:= "ctfloc";
const PROPID_MOBILE_CTF_TEAM				:= "team";
const PROPID_MOBILE_CTF_TOTAL_DEATHS		:= "totalctfdeaths";
const PROPID_MOBILE_CTF_TOTAL_KILLS			:= "totalctfkills";
const PROPID_MOBILE_DONT_WANT_FRIENDS		:= "WantNoFriends";
const PROPID_MOBILE_DRUNKNESS				:= "#proof";
const PROPID_MOBILE_EXP				    	:= "EXP";
const PROPID_MOBILE_FAME			    	:= "Fame";
const PROPID_MOBILE_FINAL_DEATH				:= "FinalDeath";
const PROPID_MOBILE_FLED_COUNT				:= "#flees";
const PROPID_MOBILE_FREE_DEATH				:= "freedeath";
const PROPID_MOBILE_FREE_TIMER				:= "freetime";
const PROPID_MOBILE_FRIENDS			    	:= "MobileFriends";
const PROPID_MOBILE_FROZEN			    	:= "frozen";
const PROPID_MOBILE_GUARD_KILLED			:= "guardkill";
const PROPID_MOBILE_GUARDS_CALLED_ON		:= "GuardsCalledOnMe";
const PROPID_MOBILE_GUILD_ID				:= "GuildId";
const PROPID_MOBILE_HEAD_POSITION			:= "head";
const PROPID_MOBILE_HITLIST			    	:= "Hitlist";
const PROPID_MOBILE_HUMUC_SERIAL			:= "Humuc";
const PROPID_MOBILE_HUNGER_LEVEL			:= "hunger";
const PROPID_MOBILE_IMMUNITIES				:= "Immunities";
const PROPID_MOBILE_IMMUNITIES_MODS			:= "#ImmunitiesMods";
const PROPID_MOBILE_INCOGNITO				:= "#Incognito";
const PROPID_MOBILE_INVISIBLE				:= "#IsInvisible";
const PROPID_MOBILE_JUST_GATED				:= "#Moved";
const PROPID_MOBILE_KARMA			    	:= "Karma";
const PROPID_MOBILE_KILLED_BY				:= "KilledBy";
const PROPID_MOBILE_KILL_ME			    	:= "KillMe";
const PROPID_MOBILE_LAST_BANK_USE			:= "#LastBankUse";	
const PROPID_MOBILE_LAST_BEGGED				:= "#LastBegged";
const PROPID_MOBILE_LAST_CAST				:= "#LastCast";
const PROPID_MOBILE_LAST_DEATH				:= "death";
const PROPID_MOBILE_LAST_GREET				:= "#lastgreet";
const PROPID_MOBILE_LAST_SHRINE_EFFECTS		:= "#LastShrineEffects";
const PROPID_MOBILE_LAST_SHRINE_TEXTS		:= "LastShrineTexts";
const PROPID_MOBILE_LOGIN_TIME				:= "#logontime";
const PROPID_MOBILE_MAGIC_PENALTY			:= "#MagicPenalty";
const PROPID_MOBILE_MAGIC_REFLECTION		:= "#Mr";
const PROPID_MOBILE_MASTER_SERIAL			:= "master";
const PROPID_MOBILE_MERCHANT_TYPE			:= "MerchantType";
const PROPID_MOBILE_MODS_NAME				:= "NameMods";
const PROPID_MOBILE_MODS_PERSISTED			:= "PersistedMods";
const PROPID_MOBILE_MODS_POLYMORPH			:= "PolymorphMods";
const PROPID_MOBILE_MODS_PROTECTION			:= "#ProtectionMods";
const PROPID_MOBILE_MODS_REGEN				:= "RegenerationMods";
const PROPID_MOBILE_MODS_TEMPORARY			:= "#TempMods";
const PROPID_MOBILE_MSG_ON				    := "MsgOn";
const PROPID_MOBILE_MURDER_COUNT_LONG_DECAY		:= "decaylongcountat";
const PROPID_MOBILE_MURDER_COUNT_SHORT_DECAY	:= "decayshortcountat";
const PROPID_MOBILE_MURDER_COUNT_LONG_TERM		:= "longmurders";
const PROPID_MOBILE_MURDER_COUNT_SHORT_TERM		:= "shortmurders";
const PROPID_MOBILE_MY_KEY_WORDS			:= "mykeywords";
const PROPID_MOBILE_NEED_CLOTHES			:= "NeedClothes";
const PROPID_MOBILE_NEED_EQUIPPING			:= "NeedEquipping";
const PROPID_MOBILE_NEWBIE			    	:= "newbie";
const PROPID_MOBILE_NEXT_HELLO				:= "#NextHello";
const PROPID_MOBILE_NEXT_SKILL_USE			:= "#NextSkillUse";
const PROPID_MOBILE_NO_LOOT_GENERATION		:= "noloot";
const PROPID_MOBILE_NPCTEMPLATE				:= "npctemplate";
const PROPID_MOBILE_OLD_BACKPACK			:= "OldBackpack";
const PROPID_MOBILE_ONLINE_TIMER			:= "onlinetimer";
const PROPID_MOBILE_PARTICIPATING_QUEST		:= "OnQuest";
const PROPID_MOBILE_OWL_SIGHT				:= "#OwlSight";
const PROPID_MOBILE_PAGING			    	:= "#Paging";
const PROPID_MOBILE_PERMANENTLY_RED			:= "permared";
const PROPID_MOBILE_PET_DATA				:= "PetData";
const PROPID_MOBILE_POISON_DATA				:= "PoisonedData";
const PROPID_MOBILE_POSSESSING				:= "Possessing";
const PROPID_MOBILE_POTION_LAST_DRINK_TIME	:= "#potion";
const PROPID_MOBILE_PROTECTIONS				:= "#Protections";
const PROPID_MOBILE_RACE				    := "Race";
const PROPID_MOBILE_REACTIVE_ARMOR			:= "#ReactiveArmor";
const PROPID_MOBILE_REACTIVE_ARMOR_PERMANENT	:= "PermReactiveArmor";
const PROPID_MOBILE_REPLY_FOR_OBJTYPE			:= "ReplyForObjtype";
const PROPID_MOBILE_REPLY_FOR_OBJTYPE_DONE		:= "DoneReplyForObjtype";
const PROPID_MOBILE_REPLY_FOR_OBJTYPE_PERMANENT	:= "PermReplyForObjtype";
const PROPID_MOBILE_REPLY_FOR_SERIAL			:= "ReplyForSerial";
const PROPID_MOBILE_REPLY_FOR_SERIAL_DONE		:= "DoneReplyForSerial";
const PROPID_MOBILE_REPLY_FOR_SERIAL_PERMANENT	:= "PermReplyForSerial";
const PROPID_MOBILE_REPLY_FOR_WORD			:= "ReplyForWord";
const PROPID_MOBILE_REPLY_FOR_WORD_DONE			:= "DoneReplyForWord";
const PROPID_MOBILE_REPLY_FOR_WORD_PERMANENT	:= "PermReplyForWord";
const PROPID_MOBILE_RESTOCK				    := "Restock";
const PROPID_MOBILE_RISE			    	:= "RiseAs";
const PROPID_MOBILE_RISE_DELAY				:= "RiseDelay";
const PROPID_MOBILE_SELF_SERIAL				:= "MySerial";
const PROPID_MOBILE_SHRINE_KARMA_AWARDS		:= "ShrineKarmaAwards";
const PROPID_MOBILE_SKILLCHECK_DONE			:= "#SkillCheckDone";
const PROPID_MOBILE_SKILLS_CAP_OFF			:= "CapOff";
const PROPID_MOBILE_SKILLS_MULTIPLIERS		:= "SkillMultipliers";
const PROPID_MOBILE_SKILLS_STATE			:= "SkillsState";
const PROPID_MOBILE_SNOOPING				:= "#Snooping";
const PROPID_MOBILE_SPAWNPOINT_SERIAL		:= "SpawnPoint";
const PROPID_MOBILE_STAT_DATA				:= "StatsData";
const PROPID_MOBILE_SUMMONED				:= "summoned";
const PROPID_MOBILE_TAMABLE_AT				:= "tamableAt";
const PROPID_MOBILE_TAME_TIMER				:= "tametimer";
const PROPID_MOBILE_TARGET			    	:= "Target";
const PROPID_MOBILE_TRAIN_ATTRIBUTE			:= "#TrainAttribute";
const PROPID_MOBILE_UNTAMABLE				:= "UnTamable";
const PROPID_MOBILE_USING_HOUSE_MENU		:= "#UsingHouseMenu";


// PropIDs for mortars
const PROPID_MORTAR_CONTENTS				:= "StoredPotion";


// PropIDs for multi
const PROPID_MULTI_BANNED			    	:= "Banned";
const PROPID_MULTI_BUILT_DEED				:= "BuiltDeed";
const PROPID_MULTI_COMPONENTS				:= "Components";
const PROPID_MULTI_CO_OWNERS				:= "CoOwners";
const PROPID_MULTI_FRIENDS				    := "Friends";
const PROPID_MULTI_KEYS					    := "HouseKeys";
const PROPID_MULTI_LOCKED_ITEMS				:= "LockedItems";
const PROPID_MULTI_MASTER_LOCKID			:= "MasterLockID";
const PROPID_MULTI_OWNER				    := "OwnerSerial";
const PROPID_MULTI_SECURE_CONTAINERS		:= "SecureContainers";


// PropIDs for potion kegs
const PROPID_POTIONKEG_FULLNESS				:= "PotionAmount";
const PROPID_POTIONKEG_POTION_SUB_TYPE		:= "PotionSubType";
const PROPID_POTIONKEG_POTION_TYPE			:= "PotionType";
const PROPID_POTIONKEG_SPECIALIST_CONCENTRATION		:= "SpecialistConcentration";


// PropIDs for shrines
const PROPID_SHRINE_SHRINE_ID				:= "ShrineId";


// PropIDs for shrink dolly
const PROPID_SHRINKDOLLY_COORDS_MODS		:= "CoordinatesMods";
const PROPID_SHRINKDOLLY_EQUIP_PACK			:= "EquipPack";
const PROPID_SHRINKDOLLY_LOOT_BAG			:= "LootBag";
const PROPID_SHRINKDOLLY_NPCTEMPLATE		:= "npctemplate";
const PROPID_SHRINKDOLLY_SPAWN_CHECK_DELAY	:= "SpawnCheckDelay";
const PROPID_SHRINKDOLLY_SPAWN_RUNNING		:= "Running";


// PoprIDs for skill gates
const PROPID_SKILLGATE_COLOR_MOD			:= "ColorMod";
const PROPID_SKILLGATE_CONFIRMATION_MSG		:= "ConfirmationMsg";
const PROPID_SKILLGATE_GRAPHIC_MOD			:= "GraphicMod";
const PROPID_SKILLGATE_MAX_STAT_MOD			:= "MaxStatMod";
const PROPID_SKILLGATE_NAME_MOD				:= "NameMod";
const PROPID_SKILLGATE_SKILL_LEVEL_MOD		:= "SkillLevelMod";
const PROPID_SKILLGATE_SKILL_MULTIPLIERS_MOD	:= "SkillMultipliersMod";
const PROPID_SKILLGATE_STAT_LEVEL_MOD		:= "StatLevelMod";


// PropIDs for spawnpoints
const PROPID_SPAWNPOINT_CAN_BE_DESTROYED	:= "CanBeDestroyed";
const PROPID_SPAWNPOINT_CREATION_TIME		:= "CreateTime";
const PROPID_SPAWNPOINT_NEXT_SPAWNING		:= "NextSpawn";
const PROPID_SPAWNPOINT_NEXT_VALIDATION		:= "#NextValidate";
const PROPID_SPAWNPOINT_SETTING				:= "PointData";
const PROPID_SPAWNPOINT_SPAWNED_OBJECTS		:= "Serials";


// PropIDs for spellbooks
const PROPID_SPELLBOOK_CASTING_OPTIONS		:= "CastingOptions";
const PROPID_SPELLBOOK_SPELLS				:= "Spells";


// PropIDs for tome of enchantments
const PROPID_TOME_CIRCLE1_SPELLS			:= "Circle1";
const PROPID_TOME_CIRCLE2_SPELLS			:= "Circle2";
const PROPID_TOME_CIRCLE3_SPELLS			:= "Circle3";
const PROPID_TOME_CIRCLE4_SPELLS			:= "Circle4";


// PropIDs for word of power teleporters
const PROPID_WOPTELEPORTER_OLD_GRAPHIC		:= "Old";
const PROPID_WOPTELEPORTER_REVEALED			:= "#Revealed";
const PROPID_WOPTELEPORTER_WORDS_OF_POWER	:= "WordOfPower";