<?php 
$plant = "moneyTree";
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
      data.addColumn('number', 'Temperature');
      data.addColumn('number', 'Pump');
      data.addRows([
	<?php 
	require_once 'connectvars.php';
	$conn = new mysqli($servername,$username,$password,$dbname);
	if ($conn->connect_error) {
	  die("Connection failed: ". $conn->connect_error);
	}

	// variables to store last known value if current value is NULL (returned as 0)
	$lastLight = 0;
	$lastMoisture = 0;
	$lastTemp = 0;
	$lastPump = 0;

	$sql = "SELECT timestamp,light,moisture FROM data WHERE pid=(SELECT pid FROM plant WHERE name='".$plant."')";
	$result = $conn->query($sql);
	$rows = array();
	while($row = $result->fetch_assoc()) {
	  if ($row['light'] > 0) { $lastLight=sprintf("%0.1f",$row['light']/40.96); }
	  if ($row['moisture'] > 0) { $lastMoisture = sprintf("%0.1f",$row['moisture']/40.96); }
	  $date = implode(",",preg_split("/[- :]/",$row['timestamp'])).",0";  // split string into year, month, day, ...
	  array_push($rows, "[new Date(".$date."), ".$lastLight.", ".$lastMoisture.", ".$lastTemp.", ".$lastPump."]");
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
