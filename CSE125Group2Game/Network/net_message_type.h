#pragma once

enum class CustomMsgTypes : uint32_t {
  ServerAccept,
  ServerDeny,
  ServerPing,
  ClientMessage,
  MessageAll,
  ServerMessage,
};
