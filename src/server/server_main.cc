#include <cstdlib>
#include <cstring>
#include <signal.h>

#include <iostream>
#include <sstream>

#include "db/database.h"
#include "db/file_db.h"
#include "db/memory_db.h"
#include "db/query.h"
#include "db/result.h"
#include "net/connection.h"
#include "net/messagehandler.h"
#include "server/server.h"
#include "server/server_message_interpreter.h"

using namespace server;
using namespace std;
using namespace net;
using namespace db;

const unsigned int default_port = 1025;
const string default_db = "file";
bool forever = true;
unsigned int port;

bool read_args(unsigned int &port, string &db_type, size_t argc, char **argv);

void sighandler(int sig) {
	clog << "Caught signal " << sig << "." << endl;
	forever = false;
	Connection c("localhost", port); // TODO no better way to end server.waitForActivity()?
}

int main(int argc, char **argv) {
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT, &sighandler);

	string db_type;
	if (!read_args(port, db_type, argc, argv)) {
		return EXIT_FAILURE;
	}

	clog << "Starting server on port " << port << " with a " << db_type << " database..." << endl;
	Server server(port);
	if (!server.isReady()) {
		cerr << "Server could not be initialized correctly." << endl;
		return EXIT_FAILURE;
	} else {
		cout << "Server is running." << endl;
	}

	Database *database;
	if (db_type == "memory") {
		database  = new MemoryDB();
	} else {
		database = new FileDB();
	}
	ServerMessageInterpreter interpreter;

	while (forever) {
		clog << "Waiting for activity." << endl;
		Connection *connection = server.waitForActivity();
		MessageHandler mh(*connection);
		if (connection) {
			Query *query = 0;
			Result *result = 0;
			try {
				query = interpreter.recieve_query(mh);
				clog << "Query received." << endl;

				result = query->getResult(*database);
				clog << "Query executed." << endl;
				//cout << *result; // Debug output.

				result->printToConnection(mh);
				clog << "Result sent." << endl;
			} catch (const IllegalCommandException &ice) {
				cerr << "Illegal commando from socket " << connection->getSocket() << ". Disconnecting it." << endl;
				server.deregisterConnection(connection);
				delete connection;
			} catch (const ConnectionClosedException &cce) {
				clog << "Socket " << connection->getSocket() << " disconnected." << endl;
				server.deregisterConnection(connection);
				delete connection;
			}
			delete result;
			delete query;
		} else if(forever) {
			clog << "New incoming connection." << endl;
			server.registerConnection(new Connection());
		}
	}
	cout << "Terminating..." << endl;
	delete database;
	return EXIT_SUCCESS;
}

const char *arg_usage = "Usage: $server_main [--db (memory | file)] [--port portnum]";

bool read_args(unsigned int &port, string &db_type, size_t argc, char **argv) {
	port = default_port;
	db_type = default_db;
	bool error = false;
	if (argc == 1) {
		// Use defaults.
	} else if (!(argc == 3 || argc == 5)) {
		error = true;
		cerr << arg_usage << endl;
	} else {
		for (size_t i = 1; i < argc; i += 2) {
			if (!strcmp(argv[i], "--db")) {
				if (!strcmp(argv[i + 1], "memory") || !strcmp(argv[i + 1], "file")) {
					db_type = argv[i + 1];
				} else {
					error = true;
					cerr << "The database type \"" << argv[i + 1] << "\"is not recognized." << endl;
				}
			} else if (!strcmp(argv[i], "--port")) {
				unsigned int read_port = atoi(argv[i + 1]);
				if (read_port == 0 || read_port < 1025) {
					error = true;
					cerr << "Port must be > 1024" << endl;
				} else {
					port = read_port;
				}
			} else {
				error = true;
				cerr << "Parameter \"" << argv[i] << "\" is unrecognized." << endl;
				cerr << arg_usage << endl;
			}
		}
	}
	return !error;
}
