// === Bus Whisperer UI Layout (Processing + ControlP5) ===
// Modular layout with top menu, waveform scope, terminal, and sidebar pin controls

import processing.serial.*;
import controlP5.*;

Serial myPort;
ControlP5 cp5;
String[] portList;
boolean portConnected = false;

int[][] data = new int[16][200];
float thresholdLow = 200;
float thresholdHigh = 410;

int menuHeight = 60;
int terminalHeight = 80;
int sidebarWidth = 160;
StringBuilder terminalLog = new StringBuilder();
boolean showTerminal = true;

void settings() {
  size(1000, 600); // final layout size
}

void setup() {
  cp5 = new ControlP5(this);
  setupMenuBar();
  setupSidebar();
  listSerialPorts();
  textFont(createFont("Courier", 12));
}

void draw() {
  background(0);
  drawMenuBar();
  drawScope();
  if (showTerminal) drawTerminal();
  drawSidebar();
  drawICSocket();
}

// === UI COMPONENTS ===

void drawMenuBar() {
  fill(20);
  noStroke();
  rect(0, 0, width, menuHeight);
  
}

void drawScope() {
  int scopeHeight = height - terminalHeight - menuHeight;
  int scopeWidth = width - sidebarWidth;
  int scopeY = menuHeight;
  int rowHeight = scopeHeight / 16;

  strokeWeight(1);
  for (int ch = 0; ch < 16; ch++) {
    float yOffset = scopeY + ch * rowHeight;
    for (int x = 0; x < data[ch].length - 1; x++) {
      int val = data[ch][x];
      float y1 = map(val, 0, 1023, yOffset + rowHeight, yOffset);
      float y2 = map(data[ch][x + 1], 0, 1023, yOffset + rowHeight, yOffset);
      float x1 = x * (scopeWidth / float(data[ch].length));
      float x2 = (x + 1) * (scopeWidth / float(data[ch].length));

      if (val < thresholdLow) stroke(0, 255, 0);
      else if (val > thresholdHigh) stroke(255, 0, 0);
      else stroke(255, 165, 0);

      line(x1, y1, x2, y2);
    }
  }
}

void drawICSocket() {
  int icX = width - sidebarWidth + 80; // offset inside sidebar
  int icY = height - 240;
  int pinSpacing = 20;
  int ledRadius = 10;

  // Draw outline
  stroke(180);
  noFill();
  rect(icX - 30, icY - 10, 60, pinSpacing * 8 + 20);  // IC body outline

  for (int i = 0; i < 8; i++) {
    int pinL = i;
    int pinR = 15 - i;

    // Analog values for each pin
    int valL = data[pinL][data[pinL].length - 1];
    int valR = data[pinR][data[pinR].length - 1];

    // Map value to LED color
    fill(getLEDColor(valL));
    ellipse(icX - 40, icY + i * pinSpacing, ledRadius, ledRadius);
    fill(200);
    textAlign(RIGHT, CENTER);
    text(" " + (pinL + 1), icX - 48, icY + i * pinSpacing);

    fill(getLEDColor(valR));
    ellipse(icX + 40, icY + i * pinSpacing, ledRadius, ledRadius);
    fill(200);
    textAlign(LEFT, CENTER);
    text((pinR + 1) + " ", icX + 48, icY + i * pinSpacing);
  }

  // Center block (future: logic diagram placeholder)
  noFill();
  rect(icX - 20, icY + 5, 40, pinSpacing * 7 - 10);
}

color getLEDColor(int val) {
  if (val > thresholdHigh) return color(255, 0, 0);     // HIGH → Red
  else if (val < thresholdLow) return color(0, 255, 0); // LOW → Green
  else return color(255, 165, 0);                       // MID → Orange
}

void drawTerminal() {
  fill(0);
  stroke(255);
  rect(0, height - terminalHeight, width, terminalHeight);

  fill(0, 255, 0);
  textAlign(LEFT, TOP);
  text(terminalLog.toString(), 10, height - terminalHeight + 5);
}

void drawSidebar() {
  fill(30);
  noStroke();
  rect(width - sidebarWidth, 0, sidebarWidth, height);
  // buttons are added via ControlP5 so nothing needed here yet
}

// === UI SETUP ===

void setupMenuBar() {
  DropdownList portSelector = cp5.addDropdownList("SelectPort")
    .setPosition(10, 10)
    .setSize(150, 150)
    .setItemHeight(20)
    .setBarHeight(20)
    .setLabel("COM Port");

  cp5.addSlider("thresholdLow")
     .setPosition(170, 10)
     .setSize(120, 15)
     .setRange(0, 1023)
     .setValue(thresholdLow)
     .setLabel("LOW Threshold");

  cp5.addSlider("thresholdHigh")
     .setPosition(170, 30)
     .setSize(120, 15)
     .setRange(0, 1023)
     .setValue(thresholdHigh)
     .setLabel("HIGH Threshold");

  cp5.addTextfield("commandInput")
     .setPosition(450, 10)
     .setSize(300, 30)
     .setAutoClear(true)
     .setLabel("Command Line")
     .setFocus(false);
}

void setupSidebar() {
  for (int i = 0; i < 16; i++) {
    cp5.addButton("BLINK" + i)
       .setPosition(width - sidebarWidth + 10, 70 + i * 25)
       .setSize(60, 20)
       .setLabel("/wb" + i);
  }
}

// === SERIAL HANDLING ===

void serialEvent(Serial p) {
  String inLine = p.readStringUntil('\n');
  if (inLine == null) return;
  inLine = trim(inLine);

  int start = inLine.indexOf("<A>");
  int end = inLine.indexOf("</A>");

  while (start != -1 && end != -1 && end > start) {
    String packet = inLine.substring(start + 3, end);
    String[] tokens = split(packet, ',');
    if (tokens.length == 16) {
      for (int i = 0; i < 16; i++) {
        int val = int(tokens[i]);
        arrayCopy(data[i], 1, data[i], 0, data[i].length - 1);
        data[i][data[i].length - 1] = val;
      }
    }
    inLine = inLine.substring(0, start) + inLine.substring(end + 4);
    start = inLine.indexOf("<A>");
    end = inLine.indexOf("</A>");
  }

  if (inLine.length() > 0) {
    terminalLog.append(inLine + " ");
    if (terminalLog.length() > 2000) {
      terminalLog = new StringBuilder(terminalLog.substring(terminalLog.length() - 2000));
    }
  }
}

void controlEvent(ControlEvent e) {
  if (e.isAssignableFrom(Textfield.class)) {
    if (e.getName().equals("commandInput")) {
      String input = e.getStringValue();
      if (portConnected && myPort != null) {
        myPort.write(input + "\n");
        println(">> " + input);
      }
    }
  } else if (e.isAssignableFrom(Button.class)) {
    String btn = e.getName();
    if (btn.startsWith("BLINK")) {
      int pin = int(btn.substring(5));
      String cmd = "/wb" + pin;
      if (portConnected && myPort != null) {
        myPort.write(cmd + "\n");
        println(">> " + cmd);
      }
    }
  }
}

void listSerialPorts() {
  portList = Serial.list();
  println("Available ports:");
  for (int i = 0; i < portList.length; i++) {
    println("[" + i + "] " + portList[i]);
  }
}
