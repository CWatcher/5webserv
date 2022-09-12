<?php
session_start();
if (isset($_POST['login']) && empty($_SESSION['name']))
{
	$_SESSION['name'] = $_POST['name'];
}
else if (isset($_POST['logout']))
{
	session_unset();
}
?>

<html>

<head>
	<meta charset="UTF-8" />
	<title>Cookies</title>
</head>

<body bgcolor=lightgray text=dimgray>
	<?php if (empty($_SESSION['name'])): ?>
		<form method="POST" aciton="cookies.php">
			<p><input type="text" name="name" required="requred" size="21"></p>
			<input type="submit" name="login" value="Login">
		</form>

	<?php else: ?>
		<h3>
			Hello <?php echo $_SESSION['name'] ?>
		</h3>
		<form method="POST" action="cookies.php">
			<input type="submit" name="logout" value="Logout">
		</form>

	<?php endif; ?>

	<hr>
	<center>webserv</center>
</body>

</html>
