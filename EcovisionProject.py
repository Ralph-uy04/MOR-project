import cv2
import numpy as np
from ultralytics import YOLO
import firebase_admin
from firebase_admin import credentials, firestore
import time

# Initialize Firebase Admin SDK
cred = credentials.Certificate("C:/Users/UyRalph/Desktop/3rd 2nd sem/MOR/ecovision-3b0ea-firebase-adminsdk-fbsvc-60cad329d5.json")
firebase_admin.initialize_app(cred)

# Connect to Firestore
db = firestore.client()

# Load YOLO model
model = YOLO("yolov10n.pt")

# Define biodegradable and non-biodegradable categories
biodegradable_items = [
    'banana', 'apple', 'broccoli', 'carrot', 'sandwich',
    'orange', 'book', 'pizza', 'donut', 'cake', 'vegetable',
    'fruit', 'hot dog', 'bread', 'meat', 'fish', 'egg'
]

non_biodegradable_items = [
    'bottle', 'wine glass', 'cup', 'fork', 'knife', 'spoon', 'bowl',
    'potted plant', 'bed', 'dining table',
    'toilet', 'tv', 'laptop', 'mouse', 'remote', 'keyboard',
    'cell phone', 'microwave', 'oven', 'toaster', 'sink',
    'refrigerator', 'clock', 'vase', 'scissors',
    'teddy bear', 'hair drier', 'toothbrush'
]

def classify_and_store(detected_items):
    """Classify items and store results in Firestore safely."""
    for item in detected_items:
        label = item['label'].lower()
        trash_type = "Biodegradable" if any(bio_item in label for bio_item in biodegradable_items) else "Non-Biodegradable"
        binary_value = 1 if trash_type == "Biodegradable" else 0

        try:
            db.collection('TrashClassification').add({
                'type': trash_type,
                'label': item['label'],
                'binary': binary_value,
                'timestamp': firestore.SERVER_TIMESTAMP
            })
            print(f"{trash_type}: {item['label']} -> Binary: {binary_value}")
        except Exception as e:
            print(f"Error storing data in Firestore: {e}")

def start_camera():
    """Start the camera feed and classify one non-person object every 3 seconds."""
    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("Error: Unable to access the camera.")
        return

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    cap.set(cv2.CAP_PROP_FPS, 30)

    print("Starting camera... Press 'q' to quit.")
    last_capture_time = time.time()

    detected_items = []  

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Unable to read frame.")
            break

        current_time = time.time()
        if current_time - last_capture_time >= 3:
            last_capture_time = current_time

            detected_items = []  

            results = model(frame)

            if not results or len(results[0].boxes) == 0:
                print("No objects detected.")
                continue  

            found = False

            for detection in results:
                for box in detection.boxes:
                    x1, y1, x2, y2 = map(int, box.xyxy[0])
                    cls = box.cls[0]
                    label = model.names[int(cls)]

                    if label.lower() == "person":
                        continue  

                    detected_items.append({
                        'bbox': (x1, y1, x2, y2),
                        'label': label,
                    })
                    found = True
                    break  # Only process one valid object
                if found:
                    break

            if detected_items:
                classify_and_store([detected_items[0]])  # Store only one item

        if detected_items:  # ✅ Now it's always initialized
            item = detected_items[0]
            x1, y1, x2, y2 = item['bbox']
            label_text = "Biodegradable" if item['label'].lower() in biodegradable_items else "Non-Biodegradable"
            color = (0, 255, 0) if label_text == "Biodegradable" else (0, 0, 255)
            cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
            cv2.putText(frame, label_text, (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)

        cv2.imshow("Object Classification", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
    print("Camera stopped.")

def fetch_recent_data_firestore():
    """Fetch classification data in order of most recent from Firestore safely."""
    try:
        trash_ref = db.collection('TrashClassification')
        query = trash_ref.order_by('timestamp', direction=firestore.Query.DESCENDING)
        results = query.stream()

        print("Recent Trash Classification Data:")
        for doc in results:
            print(f"{doc.id}: {doc.to_dict()}")
    except Exception as e:
        print(f"Error fetching data from Firestore: {e}")

if __name__ == "__main__":
    start_camera()
    fetch_recent_data_firestore()
