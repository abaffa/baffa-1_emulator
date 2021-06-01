<?php

$posts = "";
foreach ($_POST as $key => $value) {
    //$posts = $posts.$key."=".urlencode($value);
    $posts = $posts.$key."=".$value;
}

    $parameters = array();
    $parameters["method"] =  "POST";

    $handle = curl_init("http://127.0.0.1/sol1_api.php");
    curl_setopt($handle, CURLOPT_PORT, 20280);
    curl_setopt($handle, CURLOPT_POST, 1);
    curl_setopt($handle, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($handle, CURLOPT_CONNECTTIMEOUT, 60);
    curl_setopt($handle, CURLOPT_TIMEOUT, 60);
    curl_setopt($handle, CURLOPT_POSTFIELDS, $posts);

    //curl_setopt($handle, CURLOPT_HTTPHEADER, array("Content-Type: text/http"));
 
 
    $response = curl_exec($handle);

      if ($response === false) {
          $errno = curl_errno($handle);
          $error = curl_error($handle);
          //echo  "Curl returned error $errno: $error\n";
          echo "SOL-1 Homebrew Computer Emulator is currently offline.";
          curl_close($handle);
          die();
      }

      $http_code = intval(curl_getinfo($handle, CURLINFO_HTTP_CODE));
      curl_close($handle);
      

echo $response;
?>
