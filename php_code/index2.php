<?php
  $millisec = (isset($_GET["millisec"]) && $_GET["millisec"]) ? $_GET["millisec"] : NULL;
  $age = (isset($_GET["age"]) && $_GET["age"]) ? $_GET["age"] : NULL;
  $gender = (isset($_GET["gender"]) && $_GET["gender"]) ? $_GET["gender"] : NULL;
  $top_color = (isset($_GET["top_color"]) && $_GET["top_color"]) ? $_GET["top_color"] : NULL;
  $bottom_color = (isset($_GET["bottom_color"]) && $_GET["bottom_color"]) ? $_GET["bottom_color"] : NULL;
?>

<?php

$var=0;
if ($millisec!=NULL){
  $sql = "SELECT id,frame,millisec,age,gender,img_person,top_color,bottom_color FROM table_videoanalyzing WHERE millisec= '$millisec'";
  if($age!=NULL)
  {
	$sql2="AND age='$age'";
	$sql =  $sql.$sql2;
  }
  if($gender!=NULL)
  {
	$sql2="AND gender='$gender'";
	$sql =  $sql.$sql2;
  }
  if($top_color!=NULL)
  {
	$sql2="AND top_color='$top_color'";
	$sql =  $sql.$sql2;
  }
  if($bottom_color!=NULL)
  {
	$sql2="AND bottom_color='$bottom_color'";
	$sql =  $sql.$sql2;
  }
 }
 else if ($age!=NULL){
  $sql = "SELECT id,frame,millisec,age,gender,img_person,top_color,bottom_color FROM table_videoanalyzing WHERE age= '$age'";
  if($gender!=NULL)
  {
	$sql2="AND gender='$gender'";
	$sql =  $sql.$sql2;
  }
  if($top_color!=NULL)
  {
	$sql2="AND top_color='$top_color'";
	$sql =  $sql.$sql2;
  }
  if($bottom_color!=NULL)
  {
	$sql2="AND bottom_color='$bottom_color'";
	$sql =  $sql.$sql2;
  }
 }
 else if ($gender!=NULL){
  $sql = "SELECT id,frame,millisec,age,gender,img_person,top_color,bottom_color FROM table_videoanalyzing WHERE gender= '$gender'";
  if($top_color!=NULL)
  {
	$sql2="AND top_color='$top_color'";
	$sql =  $sql.$sql2;
  }
  if($bottom_color!=NULL)
  {
	$sql2="AND bottom_color='$bottom_color'";
	$sql =  $sql.$sql2;
  }
 }
 else if ($top_color!=NULL){
  $sql = "SELECT id,frame,millisec,age,gender,img_person,top_color,bottom_color FROM table_videoanalyzing WHERE top_color= '$top_color'";
 }
 else if ($bottom_color!=NULL){
  $sql = "SELECT id,frame,millisec,age,gender,img_person,top_color,bottom_color FROM table_videoanalyzing WHERE bottom_color= '$bottom_color'";
  if($bottom_color!=NULL)
  {
	$sql2="AND bottom_color='$bottom_color'";
	$sql =  $sql.$sql2;
  }
 }
$conn = mysqli_connect("127.0.0.1", "root", "1111", "video_detector", 3306);
$result = mysqli_query($conn, $sql);

$id_names = array();
$index=0;
$array_DATA = array();
$count=0;

while($row = mysqli_fetch_array($result)) {
 $id_names[$index]=$row[0];
 $index=$index+1;
 for($i=0;$i<=7;$i++)
 {
   $array_DATA[$count]=$row[$i];
   $count+=1;
 }
  }
  $id_n=array_unique($id_names);
  $id_n = array_values($id_n);
mysqli_close($conn);
?>
<!DOCTYPE HTML>
<!--
	Hielo by TEMPLATED
	templated.co @templatedco
	Released for free under the Creative Commons Attribution 3.0 license (templated.co/license)
-->
<html>
	<head>
		<title>PictureCapture</title>
		<meta charset="utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1" />
		<link rel="stylesheet" href="assets/css/main.css" />
	</head>
	<body>

		<!-- Header -->
			<header id="header" class="alt">
				<div class="logo"><a href="index.php">PictureCapture</a></div>
				<a href="#menu">obtions</a>
			</header>

		<!-- Nav -->
		<nav id="menu">
			<form method="get" action="index3.php">
			<ul class="links">
				<li>millisec<input type="text" name="millisec"></li>
				<li>age<input type="text" name="age"></li>
				<li>gender<input type="text" name="gender"></li>
				<li>top_color<input type="text" name="top_color"></li>
				<li>bottom_color<input type="text" name="bottom_color"></li>
				<li><input type="submit" value="Submit" name="var"></li>
			</ul>
			</form>
		</nav>
		
		<!-- Banner -->
		<section class="banner full">
			<article>
				<img src="images/bg2.jpg" alt="" />
				<div class="inner">
					<header>
					<video width="640" height="480" controls="controls" id="media">
					<source src="./media/video_test2.mp4" type="video/mp4" />
					</header>
				</div>
			</article>
		</section>

		<!-- One -->
		<section id="one" class="wrapper style2">
			<div class="inner">
				<header>
					<script type="text/javascript">
						$img="./person_img/bg.jpg";
						var id_ns= <?php echo(count($id_n)); ?>;
						var NAME_array = <?php echo json_encode($id_n)?>; 
						var DATA_array = <?php echo json_encode($array_DATA)?>; 
						var num=<?php echo count($array_DATA)?>;

						for(i = 0; i <=id_ns-1; i++){
							var tags="<div>";
							document.write(tags);	
							
							tags="<h1>ID: "+NAME_array[i]+"</h1>";	
							document.write(tags);	
							
							for(j = 0; j <num; j+=8){
								if(NAME_array[i]==DATA_array[j])
								{
									tags='<button onclick="javascript:chkStartTime(this.id)" id="'+j+'"value="'+DATA_array[j+1]/1000+'"><img src="'+DATA_array[j+5]+'" style="width: 100px; height: 100px;"></button>';
									document.write(tags);	
								}
							}
						
							tags="</div>";
						document.write(tags);
						}
					</script>

					
		<script type="text/javascript">
						function chkStartTime(clicked_id){
						var video = document.getElementById('media');
						var starttime = parseInt(document.getElementById(clicked_id).value);
						var endtime = 20;
						video.currentTime = starttime;
						video.addEventListener("timeupdate", function(){
						endtime = parseInt(document.all.end.value);
						if (video.currentTime >= endtime){
						video.pause();
						}
						},false);
						video.play();
						}
					</script>
				</header>	
			</div>
		</section>



		<!-- Footer -->
			<footer id="footer">
				<div class="container">
					<ul class="icons">
						<li><a href="#" class="icon fa-twitter"><span class="label">Twitter</span></a></li>
						<li><a href="#" class="icon fa-facebook"><span class="label">Facebook</span></a></li>
						<li><a href="#" class="icon fa-instagram"><span class="label">Instagram</span></a></li>
						<li><a href="#" class="icon fa-envelope-o"><span class="label">Email</span></a></li>
					</ul>
				</div>
				<div class="copyright">
					&copy; Untitled. All rights reserved.
				</div>
			</footer>

		<!-- Scripts -->
			<script src="assets/js/jquery.min.js"></script>
			<script src="assets/js/jquery.scrollex.min.js"></script>
			<script src="assets/js/skel.min.js"></script>
			<script src="assets/js/util.js"></script>
			<script src="assets/js/main.js"></script>

	</body>
</html>