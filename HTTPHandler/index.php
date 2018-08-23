<?php
$req_dump = print_r($_REQUEST, TRUE);
$fp = fopen('request.log', 'a');
fwrite($fp,"----------------------------------------------------------------\r\n");
fwrite($fp,date('m.d.Y H:i:s'));
fwrite($fp,"\r\n----------------------------------------------------------------\r\n");
fwrite($fp, $req_dump);
fclose($fp);;
?>