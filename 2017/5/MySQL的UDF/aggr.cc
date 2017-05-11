//use g++ -shared -fPIC -I /usr/include/mysql -o aggr.so aggr.cc to compile

#include <mysql/mysql.h>

extern "C" {
my_bool   mysum_init(UDF_INIT *const initid, UDF_ARGS *const args,
                           char *const message);
void mysum_deinit(UDF_INIT *const initid);
void mysum_clear(UDF_INIT *const initid, char *const is_null,
                            char *const error);
void mysum_add(UDF_INIT *const initid, UDF_ARGS *const args,
                          char *const is_null, char *const error);
long long mysum(UDF_INIT *const initid, UDF_ARGS *const args,
                      char *const result, unsigned long *const length,
                      char *const is_null, char *const error);
}



//执行前先进行初始化,分配空间
my_bool mysum_init(UDF_INIT *const initid, UDF_ARGS *const args,
                           char *const message){
    long long * i = new long long;
    initid->ptr = (char*)i;
    return 0;
}

//在执行该函数前,先执行group by, 然后遇到每个新的group, 先调用该函数.如果没有group by, 则所有的都是一个group.
void mysum_clear(UDF_INIT *const initid, char *const is_null,
                            char *const error) {
    *((long long *)(initid->ptr)) = 0;
}

//每一行数据都经过add函数处理
void   mysum_add(UDF_INIT *const initid, UDF_ARGS *const args,
                          char *const is_null, char *const error) {
    *((long long *)(initid->ptr)) =  *((long long *)(initid->ptr)) +
                                    *((long long *)args->args[0]);
}

//所有数据处理完成, 调用用户定义的mysum函数返回结果;遇到下一个group, 重新从clear开始执行.
long long mysum(UDF_INIT *const initid, UDF_ARGS *const args,
                      char *const result, unsigned long *const length,
                      char *const is_null, char *const error) {
    return *((long long *)(initid->ptr));
}

//执行结束, 释放空间
void mysum_deinit(UDF_INIT *const initid){

    delete initid->ptr;
}
