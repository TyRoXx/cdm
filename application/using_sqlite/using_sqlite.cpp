#include "sqlite3.h"

int main()
{
	sqlite3 *db;
	if (sqlite3_open(":memory:", &db))
	{
		return 1;
	}
	sqlite3_close(db);
}
