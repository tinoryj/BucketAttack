#include <bits/stdc++.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <fstream>
#include "leveldb/db.h"

int FPSIZE = 6;
 
using namespace std;

int insertSFDB(string size, string chunkHash);
int insertFCDB(string chunkHash);

struct typeNode {
    string type;
    uint64_t count = 0;
    friend bool operator () (struct typeNode const &a,struct typeNode const &b) {  
        if (strcmp(a.type.c_str(),b.type.c_str()) == 0) {  
            return false;  
        }       
        else {
            return a > b;      //降序    
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
    int cnt = 0;
    while (fgets(typeBuffer,256,typeFp)) {

        cnt++;
        cout<<cnt<<endl;

        fgets(typeNumBuffer, 256, typeFp);
        char *tmp;
        tmp = strtok(typeNumBuffer, ":\t\n ");
        uint64_t chunkNumber = atoi((const char*)tmp);
        //cout<<chunkNumber<<endl;
        string typeName(typeBuffer);  
		
		typeNode newNode;
        newNode.type = typeName;
		newNode.count = chunkNumber;

		set<typeNode>::iterator it;
        it = typeSet.find(newNode);  

        if(it-- == typeSet.end()) {
			typeSet.insert(newNode);
		}   
		else {
            cout<<it->type<<"  "<<it->count<<endl;
            newNode.count = chunkNumber + it->count;
            typeSet.insert(newNode);
            //cout<<newNode.count<<endl;
            //exit(0);
		}
        cout<<typeSet.size()<<endl;
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
    	    memcpy(typeHash, md5full, FPSIZE);

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
	readInChunks(fp,typeFp);
	cout<<"totalLogicChunkNumber "<<totalLogicChunkNumber<<endl;
    
    

	return 0;
}
