

#include "websocket_mgr.hpp"

//for stdout
#include <iostream>

using easywsclient::WebSocket;

bool websocket_manager_c::config(strmap &v_conf)
{
	conf = v_conf;
	if(utl::exists(conf,"websocket_url"))
	{
		std::cout << "websocket_url = " << conf["websocket_url"] << std::endl;
		wsp = WebSocket::from_url(conf["websocket_url"]);
	}
	return wsp;
}

void websocket_manager_c::sendLines(std::vector<std::string> &lines)
{
	if(wsp)
	if(wsp->getReadyState() != WebSocket::CLOSED) 
	{
		for(std::string cl : lines)
		{
			wsp->send(cl);
		}
	}
	
}

void websocket_manager_c::handle_messages()
{
	if(wsp)
	{
		if(wsp->getReadyState() != WebSocket::CLOSED)
		{
			//WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
			wsp->poll();
			wsp->dispatch([wsp](const std::string & message) 
			{
				std::cout << "ws_server>" << message << std::endl;
			}			);
		}
		else
		{
			wsp = 0;//kill the websocket so that it's checked later for reconnections
		}
	}
	else
	{
		ws_monitor_count++;
		if(ws_monitor_count == 100)// check every 10 s
		{
			ws_monitor_count = 0;
			if(utl::exists(conf,"websocket_url"))
			{
				std::cout << "websocket_url = " << conf["websocket_url"] << std::endl;
				wsp = WebSocket::from_url(conf["websocket_url"]);
				if(!wsp)
				{
					std::cout << "could not open websocket url" << std::endl;
					std::cout << "Continuing without websocket, will be checked later..." << std::endl;
				}
			}
		}
	}
	
}