<?php
/**
 * Provide and process Login form and set logged in state
 */
if(!isset($_SESSION['logged_in']) || $_SESSION['logged_in'] === false){
    if(isset($_POST['password']) && $_POST["password"] === $password){
       $_SESSION['logged_in'] = true;
       header("Location: index.php?url=start");
       exit;
    } else {
        Out()->Add(<<<EOT
            <form action="?url=login" method="post">
                <h1>Login</h1>
                <label>Password</label>
                <input type="password" name="password"/>
                <button type="submit" class="btn btn-primary">Login</button>
            </form>
EOT
        );
        $_SESSION['logged_in'] = false; 
    }
}else{
    header("Location: index.php?url=start");
    exit;
}
