<?
$dsn = "retstest";
$user = "Joe";
$pwd = "Schmoe";

// make a connection 
echo "about to make a make a connection<br/>";
$conn = odbc_connect($dsn, $user, $pwd);
echo "connection made<br/>";

// query supported types
echo "about to get primitive types supported<br/>";
$queryexec = odbc_gettypeinfo($conn);
echo "post getTypeinfo<br/>";

// display results
echo "about to display results<br/>";
odbc_result_all($queryexec,"BORDER=1");
 
// close a connection 
echo "about to close connection<br/>";
odbc_close($conn);
echo "connection closed<br/>";
?>