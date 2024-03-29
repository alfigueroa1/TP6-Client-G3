
#include <iostream>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#define HELLO_PORT 50013
#define RESPONSETRUE(path, length, date, date30) "HTTP/1.1 200 OK \0x0D \0x0A \
Date: " ## #date ## "\0x0D \0x0A \
Location : 127.0.0.1" ## #path ##  "\0x0D \0x0A \
Cache - Control : max - age = 30  \0x0D \0x0A \
Expires : " ## #date30 ## "  \0x0D \0x0A \
Content - Length : " ## #length ## " \0x0D \0x0A \
Content - Type : text / html; charset = iso - 8859 - 1  \0x0D \0x0A"

#define RESPONSEBAD(date, date30) "HTTP/1.1 404 Not Found \0x0D \0x0A \
Date : " ## #date ## " \0x0D \0x0A \
Cache - Control : public, max - age = 30 \0x0D \0x0A \
Expires : " ## #date30 ## " \0x0D \0x0A \
Content - Length : 0 \0x0D \0x0A \
Content - Type : text / html; charset = iso - 8859 - 1 \0x0D \0x0A"

class server
{
public:
	server();
	void startConnection();
	void receiveMessage();
	void parseMessage();
	void sendMessage();
	~server();

private:
	char buf[8000];
	std::string path;
	std::string length;
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::acceptor* server_acceptor;
	boost::asio::ip::tcp::socket* socket_forServer;
};


void server::
startConnection()
{

	//Uncomment if non-blocking mode is desired
	//
	//When non-blocking mode is chosen accept operation
	//will fail with boost::asio::error::would_block
	//if there's no client inmediatelly connected when accept operation is performed.
	//Must comment blocking mode.
	//
	server_acceptor->non_blocking(true);
	boost::system::error_code error;
	do
	{
		server_acceptor->accept(*socket_forServer, error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to listen to " << HELLO_PORT << "Port " << error.message() << std::endl;

	//server_acceptor->accept(*socket_forServer);

	socket_forServer->non_blocking(true);
}

void server::
receiveMessage()
{

	size_t len;
	boost::system::error_code error;

	do
	{
		len = socket_forServer->read_some(boost::asio::buffer(buf, strlen(buf)), error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
}

void server::
parseMessage()
{
	path = "/file/path/name";
	length = "60";
}

void server::
sendMessage()
{
	std::time_t time = std::time(nullptr);
	std::string date = std::asctime(std::localtime(&time));
	time = time + 30;
	std::string date30 = std::asctime(std::localtime(&time));

	std::string message = RESPONSETRUE(path, length, date, date30);
	size_t len;
	boost::system::error_code error;

	std::cout << message << std::endl;
	/*do
	{
		len = socket_forServer->write_some(boost::asio::buffer(buf, strlen(buf)), error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;*/
}


server::
server()
{
	IO_handler = new boost::asio::io_service();
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), HELLO_PORT);

	socket_forServer = new boost::asio::ip::tcp::socket(*IO_handler);
	server_acceptor = new boost::asio::ip::tcp::acceptor(*IO_handler, ep);
}

server::
~server()
{
	server_acceptor->close();
	socket_forServer->close();
	delete server_acceptor;
	delete socket_forServer;
	delete IO_handler;
}

int
main(int argc, char* argv[])
{
	server conquering;
	std::cout << std::endl << "Start Listening on port " << HELLO_PORT << std::endl;
	//conquering.startConnection();
	//std::cout << "Somebody connected to port " << HELLO_PORT << std::endl;
	//std::cout << "Press Enter to Send Message  " << std::endl;
	//getchar();
	conquering.parseMessage();
	conquering.sendMessage();
	Sleep(50); // Le damos 50ms para que llegue el mensaje antes de cerrar el socket.

	return 0;
}