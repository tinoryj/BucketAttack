# Bucket Attack Work Flow

1. Python 处理hf_stat_f文件,获取到每个文件有多少个Chunk,以及文件的后缀名. 输出metaData文件:
    
    ```
    log
    13
    doc 
    10
    ```
2. readInChunks在原有基础上多读取metaData文件,新增一个metaData数据库(考虑必须使用ramdisk,用IO速度换取执行速度).
    在metaData数据库中:
        
    ```
    Key: Chunk Hash 
    Value: 用`|`作为分割符插入所有出现过的文件类型
    ```

3. 在InferChunk原有的基础上添加一步查询操作,查找metaData数据库.

    维护一个结构数组,结构数组中存(vector/set实现):
    
    ```
    struct node {
        string chunkHash;
        uint64_t logicCount = 0;
        uint64_t correctUniqueCount = 0;
        uint64_t correctLogicCount = 0;
    };
    ```
    在去重文件类型的基础上,存储每种文件的infer的logic chunk个数, infer正确的correct unique chunk个数, infer正确的correct logic chunk个数.
    
    查询metaData数据库的操作:
    
    原有mQue,cQue:
    
    ```
    struct FC {
	   string key;
	   uint64_t count = 0;
    };
    struct cmp {
        bool operator()(FC a, FC b) {
		  
		      return a.count < b.count; 
	   }
    };
    priority_queue<FC, vector<FC>, cmp > cQue;
    priority_queue<FC, vector<FC>, cmp > mQue;
    ```
    
    新mQue,cQue:
    
    ```
    struct FC {
	   string key;
	   uint64_t count = 0;
	   string fileType;
    };
    struct cmp {
        bool operator()(FC a, FC b) {
		  
		      return a.count < b.count; 
	   }
    };
    priority_queue<FC, vector<FC>, cmp > cQue;
    priority_queue<FC, vector<FC>, cmp > mQue;
    ```
    
    通过新增一个fileType的string来记录每个chunk的文件类型.(在建立mQue和cQue时查询metaData数据库).
    
    
    

