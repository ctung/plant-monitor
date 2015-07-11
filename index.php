<?php 
require_once 'connectvars.php';
$plant = "roger";
?>
<!DOCTYPE html>
<html>
  <head>
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <title>Internet of Plants</title>
    <script>
    google.load('visualization', '1', {packages: ['corechart', 'line']});
    google.setOnLoadCallback(drawBasic);
    
    function drawBasic() {
      var data = new google.visualization.DataTable();
      data.addColumn('datetime', 'Datetime');
      data.addColumn('number', 'Light');
      data.addColumn('number', 'Moisture');
      
      data.addRows([
	<?php 
	require_once 'connectvars.php';
	$conn = new mysqli($servername,$username,$password,$dbname);
	if ($conn->connect_error) {
	  die("Connection failed: ". $conn->connect_error);
	}
	$sql = "SELECT timestamp,light,moisture FROM data WHERE pid=(SELECT pid FROM plant WHERE name='".$plant."')";
	$result = $conn->query($sql);
	$rows = array();
	while($row = $result->fetch_assoc()) {
	  array_push($rows, "[new Date('".$row['timestamp']."'), ".$row['light']/40.96 .", ".$row['moisture']/40.96 ."]");
	}
	echo implode(",",$rows);
	mysqli_close($conn);
	?>
      ]);
      
      var options = {
        hAxis: {
          title: 'Time'
        },
        vAxis: {
          title: 'Level'
        }
      };
      
      var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
      
      chart.draw(data, options);
    }
    </script>
  </head>
  <body>
    <h1>Chuck's Internet of Plants</h1>
    <hr>
    <div id="chart_div"></div>
  </body>
</html>
