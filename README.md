# SAMP-DropMaker
Easily create drops with expiry time!

```pwn
native Drop:CreateDrop(const text[], item, count, Float:x, Float:y, Float:z, time = 180);
native DeleteDrop(Drop:dropid);
native IsValidDrop(Drop:dropid);
native GetDropItem(Drop:dropid);
native GetDropText(Drop:dropid, dest[], len = sizeof dest);
native SetDropText(Drop:dropid, const text[]);
native GetDropCount(Drop:dropid);
native GiveDropCount(Drop:dropid, count);
native Drop:GetPlayerDropID(playerid);
```
```
Permite criar "drops" com tempo de validade
É nescessário que você tenha um sistema de inventário funcionando para que possa ser implementado sem problemas
```
<b>Autor:</b> DeviceBlack
