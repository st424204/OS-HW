<?php
	exec('sudo ./cpu_load_process -ip '. $_GET['ip'].' -i '. $_GET['limit'].' -t '. $_GET['timeout']); 
?>
