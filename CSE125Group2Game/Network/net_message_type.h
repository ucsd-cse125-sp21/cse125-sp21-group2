#pragma once

enum class CustomMsgTypes : uint32_t {
  ServerAccept,
  ServerDeny,
  StartGame,
  EndGame,
  WaveTimer,
  ClientMessage,
  MessageAll,
  ServerMessage,
};
