/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-websocket
 */

const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html>
<head>
<title>Arduino WebSocket</title>
<meta name="viewport" content="width=device-width, initial-scale=0.7, maximum-scale=0.7">
<link rel="icon" href="https://diyables.io/images/page/diyables.svg">
<style>
/* Add some basic styling for the chat window */
body {
  background-color: #E1EFEF;
  font-size: 20px;
  line-height: 1.3;
}
button, input {
  font-size: 20px;
  line-height: 1.3;
}
.chat-container {
  margin: 0 auto;
  padding: 10px;
}
.chat-messages {
  height: 250px;
  overflow-y: auto;
  padding: 5px;
  margin-bottom: 5px;
}
.user-input {
  display: flex;
  margin-bottom: 20px;
}
.user-input input {
  flex: 1;
  border: 1px solid #444;
  padding: 5px;
}
.user-input button {
  margin-left: 5px;
  background-color: #007bff;
  color: #fff;
  border: none;
  padding: 5px 10px;
  cursor: pointer;
}
.websocket {
  display: flex;
  align-items: center;
  margin-bottom: 5px;
}
.websocket button {
  background-color: #007bff;
  color: #fff;
  border: none;
  padding: 5px 10px;
  cursor: pointer;
}
.websocket .label {
  margin-left: auto;
}

.message-sent {
  border-radius: 25px;
  background-color: #d35400;
  float: right;
  width: fit-content;
  padding: 10px 20px;
  margin: 0;
}

.message-received {
  border-radius: 25px;
  background-color: white;
  float: left;
  width: fit-content;
  padding: 10px 20px;
  margin: 0;
}

</style>
<script>
var ws;
var wsm_max_len = 4096; /* bigger length causes uart0 buffer overflow with low speed smart device */

function update_text(text) {
  var chat_messages = document.getElementById("chat-messages");
  chat_messages.innerHTML += '<div style="width:100%;overflow: auto;">' + text + '</div>';
  chat_messages.scrollTop = chat_messages.scrollHeight;
}

function send_onclick() {
  if(ws != null) {
    var message = document.getElementById("message").value;
    
    if (message) {
      document.getElementById("message").value = "";
      ws.send(message + "\n");
      update_text('<p class="message-sent">' + message + '</p>');
      // You can send the message to the server or process it as needed
    }
  }
}

function connect_onclick() {
  if(ws == null) {
    ws = new WebSocket("http://" + window.location.host + ":81");
    document.getElementById("ws_state").innerHTML = "CONNECTING";
    ws.onopen = ws_onopen;
    ws.onclose = ws_onclose;
    ws.onmessage = ws_onmessage;
  } else
    ws.close();
}

function ws_onopen() {
  document.getElementById("ws_state").innerHTML = "<span style='color:blue'>CONNECTED</span>";
  document.getElementById("bt_connect").innerHTML = "Disconnect";
  document.getElementById("chat-messages").innerHTML = "";
}

function ws_onclose() {
  document.getElementById("ws_state").innerHTML = "<span style='color:gray'>CLOSED</span>";
  document.getElementById("bt_connect").innerHTML = "Connect";
  ws.onopen = null;
  ws.onclose = null;
  ws.onmessage = null;
  ws = null;
}

function ws_onmessage(e_msg) {
  e_msg = e_msg || window.event; // MessageEvent
  console.log(e_msg.data);
  update_text('<p class="message-received">' + e_msg.data + '</p>');
}
</script>
</head>
<body>
  <div class="chat-container">
    <h2>Arduino WebSocket</h2>
    <div class="websocket">
      <button class="connect-button" id="bt_connect" onclick="connect_onclick()">Connect</button>
      <span class="label">WebSocket: <span id="ws_state"><span style="color:blue">CLOSED</span></span></span>
    </div>
    <div class="chat-messages" id="chat-messages"></div>
    <div class="user-input">
      <input type="text" id="message" placeholder="Type your message...">
      <button onclick="send_onclick()">Send</button>
    </div>
  
  <div class="sponsor">Sponsored by <a href="https://amazon.com/diyables">DIYables</a></div>
</div>

</body></html>
)=====";
