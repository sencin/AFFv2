const firebaseConfig = {
    apiKey: "AIzaSyC0WToORO9UG83T1MlsudbVripZ7FdGli0",
    authDomain: "frenzy-ecfc7.firebaseapp.com",
    databaseURL: "https://frenzy-ecfc7-default-rtdb.firebaseio.com",
    projectId: "frenzy-ecfc7",
    storageBucket: "frenzy-ecfc7.appspot.com",
    messagingSenderId: "783829962407",
    appId: "1:783829962407:web:35b6506ef000dd682fd582"
  };

  firebase.initializeApp(firebaseConfig);
  
let scheduletime, spintimes,timerPath, feednow;

let timersCount = 0;

 document.querySelector("#inputsave").onclick = function () {
    scheduletime = document.querySelector("#inputtime").value;
    spintimes = document.querySelector("#inputspin").value;
     timersCount++;
     timerPath = `AFFV2/timers/timer${timersCount}`;
  
  firebase.database().ref(timerPath).set({
      schedule : scheduletime,
      spin : spintimes
    });
    
  alert("Data Inserted");
};


document.querySelector("#feedbtn").onclick = function () {
    feednow = document.querySelector("#feedbtn").value;
    

  firebase.database().ref('feednow').set({
      feednow : 1  
    });
    
  alert("Data Inserted");
};