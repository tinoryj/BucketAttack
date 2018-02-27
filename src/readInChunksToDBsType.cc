#include <bits/stdc++.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <fstream>
#include "leveldb/db.h"

int FPSIZE = 6;
 
using namespace std;

uint64_t insertSFDB(string size, string chunkHash);
uint64_t insertFCDB(string chunkHash);



struct typeNode {
    string type;
    uint64_t count = 0;
    friend bool operator < (struct typeNode const &a,struct typeNode const &b) {  
        if (a.type == b.type) {  
            return false;  
        }       
    }  
};
set<typeNode> typeSet;
// data counter--------------------

uint64_t totalLogicChunkNumber = 0;

// store DBs-----------------------
leveldb::DB *FCdb; //C chunk
leveldb::DB *SFdb; //M chunk

//init leveldb======================================================
int initDB(char *db1, char *db2) {

    leveldb::Options options;
    options.create_if_missing = true;

	leveldb::Status status = leveldb::DB::Open(options, db1, &FCdb);
    assert(status.ok());
    assert(FCdb != NULL);

	status = leveldb::DB::Open(options, db2, &SFdb);
    assert(status.ok());
    assert(SFdb != NULL);
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
        tmp = strtok(typeBuffer, ":\t\n ");
        uint64_t chunkNumber = atoi((const char*)tmp);
        string typeName(typeBuffer);  
		
		typeNode newNode;
		newNode.type = chunkNumber;

		set<typeNode>::iterator it;
        it = typeSet.find(newNode);  

        if(it == typeSet.end()) {
			typeSet.insert(newNode);
		}   
		else {
			it.count += chunkNumber;
		}
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

            char typeHash[FPSIZE];
   		    unsigned char md5full[64];
    	    MD5((unsigned char*)typeBuffer, FPSIZE, md5full);
    	    memcpy(typpeHash, md5full, FPSIZE);

		    string chunkHash(hash,FPSIZE);
            string inputHash = chunkHash + typeHash;

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
        status = MFCdb->Put(leveldb::WriteOptions(),key,countInsert); 
		return 1;
    }
	else {
		count = atoi((const char*)exs.c_str());
		count++;
		string countInsert = to_string(count);
		status = MFCdb->Put(leveldb::WriteOptions(),key,countInsert); 
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

	assert(argc >= 4); 
	//filename CFCdb CSFdb MFCdb MSFdb

	initDB(argv[3], argv[4]);
	FILE *fp = NULL;
    FILE *typeFp = NULL;
	fp = fopen(argv[1], "r");
    typeFp = fopen(argv[2],"r");
    assert(typeFp != NULL);
	assert(fp != NULL);
	readInChunks(fp);
	cout<<"totalLogicChunkNumber "<<totalLogicChunkNumber<<endl;

	return 0;
}
