#include <mysql++.h>
#include <ssqls.h>

// The following is calling a very complex macro which will create
// "struct stock", which has the member variables:
//
//   sql_char item;
//   ...
//   sql_mediumtext_null description;
//
// plus methods to help populate the class from a MySQL row.  See the
// SSQLS sections in the user manual for further details.
sql_create_6(orderLines,
             1, 6, // The meaning of these values is covered in the user manual

             mysqlpp::sql_int, idtblOrder,
             mysqlpp::sql_int, idtblOrderLine,
             mysqlpp::sql_int, idtblPart,
             mysqlpp::sql_int,  Qty,
             mysqlpp::sql_double, Price,
             mysqlpp::sql_double, Cost
            );
