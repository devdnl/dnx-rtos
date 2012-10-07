const ch_t index_html[] =
"<html>\
<head>\
<title>Weather station</title>\
<META HTTP-EQUIV='refresh' content='2' CONTENT='text/html; charset=iso-8859-2'>\
</head>\
<body bgcolor='white' text='black'>\
<br>\
<table bgcolor='silver' width='600' align='center'>\
<tr>\
<td align='center'>\
<h1>Weather station</h1>\
<img src='header.gif'/>\
</td>\
</tr>\
<tr bgcolor='gray'>\
<td align='center'>\
<font size='5'>\
<?date/?><br>\
Temperature: <b><?temp/?></b> &degC<br>\
Pressure: <b><?pres/?></b> hPa\
</font>\
</td>\
</tr>\
<tr>\
<td align='center'>\
<hr color='#202020'/>\
<font size='1' color='#303030'>\
<small>\
Copyright &copy 2012 Daniel Zorychta\
</small>\
</font>\
</td>\
</tr>\
</table>\
</body>\
</html>";
