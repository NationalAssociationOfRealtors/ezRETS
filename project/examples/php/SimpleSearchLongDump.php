<?
$dsn  = 'retstest';
$user = 'Joe';
$pwd  = 'Schmoe';

$conn = odbc_connect($dsn, $user, $pwd);

$query = 'SELECT * FROM data:Property:ResidentialProperty WHERE ListPrice >
0';
$exec = odbc_exec($conn, $query);

$field_count = odbc_num_fields($exec);

while (odbc_fetch_row($exec)) {
    for ($i=1; $i <= $field_count; $i++) {
        $field_name  = odbc_field_name($exec, $i);
        $field_value = odbc_result($exec, $i);
        echo $field_name . ": " . $field_value . "\n";
    }
    echo "\n";
}
?>
