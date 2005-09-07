<?
$dsn = "retstest";
$user = "Joe";
$pwd = "Schmoe";

// make a connection 
echo "about to make a connection<br/>";
$conn = odbc_connect($dsn, $user, $pwd);
echo "connection made<br/>";

// create a query
$query = "SELECT * FROM data:Property:ResidentialProperty WHERE ListPrice > 0";
echo "About to execute: $query<br/>";
$queryexec = odbc_exec($conn, $query);
echo "Query done<br/>";

// Check width
echo "About to get field count<br/>";
$field_count = odbc_num_fields($queryexec);
echo "COUNT $field_count<br/>";

// Get results
echo "About to display results<br/>";
odbc_result_all($queryexec,"BORDER=1");
echo "Done displaying results<br/>";

// close a connection 
echo "about to close connection<br/>";
odbc_close($conn);
echo "connection closed<br/>";
?>