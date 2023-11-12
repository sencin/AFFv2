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

readDataFromFirebase();

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
    
  alert("Data Inserted Onii-chan");
  readDataFromFirebase();
};


document.querySelector("#feedbtn").onclick = function () {
    feednow = document.querySelector("#feedbtn").value;
    

  firebase.database().ref('feednow').set({
      feednow : 1  
    });
    
  alert("Yametee oniichan");
};


function TimerData(timerKey, schedule, spin) {
  this.timerKey = timerKey;
  this.schedule = schedule;
  this.spin = spin;
}


function readDataFromFirebase() {
  var database = firebase.database();
  var dataRef = database.ref('AFFV2/timers');

  dataRef.on('value', function(snapshot) {
    var data = snapshot.val();


    displayDataInHtml(data);
  }, function(error) {
    console.error("Error reading data: " + error.code);
  });
}


function displayDataInHtml(data) {
  var dataDisplayDiv = document.getElementById('dataDisplay');

  if (dataDisplayDiv && data) {
   
    dataDisplayDiv.innerHTML = "";

  
    for (var timerKey in data) {
      if (data.hasOwnProperty(timerKey)) {
        var timerData = data[timerKey];


        var timerInstance = new TimerData(timerKey, timerData.schedule, timerData.spin);

   
        var htmlContent = "<div class='timer'>";
        htmlContent += "<p>Timer: " + timerInstance.timerKey + "</p>";
        htmlContent += "<p>Schedule: " + timerInstance.schedule + "</p>";

   
        if (timerInstance.spin) {
    
          htmlContent += "<p>Spin Property: " + timerInstance.spin + "</p>";
        }

        htmlContent += "</div>";

    
        dataDisplayDiv.innerHTML += htmlContent;
      }
    }
  }
}

