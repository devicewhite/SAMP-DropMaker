#if defined _drop_system
	#endinput
#endif
#define _drop_system

#include	<streamer>
#include	<foreach>

// ----------- LIMITS
#define MAX_DROPS		100
#define INVALID_DROP		Drop:-1
#define INVALID_ITEM		-1

// ----------- MACROS
#if !defined isempty
	#define isempty(%1) \
		(ispacked(%1) ? (%1{0} == EOS) : (%1[0] == EOS))
#endif

// ----------- CALLBACKS
forward OnPlayerAboveItemUpdate(playerid, Drop:dropid, bool:state);

// ----------- STRUCTURES
enum DROP_INFO
{
	drop_area,
	drop_time,
	drop_item,
	drop_count,
	Text3D:drop_text
};


// ----------- VARIABLES
static DropInfo[MAX_DROPS][DROP_INFO];
static Drop:PlayerDrop[MAX_PLAYERS];
static Iterator:Drop<MAX_DROPS>;
static droptimer;

// ----------- FUNCTIONS

stock Drop:CreateDrop(const text[], item, count, Float:x, Float:y, Float:z, time = 180)
{
	if(!isempty(text)) return INVALID_DROP;

	new dropid = Free_DropID();
	if(0 <= dropid < MAX_DROPS)
	{
		DropInfo[dropid][drop_text] = CreateDynamic3DTextLabel(text, -1, x, y, z, 2.5);
		DropInfo[dropid][drop_area] = CreateDynamicCP(x, y, z, 0.8);
		DropInfo[dropid][drop_time] = gettime() + time;
		DropInfo[dropid][drop_count] = count;
		DropInfo[dropid][drop_item] = item;
		Iter_Add(Drop, dropid);

		if(Iter_Count(Drop) == 1)
			droptimer = SetTimer("OnSystemDropUpdate", 1000, true);
	}
	return Drop:dropid;
}

stock bool:DeleteDrop(Drop:dropid)
{
	if(IsValidDrop(dropid))
	{
		DestroyDynamic3DTextLabel(DropInfo[_:dropid][drop_text]);
		DestroyDynamicCP(DropInfo[_:dropid][drop_area]);
		Iter_Remove(Drop, _:dropid);

		if(!Iter_Count(Drop)) KillTimer(droptimer);
		return true;
	}
	return false;
}

stock bool:IsValidDrop(Drop:dropid)
{
	return bool:Iter_Contains(Drop, _:dropid);
}

stock bool:GetDropItem(Drop:dropid, &item)
{
	if(IsValidDrop(dropid))
	{
		item = DropInfo[_:dropid][drop_item];
		return true;
	}
	else item = INVALID_ITEM;
	return false;
}

stock bool:GetDropText(Drop:dropid, dest[], len = sizeof dest)
{
	if(IsValidDrop(dropid))
	{
		GetDynamic3DTextLabelText(DropInfo[_:dropid][drop_text], dest, len);
		return true;
	}
	return false;
}

stock bool:SetDropText(Drop:dropid, const text[])
{
	if(IsValidDrop(dropid))
	{
		UpdateDynamic3DTextLabelText(DropInfo[_:dropid][drop_text], -1, text);
		return true;
	}
	return false;
}

stock bool:GetDropCount(Drop:dropid, &count)
{
	if(IsValidDrop(dropid))
	{
		count = DropInfo[_:dropid][drop_count];
		return true;
	}
	return false;
}

stock bool:GiveDropCount(Drop:dropid, count)
{
	if(IsValidDrop(dropid))
	{
		DropInfo[_:dropid][drop_count] += count;

		if(!DropInfo[_:dropid][drop_count])
			DeleteDrop(dropid);

		return true;
	}
	return false;
}

stock bool:SetDropPos(Drop:dropid, Float:x, Float:y, Float:z)
{
	if(IsValidDrop(dropid))
	{
		Streamer_SetItemPos(STREAMER_TYPE_CP, DropInfo[_:dropid][drop_area], x, y, z);
		Streamer_SetItemPos(STREAMER_TYPE_3D_TEXT_LABEL, DropInfo[_:dropid][drop_text], x, y, z);
		return true;
	}
	return false;
}

stock bool:GetDropPos(Drop:dropid, &Float:x, &Float:y, &Float:z)
{
	if(IsValidDrop(dropid))
	{
		Streamer_GetItemPos(STREAMER_TYPE_CP, DropInfo[_:dropid][drop_area], x, y, z);
		return true;
	}
	return false;
}

stock Drop:GetPlayerDropID(playerid)
{
	return PlayerDrop[playerid];
}

static stock Free_DropID() return Iter_Free(Drop);


// ----------- CALLBACKS
forward OnSystemDropUpdate();
public OnSystemDropUpdate()
{
	foreach(new dropid : Drop)
	{
		if(gettime() > DropInfo[_:dropid][drop_time])
			DeleteDrop(Drop:dropid);
	}
	return 1;
}

public OnPlayerEnterDynamicCP(playerid, STREAMER_TAG_CP:checkpointid)
{
	foreach(new dropid : Drop)
	{
		if(checkpointid == DropInfo[_:dropid][drop_area])
		{
			PlayerDrop[playerid] = Drop:dropid;
			#if defined OnPlayerAboveItemUpdate
				OnPlayerAboveItemUpdate(playerid, Drop:dropid, true);
			#endif
			break;
		}
	}

	#if defined Drop_EnterCP
		return Drop_EnterCP(playerid, checkpointid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterDynamicCP
	#undef OnPlayerEnterDynamicCP
#else
	#define _ALS_OnPlayerEnterDynamicCP
#endif
#define OnPlayerEnterDynamicCP Drop_EnterCP
#if defined Drop_EnterCP
	forward Drop_EnterCP(playerid, STREAMER_TAG_CP:checkpointid);
#endif

public OnPlayerLeaveDynamicCP(playerid, STREAMER_TAG_CP:checkpointid)
{
	if(checkpointid == DropInfo[_:PlayerDrop[playerid]][drop_area])
	{
		#if defined OnPlayerAboveItemUpdate
			OnPlayerAboveItemUpdate(playerid, dropid, false);
		#endif
		
		PlayerDrop[playerid] = INVALID_DROP;
	}

	#if defined Drop_LeaveCP
		return Drop_LeaveCP(playerid, checkpointid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerLeaveDynamicCP
	#undef OnPlayerLeaveDynamicCP
#else
	#define _ALS_OnPlayerLeaveDynamicCP
#endif
#define OnPlayerLeaveDynamicCP Drop_LeaveCP
#if defined Drop_LeaveCP
	forward Drop_LeaveCP(playerid, STREAMER_TAG_CP:checkpointid);
#endif
