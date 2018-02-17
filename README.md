## Usage

### step 1

```
make 
```

This will creat 3 output exec (encryptFSL inferChunks readChunksToDBs)

### step 2

```
./encryptFSL FSL_H_filename
```

This step will creat the encrypted FSL_H file named "xxx.enc"

### step 3

using 

```
./readChunksToDBs /fsl-data.enc /cdbs/xxxFCDB /cdbs/xxxSFDB
./readChunksToDBs /fsl-data /mdbs/xxxFCDB /mdbs/xxxSFDB
```

### step 4

using 

```
./inferChunks /CFC /CSF /MFC /MSF threhold
```

to start infer step accroding to CFC CSF MCF MSF DBs
(couldn't run with step 3,[they using the same dbs])




