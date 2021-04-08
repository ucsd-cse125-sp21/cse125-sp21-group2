#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include "olc_net.h"

class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort, uint16_t server_tick_ms) : olc::net::server_interface<CustomMsgTypes>(nPort)
	{
		tick_ms = server_tick_ms;
	}

	DWORD GetServerTick() 
	{
		return tick_ms;
	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetID() << "]: Message All\n";

			// Construct a new message and send it to all clients
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::ServerMessage;
			msg << client->GetID();
			MessageAllClients(msg, client);

		}
		break;
		}
	}

	DWORD tick_ms;


};

int main()
{
	DWORD before, after, diff;
	CustomServer server(60000, 10000);	// currently the server polls once per second
	server.Start();

	while (1)
	{
		before = GetTickCount();
		server.Update(-1, true);
		after = GetTickCount();

		std::cout << diff;
		if (server.GetServerTick() > diff) {	// need to ensure that server tick is big enough 
			Sleep(server.GetServerTick() - diff);
		}
	}

	return 0;
}