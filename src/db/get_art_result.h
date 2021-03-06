#ifndef GETARTRESULT_H
#define GETARTRESULT_H

#include <iostream>
#include <string>

#include "db/result.h"
#include "net/messagehandler.h"
#include "net/connectionclosedexception.h"
#include "net/protocol.h"

namespace db {
	class GetArtResult : public Result {
	public:
		explicit GetArtResult(unsigned char err_msg) : message(err_msg) {}
		GetArtResult(std::string ti, std::string au, std::string te) :
				title(ti), author(au), text(te) , message(net::Protocol::ANS_ACK) {}
		void printToConnection(net::MessageHandler &mh) throw (net::ConnectionClosedException);
	private:
		void toString(std::ostream &out) const;

		std::string title;
		std::string author;
		std::string text;
		unsigned char message;
	};
}

#endif
