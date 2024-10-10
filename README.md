# SAMP-DropMaker
Easily create drops with expiry time!

```pwn
native Drop:CreateDrop(const text[], item, count, Float:x, Float:y, Float:z, time = 180);
native bool:DeleteDrop(Drop:dropid);
native bool:IsValidDrop(Drop:dropid);
native bool:GetDropItem(Drop:dropid, &item);
native bool:GetDropText(Drop:dropid, dest[], len = sizeof dest);
native bool:SetDropText(Drop:dropid, const text[]);
native bool:GetDropCount(Drop:dropid, &count);
native bool:GiveDropCount(Drop:dropid, count);
native bool:SetDropPos(Drop:dropid, Float:x, Float:y, Float:z);
native bool:GetDropPos(Drop:dropid, &Float:x, &Float:y, &Float:z);
native Drop:GetPlayerDropID(playerid);

forward OnPlayerAboveItem(playerid, Drop:dropid);
forward OnPlayerFarFromItem(playerid, Drop:dropid);
```
```
Permite criar "drops" com tempo de validade
É nescessário que você tenha um sistema de inventário funcionando para que possa ser implementado sem problemas
```
<b>Autor:</b> DeviceBlack
