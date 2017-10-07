<html>
<head>
<title>HepMC Interface</title>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='HepMCInterface.php'>

<h2>HepMC Interface</h2>

An interface to the HepMC [<a href="Bibliography.php" target="page">Dob01</a>] standard event record 
format has been provided by M. Kirsanov. To use it, the relevant 
libraires need to be linked, as explained in the <code>README</code> 
file. Version 1 of the program requires that CLHEP is also linked,
for the handling of four-vectors, while version 2 is standalone.
This requires some adjustments in the interface code based on which 
version is going to be used. 

The (simple) procedure to translate PYTHIA 8 events into HepMC ones 
is illustrated in the <code>main31.cc</code>, <code>main32.cc</code>   
and <code>main54.cc</code> main programs. At the core is a call to
the
<pre>
HepMC::I_Pythia8::fill_next_event( pythia.event, hepmcevt ) 
</pre>
routine, which takes a PYTHIA event as input and returns a HepMC one.

</body>
</html>

<!-- Copyright (C) 2007 Torbjorn Sjostrand -->