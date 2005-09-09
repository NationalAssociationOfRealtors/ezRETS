<html>
<head>
<title>ezRETS Metadata Viewer with PHP</title>
</head>
<body>
About to make a connection.<br/>
<?

$dsn = "retstest";
$user = "Joe";
$pwd = "Schmoe";
$conn = odbc_connect($dsn, $user, $pwd);

?>
Connection made.<br/>
<?

// lookup tables 
$tableexec = odbc_tables($conn);
$table_count = odbc_num_fields($tableexec);
echo "There are $table_count tables on the server</br>";

?>
<table border="0" width="100%" cellpadding="10" cellspacing="0">
<?

// loop through tables
while (odbc_fetch_row($tableexec)) 
{

?>
  <tr>
    <td>
      <table border=\"1\">
        <tr bgcolor="yellow">
          <th>Table Name</th>
          <th>Description</th>
          <th>Columns</th>
        </tr>
        <tr>
          <td>
<?

// table name 
    $name = odbc_result($tableexec, "TABLE_NAME");
    echo "$name";

?>
          </td>
          <td>
<?

// table description 
    echo odbc_result($tableexec, "REMARKS");

?>
          </td>
          <td>
<?

// number of columns.  Need to look them up first.  
    $colexec = odbc_columns($conn, "", "", $name);
    echo odbc_num_fields($colexec);

?>
          </td>
        </tr>
        <tr>
          <td align="right" colspan="3">
            <table border="0" cellpadding="10" cellspacing="0">
              <tr>
                <td>
                  <table border="1">
                    <tr bgcolor="yellow">
                      <th>Column Name</th>
                      <th>Description</th>
                      <th>Data Type</th>
                      <th>Size</th>
                    </tr>

<?

// loop through columns
    while (odbc_fetch_row($colexec)) 
    {

?>

                    <tr>
                      <td>
<?

// column name
        echo odbc_result($colexec, "COLUMN_NAME");

?>
                      </td>
                      <td>
<?

// description 
        echo odbc_result($colexec, "REMARKS");

?>
                      </td>
                      <td>
<?

// data type 
        echo odbc_result($colexec, "TYPE_NAME");

?>
                      </td>
                      <td>
<?

// size 
        echo odbc_result($colexec, "COLUMN_SIZE");

?>
                      </td>
                    </tr>
<?

// end looping through columns 
    }

?>
                  </table>
                </td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
    </td>
  </tr>
<?

// end looping through tables
}

?>
</table>
About to close connection.<br/>
<?

// close connection
odbc_close($conn);

?>
Connection closed.<br/>
</body>
