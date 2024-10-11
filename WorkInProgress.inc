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
	#warning "Você pode desativar avisos definindo 'DROPMAKER_DISABLE_WARNINGS'!"
#endif

#if !defined DROPMAKER_ENABLE_TAG
	#define Drop _

	#if !defined DROPMAKER_DISABLE_WARNINGS
		#warning "Você pode utilizar a tag do dropmaker definindo 'DROPMAKER_ENABLE_TAG'"
	#endif
#endif

#if !defined DROPMAKER_MAX_DROPS
	#define DROPMAKER_MAX_DROPS	(Drop:500)

	#if !defined DROPMAKER_DISABLE_WARNINGS 
		#warning "'DROPMAKER_MAX_DROPS' não foi definido pelo usuário, então por padrão será limitado a 500 drops!"
	#endif
#endif

#if !defined DROPMAKER_USE_AREA && !defined DROPMAKER_DISABLE_WARNINGS 
	#warning "'DROPMAKER_USE_AREA' não foi definido pelo usuário, então por padrão utilizaremos Checkpoints!"
#endif

#if !defined DROPMAKER_USE_OBJECTS && !defined DROPMAKER_DISABLE_WARNINGS
	#warning "'DROPMAKER_USE_OBJECTS' não foi definido pelo usuário, então por padrão não utilizaremos objetos!"
#endif

#define DROPMAKER_INVALID_DROP	(Drop:-1)

// ============= [[ INCLUDES ]] ============= //

#if !defined _streamer_included
	#include <streamer>
#endif

#if !defined DROPMAKER_DISABLE_WARNINGS && Streamer_IncludeFileVersion < 0x296
	#warning "Plugin SAMP-STREAMER desatualizado! Instale a versão recente: https://github.com/samp-incognito/samp-streamer-plugin/releases/latest"
#endif

#if !defined _FOREACH_LOCAL_VERSION && !defined _Y_ITERATE_LOCAL_VERSION
	#tryinclude <foreach>
	
	#if defined _FOREACH_LOCAL_VERSION
		#if !defined DROPMAKER_DISABLE_WARNINGS && _FOREACH_LOCAL_VERSION < 19
			#warning "Include FOREACH desatualizada! Instale a versão recente: https://github.com/Open-GTO/foreach/releases/latest"
		#endif
	#else
		#define _WITHOUT_FOREACH

		#if !defined DROPMAKER_DISABLE_WARNINGS
			#warning "Recomendo que utilize FOREACH para loops! Instale a versão recente: https://github.com/Open-GTO/foreach/releases/latest"
		#endif
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
				#warning "Plugin COLANDREAS desatualizado! Instale a versão recente: https://github.com/Pottus/ColAndreas/releases/latest"
			#endif
		#else
			#error "Instale o plugin COLANDREAS ou remova a definição 'DROPMAKER_USE_OBJECTS'!"
		#endif
	#endif
#endif


// ============== [[ MACROS ]] ============== //

#if !defined isempty
	#define isempty(%1) (%1[0] == EOS)
#endif


// =============== [[ INIT ]] =============== //

#if !defined DROPMAKER_HIGHMAP_ALREADY
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
#endif


// ============= [[ VARIABLES ]] ============ //

#if !defined _WITHOUT_FOREACH
	static Iterator:Drops<DROPMAKER_MAX_DROPS>;
#else
	static Drops[DROPMAKER_MAX_DROPS], Drop:DropCount;
	
#endif

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

static DropMakerTimer;
	

// ============= [[ PUBLICS ]] ============ //

forward DropMaker_CallbackTimer();
public DropMaker_CallbackTimer() {
	#if !defined _WITHOUT_FOREACH
		foreach(new Drop:dropid : Drops) {
	#else
		for(new Drop:dropid; dropid < DropCount; dropid++) {
	#endif
		if(gettime() > DropInfo[_:dropid][drop_time]) {
			CallLocalFunction("OnDropTimeExpires", "d", _:dropid);
			DeleteDrop(dropid);
		}
	}
	return 1;
}


// ============= [[ FORWARDS ]] ============ //

forward OnDropCreated(Drop:dropid);
forward OnDropDeleted(Drop:dropid);
forward OnDropTimeExpires(Drop:dropid);
forward OnDropCountUpdate(Drop:dropid, current, older);
forward OnDropStreamIn(Drop:dropid, forplayerid);
forward OnDropStreamOut(Drop:dropid, forplayerid);
forward OnPlayerEnterDropArea(playerid, Drop:dropid);
forward OnPlayerLeaveDropArea(playerid, Drop:dropid);

