#if defined _drop_system
	#endinput
#endif
#define _drop_system

#define AUTHOR	"DeviceBlack"

/*
native Drop:CreateDrop(const text[], item, count, Float:x, Float:y, Float:z, time = 180);
native DeleteDrop(Drop:dropid);
native IsValidDrop(Drop:dropid);
native GetDropItem(Drop:dropid);
native GetDropText(Drop:dropid, dest[], len = sizeof dest);
native SetDropText(Drop:dropid, const text[]);
native GetDropCount(Drop:dropid);
native GiveDropCount(Drop:dropid, count);
native Drop:GetPlayerDropID(playerid);
*/

#include	<streamer>
#include	<foreach>

#define MAX_DROPS		100
#define INVALID_DROP	Drop:-1

enum DROP_INFO
{
	drop_area,
	drop_time,
	drop_item,
	drop_count,
	Text3D:drop_text
};

static DropInfo[MAX_DROPS][DROP_INFO];
static Drop:PlayerDrop[MAX_PLAYERS];
static Iterator:Drop<MAX_DROPS>;
static droptimer;

// ----------- FUNCTIONS

stock Drop:CreateDrop(const text[], item, count, Float:x, Float:y, Float:z, time = 180)
{
	if(!strlen(text) || !count) return INVALID_DROP;

	static dropid;
	if((dropid = Free_DropID()) != -1)
	{
		DropInfo[dropid][drop_text] = CreateDynamic3DTextLabel(text, -1, x, y, z, 2.5);
		DropInfo[dropid][drop_area] = CreateDynamicCP(x, y, z, 0.8);
		DropInfo[dropid][drop_time] = gettime() + time;
		DropInfo[dropid][drop_count] = count;
		DropInfo[dropid][drop_item] = item;
		Iter_Add(Drop, dropid);

		if(Iter_Count(Drop) == 1) droptimer = SetTimer("OnSysteDropUpdate", 1000, 1);
	}
	return Drop:dropid;
}

stock DeleteDrop(Drop:dropid)
{
	if(IsValidDrop(dropid))
	{
		DestroyDynamic3DTextLabel(DropInfo[_:dropid][drop_text]);
		DestroyDynamicCP(DropInfo[_:dropid][drop_area]);
		Iter_Remove(Drop, _:dropid);

		if(Iter_Count(Drop) == 0) KillTimer(droptimer);
	}
	else return 0;
	return 1;
}

stock IsValidDrop(Drop:dropid)
{
	return Iter_Contains(Drop, _:dropid);
}

stock GetDropItem(Drop:dropid)
{
	if(IsValidDrop(dropid))
		return DropInfo[_:dropid][drop_item];

	return 0;
}

stock GetDropText(Drop:dropid, dest[], len = sizeof dest)
{
	if(IsValidDrop(dropid))
		return GetDynamic3DTextLabelText(DropInfo[_:dropid][drop_text], dest, len);

	return 0;
}

stock SetDropText(Drop:dropid, const text[])
{
	if(IsValidDrop(dropid))
		return UpdateDynamic3DTextLabelText(DropInfo[_:dropid][drop_text], -1, text);

	return 0;
}

stock GetDropCount(Drop:dropid)
{
	if(IsValidDrop(dropid))
		return DropInfo[_:dropid][drop_count];

	return 0;
}

stock GiveDropCount(Drop:dropid, count)
{
	if(IsValidDrop(dropid))
	{
		DropInfo[_:dropid][drop_count] += count;

		if(DropInfo[_:dropid][drop_count] == 0)
		{
			DeleteDrop(dropid);
		}
		else if(!DropInfo[_:dropid][drop_count])
		{
			DropInfo[_:dropid][drop_count] -= count;
			return 0;
		}
	}
	else return 0;
	return 1;
}

stock Drop:GetPlayerDropID(playerid)
{
	return PlayerDrop[playerid];
}

static stock Free_DropID()
{
	return Iter_Free(Drop);
}


// ----------- CALLBACKS

forward void:OnSysteDropUpdate();
public void:OnSysteDropUpdate()
{
	static dropid;
	foreach(dropid : Drop)
	{
		if(gettime() > DropInfo[_:dropid][drop_time])
			DeleteDrop(Drop:dropid);
	}
}

public OnPlayerEnterDynamicCP(playerid, STREAMER_TAG_CP:checkpointid)
{
	static dropid;
	foreach(dropid : Drop)
	{
		if(checkpointid == DropInfo[_:dropid][drop_area])
		{
			PlayerDrop[playerid] = Drop:dropid;
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
	if(checkpointid == _:PlayerDrop[playerid])
		PlayerDrop[playerid] = INVALID_DROP;

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
