#include <string>
#include <vector>
#include <iostream>

#include "db/list_art_result.h"
#include "net/protocol.h"
#include "net/connectionclosedexception.h"


namespace db {

using namespace net;
using namespace std;

	void ListArtResult::printToConnection(MessageHandler &mh) throw (ConnectionClosedException){
		mh.print_byte(Protocol::ANS_LIST_ART);
		if (message != Protocol::ANS_ACK) {
			mh.print_byte(Protocol::ANS_NAK);
			mh.print_byte(message);
		} else {
			mh.print_byte(Protocol::ANS_ACK);
			mh.print_num(articles.size());

			for (vector<pair<int, string> >::iterator iter = articles.begin(); iter != articles.end(); ++iter) {
				vector<pair<int, string> >::value_type art = *iter;
				mh.print_num(art.first);
				mh.print_string(art.second);
			}
		}
		mh.print_byte(Protocol::ANS_END);
	}

	void ListArtResult::printToCout() {
		if (message != Protocol::ANS_ACK){
			cout << "ERROR: Newsgroup doesn't exist" << endl;
		} else {
			pair<int, string> current_pair;
			for (size_t i = 0; i < articles.size(); ++i) {
				current_pair = articles[i];
				cout << current_pair.first << ". " << current_pair.second << endl;
			}
		}
	}

}
