#include <bits/stdc++.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <fstream>
#include "leveldb/db.h"

int FPSIZE = 6;
 
using namespace std;

int insertTypeDB(string typeName, int chunkNum);
int insertSFDB(string size, string chunkHash);
int insertFCDB(string chunkHash);


// data counter--------------------

uint64_t totalLogicChunkNumber = 0;

// store DBs-----------------------
leveldb::DB *FCdb; //C chunk
leveldb::DB *SFdb; //M chunk
leveldb::DB *Tdb; //type 

//init leveldb======================================================
int initDB(char *db1, char *db2, char *db3) {

    leveldb::Options options;
    options.create_if_missing = true;

	leveldb::Status status = leveldb::DB::Open(options, db1, &FCdb);
    assert(status.ok());
    assert(FCdb != NULL);

	status = leveldb::DB::Open(options, db2, &SFdb);
    assert(status.ok());
    assert(SFdb != NULL);

	status = leveldb::DB::Open(options, db3, &Tdb);
    assert(status.ok());
    assert(Tdb != NULL);
}

int readInChunks(FILE *fp, FILE *typeFp) {

	char read_buffer[256];
    char typeBuffer[256];
    char typeNumBuffer[256];
	char *item;
    
	fgets(read_buffer, 256, fp);// skip title line
    while (fgets(typeBuffer,256,typeFp)) {


        fgets(typeNumBuffer, 256, typeFp);
        char *tmp;
        tmp = strtok(typeNumBuffer, ":\t\n ");
        int chunkNumber = atoi((const char*)tmp);
        //cout<<chunkNumber<<endl;
        string typeName(typeBuffer);  
		cout<<typeName<<" "<<chunkNumber<<endl;
        char typeHash[FPSIZE];
   		unsigned char md5full[64];
    	MD5((unsigned char*)typeBuffer, FPSIZE, md5full);
    	memcpy(typeHash, md5full, FPSIZE);
        string typeHashStr(typeHash);
		insertTypeDB(typeHashStr, chunkNumber);
        for (uint64_t i = 0; i < chunkNumber; i++) {
            
			fgets(read_buffer, 256, fp);
            totalLogicChunkNumber++;
		    //cout<<totalLogicChunkNumber<<endl;
		    char hash[FPSIZE];
		    memset(hash, 0, FPSIZE);
		    item = strtok(read_buffer, ":\t\n ");
		    int index = 0;
	    	while (item != NULL && index < FPSIZE){
			    hash[index++] = strtol(item, NULL, 16);
			    item = strtok(NULL, ":\t\n");
		    }

		    string chunkHash(hash,FPSIZE);
            string inputHash = chunkHash + typeHashStr;

		    uint64_t size = atoi((const char*)item);   //string-->int
		    string sizeStr = to_string(size);
		    if (insertFCDB(inputHash) == 1) {
			    insertSFDB(sizeStr, chunkHash);
		    }
        }
    }
}


int insertFCDB(string key) {

	string exs="";
	uint64_t count;
    leveldb::Status status = FCdb->Get(leveldb::ReadOptions(),key,&exs);
    if (status.ok() == 0) {
		count = 1;
		string countInsert = "1";
        status = FCdb->Put(leveldb::WriteOptions(),key,countInsert); 
		return 1;
    }
	else {
		count = atoi((const char*)exs.c_str());
		count++;
		string countInsert = to_string(count);
		status = FCdb->Put(leveldb::WriteOptions(),key,countInsert); 
		return count;
	}
}

int insertTypeDB(string typeName, int chunkNum) {

	string exs="";
	int count;
    leveldb::Status status = Tdb->Get(leveldb::ReadOptions(),typeName,&exs);
    if (status.ok() == 0) {
		string countInsert = to_string(chunkNum);
        status = Tdb->Put(leveldb::WriteOptions(),typeName,countInsert); 
		return 1;
    }
	else {
		count = atoi((const char*)exs.c_str());
		count += chunkNum;
		string countInsert = to_string(count);
		status = Tdb->Put(leveldb::WriteOptions(),typeName,countInsert); 
		return count;
	}
}

int insertSFDB(string size, string key) {

	string exs="";
    leveldb::Status status = SFdb->Get(leveldb::ReadOptions(),size,&exs);
    if (status.ok() == 0) {

        status = SFdb->Put(leveldb::WriteOptions(),size,key); 
		return 1;
    }
	else {
		string insertKey = exs + key;
		if (insertKey.size() % 6 != 0) {
			cout<<"insert key error"<<endl;
			exit(0);
		}
		status = SFdb->Put(leveldb::WriteOptions(), size, insertKey);
		return -1; 
	}
}


int main (int argc, char *argv[]){

	assert(argc >= 5); 
	//filename CFCdb CSFdb MFCdb MSFdb

	initDB(argv[3], argv[4],argv[5]);
	FILE *fp = NULL;
    FILE *typeFp = NULL;
	fp = fopen(argv[1], "r");
    typeFp = fopen(argv[2],"r");
    assert(typeFp != NULL);
	assert(fp != NULL);
	readInChunks(fp,typeFp);
	cout<<"totalLogicChunkNumber "<<totalLogicChunkNumber<<endl;

	return 0;
}
