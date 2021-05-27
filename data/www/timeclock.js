  function check(element) {
    console.log(element.id);
    var msg = { Command: "DigitalWrite", Pin: element.id, Data: element.checked?1:0 , Date: Date.now()}
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
    var data = e.data;
    if (data != "null"){
      console.log(e.data);
      var jsondata = JSON.parse(data);
      if(jsondata.Command == "message"){
        var newLine = document.createElement("P");
        newLine.className = "ConsoleLine"
        var newMessage = document.createTextNode(e.data);
        newLine.appendChild(newMessage);
        document.getElementById("messagebox").appendChild(newLine);
        document.getElementById("messagebox").lastElementChild.scrollIntoView(false);
      }
      else if(jsondata.Command == "update"){
        if (document.getElementById(jsondata.pin) != null){
          document.getElementById(jsondata.pin).checked = jsondata.value=="1"?true:false;
        }
      }
      else if(jsondata.Command == "DailySchedules"){
        delete jsondata.Command;
        var daylist = document.getElementsByClassName("weekday-list-item")
        for(var day of daylist){
          var elem = document.getElementById(day.id).childNodes[3];
          if(jsondata[day.id]){
            elem.innerHTML = "";
            for(var scheduleName of jsondata[day.id]){
              elem.innerHTML = elem.innerHTML + `<li class="daily-list-item">${scheduleName}
                                                  <div class="edit-block">
                                                    <div class="list-item-edit" id="EditSchedule_${scheduleName}">edit</div>
                                                    <div class="list-item-edit"id="DeleteSchedule_${scheduleName}">delete</div>
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
      else if(jsondata.Command == "GetSchedules"){
        delete jsondata.command;
        var scheduleList = document.getElementsByClassName("weekday-list");
        //scheduleList[0].innerHTML = ""
        for(var scheduleName of jsondata.Schedules){
          var el = document.getElementById(scheduleName.Name);
          if (el){
            el.innerHTML = `${scheduleName.Name}<div class="edit-block"><div class="list-item-edit" id="AddAlert_${scheduleName.Name}">Add Alert</div></div>`;
          } else {
              
            var el  = document.createElement("li");
            el.innerHTML = `${scheduleName.Name}<div class="edit-block"><div class="list-item-edit" id="AddAlert_${scheduleName.Name}">Add Alert</div></div>`;
            el.id = scheduleName.Name;
            el.classList.add("weekday-list-item");
            scheduleList[0].appendChild(el);
          }
            // var mainNode = document.getElementById(scheduleName.Name);
            for(var alert of scheduleName.Alerts){
              var timestr = get12HrString(alert.Time)
              el.innerHTML = el.innerHTML +  `<ul class="alert-list">
                                                            <li class="alert-list-item">
                                                              <span class="alert">
                                                                <span class="alert-time">${timestr}</span>
                                                                <span class="alert-durration">${alert.Durration}</span>
                                                                <span class="alert-tone">${alert.Tone}</span>
                                                              </span>
                                                              <div class="edit-block"><div class="list-item-edit" id="EditAlert_${alert.Time}_${scheduleName.Name}">edit</div><div class="list-item-edit"id="DeleteAlert_${alert.Time}_${scheduleName.Name}">delete</div></div>
                                                            </li>
                                                          </ul>`
            
          }

        }

      }
    }
  }

  function addScheduleToDay(event){
    var msg = { Command: event.target.id , Date: Date.now()}
    console.log(event.target.id)
    var item;
    var command;
    var item2;
    [ command, item, item2] = event.target.id.split("_")
    switch(command){
      case "EditSchedule":
        window.location.href = "/schedules.htm";
        break;
      case "DeleteAlert":
        event.target.parentElement.parentElement.remove();
        msg.Command = command;
        msg.Alert = item;
        msg.Schedule = item2;
        break;
      case "AddAlert":
        document.getElementById("formAdd").style.display="block";
        document.getElementById("ScheduleName").innerText = item

        break;
      case "RemoveSchedule":
        break;
      default:
        console.log(`${command} not found`);
    }
   
    if (webSocket.readyState == WebSocket.CLOSED){
      webSocket = new WebSocket(gwUrl);
    }

    webSocket.send(JSON.stringify(msg));




  }

  function getDailySchedules(){
    var msg = { Command: "GetDailySchedules", Date: Date.now()}
    waitForSocketConnection(webSocket, function(){webSocket.send(JSON.stringify(msg));})
    
  }
  function getSchedules(){
    var msg = { Command: "GetSchedules", Date: Date.now()}
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

function get12HrString(timestr){
  var hours, minutes;
  [hours, minutes] = timestr.split(":");
  var hourint = parseInt(hours);
  return String(hourint>12?hourint-12:hourint) + `:${minutes}` + (hourint>=12?" PM":" AM")
}

function hideModal(){
  document.getElementsByClassName("modal")[0].style.display = "none";
}
function AddAlertToSchedule(event){
  var time = (document.getElementById("alertTime").value);
  var schedule = (document.getElementById("ScheduleName").innerText);
  // document.getElementById("alertTime").value = null;
  hideModal();
  var msg = { Command: "AddAlert", Schedule: schedule, AlertTime: time};
  webSocket.send(JSON.stringify(msg));


}
