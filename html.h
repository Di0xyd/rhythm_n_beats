extern const char* html_index = "\
<!DOCTYPE html><html><head><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><meta charset='utf-8'>\
<meta http-equiv='X-UA-Compatible' content='IE=edge'> <meta name='viewport' content='width=device-width, initial-scale=1'>\
<title>IoT - Rythm'n Hits</title><style type='text/css'>body{padding-top: 50px;}.content{padding: 40px 15px; text-align: center;}</style></head>\
<body><nav class='navbar navbar-inverse navbar-fixed-top'> <div class='container'> <div class='navbar-header'> <a class='navbar-brand' href='#'>Rythm'n Hits</a> </div>\
</div></nav><div class='container'><div class='content'><h1>IoT Project - Rythm'n Hits</h1><br><br><h3 class='actual'>Actual BPM: <span id='actualBPM'></span></h3>\
<form method='get' action='/setbpm' id='updateBPMForm' class='form-inline'><input type='number' id='inputBpm' class='form-control mb-2 mr-sm-2 mb-sm-0' name='bpm'>\
<input type='submit' class='btn btn-primary' value='Update BPM'></form></div></div><script src='https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js'></script>\
<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>\
<script>$(document).ready(function(){getCurrentBPM();$('#updateBPMForm').submit(function(form){form.preventDefault();$.ajax({url: '/setbpm',type: 'get',data: $('#updateBPMForm').serialize(),success: function(){getCurrentBPM();}});});});\
function getCurrentBPM(){$.get('/getbpm', function (data){$('#actualBPM').html(data);$('#inputBpm').val(data);});}</script></body></html>
";