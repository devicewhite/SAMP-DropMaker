/*
 * Copyright (C) 2024 DeviceBlack
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined _dropmaker_included
	#endinput
#endif
#define _dropmaker_included
#pragma library dropmaker

// ============ [[ CONSTANTS ]] ============= //

#if !defined DROPMAKER_DISABLE_WARNINGS
	#warning Você pode desativar avisos definindo 'DROPMAKER_DISABLE_WARNINGS'!
#endif

#if !defined DROPMAKER_ENABLE_TAG
	#define Drop _

	#if !defined DROPMAKER_DISABLE_WARNINGS
		#warning Você pode utilizar a tag do dropmaker definindo 'DROPMAKER_ENABLE_TAG'
	#endif
#endif

#if !defined DROPMAKER_MAX_DROPS
	#define DROPMAKER_MAX_DROPS	Drop:500

	#if !defined DROPMAKER_DISABLE_WARNINGS 
		#warning 'DROPMAKER_MAX_DROPS' não foi definido pelo usuário, então por padrão será limitado a 500!
	#endif
#endif

#if !defined DROPMAKER_RADIUS
	#define DROPMAKER_RADIUS	(Float:1.0)

	#if !defined DROPMAKER_DISABLE_WARNINGS 
		#warning 'DROPMAKER_RADIUS' não foi definido pelo usuário, então por padrão será limitado a 1.0!
	#endif
#endif

#if !defined DROPMAKER_USE_AREA && !defined DROPMAKER_DISABLE_WARNINGS 
	#warning 'DROPMAKER_USE_AREA' não foi definido pelo usuário, então por padrão utilizaremos Checkpoints!
#endif

#if !defined DROPMAKER_USE_OBJECTS && !defined DROPMAKER_DISABLE_WARNINGS
	#warning 'DROPMAKER_USE_OBJECTS' não foi definido pelo usuário, então por padrão não utilizaremos objetos!
#endif

#define DROPMAKER_INVALID_ID	(Drop:-1)

// ============= [[ INCLUDES ]] ============= //

#if !defined _streamer_included
	#include <streamer>
#endif

#if !defined _FOREACH_LOCAL_VERSION && !defined _Y_ITERATE_LOCAL_VERSION
	#tryinclude <foreach>
	
	#if defined _FOREACH_LOCAL_VERSION
		#if !defined DROPMAKER_DISABLE_WARNINGS && _FOREACH_LOCAL_VERSION < 19
			#warning Include FOREACH desatualizada! Instale a versão recente: https://github.com/Open-GTO/foreach/releases/latest
		#endif
	#else
		#error Obrigatório que utilize FOREACH para loops! Instale a versão recente: https://github.com/Open-GTO/foreach/releases/latest
	#endif
#endif

#if defined DROPMAKER_USE_OBJECTS
	#if defined _colandreas_included
		#define DROPMAKER_HIGHMAP_ALREADY
	#else
		#if !defined _colandreas_included
			#tryinclude <colandreas>
		#endif

		#if defined _colandreas_included
			#if COLANDREAS_VERSION < 10500 && !defined DROPMAKER_DISABLE_WARNINGS
				#warning Plugin COLANDREAS desatualizado! Instale a versão recente: https://github.com/Pottus/ColAndreas/releases/latest
			#endif
		#else
			#if !defined DROPMAKER_DISABLE_WARNINGS
				#warning Recomendamos que instale o plugin COLANDREAS para cálculos de altura do mapa!
			#endif
		#endif
	#endif
#endif


// ============== [[ MACROS ]] ============== //

#if !defined isempty
	#define isempty(%1) (%1[0] == EOS)
#endif


// ============= [[ VARIABLES ]] ============ //

static enum DROPMAKER_INFO {
	#if defined DROPMAKER_USE_AREA
		STREAMER_TAG_AREA:drop_area,
	#else
		STREAMER_TAG_CP:drop_area,
	#endif
	
	drop_time,
	drop_item,
	drop_count,
	STREAMER_TAG_OBJECT:drop_model,
	STREAMER_TAG_3D_TEXT_LABEL:drop_text
};

static DropMakerInfo[Drop:DROPMAKER_MAX_DROPS][DROPMAKER_INFO];
static Iterator:Drops<_:DROPMAKER_MAX_DROPS>;
static DropMakerTimer;

// ============ [[ FUNCTIONS ]] ============ //

stock Drop:CreateDrop(const text[], itemid, count, modelid, Float:x, Float:y, Float:z, virtualworld = 0, interior = 0, time = 180) {
	new Drop:dropid = Drop:Iter_Free(Drops);

	if(dropid != DROPMAKER_INVALID_ID) {
		#if defined DROPMAKER_USE_AREA
			DropMakerInfo[dropid][drop_area] = CreateDynamicSphere(x, y, z, DROPMAKER_RADIUS, virtualworld, interior);
		#else
			DropMakerInfo[dropid][drop_area] = CreateDynamicCP(x, y, z, DROPMAKER_RADIUS, virtualworld, interior);
		#endif
		
		DropMakerInfo[dropid][drop_text] = CreateDynamic3DTextLabel(text, -1, x, y, z, DROPMAKER_RADIUS, .worldid=virtualworld, .interiorid=interior);

		#if defined DROPMAKER_USE_OBJECTS
			#if defined _colandreas_included
				CA_FindZ_For2DCoord(x, y, z);
			#endif

			DropMakerInfo[dropid][drop_model] = CreateDynamicObject(modelid, x, y, z, 0.0, 0.0, 0.0, virtualworld, interior);
		#else
			DropMakerInfo[dropid][drop_model] = modelid;
		#endif

		DropMakerInfo[dropid][drop_time] = gettime() + time;
		DropMakerInfo[dropid][drop_item] = itemid;
		DropMakerInfo[dropid][drop_count] = count;
		Iter_Add(Drops, _:dropid);

		CallLocalFunction("OnDropCreated", "d", _:dropid);

		if(Iter_Count(Drops) == 1)
			DropMakerTimer = SetTimerEx("DropMaker_CallbackTimer", 1000, true);
	}

	return dropid;
}

stock bool:IsValidDrop(Drop:dropid) {
	return bool:Iter_Contains(Drops, _:dropid);
}

stock bool:DeleteDrop(Drop:dropid) {
	if(IsValidDrop(dropid)) {
		#if defined DROPMAKER_USE_AREA
			DestroyDynamicArea(DropMakerInfo[dropid][drop_area]);
		#else
			DestroyDynamicCP(DropMakerInfo[dropid][drop_area]);
		#endif
		
		#if defined DROPMAKER_USE_OBJECTS
			DestroyDynamicObject(DropMakerInfo[dropid][drop_model]);
		#endif
		
		DestroyDynamic3DTextLabel(DropMakerInfo[dropid][drop_text]);
		
		CallLocalFunction("OnDropDeleted", "d", _:dropid);
		Iter_Remove(Drops, _:dropid);
		
		if(!Iter_Count(Drops))
			KillTimer(DropMakerTimer);

		new Reset[DROPMAKER_INFO];
		DropMakerInfo[dropid] = Reset;
		return true;
	}
	return false;
}

stock bool:SetDropText(Drop:dropid, const text[]) {
	if(IsValidDrop(dropid)) {
		UpdateDynamic3DTextLabelText(DropMakerInfo[dropid][drop_text], -1, text);
		return true;
	}
	return false;
}

stock bool:GetDropText(Drop:dropid, text[], maxtext = sizeof text) {
	if(IsValidDrop(dropid)) {
		GetDynamic3DTextLabelText(DropMakerInfo[dropid][drop_text], text, maxtext);
		return true;
	}
	return false;
}

stock bool:SetDropModelID(Drop:dropid, modelid) {
	if(IsValidDrop(dropid)) {
		#if defined DROPMAKER_USE_OBJECTS
			Streamer_SetIntData(STREAMER_TYPE_OBJECT, DropMakerInfo[dropid][drop_model], E_STREAMER_MODEL_ID, modelid);
		#else
			DropMakerInfo[dropid][drop_model] = modelid;
		#endif
		return true;
	}
	return false;
}

stock bool:GetDropModelID(Drop:dropid, &modelid) {
	if(IsValidDrop(dropid)) {
		#if defined DROPMAKER_USE_OBJECTS
			modelid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, DropMakerInfo[dropid][drop_model], E_STREAMER_MODEL_ID);
		#else
			modelid = DropMakerInfo[dropid][drop_model];
		#endif
		return true;
	}
	return false;
}

stock bool:SetDropItemID(Drop:dropid, itemid) {
	if(IsValidDrop(dropid)) {
		DropMakerInfo[dropid][drop_item] = itemid;
		return true;
	}
	return false;
}

stock bool:GetDropItemID(Drop:dropid, &itemid) {
	if(IsValidDrop(dropid)) {
		itemid = DropMakerInfo[dropid][drop_item];
		return true;
	}
	return false;
}

stock bool:SetDropTime(Drop:dropid, time) {
	if(IsValidDrop(dropid)) {
		DropMakerInfo[dropid][drop_time] = gettime() + time;
		return true;
	}
	return false;
}

stock bool:GetDropTime(Drop:dropid, &time) {
	if(IsValidDrop(dropid)) {
		time = DropMakerInfo[dropid][drop_time];
		return true;
	}
	return false;
}

stock bool:GiveDropCount(Drop:dropid, count) {
	if(IsValidDrop(dropid)) {
		new older = DropMakerInfo[dropid][drop_count];
		DropMakerInfo[dropid][drop_count] += count;
		CallLocalFunction("OnDropCountUpdate", "ddd", _:dropid, DropMakerInfo[dropid][drop_count], older);
		return true;
	}
	return false;
}

stock bool:SetDropCount(Drop:dropid, count) {
	if(IsValidDrop(dropid)) {
		new older = DropMakerInfo[dropid][drop_count];
		DropMakerInfo[dropid][drop_count] = count;
		CallLocalFunction("OnDropCountUpdate", "ddd", _:dropid, DropMakerInfo[dropid][drop_count], older);
		return true;
	}
	return false;
}

stock bool:GetDropCount(Drop:dropid, &count) {
	if(IsValidDrop(dropid)) {
		count = DropMakerInfo[dropid][drop_count];
		return true;
	}
	return false;
}

stock bool:SetDropPosition(Drop:dropid, Float:x, Float:y, Float:z) {
	if(IsValidDrop(dropid)) {
		#if defined DROPMAKER_USE_AREA
			Streamer_SetFloatData(STREAMER_TYPE_AREA, DropMakerInfo[dropid][drop_area], E_STREAMER_X, x);
			Streamer_SetFloatData(STREAMER_TYPE_AREA, DropMakerInfo[dropid][drop_area], E_STREAMER_Y, y);
			Streamer_SetFloatData(STREAMER_TYPE_AREA, DropMakerInfo[dropid][drop_area], E_STREAMER_Z, z);
		#else
			Streamer_SetFloatData(STREAMER_TYPE_CP, DropMakerInfo[dropid][drop_area], E_STREAMER_X, x);
			Streamer_SetFloatData(STREAMER_TYPE_CP, DropMakerInfo[dropid][drop_area], E_STREAMER_Y, y);
			Streamer_SetFloatData(STREAMER_TYPE_CP, DropMakerInfo[dropid][drop_area], E_STREAMER_Z, z);
		#endif

		Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, DropMakerInfo[dropid][drop_text], E_STREAMER_X, x);
		Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, DropMakerInfo[dropid][drop_text], E_STREAMER_Y, y);
		Streamer_SetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, DropMakerInfo[dropid][drop_text], E_STREAMER_Z, z);

		#if defined DROPMAKER_USE_OBJECTS
			#if defined _colandreas_included
				CA_FindZ_For2DCoord(x, y, z);
			#endif

			Streamer_SetFloatData(STREAMER_TYPE_OBJECT, DropMakerInfo[dropid][drop_model], E_STREAMER_X, x);
			Streamer_SetFloatData(STREAMER_TYPE_OBJECT, DropMakerInfo[dropid][drop_model], E_STREAMER_Y, y);
			Streamer_SetFloatData(STREAMER_TYPE_OBJECT, DropMakerInfo[dropid][drop_model], E_STREAMER_Z, z);
		#endif

		foreach(new id : Player) Streamer_Update(id);
		return true;
	}
	return false;
}

stock bool:GetDropPosition(Drop:dropid, &Float:x, &Float:y, &Float:z) {
	if(IsValidDrop(dropid)) {
		x = Streamer_GetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, DropMakerInfo[dropid][drop_text], E_STREAMER_X);
		y = Streamer_GetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, DropMakerInfo[dropid][drop_text], E_STREAMER_Y);
		z = Streamer_GetFloatData(STREAMER_TYPE_3D_TEXT_LABEL, DropMakerInfo[dropid][drop_text], E_STREAMER_Z);
		return true;
	}
	return false;
}

stock bool:IsPlayerInAnyDropArea(playerid) {
	foreach(new drop : Drops) {
		#if defined DROPMAKER_USE_AREA
			if(IsPlayerInDynamicArea(playerid, DropMakerInfo[Drop:drop][drop_area])) return true;
		#else
			if(IsPlayerInDynamicCP(playerid, DropMakerInfo[Drop:drop][drop_area])) return true;
		#endif
	}
	return false;
}

stock bool:IsPlayerInDropArea(playerid, Drop:dropid) {
	#if defined DROPMAKER_USE_AREA
		if(IsPlayerInDynamicArea(playerid, DropMakerInfo[dropid][drop_area])) return true;
	#else
		if(IsPlayerInDynamicCP(playerid, DropMakerInfo[dropid][drop_area])) return true;
	#endif
	return false;
}

#if defined DROPMAKER_USE_AREA
stock bool:IsAnyPlayerInAnyDropArea(playerid) {
	foreach(new drop : Drops) {
		if(IsAnyPlayerInDynamicArea(DropMakerInfo[Drop:drop][drop_area])) return true;
	}
	return false;
}

stock bool:IsAnyPlayerInDropArea(playerid, Drop:dropid) {
	return bool:IsAnyPlayerInDynamicArea(DropMakerInfo[Drop:drop][drop_area]);
}
#endif

// ============= [[ PUBLICS ]] ============ //

#if defined _colandreas_included && !defined DROPMAKER_HIGHMAP_ALREADY
	#if defined FILTERSCRIPT
		public OnFilterScriptInit() {
	#else
		public OnGameModeInit() {
	#endif
		if(!CA_Init()) {
			print("[DropMaker -- ERRO FATAL] > Precisamos do arquivo de dados \"ColAndreas.cadb\"");
			SendRconCommand("exit");
		}
	
		#if defined DropMaker_ScriptInit
			return DropMaker_ScriptInit();
		#else
			return 1;
		#endif
	}
	#if defined FILTERSCRIPT
		#if defined ALS_OnFilterScriptInit
			#undef OnFilterScriptInit
		#else
			#define ALS_OnFilterScriptInit
		#endif
		#define DropMaker_ScriptInit OnFilterScriptInit
	#else
		#if defined ALS_OnGameModeInit
			#undef OnGameModeInit
		#else
			#define ALS_OnGameModeInit
		#endif
		#define DropMaker_ScriptInit OnGameModeInit
	#endif
	#if defined DropMaker_ScriptInit
		forward DropMaker_ScriptInit();
	#endif
#endif

forward DropMaker_CallbackTimer();
public DropMaker_CallbackTimer() {
	new time = gettime();

	foreach(new dropid : Drops) {
		if(time > DropMakerInfo[Drop:dropid][drop_time]) {
			CallLocalFunction("OnDropTimeExpires", "d", dropid);
			if(time > DropMakerInfo[Drop:dropid][drop_time]) DeleteDrop(Drop:dropid);
		}
	}
	return 1;
}

public Streamer_OnItemStreamIn(type, STREAMER_ALL_TAGS:id, forplayerid) {
	#if defined DROPMAKER_USE_AREA
		if(type == STREAMER_TYPE_AREA) {
	#else
		if(type == STREAMER_TYPE_CP) {
	#endif
		new Drop:dropid = DROPMAKER_INVALID_ID;
		
		foreach(new drop : Drops) {
			if(_:DropMakerInfo[Drop:drop][drop_area] == _:id) {
				dropid = drop;
				break;
			}
		}
		
		if(dropid != DROPMAKER_INVALID_ID)
			CallLocalFunction("OnDropStreamIn", "dd", _:dropid, forplayerid);
	}
	
	#if defined DropMaker_StreamIn
		return DropMaker_StreamIn(type, STREAMER_ALL_TAGS:id, forplayerid);
	#else
		return 1;
	#endif
}
#if defined ALS_Streamer_OnItemStreamIn
	#undef Streamer_OnItemStreamIn
#else
	#define ALS_Streamer_OnItemStreamIn
#endif
#define DropMaker_StreamIn Streamer_OnItemStreamIn
#if defined DropMaker_StreamIn
	forward DropMaker_StreamIn(type, STREAMER_ALL_TAGS:id, forplayerid);
#endif

public Streamer_OnItemStreamOut(type, STREAMER_ALL_TAGS:id, forplayerid) {
	#if defined DROPMAKER_USE_AREA
		if(type == STREAMER_TYPE_AREA) {
	#else
		if(type == STREAMER_TYPE_CP) {
	#endif
		new Drop:dropid = DROPMAKER_INVALID_ID;
		
		foreach(new drop : Drops) {
			if(_:DropMakerInfo[Drop:drop][drop_area] == _:id) {
				dropid = drop;
				break;
			}
		}
		
		if(dropid != DROPMAKER_INVALID_ID)
			CallLocalFunction("OnDropStreamOut", "dd", _:dropid, forplayerid);
	}
	
	#if defined DropMaker_StreamOut
		return DropMaker_StreamOut(type, STREAMER_ALL_TAGS:id, forplayerid);
	#else
		return 1;
	#endif
}
#if defined ALS_Streamer_OnItemStreamOut
	#undef Streamer_OnItemStreamOut
#else
	#define ALS_Streamer_OnItemStreamOut
#endif
#define DropMaker_StreamOut Streamer_OnItemStreamOut
#if defined DropMaker_StreamOut
	forward DropMaker_StreamOut(type, STREAMER_ALL_TAGS:id, forplayerid);
#endif

#if defined DROPMAKER_USE_AREA
	public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
		new Drop:dropid = DROPMAKER_INVALID_ID;
			
		foreach(new drop : Drops) {
			if(DropMakerInfo[Drop:drop][drop_area] == areaid) {
				dropid = drop;
				break;
			}
		}
			
		if(dropid != DROPMAKER_INVALID_ID)
			CallLocalFunction("OnPlayerEnterDropArea", "dd", playerid, _:dropid);

		#if defined DropMaker_EnterArea
			return DropMaker_EnterArea(playerid, areaid);
		#else
			return 1;
		#endif
	}
	#if defined ALS_OnPlayerEnterDynamicArea
		#undef OnPlayerEnterDynamicArea
	#else
		#define ALS_OnPlayerEnterDynamicArea
	#endif
	#define DropMaker_EnterArea OnPlayerEnterDynamicArea
	#if defined DropMaker_EnterArea
		forward DropMaker_EnterArea(playerid, STREAMER_TAG_AREA:areaid);
	#endif

	public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
		new Drop:dropid = DROPMAKER_INVALID_ID;
			
		foreach(new drop : Drops) {
			if(DropMakerInfo[Drop:drop][drop_area] == areaid) {
				dropid = drop;
				break;
			}
		}
			
		if(dropid != DROPMAKER_INVALID_ID)
			CallLocalFunction("OnPlayerLeaveDropArea", "dd", playerid, _:dropid);

		#if defined DropMaker_LeaveArea
			return DropMaker_LeaveArea(playerid, areaid);
		#else
			return 1;
		#endif
	}
	#if defined ALS_OnPlayerLeaveDynamicArea
		#undef OnPlayerLeaveDynamicArea
	#else
		#define ALS_OnPlayerLeaveDynamicArea
	#endif
	#define DropMaker_LeaveArea OnPlayerLeaveDynamicArea
	#if defined DropMaker_LeaveArea
		forward DropMaker_LeaveArea(playerid, STREAMER_TAG_AREA:areaid);
	#endif
#else
	public OnPlayerEnterDynamicCP(playerid, STREAMER_TAG_CP:checkpointid) {
		new Drop:dropid = DROPMAKER_INVALID_ID;
		
		foreach(new drop : Drops) {
			if(DropMakerInfo[Drop:drop][drop_area] == checkpointid) {
				dropid = drop;
				break;
			}
		}
		
		if(dropid != DROPMAKER_INVALID_ID)
			CallLocalFunction("OnPlayerEnterDropArea", "dd", playerid, _:dropid);
		
		#if defined DropMaker_LeaveArea
			return DropMaker_LeaveArea(playerid, checkpointid);
		#else
			return 1;
		#endif
	}
	#if defined ALS_OnPlayerEnterDynamicCP
		#undef OnPlayerEnterDynamicCP
	#else
		#define ALS_OnPlayerEnterDynamicCP
	#endif
	#define DropMaker_EnterArea OnPlayerEnterDynamicCP
	#if defined DropMaker_EnterArea
		forward DropMaker_EnterArea(playerid, STREAMER_TAG_CP:checkpointid);
	#endif
	
	public OnPlayerLeaveDynamicCP(playerid, STREAMER_TAG_CP:checkpointid) {
		new Drop:dropid = DROPMAKER_INVALID_ID;
		
		foreach(new drop : Drops) {
			if(DropMakerInfo[Drop:drop][drop_area] == checkpointid) {
				dropid = drop;
				break;
			}
		}
		
		if(dropid != DROPMAKER_INVALID_ID)
			CallLocalFunction("OnPlayerLeaveDropArea", "dd", playerid, _:dropid);

		#if defined DropMaker_LeaveArea
			return DropMaker_LeaveArea(playerid, checkpointid);
		#else
			return 1;
		#endif
	}
	#if defined ALS_OnPlayerLeaveDynamicCP
		#undef OnPlayerLeaveDynamicCP
	#else
		#define ALS_OnPlayerLeaveDynamicCP
	#endif
	#define DropMaker_LeaveArea OnPlayerLeaveDynamicCP
	#if defined DropMaker_LeaveArea
		forward DropMaker_LeaveArea(playerid, STREAMER_TAG_CP:checkpointid);
	#endif
#endif

// ============= [[ FORWARDS ]] ============ //

forward OnDropCreated(Drop:dropid);
forward OnDropDeleted(Drop:dropid);
forward OnDropTimeExpires(Drop:dropid);
forward OnDropCountUpdate(Drop:dropid, current, older);
forward OnDropStreamIn(Drop:dropid, forplayerid);
forward OnDropStreamOut(Drop:dropid, forplayerid);
forward OnPlayerEnterDropArea(playerid, Drop:dropid);
forward OnPlayerLeaveDropArea(playerid, Drop:dropid);

