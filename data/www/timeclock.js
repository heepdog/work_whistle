function check(element) {
    console.log(element.id);
    var msg = { command: "DigitalWrite", pin: element.id, data: element.checked?1:0 , date: Date.now()}
    if (webSocket.readyState == WebSocket.CLOSED){
      webSocket = new WebSocket(gwUrl);
    }

     webSocket.send(JSON.stringify(msg));
   }

   var gwUrl = "ws://" + location.host + "/ws";
   var webSocket = new WebSocket(gwUrl);
   webSocket.onopen = function(e) {
       console.log("open");
       document.getElementById("databox").classList.remove("disabledbutton");
   }
   webSocket.onclose = function(e) {
       //document.getElementById("databox").classList.add("disabledbutton");
       console.log("close");
       webSocket = new WebSocket(gwUrl);
   }
 
   webSocket.onmessage = function(e) {
     console.log("message");
     var data = e.data;
     var jsondata = JSON.parse(data);
     if(jsondata.command == "message"){
       var newLine = document.createElement("P");
       newLine.className = "ConsoleLine"
       var newMessage = document.createTextNode(e.data);
       newLine.appendChild(newMessage);
       document.getElementById("messagebox").appendChild(newLine);
       document.getElementById("messagebox").lastElementChild.scrollIntoView(false);
     }
     else if(jsondata.command == "update")
       document.getElementById(jsondata.pin).checked = jsondata.value=="1"?true:false;
     //if (data == '1')
       //document.getElementById('s1').checked = true;
     //else
      //document.getElementById('s1').checked = false;
   }