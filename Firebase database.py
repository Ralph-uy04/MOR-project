from flask import Flask, jsonify
import firebase_admin
from firebase_admin import credentials, firestore

app = Flask(__name__)

# Initialize Firebase Admin SDK
cred = credentials.Certificate("C:/Users/UyRalph/Desktop/3rd 2nd sem/MOR/ecovision-new-firebase-adminsdk-fbsvc-5750de4f08.json")
firebase_admin.initialize_app(cred)

# Connect to Firestore
db = firestore.client()

@app.route('/get-latest-binary', methods=['GET'])
def get_latest_binary():
    docs = db.collection('TrashClassification').order_by('timestamp', direction=firestore.Query.DESCENDING).limit(1).stream()
    for doc in docs:
        data = doc.to_dict()
        binary_value = data.get('binary', -1)  
        return jsonify({'binary': binary_value})

    return jsonify({'error': 'No data found'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
