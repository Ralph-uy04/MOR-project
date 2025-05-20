Step-by-Step Execution Flow
1.  Start the Python Object Detection & Classification
- This script runs YOLOv10 to detect trash, classifies it, and stores results in Firestore.
Run:
python detection.py


✔ What happens?
- The camera captures images every 3 seconds.
- YOLO detects objects and assigns labels.
- Items are classified as Biodegradable (1) or Non-Biodegradable (0).
- Classification results are stored in Firestore Database.

2.  Start the Flask API Server
- This server allows the ESP32 to fetch classification results from Firestore.
Run:
python flask_server.py


✔ What happens?
- Flask listens for API requests at http://<your-device-IP>:5000/get-latest-binary.
- ESP32 can now fetch the latest classification result (binary = 0 or 1).

3.  Upload and Run the ESP32 Code
- This connects the ESP32 to WiFi and fetches classification data.
- Open Arduino IDE.
- Load the ESP32 sketch (esp32_code.ino).
- Select the correct Board & Port (ESP32 Dev Module).
- Click Upload.
✔ What happens?
- ESP32 connects to WiFi.
- Sends a GET request to Flask (/get-latest-binary).
- Parses the JSON response.
- Controls LEDs based on classification:
- Green LED ON → Biodegradable (binary = 1)
- Red LED ON → Non-Biodegradable (binary = 0)


[Run Python Object Detection] → [Store Classification in Firestore]
    ↓
[Run Flask API] → [Fetch Latest Classification]
    ↓
[Run ESP32 Code] → [Retrieve Binary Value]
    ↓
[Control LEDs Based on Classification]
