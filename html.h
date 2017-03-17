extern const char* html_index = "\
<!DOCTYPE html><html><head><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>\
<meta charset='utf-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1'>\
<title>IoT - Rythm'n Hits</title><style type='text/css'>body{padding-top: 50px;}.title{padding: 40px 15px; text-align: center;}\
footer{position: absolute;bottom: 0;width: 100%;height: 60px;background-color: #f5f5f5;font-weight: bold;}.container .text-muted{margin: 20px 0;}\
#inputSignature{width: 15%;}.row{margin: 0 50px;}.img{border: 1px solid transparent;}.statsNotes{width:200px;height:200px;border-radius: 50%;-webkit-border-radius: 50%;\
-moz-border-radius: 50%;text-align: center;line-height: 200px;font-size: 3em;font-weight: bold;}#allNotes{color: #fff;background-color: #5bc0de;border-color: #46b8da;}\
#goodNotes{color: #fff;background-color: #5cb85c;border-color: #4cae4c;}#badNotes{color: #fff;background-color: #d9534f;border-color: #d43f3a;}\
.notesLegend{width: 100%;font-size: 2.5em;font-weight: bold;}</style></head><body><nav class='navbar navbar-inverse navbar-fixed-top'>\
<div class='container'><div class='navbar-header'><a class='navbar-brand' href='#'>Rythm'n Beats</a></div></div></nav><div class='content'>\
<div class='container'><div class='title'><h1>IoT Project - Rythm'n Beats</h1></div><div class='settings'><h2>Settings</h2>\
<div class='row'><div class='col-md-6'><h3 class='actual'>Actual BPM: <span id='actualBPM'></span></h3>\
<form method='get' action=' id='updateBPMForm' class='form-inline'><input type='number' id='inputBpm' class='form-control mb-2 mr-sm-2 mb-sm-0' name='bpm'>\
<input type='submit' class='btn btn-primary' value='Update BPM'></form></div><div class='col-md-6'>\
<h3 class='actual'>Actual Signature: <span id='actualSignature'></span> / <span id='actualDefinition'></span>\
</h3><form method='get' action=' id='updateTimeForm' class='form-inline'>\
<input type='number' id='inputSignature' class='form-control mb-2 mr-sm-2 mb-sm-0' name='timesignature'>\
<select id='selectDefinition' class='form-control' name='timedefinition'><option value='1'>1</option><option value='2'>2</option>\
<option value='4'>4</option><option value='8'>8</option><option value='16'>16</option><option value='32'>32</option></select>\
<input type='submit' class='btn btn-primary' value='Update Signature'></form></div></div></div><br><br><br><div class='stats'>\
<h2>Notes stats</h2><div class='row'><div class='col-md-4'><div class='statsNotes' id='allNotes'><span id='textAllNotes'></span></div>\
<span class='notesLegend' id='allNotesLegend'>Total</span></div><div class='col-md-4'><div class='statsNotes' id='goodNotes'>\
<span id='textGoodNotes'></span></div><span class='notesLegend' id='goodNotesLegend'>Good</span></div><div class='col-md-4'>\
<div class='statsNotes' id='badNotes'><span id='textBadNotes'></span></div><span class='notesLegend' id='badNotesLegend'>Bad</span></div></div></div></div></div>\
<footer class='footer'><div class='container'><p class='text-muted'>Anthony REY - Jeevanath KALANITHI - Thomas ALLARD</span></p></div></footer>\
<script src='https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'>\
</script><script>/* vars */var serverUrl='';var allNotes=0;var goodNotes=0;var badNotes=0;var refreshTime=5000;$(document).ready(function(){/* Display all dynamics elements */displayCurrentBPM();\
displayActualSignature();displayActualDefinition();displayNotes();/* Update BPM */$('#updateBPMForm').submit(function(form){form.preventDefault();\
$.ajax({url: serverUrl + '/setbpm',type: 'get',data: $('#updateBPMForm').serialize(),success: function(){displayCurrentBPM();}});});\
/* Update timeSignature / timeDefinition */$('#updateTimeForm').submit(function(form){form.preventDefault();\
/* Update Time Signature */$.ajax({url: serverUrl + '/settimesignature',type: 'get',data: $('#inputSignature').val(),success: function(){displayActualSignature();}});\
/* Update Time Definition */$.ajax({url: serverUrl + '/settimedefinition',type: 'get',data: $('#selectpicker').find('option.selected').text(),success: function(){displayActualDefinition();}});});\
/* Update Stats every X seconds */setInterval(function(){displayNotes();});}, refreshTime);\
function displayCurrentBPM(){$.get(serverUrl + '/getbpm', function (data){$('#actualBPM').html(data);$('#inputBpm').val(data);});}\
function displayActualSignature(){$.get(serverUrl + '/gettimesignature', function (data){$('#actualSignature').html(data);$('#inputSignature').val(data);});}\
function displayActualDefinition(){$.get(serverUrl + '/gettimedefinition', function (data){$('#actualDefinition').html(data);$('#selectDefinition').val(data);});}\
function displayNotes(){allNotes=goodNotes + badNotes;$('textAllNotes').html(allNotes);$('textGoodNotes').html(goodNotes);$('textBadNotes').html(badNotes);}\
function getGoodBeats(){$.get(serverUrl + '/getgoodbeats', function (data){goodNotes=data;});}\
function getBadBeats(){$.get(serverUrl + '/getbadbeats', function (data){badNotes=data;});}</script></body></html>\
";
