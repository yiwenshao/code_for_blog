#include<stdio.h>
#include<mysql.h>
#include<my_global.h>
static bool lib_initialized = false;

int main(){
    if (!__sync_bool_compare_and_swap(&lib_initialized, false, true)) {
        return 0;
    }
    char dir_arg[1024];
    snprintf(dir_arg, sizeof(dir_arg), "--datadir=%s", "./shadow");
    char *mysql_av[4]=
    { (char*)"progname",
            (char*)"--skip-grant-tables",
            dir_arg,
            (char*)"--character-set-server=utf8",
            //(char*)"--language=/home/casualet/Desktop/cryptdb/mysql-src/build/sql/share/"
    };
    assert(0 == mysql_library_init(sizeof(mysql_av)/sizeof(mysql_av[0]),(char**) mysql_av, 0));
    assert(0 == mysql_thread_init());
    MYSQL* mysql = mysql_init(NULL);
    if(mysql==NULL){
        printf("error 26\n");
    }else{
        printf("mysql init succeed\n");
    }
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client");
    mysql_options(mysql, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
    mysql_real_connect(mysql, NULL, NULL, NULL, "information_schema", 0, NULL, 0);
    mysql_query(mysql, "SHOW DATABASES;");
    MYSQL_RES *results = mysql_store_result(mysql);
    MYSQL_ROW record;
    while(record=mysql_fetch_row(results)){
        printf("hehe\n");
        printf("%s\n", record[0]);
    }
    mysql_query(mysql, "CREATE DATABASE testdb1;");
    return 0;
}
