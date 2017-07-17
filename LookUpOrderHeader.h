#include <mysql++.h>
#include <ssqls.h>

// The following is calling a very complex macro which will create
// "struct LookUpOrderHeader", which has the member variables:
//
//   sql_char item;
//   ...
//   sql_mediumtext_null description;
//
// plus methods to help populate the class from a MySQL row.  See the
// SSQLS sections in the user manual for further details.
sql_create_10(LookUpOrderHeader,
              1, 10, // The meaning of these values is covered in the user manual

              mysqlpp::sql_int, idtblOrder,
              mysqlpp::sql_char,charidtblOrder,
              mysqlpp::sql_int, idtblCustomer,
              mysqlpp::sql_char, transDate,
              mysqlpp::sql_char, CustId,
              mysqlpp::sql_char, CustName,
              mysqlpp::sql_char, Address,
              mysqlpp::sql_char, City,
              mysqlpp::sql_char, State,
              mysqlpp::sql_char, Zip

             );

