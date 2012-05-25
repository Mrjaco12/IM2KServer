#include <stdio.h>
#include <mongo.h>

int main() {

	mongo conn[1];
	bson b[1];

	printf("I am totally working \n");

	int status = mongo_connect( conn, "127.0.0.1", 27017);

	printf("Status = %d\n", status);

	if( status != MONGO_OK) {
		switch ( conn->err ) {
			case MONGO_CONN_SUCCESS: printf("connection succeeded\n"); break;
			case MONGO_CONN_NO_SOCKET: printf("no socket\n"); return 1;
			case MONGO_CONN_FAIL: printf("connection failed\n"); return 1;
			case MONGO_CONN_NOT_MASTER: printf("not master\n"); return 1;
		}
	}

	bson_init(b);
	bson_append_new_oid(b, "_id");
	bson_append_string(b, "name", "Hey Joe");
	bson_append_string(b, "RIAA", "Double Diamond");
	bson_append_int(b, "sales", 5);
	bson_finish(b);

	mongo_insert(conn, "test.data", b, 1);

	//Destroy objects
	bson_destroy(b);
	mongo_destroy(conn);

	printf("I am worked!\n");

	return 0;
}
