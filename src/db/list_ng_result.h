#ifndef LISTNGRESULT_H
#define LISTNGRESULT_H

#include <string>
#include <vector>

#include "db/result.h"
#include "net/connectionclosedexception.h"
#include "net/messagehandler.h"

namespace db {
	class ListNGResult : public Result {
	public:
		ListNGResult(std::vector<std::pair<int, std::string> > ng) : news_groups(ng) {}
		void printToConnection(net::MessageHandler &mh) throw(net::ConnectionClosedException);
		void printToCout();
	private:
		std::vector<std::pair<int, std::string> > news_groups;
	};
}

#endif
