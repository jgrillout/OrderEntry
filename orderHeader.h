#include <mysql++.h>
#include <ssqls.h>
//
//#if !defined(EXPAND_MY_SSQLS_STATICS)
//#   define MYSQLPP_SSQLS_NO_STATICS
//#endif


// The following is calling a very complex macro which will create
// "struct orderHeader", which has the member variables:
//
//   sql_char item;
//   ...
//   sql_mediumtext_null description;
//
// plus methods to help populate the class from a MySQL row.  See the
// SSQLS sections in the user manual for further details.
sql_create_4(orderHeader,
             1, 4, // The meaning of these values is covered in the user manual

             mysqlpp::sql_int, idtblOrder,
             mysqlpp::sql_char,charidtblOrder,
             mysqlpp::sql_int, idtblCustomer,
             mysqlpp::sql_char, transDate

            );

