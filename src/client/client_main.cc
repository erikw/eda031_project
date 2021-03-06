#include <cstdlib>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "client/client_input_interpreter.h"
#include "client/client_message_interpreter.h"
#include "net/connection.h"
#include "net/messagehandler.h"
#include "db/query.h"
#include "db/result.h"

using namespace db;
using namespace std;
using namespace net;
using namespace client;

const unsigned int default_port = 1025;
const string default_host = "localhost";
bool forever = true;

bool read_args(string &host, unsigned int &port, size_t argc, char **argv);

int main(int argc, char** argv) {

	string host;
	unsigned int port;
	if (!read_args(host, port, argc, argv)) {
		return EXIT_FAILURE;
	}

	clog << "Connecting to " << host << ":" << port << "... ";
	Connection conn(host.c_str(), port);
	if (! conn.isConnected()) {
		cerr << "failed!" << endl;
		return EXIT_FAILURE;
	} else {
		cout << "succeed." << endl;
	}

	cout << "Type 'help' for a list of commands." << endl;
	MessageHandler m_handler(conn);
	ClientInputInterpreter input_interpret;
	ClientMessageInterpreter message_interpret;
	while (forever) {
		cout << "news> ";
		string line;
		getline(cin,line);
		Query *query = 0;
		Result *result = 0;
		try {
			if (cin.eof()) {
				cout << endl;
				forever = false;
				query = 0;	
			} else if (line.compare("exit") == 0 || line.compare("q") == 0) {
				forever = false;
				query = 0;	
			} else {
				query = input_interpret.recieve_query(line);
			}	

			if (query != 0) {
				query->send(m_handler);
				result = message_interpret.recieve_result(m_handler);
				cout << *result << endl;
			}
		} catch (InputSyntaxError &ise) {

		} catch (ConnectionClosedException &cce) {
			cerr << "Connection closed by server." << endl;
			return EXIT_FAILURE;	
		}
		delete query;
		delete result;

	}
	return EXIT_SUCCESS;
}

const char *arg_usage = "Usage: $server_main [--db (memory | file)] [--port portnum]";

bool read_args(string &host, unsigned int &port, size_t argc, char **argv) {
	port = default_port;
	host = default_host;
	bool error = false;
	if (argc == 1) {
		// Use defaults.
	} else if (!(argc == 3 || argc == 5)) {
		error = true;
		cerr << arg_usage << endl;
	} else {
		for (size_t i = 1; i < argc; i += 2) {
			if (!strcmp(argv[i], "--host")) {
				host = argv[i + 1];
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
