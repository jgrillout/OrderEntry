#include <mysql++.h>
#include <ssqls.h>
//#if !defined(EXPAND_MY_SSQLS_STATICS)
//#   define MYSQLPP_SSQLS_NO_STATICS
//#endif


// The following is calling a very complex macro which will create
// "struct nextOrderId", which has the member variables:
//
//   sql_char item;
//   ...
//   sql_mediumtext_null description;
//
// plus methods to help populate the class from a MySQL row.  See the
// SSQLS sections in the user manual for further details.

sql_create_1(nextOrderId,
             1, 0, // The meaning of these values is covered in the user manual

             mysqlpp::sql_int, MaxOrderId

            );




