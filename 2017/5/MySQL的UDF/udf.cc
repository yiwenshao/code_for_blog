// g++ -shared -fPIC -I /usr/include/mysql -o udf.so udf.cc  to compile

#include <mysql/mysql.h>

extern "C"{
long long myadd(UDF_INIT *initid, UDF_ARGS *args,
                char *is_null, char *error);
my_bool myadd_init(UDF_INIT *initid, UDF_ARGS *args,
                  char *message);
}


long long myadd(UDF_INIT *initid, UDF_ARGS *args,
                char *is_null, char *error) {
    int a = *((long long *)args->args[0]);
    int b = *((long long *)args->args[1]);
    return a + b;
}

my_bool myadd_init(UDF_INIT *initid, UDF_ARGS *args,
                  char *message){
    return 0;
}
