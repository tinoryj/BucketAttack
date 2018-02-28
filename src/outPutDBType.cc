#include <bits/stdc++.h>
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <sys/time.h>

#include "leveldb/db.h"

#define FPSIZE 6
using namespace std;

// store DBs-----------------------
leveldb::DB *CFCdb; //fp-count pair
leveldb::DB *CSFdb; //size-fp pair
leveldb::DB *Tdb; 


void initDB(char *CFC, char *CSF, char *type){

    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, CFC, &CFCdb);
    assert(status.ok());
    assert(CFCdb != NULL);

	status = leveldb::DB::Open(options, CSF, &CSFdb);
    assert(status.ok());
    assert(CFCdb != NULL);

	status = leveldb::DB::Open(options, type, &Tdb);
    assert(status.ok());
    assert(Tdb != NULL);
}


void CSF() {

    freopen("CSF.txt", "w", stdout);
    leveldb::Iterator* it = CSFdb->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        char mk1[256];
        string mKey = it->value().ToString();
        uint64_t len = mKey.length();
        cout<<it->key().ToString()<<"\t";
		for (uint64_t i = 0; i < len; i += FPSIZE) {

            sprintf(mk1,"%02x:%02x:%02x:%02x:%02x:%02x\t",mKey[0+i]&0xFF,mKey[1+i]&0xFF,mKey[2+i]&0xFF,mKey[3+i]&0xFF,mKey[4+i]&0xFF,mKey[5+i]&0xFF);
            cout<<mk1;
		}
        cout<<endl;
    }
    assert(it->status().ok());
    delete it;
}

void CFC() {

    freopen("CFC.txt", "w", stdout);
    leveldb::Iterator* it = CFCdb->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        char mk1[256];
        string mKey = it->key().ToString();
        sprintf(mk1,"%02x:%02x:%02x:%02x:%02x:%02x%02x:%02x:%02x:%02x:%02x:%02x\t",mKey[0]&0xFF,mKey[1]&0xFF,mKey[2]&0xFF,mKey[3]&0xFF,mKey[4]&0xFF,mKey[5]&0xFF,mKey[6]&0xFF,mKey[7]&0xFF,mKey[8]&0xFF,mKey[9]&0xFF,mKey[10]&0xFF,mKey[11]&0xFF);
        cout<<mk1<<"\t";
		cout<<it->value().ToString()<<endl;
    }
    assert(it->status().ok());
    delete it;
}
/*
void type() {

    freopen("type.txt", "w", stdout);
    leveldb::Iterator* it = Tdb->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        char mk1[256];
        string mKey = it->key().ToString();
        sprintf(mk1,"%02x:%02x:%02x:%02x:%02x:%02x\t",mKey[0]&0xFF,mKey[1]&0xFF,mKey[2]&0xFF,mKey[3]&0xFF,mKey[4]&0xFF,mKey[5]&0xFF);
        cout<<mk1<<"\t";
		cout<<it->value().ToString()<<endl;
    }
    assert(it->status().ok());
    delete it;
}
*/
void type() {

    freopen("type.txt", "w", stdout);
    leveldb::Iterator* it = Tdb->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        //string mKey = it->key().ToString();
        cout<<it->key().ToString()<<"\t"<<it->value().ToString()<<endl;
    }
    assert(it->status().ok());
    delete it;
}
int main (int argc, char *argv[]){

	initDB(argv[1], argv[2],argv[3]);
    CSF();
    CFC();
    type();

	return 0;
}
