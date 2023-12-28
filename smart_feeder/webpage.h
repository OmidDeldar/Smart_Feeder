

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
 <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 <meta charset='utf-8'>
 <style>
  body {font-size:140%; background-color: #374453;} 
  #main {display: table; margin: auto;  padding: 10px 100px 0 100px;   border-style: solid;
  border-color: #bfc7d1;}  

  canvas {margin: 0px 0px 0px 30px; background-color: #ffffff;}
  #FEED_button { padding:10px 10px 10px 10px; width:100%;  background-color: #007400; font-size: 120%;}
  h2 {text-align:center; margin: 10px 0px 10px 0px;} 
  p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
  #time_P { margin: 10px 0px 15px 0px;}
 </style>

<script>
   var tankArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
   function updateTime() 
  {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;


  }
   function updateWeb() 
  {  
       ajaxLoad('getValues'); 
  }

   function switchFeed() 
  {
       var button_text = document.getElementById("FEED_button").value;
       document.getElementById("FEED_button").value = "Feeding...";
       ajaxLoad('FeedOn'); 
       setTimeout(function() {
      document.getElementById("FEED_button").value = "Feed Now!";
      },2000)

     
  }

var ajaxRequest = null;
if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }

  // ===========================================  DIAL  =========================================
  
  function drawDial(canvasID, dialColour, startAngle, stopAngle, minVal, maxVal, dialValue)
  {
    oneDegreeInRadians = Math.PI/180;
    if (stopAngle < startAngle) { stopAngle = stopAngle + 360;}
 
    let arcStartAngleInRadians =  oneDegreeInRadians * (startAngle-5)  ;
    let arcStopAngleInRadians  =  oneDegreeInRadians * (stopAngle+5) ;  
 
    var c = document.getElementById(canvasID);
    var ctx = c.getContext('2d');
    ctx.clearRect(0, 0, c.width, c.height);
    ctx.save();
 
    let H = c.height;
    let W = c.width;
 
    let arcLineWidth = W/5;
    ctx.translate(W/2, W/2);        // move coordinates 0,0 to the centre of the canvas
 
    // draw arc
    ctx.beginPath();
    let radius = W/2 - (arcLineWidth/2) - (W/100);      
    ctx.lineWidth = arcLineWidth;
    ctx.lineCap = 'butt';
    ctx.strokeStyle = dialColour;
    ctx.arc(0, 0, radius, arcStartAngleInRadians, arcStopAngleInRadians, false);
    ctx.stroke();
 
 
    // draw centre circle
    ctx.beginPath();
    let centerCircleRadius = W/100*3.5
    ctx.strokeStyle = '#000000';
    ctx.fillStyle = '#222222';
    ctx.lineWidth = 2;
    ctx.arc(0, 0, centerCircleRadius, 0, 2 * Math.PI, true);
    ctx.stroke();
    ctx.fill();
 
 
    // draw ticks 
    ctx.beginPath();
    ctx.lineWidth = 1;
    ctx.lineCap = 'butt';
    ctx.strokeStyle = '#333333';
 
    ctx.font = '12px Arial';
    ctx.fillStyle = '#333333';
    ctx.textAlign = 'center'; 
    ctx.textBaseline = 'top'; 
 
    let tickStartPoint = radius - (arcLineWidth/2) ;   // bottom of the arc
    let tickLength =  5/8 * arcLineWidth - 5; 
 
    let labelPos = radius + (arcLineWidth/2) - 2; 
 
    for (let angle=minVal; angle<=maxVal; angle = angle+10)
    {   
      let angleInDegrees =  (angle-minVal) *  ((stopAngle - startAngle) / (maxVal - minVal)) + startAngle  ;
      let angleInRadians = angleInDegrees * oneDegreeInRadians;
 
      ctx.rotate(angleInRadians );  
      ctx.moveTo(tickStartPoint, 0 );                   
      ctx.lineTo(tickStartPoint + tickLength, 0 );
      ctx.stroke();
 
      // draw the label at the right angle.
      // rotate the dial - 90 degree, draw the text at the new top of the dial, then rotate +90.
      // this means we use the - y axis.
 
      ctx.rotate(90*oneDegreeInRadians); 
      ctx.fillText(angle.toString(), 0, -labelPos );        
      ctx.rotate(-90*oneDegreeInRadians); 
 
      ctx.rotate(-angleInRadians);  //  this puts the dial back where it was.     
    }
 
 
    // draw the pointer
 
    // map the value to a degree
    let pointerAngleInDegrees =  (dialValue-minVal) *  ((stopAngle - startAngle) / (maxVal - minVal)) + startAngle  ;
    let pointerAngleInRadians = pointerAngleInDegrees * oneDegreeInRadians;
 
    let pointerLength = radius*0.86;
    let pointerWidth = W/100 * 2; 
 
    ctx.beginPath();
    ctx.lineWidth = pointerWidth;
    ctx.lineCap = 'round';  
    ctx.moveTo(0,0);
    ctx.rotate(pointerAngleInRadians);
    ctx.lineTo(pointerLength, 0);
    ctx.stroke();
    ctx.rotate(-pointerAngleInRadians);
 
    // reset the coordinates ready for next time    
    ctx.restore();
 
  }

function ajaxLoad(ajaxURL)
{
  if(!ajaxRequest){ alert("AJAX is not supported."); return; }
  
  ajaxRequest.open("GET",ajaxURL,true);
  ajaxRequest.onreadystatechange = function()
  {
    if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
    {
      var ajaxResult = ajaxRequest.responseText;
      var tmpArray = ajaxResult.split("|");
      console.log(tmpArray[4])
      var d = new Date();
      var currentTime = d.toLocaleTimeString();
      var feedTime = tmpArray[4];  // in seconds
      var nextFeedSchedule = 60 - feedTime
      var previousFeed = new Date(d.getTime() - (feedTime * 1000));
      var nextFeed = new Date(d.getTime() + (nextFeedSchedule * 1000));

      // Format dates as strings
      var previousFeedString = previousFeed.toLocaleTimeString();
      var nextFeedString = nextFeed.toLocaleTimeString();
      drawDial('canvasTank', '#aaaaff', 160, 20,   0, 100, tmpArray[1]);
      document.getElementById('count').innerHTML = tmpArray[0];
      document.getElementById('dst').innerHTML = tmpArray[1];
      if(tmpArray[1] < 20){
        document.getElementById("dst").style.color = "#880a0a";
      }
      else{
        document.getElementById("dst").style.color = "black"; 
      }
      document.getElementById('boul').innerHTML = tmpArray[2];
      document.getElementById('appCount').innerHTML = tmpArray[3];
      document.getElementById('fed_time').innerHTML =previousFeedString;
      document.getElementById('next_feed').innerHTML =nextFeedString;

    }
  }
  ajaxRequest.send();
}
    var myVar1 = setInterval(updateWeb, 5000);
    var myVar2 = setInterval(updateTime, 1000);

      // arrays to hold the svg data for the sun icon
  var svgArray = [];
  svgArray[0] = "<?xml version='1.0' encoding='UTF-8'?> <!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' 'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'> <svg xmlns='http://www.w3.org/2000/svg' xml:space='preserve' width='25mm' height='25mm' version='1.1' viewBox='0 0 2500 2500'   xmlns:xlink='http://www.w3.org/1999/xlink'> <defs> <style type='text/css'> <![CDATA[ .str0 {stroke:#aaaaaa; stroke-width:20.00} .str1 {stroke:#aaaaaa; stroke-width:7.5} .str2 {stroke:#aaaaaa; stroke-width:15.00} .fil0 {fill:black} .fil1 {fill:yellow} .fnt0 {font-weight:normal;font-size:400px;font-family:'Verdana'} ]]> </style> </defs> <g id='Layer_x0020_1'> <circle class='fil1 str2' cx='1250' cy='1250' r='680'/>";
  //svgArray[0] = svgArray[0] + "<text x='50%' y='50%' dominant-baseline='middle' text-anchor='middle' class='fil0 fnt0'>[bright]</text>";
  svgArray[1] = "<rect class='fil1 str0' transform='matrix(0.133299 -0.23088 0.246464 0.142296 1562.82 538.757)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[2] = "<rect class='fil1 str0' transform='matrix(0.23088 -0.133299 0.142296 0.246464 1876.53 790.456)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[3] = "<rect class='fil1 str1' x='2022' y='1165' width='428' height='169' rx='75' ry='80'/>";
  svgArray[4] = "<rect class='fil1 str0' transform='matrix(-0.23088 -0.133299 0.142296 -0.246464 2246.87 1923.36)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[5] = "<rect class='fil1 str0' transform='matrix(-0.133299 -0.23088 0.246464 -0.142296 1776.64 2331.58)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[6] = "<rect class='fil1 str0' transform='matrix(-2.11373E-014 0.266598 -0.284592 -2.25059E-014 1334.71 2022.37)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[7] = "<rect class='fil1 str0' transform='matrix(0.133299 -0.23088 0.246464 0.142296 576.632 2246.88)' width='1604' height='595' rx='280' ry='280'/> ";
  svgArray[8] = "<rect class='fil1 str0' transform='matrix(0.23088 -0.133299 0.142296 0.246464 168.409 1776.64)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[9] = "<rect class='fil1 str1' x='50' y='1165' width='428' height='169' rx='75' ry='80'/>";
  svgArray[10] = "<rect class='fil1 str0' transform='matrix(-0.23088 -0.133299 0.142296 -0.246464 538.753 937.178)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[11] = "<rect class='fil1 str0' transform='matrix(-0.133299 -0.23088 0.246464 -0.142296 790.452 623.465)' width='1604' height='595' rx='280' ry='280'/>";
  svgArray[12] = "<rect class='fil1 str0' transform='matrix(-2.11373E-014 0.266598 -0.284592 -2.25059E-014 1334.71 50.0007)' width='1604' height='595' rx='280' ry='280'/>";
  // ]]>    
</script>

 <title>Pet Feeder</title>
</head>
<body>
 <div id='main'>
  <h2>Pet Feeder</h2>
  <input type="button" id = "FEED_button" onclick="switchFeed()" value="Feed Now!"       /> 
  
 
 
     <div id='content'>
       <h2>Time</h2> 
       <p id='P_time'>-</p>
       <h2>Last fed time</h2>
       <p id='fed_time'>-</p>
       <h2>Next feed schedule</h2>
       <p id='next_feed'>-</p>
       <h2>Manual feed</h2>
       <p> <span id='count'>-</span>  </p>
       <h2>App feed</h2>
       <p> <span id='appCount'>-</span>  </p>
       <h2>Storage</h2>
       <canvas id='canvasTank' width='200' height='150'  style='border:1px solid #000000;' > </canvas>
       <p> <span id='dst'>NA</span> % </p>
       <h2>Boul status</h2>
       <p> <span id='boul'>NA</span>  </p>
     </div>
     </div>
</body>
</html>
)=====";