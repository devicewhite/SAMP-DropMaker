# DropMaker Include (dropmaker.inc)

#### Versão: 1.0
#### Plataforma: SA-MP / Open.MP
#### Autor: DeviceBlack

---

## Descrição

A DropMaker é uma include poderosa e simplificada para criar e gerenciar objetos "Drop" no SA-MP / Open.MP. Um "Drop" representa itens que podem ser deixados no mapa, associados a um modelo 3D e posições específicas, permitindo interações como coleta e exibição de informações.

Esta include facilita a criação, manipulação e monitoramento de "Drops" em tempo real, incluindo sistemas de contagem de itens, tempo de vida e textos descritivos.


## Instalação

1. Baixe o arquivo dropmaker.inc e adicione à sua pasta `pawno/include` ou `qawno/include` ou `/sdcard/Termux-Pawn`

2. Em seu script principal (.pwn), inclua o arquivo:

```pwn
#include <dropmaker>
```

3. Compile e execute o seu servidor.


## Funções e Eventos

#### Callbacks

Estes eventos são disparados automaticamente conforme as interações com os Drops acontecem no servidor.

```pwn
forward OnDropCreated(Drop:dropid);
```

Disparado quando um novo drop é criado.


```pwn
forward OnDropDeleted(Drop:dropid);
```

Disparado quando um drop é deletado.


```pwn
forward OnDropTimeExpires(Drop:dropid);
```

Disparado quando o tempo de vida de um drop se esgota.


```pwn
forward OnDropCountUpdate(Drop:dropid, current, older);
```

Disparado quando a contagem de itens no drop é atualizada.


```pwn
forward OnDropStreamIn(Drop:dropid, forplayerid);
```

Disparado quando um drop entra no campo de visão de um jogador.


```pwn
forward OnDropStreamOut(Drop:dropid, forplayerid);
```

Disparado quando um drop sai do campo de visão de um jogador.


```pwn
forward OnPlayerEnterDropArea(playerid, Drop:dropid);
```

Disparado quando um jogador entra na área de um drop.


```pwn
forward OnPlayerLeaveDropArea(playerid, Drop:dropid);
```

Disparado quando um jogador sai da área de um drop.


#### Funções

Estas funções permitem criar, editar e verificar drops no servidor.


```pwn
bool:IsAnyPlayerInDropArea(Drop:dropid)
```
Verifica se algum jogador está na área de um determinado drop.


```pwn
bool:IsAnyPlayerInAnyDropArea()
```

Verifica se algum jogador está em qualquer área de drop.


```pwn
bool:IsPlayerInDropArea(playerid, Drop:dropid)
```

Verifica se um jogador específico está na área de um determinado drop.


```pwn
bool:IsPlayerInAnyDropArea(playerid)
```

Verifica se um jogador específico está em qualquer área de drop.


```pwn
bool:GetDropPosition(Drop:dropid, &Float:x, &Float:y, &Float:z)
```
Obtém a posição de um drop.


```pwn
bool:SetDropPosition(Drop:dropid, Float:x, Float:y, Float:z)
```

Define a posição de um drop.


```pwn
bool:GetDropCount(Drop:dropid, &count)
```

Obtém a quantidade de itens em um drop.


```pwn
bool:SetDropCount(Drop:dropid, count)
```

Define a quantidade de itens em um drop.


```pwn
bool:GiveDropCount(Drop:dropid, count)
```

Adiciona uma quantidade de itens ao drop.

```pwn
bool:GetDropTime(Drop:dropid, &time)
```

Obtém o tempo restante de um drop.


```pwn
bool:SetDropTime(Drop:dropid, time)
```

Define o tempo de vida de um drop.


```pwn
bool:GetDropItemID(Drop:dropid, &itemid)
```

Obtém o ID do item de um drop.


```pwn
bool:SetDropItemID(Drop:dropid, itemid)
```

Define o ID do item de um drop.


```pwn
bool:GetDropModelID(Drop:dropid, &modelid)
```

Obtém o ID do modelo 3D de um drop.


```pwn
bool:SetDropModelID(Drop:dropid, modelid)
```

Define o ID do modelo 3D de um drop.


```pwn
bool:GetDropText(Drop:dropid, text[], maxtext = sizeof text)
```

Obtém o texto descritivo de um drop.


```pwn
bool:SetDropText(Drop:dropid, const text[])
```

Define o texto descritivo de um drop.


```pwn
bool:DeleteDrop(Drop:dropid)
```

Deleta um drop específico.


```pwn
bool:IsValidDrop(Drop:dropid)
```

Verifica se um drop é válido (se existe).


```pwn
Drop:CreateDrop(const text[], itemid, count, modelid, Float:x, Float:y, Float:z, virtualworld = 0, interior = 0, time = 180)
```

Cria um novo drop no mapa.

---

#### Exemplo de Uso

```pwn
public OnGameModeInit()
{
    // Criando um drop no mapa
    new Drop:drop = CreateDrop("Arma Especial", 24, 1, 355, 100.0, 200.0, 10.0);

    // Definindo algumas propriedades adicionais
    SetDropTime(drop, 300); // Drop expira em 300 segundos
    SetDropText(drop, "Este drop contém uma arma rara.");

    return 1;
}

public OnPlayerEnterDropArea(playerid, Drop:dropid)
{
    SendClientMessage(playerid, -1, "Você entrou na área de um drop!");
    return 1;
}
```

---

Licença

Este include é de uso livre para servidores **SA-MP / Open.MP**. Sinta-se à vontade para modificar e compartilhar.
