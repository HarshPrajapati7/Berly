
import cv2
import pytesseract
from gtts import gTTS
import os


pytesseract.pytesseract.tesseract_cmd = r"C:\Program Files\Tesseract-OCR\tesseract.exe"

def capture_frame_from_camera():
    """Capture a single frame from the camera when SPACE is pressed."""
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Error: Unable to open camera")
        return None

    print("Press SPACE to capture an image, or ESC to exit.")
    captured_frame = None

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to grab frame")
            break

        cv2.imshow("Camera Feed", frame)
        key = cv2.waitKey(1) & 0xFF

        if key == 27:  
            break
        elif key == 32:  
            captured_frame = frame
            break

    cap.release()
    cv2.destroyAllWindows()
    return captured_frame

def preprocess_image(frame):
    """Convert frame to grayscale and apply thresholding for better OCR."""
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # Use Otsu's thresholding to binarize the image
    _, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    return thresh

def extract_text_from_image(image):
    """Extract text from the preprocessed image using Tesseract OCR."""
    # Using page segmentation mode 6 (assumes a single uniform block of text)
    text = pytesseract.image_to_string(image, lang='eng', config='--psm 6')
    return text.strip()

def convert_text_to_audio(text, filename="output.mp3"):
    """Convert the provided text to speech and save as an MP3 file."""
    tts = gTTS(text=text, lang='en')
    tts.save(filename)
    print(f"Audio saved as {filename}")

def main():
    # Capture image 
    frame = capture_frame_from_camera()
    if frame is None:
        print("No frame captured. Exiting.")
        return

    # Process the frame for OCR
    processed_image = preprocess_image(frame)
    extracted_text = extract_text_from_image(processed_image)

    # Check if any text was detected and convert to speech accordingly
    if extracted_text:
        print("Extracted Text:", extracted_text)
        convert_text_to_audio(extracted_text, "output.mp3")
    else:
        print("No text detected!")
        convert_text_to_audio("No text detected", "output.mp3")

if __name__ == "__main__":
    main()
