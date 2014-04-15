
#include "isolated.h"
#include "v8merge.h"

#include <iostream>

#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>

#include <v8.h>

using namespace std;
using namespace v8;


int main(int argc, const char* const argv[]) {
	Isolated::create();
	
	Isolated isolated;
	
	/*const char* kernel = R"(
		({
			"merge": function(key, old, value) {
				return (old||0) + value;
				
				//if (typeof value == "string")
				//	eval("var value = " + value);
			
				//if (value instanceof Function)
				//	return value(old);
				//else
				//	return value;
				
			},
			"partialMerge": function(key, left, right) {
				return left + right;
			}
		});
	)";*/
	
	const char* kernel = R"(
		({
			"name": "Counter",
			"author": "Subprotocol",
			"merge": function(key, old, value) {
				return (old||0) + value;
			},
			"partialMerge": function(key, left, right) {
				return left + right;
			}
		});
	)";
	
	/*const char* kernel = R"(
		({
			"merge": function(key, old, value) {
				return (old||0) + value;
			}
		});
	)";*/
	
	// rocksdb options
	rocksdb::Options options;
	options.merge_operator.reset(new V8MergeOperator(kernel));
	options.create_if_missing = true;
	
	// rocksdb db
	rocksdb::DB* db;
	rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
	
	rocksdb::Status s;
	
	s = db->Merge(rocksdb::WriteOptions(), "a", "1");
	
	//s = db->Merge(rocksdb::WriteOptions(), "a", "[1,2,3]");
	//s = db->Merge(rocksdb::WriteOptions(), "a", "\"function (v) { return v.concat([v.length+1]); }\"");
	//s = db->Merge(rocksdb::WriteOptions(), "a", "\"function (v) { return v.map(function (x) { return x*2; }); }\"");
	
	rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst();it->Valid();it->Next()) {
		cout << it->key().ToString() << " = " << it->value().ToString() << endl;
	}
	delete it;

	
	delete db;
		
	Isolated::release();
	
	return 0;
}
