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
    var webconsole = document.getElementById("databox");
    if (webconsole){
      webconsole.classList.remove("disabledbutton");
    }
    
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
    else if(jsondata.command == "update"){
      if (document.getElementById(jsondata.pin) != null){
        document.getElementById(jsondata.pin).checked = jsondata.value=="1"?true:false;
      }
    }
    else if(jsondata.command == "DailySchedules"){
      delete jsondata.command;
      var daylist = document.getElementsByClassName("weekday-list-item")
      for(var day of daylist){
        var elem = document.getElementById(day.id).childNodes[3];
        if(jsondata[day.id]){
          elem.innerHTML = "";
          for(var scheduleName of jsondata[day.id]){
            elem.innerHTML = elem.innerHTML + `<li class="daily-list-item">${scheduleName}
                                                <div class="edit-block">
                                                  <div class="list-item-edit" id=\"${scheduleName}_edit">edit</div>
                                                  <div class="list-item-edit"id=\"${scheduleName}_delete\">delete</div>
                                                </div>
                                              </li>`;

          }
        }
        else{
          elem.innerHTML = `<li class="daily-list-item">
                              <div class="edit-block"></div>
                            </li>`;
        }

        
      }
    }
    else if(jsondata.command == "GetSchedules"){
      delete jsondata.command;
      var scheduleList = document.getElementsByClassName("weekday-list");
      scheduleList[0].innerHTML = ""
      for(var scheduleName of jsondata.Schedules){
        if (!document.getElementById(scheduleName.Name)){
          var mainNode  = document.createElement("li");
          mainNode.innerHTML = `${scheduleName.Name}<div class="edit-block"><div class="list-item-edit" id="${scheduleName.Name}_add">Add Alert</div></div>`;
          mainNode.id = scheduleName.Name;
          mainNode.classList.add("weekday-list-item");
          scheduleList[0].appendChild(mainNode);
          for(var alert of scheduleName.Alerts){
            mainNode.innerHTML = mainNode.innerHTML +  `<ul class="alert-list">
                                                          <li class="alert-list-item">
                                                            <span class="alert">
                                                              <span class="alert-time">${alert.Time}</span>
                                                              <span class="alert-durration">${alert.Durration}</span>
                                                              <span class="alert-tone">${alert.Tone}</span>
                                                            </span>
                                                            <div class="edit-block"><div class="list-item-edit" id="${alert.Time}_edit">edit</div><div class="list-item-edit"id="${alert.Time}_delete">delete</div></div>
                                                          </li>
                                                        </ul>`

          }
          
        }

      }

    }
  }

  function addScheduleToDay(event){

    console.log(event.target.id)

    var msg = { command: event.target.id , date: Date.now()}
    if (webSocket.readyState == WebSocket.CLOSED){
      webSocket = new WebSocket(gwUrl);
    }

    webSocket.send(JSON.stringify(msg));


  }

  function getDailySchedules(){
    var msg = { command: "getDailySchedules", date: Date.now()}
    waitForSocketConnection(webSocket, function(){webSocket.send(JSON.stringify(msg));})
    
  }
  function getSchedules(){
    var msg = { command: "getSchedules", date: Date.now()}
    waitForSocketConnection(webSocket, function(){webSocket.send(JSON.stringify(msg));})
    
  }

  
// Make the function wait until the connection is made...
function waitForSocketConnection(socket, callback){
    setTimeout(
        function () {
            if (socket.readyState === 1) {
                if (callback != null){
                    callback();
                }
            } else {
                console.log("wait for connection...")
                waitForSocketConnection(socket, callback);
            }

        }, 5); // wait 5 milisecond for the connection...
}