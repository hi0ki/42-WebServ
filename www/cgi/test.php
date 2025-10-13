#!/usr/bin/php
<?php
// Indiquer le type de contenu
header("Content-Type: text/html");

// Si tu veux récupérer des variables GET
// Exemple : /cgi-bin/hello.php?name=Hafssa
$name = isset($_GET['name']) ? htmlspecialchars($_GET['name']) : 'Guest';

// Si tu veux récupérer des variables POST
// $city = isset($_POST['city']) ? htmlspecialchars($_POST['city']) : 'Unknown';

echo "<html>";
echo "<head><title>CGI PHP Test</title></head>";
echo "<body>";
echo "<h1>Hello CGI!</h1>";
echo "<p>Welcome, $name!</p>";
// echo "<p>City: $city</p>";
echo "</body>";
echo "</html>";
?>
