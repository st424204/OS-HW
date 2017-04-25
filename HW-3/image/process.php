<?php
chdir('../'.$_GET['scale']);
shell_exec('cp ../image/orginal.jpg .');
shell_exec('mpirun -n 3 -host master,slave1,slave2 ./'.$_GET['scale'].'_negative orginal.jpg');
shell_exec('mpirun -n 3 -host master,slave1,slave2 ./'.$_GET['scale'].'_rotation orginal.jpg');
shell_exec('mpirun -n 3 -host master,slave1,slave2 ./'.$_GET['scale'].'_blur orginal.jpg');
shell_exec('cp *.jpg ../image/');
?>

