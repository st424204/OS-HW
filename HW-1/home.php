<!DOCTYPE html>
<html>
<head>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js"></script>
<style>
table {
    font-family: arial, sans-serif;
    border-collapse: collapse;
    width: 100%;
}

td, th {
    border: 1px solid #dddddd;
    text-align: left;
    padding: 8px;
}

tr:nth-child(even) {
    background-color: #dddddd;
}
</style>
</head>
<body>
<h1>VM Loading Controller</h1>
<label><b>IP address</b></label>
<input type="text" name="ip" required><br>

<label><b>CPU Loading</b></label>
<input type="text" name="limit" required><br>

<label><b>Execution Time</b></label>
<input type="text" name="timeout" required><br>

<button type="button">Generate</button>

<h3>VM list</h3>
<div id="div1"></div>
<script>

$("button").click(function(){
    var ip = $('input[name="ip"]').val();
    var limit = $('input[name="limit"]').val();
    var timeout = $('input[name="timeout"]').val();
    if(ip!=""&&limit!=""&&timeout!="")
	$.ajax({url: "gen.php?ip="+ip+"&limit="+limit+"&timeout="+timeout, success: function(result){}});
});



setInterval(function(){
        $.ajax({url: "api.php", success: function(result){
            $("#div1").html(result);
        }});
},1500);

</script>

</body>
</html>
