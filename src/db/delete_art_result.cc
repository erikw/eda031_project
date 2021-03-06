#include "db/delete_art_result.h"

#include <iostream>

#include "net/connectionclosedexception.h"
#include "net/connection.h"
#include "net/protocol.h"

using namespace net;
using namespace std;

namespace db {
	void DeleteArtResult::printToConnection(MessageHandler &mh) throw(net::ConnectionClosedException) {
		mh.print_byte(Protocol::ANS_DELETE_ART);
		if (message != Protocol::ANS_ACK)
			mh.print_byte(Protocol::ANS_NAK);
		mh.print_byte(message);
		mh.print_byte(Protocol::ANS_END);
	}

	void DeleteArtResult::toString(ostream &out) const {
		if (message == Protocol::ANS_ACK) {
			out << "Article successfully deleted.";
		} else if(message == Protocol::ERR_ART_DOES_NOT_EXIST) {
			out << "ERROR: Article doesn't exist.";
		} else {
			out << "ERROR: Newsgroup doesn't exist.";
		}
	}
}

