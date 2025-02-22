#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DHT_PIN 7  // GPIO4 (Pin 7) for DHT22
#define SPEAKER_PIN 18  // GPIO18 (Pin 12) for PWM speaker

using namespace cv;

// Function to capture image using USB Camera
void capture_image() {
    VideoCapture cap(0);  // Open default camera
    if (!cap.isOpened()) {
        printf("Error: Cannot access camera\n");
        return;
    }
   
    Mat frame;
    cap >> frame;  // Capture frame
    imwrite("captured.jpg", frame);  // Save image
    printf("Image captured and saved as 'captured.jpg'\n");
}

// Function to process Braille text using Tesseract OCR
void process_braille() {
    printf("Processing Braille text...\n");
    system("tesseract captured.jpg output_text");  // Run OCR
    system("cat output_text.txt");  // Display detected text
}

// Function to convert text to speech
void text_to_speech() {
    printf("Converting text to speech...\n");
    system("espeak -f output_text.txt");  // Convert text file to speech
}

// Function to record audio from microphone
void record_audio() {
    printf("Recording audio...\n");
    system("arecord -D plughw:1,0 -f cd -d 5 recorded_audio.wav && lame recorded_audio.wav recorded_audio.mp3");
    printf("Audio saved as 'recorded_audio.mp3'\n");
}

// Function to read temperature and humidity from DHT22
void read_temperature_humidity() {
    int data[5] = {0,0,0,0,0};
   
    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(18);
    digitalWrite(DHT_PIN, HIGH);
    delayMicroseconds(40);
    pinMode(DHT_PIN, INPUT);
   
    for (int i = 0; i < 40; i++) {
        while (digitalRead(DHT_PIN) == LOW);
        delayMicroseconds(30);
        if (digitalRead(DHT_PIN) == HIGH) data[i / 8] <<= 1;
        if (digitalRead(DHT_PIN) == LOW) data[i / 8] |= 1;
        while (digitalRead(DHT_PIN) == HIGH);
    }
   
    if (data[4] == (data[0] + data[1] + data[2] + data[3])) {
        printf("Temperature: %d°C, Humidity: %d%%\n", data[2], data[0]);
    } else {
        printf("DHT22 Sensor Error: Check wiring!\n");
    }
}

// Function to play text-to-speech audio
void play_audio() {
    printf("Playing saved audio...\n");
    system("mpg123 recorded_audio.mp3");
}

int main() {
    wiringPiSetupGpio();  // Initialize GPIO
    pinMode(SPEAKER_PIN, PWM_OUTPUT);
   
    capture_image();  // Capture image from webcam
    process_braille();  // Extract Braille text using OCR
    text_to_speech();  // Convert text to speech
    record_audio();  // Record user audio input
    read_temperature_humidity();  // Read from DHT22 sensor
    play_audio();  // Play saved audio
   
    return 0;
}