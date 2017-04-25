
<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>

Upload Image File
<input type="file" id="file" name="file" />
<br>
Scale
<select id="scale" name="scale">
  <option value="gray">Gray</option>
  <option value="color">Color</option>
</select>


<br>

<div id="display" "></div>


<script>




$("#file").change(function(){
	$('#display').html("");
	var form_data = new FormData();
    	form_data.append('file', $('input[type=file]')[0].files[0]);
	$.ajax({
       		url : 'upload.php',
       		type : 'POST',
       		data: form_data,
		enctype: 'multipart/form-data',
		processData: false,  // tell jQuery not to process the data
       		contentType: false,  // tell jQuery not to set contentType
       		success : function(data) {
			$.ajax({
				url : 'process.php?scale='+$('#scale').val(),
				type : 'get',
				success : function(d){
					$('#display').html("<img src=\"orginal.jpg?"+Math.random()+"\"><img src=\""+$('#scale').val()+"_blur.jpg?"+Math.random()+"\"><br><img src=\""+$('#scale').val()+"_rotation.jpg?"+Math.random()+"\"><img src=\""+$('#scale').val()+"_negative.jpg?"+Math.random()+"\">");
				}
			});
		},
		error: function (xhr, ajaxOptions, thrownError) {
        		alert(xhr.status);
        		alert(thrownError);
      		}
	});
});

</script>
