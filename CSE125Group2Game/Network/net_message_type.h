#pragma once

enum class CustomMsgTypes : uint32_t {
  ServerAccept,
  ServerDeny,
  StartGame,
  FriendlyFire,
  WaitingForPlayers,
  EndGame,
  WaveTimer,
  ClientMessage,
  MessageAll,
  ServerMessage,
};
