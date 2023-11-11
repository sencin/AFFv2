const firebaseConfig = {
  apiKey: "AIzaSyC0WToORO9UG83T1MlsudbVripZ7FdGli0",
  authDomain: "frenzy-ecfc7.firebaseapp.com",
  databaseURL: "https://frenzy-ecfc7-default-rtdb.firebaseio.com",
  projectId: "frenzy-ecfc7",
  storageBucket: "frenzy-ecfc7.appspot.com",
  messagingSenderId: "783829962407",
  appId: "1:783829962407:web:35b6506ef000dd682fd582"
};

document.getElementById('inputsave').addEventListener('click', () => {
    // Create operation
    const inputTime = document.getElementById('inputtime').value;
    const inputSpin = document.getElementById('inputspin').value;

    // Push new data to the database
    const newScheduleRef = database.ref('schedules').push();
    newScheduleRef.set({
        time: inputTime,
        spin: inputSpin
    });
});