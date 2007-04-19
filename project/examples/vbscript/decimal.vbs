' This can be excuted from the command line using cscript as in:
' % cscript GetObject.vbs
' To run it in the debugger, pass the //X flag to cscript

' Stream constants
Const adTypeBinary = 1
Const adTypeText = 2
Const adSaveCreateOverWrite = 2

' create a Connection object...
Set m_Conn = CreateObject("Adodb.Connection")
m_conn.Open "dsn=retstest"
Set m_rs = CreateObject("ADODB.Recordset")
Set mstream = CreateObject("ADODB.Stream")
mstream.Type = adTypeBinary

strSQL = "SELECT LN,LP,SQFT FROM data:Property:ResidentialProperty WHERE LN='LN000003'"
Set m_rs = m_conn.Execute(strSQL)

While m_rs.eof = False
'	wscript.echo "EOF: " & m_rs.eof
	wscript.echo "m_rs(0): " & m_rs(0).Name & " = " & m_rs(0)
	wscript.echo "m_rs(1): " & m_rs(1).Name & " = " & m_rs(1)
	wscript.echo "m_rs(2): " & m_rs(2).Name & " = " & m_rs(2).value
	m_rs.MoveNext
Wend

m_rs.close
Set m_rs = nothing
m_conn.close
Set m_conn = nothing
