#include "db/create_ng_result.h"

#include <iostream>

#include "net/protocol.h"
#include "net/connectionclosedexception.h"

using namespace net;
using namespace std;

namespace db {
	void CreateNGResult::printToConnection(MessageHandler &mh) throw(ConnectionClosedException) {
		mh.print_byte(Protocol::ANS_CREATE_NG);
		if (message != Protocol::ANS_ACK)
			mh.print_byte(Protocol::ANS_NAK);
		mh.print_byte(message);
		mh.print_byte(Protocol::ANS_END);
	}

	void CreateNGResult::toString(ostream &out) const {
		if (message == Protocol::ANS_ACK) {
			out << "Newsgroup successfully created.";
		} else {
			out << "ERROR: Newsgroup already exists.";
		}
	}
}
