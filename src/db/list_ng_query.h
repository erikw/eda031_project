#ifndef LISTNGQUERY_H
#define LISTNGQUERY_H

#include "query.h"

namespace db {
	class ListNGQuery : public Query {
	public:
		ListNGQuery(Database &db) : Query(db) {}
		Result *execute();
	};

}

#endif