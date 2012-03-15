#include "create_ng_result.h"
#include <string>

using namespace net;
using namespace std;

namespace db {
	CreateNGResult::printToConnection(Connection& conn){
		conn.write(Protocol::ANS_CREATE_NG);		
		if(message!=Protocol::ANS_ACK)
			conn.write(Protocol::ANS_NACK);
		conn.write(message);
		conn.write(Protocol::ANS_END);
	}
}
