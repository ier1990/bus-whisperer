
import processing.serial.*;
import controlP5.*;

Serial myPort;
ControlP5 cp5;

int menuHeight = 60;
PFont font;

void settings() {
  //fullScreen();  // or size(displayWidth, displayHeight);
  size(1000, 600);
}

void setup() {
  //size(1000, 600);
  cp5 = new ControlP5(this);
  font = createFont("Arial", 12);
  textFont(font);

  println("Available ports:");
  String[] ports = Serial.list();
  for (int i = 0; i < ports.length; i++) {
    println("[" + i + "] " + ports[i]);
  }

  // Example COM port connection (optional)
  // myPort = new Serial(this, ports[0], 9600);
}

void draw() {
  background(0);
  drawICSocket();
}

void drawICSocket() {
  int centerX = width / 2;
  int centerY = height / 2;
  int pinSpacing = 30;
  int pinRadius = 12;

  fill(50);
  rect(centerX - 30, centerY - 120, 60, 240);  // IC body

  // Left side pins (1-8)
  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    fill(0, 255, 0);
    ellipse(centerX - 40, py, pinRadius, pinRadius);
    fill(255);
    textAlign(CENTER, CENTER);
    text(i + 1, centerX - 70, py);

    stroke(0, 255, 0);
    line(centerX - 40, py, 10, py);
  }

  // Right side pins (9-16)
  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;
    fill(0, 255, 0);
    ellipse(centerX + 40, py, pinRadius, pinRadius);
    fill(255);
    textAlign(CENTER, CENTER);
    text(i + 9, centerX + 70, py);

    stroke(0, 255, 0);
    line(centerX + 40, py, width - 10, py);
  }
}

void mousePressed() {
  int centerX = width / 2;
  int centerY = height / 2;
  int pinSpacing = 30;
  int pinRadius = 12;

  for (int i = 0; i < 8; i++) {
    int py = centerY - 105 + i * pinSpacing;

    if (dist(mouseX, mouseY, centerX - 40, py) < pinRadius) {
      handlePinClick(i + 1);
      return;
    }

    if (dist(mouseX, mouseY, centerX + 40, py) < pinRadius) {
      handlePinClick(i + 9);
      return;
    }
  }
}

void handlePinClick(int pin) {
  if (mouseButton == LEFT) {
    println("Blink pin " + pin);
    if (myPort != null) myPort.write("/WB" + (pin - 1) + "\n");
  } else if (mouseButton == RIGHT) {
    println("Right click - show menu for pin " + pin);
    // Dropdown or menu logic can go here
  }
}
