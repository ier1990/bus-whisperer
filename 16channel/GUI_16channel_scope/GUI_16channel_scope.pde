import processing.serial.*;
import controlP5.*;

Serial myPort;
ControlP5 cp5;

int menuHeight = 60;
String[] portList;
boolean portConnected = false;

String serialBuffer = "";
boolean insideAnalogBlock = false;
StringBuilder terminalLog = new StringBuilder();


// 16-channel analog buffer
int[][] data = new int[16][200];

// Logic thresholds
float thresholdLow = 200;
float thresholdHigh = 410;

void settings() {
  size(800, 600);
  //size(displayWidth, displayHeight);
}

void setup() {
  cp5 = new ControlP5(this);

  // List COM ports
  portList = Serial.list();
  println("Available ports:");
  for (int i = 0; i < portList.length; i++) {
    println("[" + i + "] " + portList[i]);
  }

  // COM Port Selector
  DropdownList portSelector = cp5.addDropdownList("SelectPort")
    .setPosition(10, 10)
    .setSize(150, 150)
    .setItemHeight(20)
    .setBarHeight(20)
    .setLabel("COM Port");

  for (int i = 0; i < portList.length; i++) {
    portSelector.addItem(portList[i], i);
  }

  // Threshold sliders
  cp5.addSlider("thresholdLow")
     .setPosition(170, 10)
     .setSize(120, 15)
     .setRange(0, 1023)
     .setValue(thresholdLow)
     .setLabel("LOW Threshold");

  cp5.addSlider("thresholdHigh")
     .setPosition(300, 10)
     .setSize(120, 15)
     .setRange(0, 1023)
     .setValue(thresholdHigh)
     .setLabel("HIGH Threshold");
     
  cp5.addTextfield("commandInput")
     //.setPosition(10, height - 40)  // bottom of screen
     .setPosition(450, 10)
     .setSize(400, 30)
     .setAutoClear(true)
     .setLabel("Command Line")
     .setFocus(false);     
     

  textAlign(LEFT, CENTER);
}

void draw() {
  background(0);
  drawHeader();
  drawScope();
  drawTerminal();
}

void drawHeader() {
  fill(20);
  noStroke();
  rect(0, 0, width, menuHeight);

  fill(255);
  //textSize(14);
  //text("Bus Whisperer 16ch Scope | Select COM & Adjust Thresholds", 220, 20);
}

void drawScope() {
  strokeWeight(1);
  for (int ch = 0; ch < 16; ch++) {
    float yOffset = menuHeight + ch * ((height - menuHeight) / 16.0);
    for (int x = 0; x < data[ch].length - 1; x++) {
      int val = data[ch][x];
      float y1 = map(val, 0, 1023, yOffset + ((height - menuHeight) / 16.0), yOffset);
      float y2 = map(data[ch][x + 1], 0, 1023, yOffset + ((height - menuHeight) / 16.0), yOffset);
      float x1 = x * (width / float(data[ch].length));
      float x2 = (x + 1) * (width / float(data[ch].length));

      if (val < thresholdLow) stroke(0, 255, 0);        // LOW
      else if (val > thresholdHigh) stroke(255, 0, 0);  // HIGH
      else stroke(255, 165, 0);                         // MIDDLE

      line(x1, y1, x2, y2);
    }
  }
}


void drawTerminal() {
  fill(0);
  stroke(255);
  rect(0, height - 100, width, 100);

  fill(0, 255, 0);
  textAlign(LEFT, TOP);
  textFont(createFont("Courier", 12));
  text(terminalLog.toString(), 10, height - 95);
}


void SelectPort(int index) {
  if (portConnected) return;

  try {
    println("Connecting to: " + portList[index]);
    myPort = new Serial(this, portList[index], 115200);
    myPort.bufferUntil('\n');  // ✅ CORRECT — single backslash + single quotes
    portConnected = true;
  } catch (Exception e) {
    println("Port error: " + e.getMessage());
  }
}










void serialEvent(Serial p) {
  String inLine = p.readStringUntil('\n');
  if (inLine == null) return;

  inLine = trim(inLine);

  // 🧠 Search for all <A>...</A> blocks (even embedded)
  int start = inLine.indexOf("<A>");
  int end = inLine.indexOf("</A>");

  while (start != -1 && end != -1 && end > start) {
    String packet = inLine.substring(start + 3, end); // extract inner CSV
    String[] tokens = split(packet, ',');

    if (tokens.length == 16) {
      for (int i = 0; i < 16; i++) {
        int val = int(tokens[i]);
        arrayCopy(data[i], 1, data[i], 0, data[i].length - 1);
        data[i][data[i].length - 1] = val;
      }
    }

    // Remove just that <A>...</A> section
    inLine = inLine.substring(0, start) + inLine.substring(end + 4);
    start = inLine.indexOf("<A>");
    end = inLine.indexOf("</A>");
  }

  // Whatever is left (command characters, /, w, b, 7, etc)
  inLine = trim(inLine);
  if (inLine.length() > 0) {
    terminalLog.append(inLine + " ");
    if (terminalLog.length() > 100) {
      terminalLog = new StringBuilder(terminalLog.substring(terminalLog.length() - 100));
    }
  }
}





void controlEvent(ControlEvent theEvent) {
  if (theEvent.isAssignableFrom(Textfield.class)) {
    if (theEvent.getName().equals("commandInput")) {
      String input = theEvent.getStringValue();

      // Send it to Arduino
      if (portConnected && myPort != null) {
        myPort.write(input + "\n");
        println(">> " + input); // local echo
      }
    }
  }
}
