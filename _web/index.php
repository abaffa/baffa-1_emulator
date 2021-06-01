<?php include 'incs/inc_spage.php'; ?>

<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.2.6/jquery.min.js" type="text/javascript" charset="utf-8"></script>
   
<script type="text/javascript" charset="utf-8"> 
	var auto = true;
	function changeAuto(){
		auto = !auto;
	}
	
   $(document).ready(function(){  
	(function poll(){ 
		var Q = $.ajax({ 
			url: "sol1_api.php/getdata", 
			success: function(data){
				
					if(auto==true){
					$("textarea.prompt").replaceWith('<textarea rows="40" cols="110"  class="prompt" readonly="true" id="prompt_out">' + data + '</textarea>');  
					var textarea = document.getElementById('prompt_out');
					textarea.scrollTop = textarea.scrollHeight;}
				
				
			}, 
			dataType: "html", 
			complete: function(){setTimeout(poll, 500)}, 
			timeout: 30000
			});  
			
			
			
			
		})(); 

		$("#prompt_field").keydown(function (e) {
	          if (e.keyCode == 13) {
           
	       	     e.preventDefault();
		     $.post( "sol1_api.php", { cpu_command: $("#prompt_field").val()}, function(data){
				
					if(auto==true){
					$("textarea.prompt").replaceWith('<textarea rows="40" cols="110"  class="prompt" readonly="true" id="prompt_out">' + data + '</textarea>');  
					var textarea = document.getElementById('prompt_out');
					textarea.scrollTop = textarea.scrollHeight;}
				
				
			});
		     $("#prompt_field").val("");
        	  }
	        });
	}); 
	
</script>

<form method="post" target="index.html">

<textarea rows="40" cols="110" class="prompt" readonly="true" id="prompt_out" >


</textarea>
<br>
<input size="124" id="prompt_field" class="texto" type="text" name="cpu_command" autofocus autocomplete="off">
</td></tr>

</table>
</form>

<table  align="center" height="25">
	<tr>
		<td width="230" class="menu_button" onMouseOver="this.className='menu_button2';" onMouseOut="this.className='menu_button';" onclick="changeAuto();"><font size="2">toggle auto-refresh/scroll</font></td>
	</tr>
	
</table>

<?php include 'incs/inc_epage.php'; ?>