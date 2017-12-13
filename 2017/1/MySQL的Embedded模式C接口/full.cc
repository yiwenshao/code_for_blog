#include<iostream>
#include<stdio.h>
#include<string>
#include<memory>
#include<vector>
#include<tuple>
#include <pthread.h>
#include<mysql.h>
#include<my_global.h>
#define THREAD_NUM 16
using namespace std;
static bool lib_initialized = false;
//调用出错是用到的函数
void finish_with_error(MYSQL *con){
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    return;
}
//获得命令执行的结果.
void mysql_result_wrapper(MYSQL *con){
    MYSQL_RES *result = mysql_store_result(con);
    if(result == NULL) return;
    int num_fields = mysql_num_fields(result);
    if(num_fields==0) return;
    MYSQL_FIELD *field;
    MYSQL_ROW row;
    //获得参数affected rows.
    printf("%ld products updated",
       (long) mysql_affected_rows(con));
    while(row = mysql_fetch_row(result)){
        for(int i = 0; i < num_fields; i++) {
          if (i == 0)
          {
             while(field = mysql_fetch_field(result))
             {
                 printf("%s ", field->name);
                 printf("%d ",field->type);
             }
             printf("\n");
          }
            printf("%s ", row[i] ? row[i] : "NULL");
        }
        //以数组的方式返回get的当前row的所有字段的长度
        //const unsigned long *const l = mysql_fetch_lengths(result);
        //for(int i = 0; i < num_fields; i++) {
        //    cout<<l[i]<<"\t"<<endl;
        //}
    }
    printf("\n");
}
//发送单个MySQL命令
void mysql_query_wrapper(MYSQL *con,string s){
    if(mysql_query(con, s.c_str())) {
        finish_with_error(con);
    }
    mysql_result_wrapper(con);
}
//初始化MySQL
void init_mysql(){
    //保证该函数只被调用一次
    if (!__sync_bool_compare_and_swap(&lib_initialized, false, true)) {
        return ;
    }
    char dir_arg[1024];
    snprintf(dir_arg, sizeof(dir_arg), "--datadir=%s", "./shadow");
    //第一个参数被忽略, 一般给个名字. 所有参数会复制, 所以调用以后销毁参数是可以的.用这个函数, 是为了多线程,否则mysql_init就可以了.
    char *mysql_av[4]=
    { (char*)"progname",
            (char*)"--skip-grant-tables",
            dir_arg,
            (char*)"--character-set-server=utf8"
    };
    //int mysql_library_init(int argc, char **argv, char **groups)
    assert(0 == mysql_library_init(sizeof(mysql_av)/sizeof(mysql_av[0]),(char**) mysql_av, 0));
    //多线程条件下, 每个线程在使用mysql库中的函数时, 都要调用该函数进行初始化.
    assert(0 == mysql_thread_init());
}
void *thread_func(void *arg) {
    int v = (long)arg;
    //每个线程使用mysql相关的函数之前, 先调用该函数进行初始化.
    assert(0 == mysql_thread_init());
    MYSQL* mysql = mysql_init(NULL);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client");
    mysql_options(mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
    mysql_real_connect(mysql, NULL, NULL, NULL, "information_schema", 0, NULL, 0);
    MYSQL* con = mysql;
    string use="use ttddbb;";
    mysql_query_wrapper(con,use);
    string num = to_string(v);
    string s="insert into student values("+ num+",'zhangfei')";
    mysql_query_wrapper(con,s);
    return (void*)0;
}
int main(){
    //初始化, 只需要调用一次.
    init_mysql();
    assert(0 == mysql_thread_init());
    MYSQL* mysql = mysql_init(NULL);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client");
    mysql_options(mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
    mysql_real_connect(mysql, NULL, NULL, NULL, "information_schema", 0, NULL, 0);
    MYSQL* con = mysql;
    string s;
    s="SELECT @@sql_safe_updates";
    mysql_query_wrapper(con,s);
    s="create database if not exists ttddbb;";
    mysql_query_wrapper(con,s);
    s="use ttddbb;";
    mysql_query_wrapper(con,s);
    s="create table if not exists student (id integer, name varchar(20));";
    mysql_query_wrapper(con,s);
    pthread_t pids[THREAD_NUM];
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        pthread_create(&pids[i], NULL, thread_func, (void*)(unsigned long)(i));
    }
     for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(pids[i], NULL);
    }
    s="select * from student";
    mysql_query_wrapper(con,s);
    return 0;
}
