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
  
let scheduletime, spintimes,timerPath, feednow, numberOfChildrenAsInt;

readDataFromFirebase();

let timersCount = 0;

 document.querySelector("#inputsave").onclick = function () {
    scheduletime = document.querySelector("#inputtime").value;
    spintimes = document.querySelector("#inputspin").value;
    
     timerPath = `AFFV2/timers/timer${timersCount}`;
     timersCount++;
  firebase.database().ref(timerPath).set({
      schedule : scheduletime,
      spin : spintimes
    });
    
  alert("Data Inserted");
  readDataFromFirebase();
};


document.querySelector("#feedbtn").onclick = function () {
    feednow = document.querySelector("#feedbtn").value;
    

    firebase.database().ref().update({
      feednow: 1
      
  });
    
  alert("Thank Youu for The Food Hooman");
};


// Define a constructor for your data nodes
function TimerData(timerKey, schedule, spin) {
  this.timerKey = timerKey;
  this.schedule = schedule;
  this.spin = spin;
}

// Function to read data from Realtime Database
function readDataFromFirebase() {
  var database = firebase.database();
  var dataRef = database.ref('AFFV2/timers');

  dataRef.on('value', function(snapshot) {
    var data = snapshot.val();

    if (data !== null && data !== undefined) {
      // Get the number of children
      numberOfChildrenAsInt = Object.keys(data).length;

      timersCount = parseInt(numberOfChildrenAsInt, 10);

      console.log(timersCount);
    // Call a function to update the HTML div with the data
    displayDataInHtml(data);
      // Now you can use numberOfChildrenAsInt as an integer
    } else {
      console.log("Data is null or undefined");
      timersCount = 0;
      console.log(timersCount);
    }
 
  }, function(error) {
    console.error("Error reading data: " + error.code);
  });
}



function deleteDataFromFirebase() {
  var database = firebase.database();
  var dataRef = database.ref('AFFV2/timers');

  // Use limitToLast(1) to get the last item
  dataRef.orderByKey().limitToLast(1).once('value')
    .then(function(snapshot) {
      snapshot.forEach(function(childSnapshot) {
        var lastTimerKey = childSnapshot.key;

        var lastTimerRef = database.ref('AFFV2/timers/' + lastTimerKey);

        lastTimerRef.remove()
          .then(function() {
            console.log("Last data removed successfully!");

            const pathReference = database.ref("/AFFV2");
              // Use the once method to check if the path exists
            pathReference.once('value')
  .           then((snapshot) => {
              if (snapshot.exists()) 
              {
                console.log("Path exists");
                console.log("After Deleted"+ timersCount);
              } else 
              {
                document.querySelector("#dataDisplay").innerHTML = "--------NULL---------";
                  console.log("Path does not exist");
                  timersCount = 0;
              }
             })
          })
          .catch(function(error) {
            console.error("Error removing last data: " + error.message);
          });
      });
    })
    .catch(function(error) {
      console.error("Error getting last data: " + error.message);
    });
}

function createDeleteHandler() {
  return function() {

    var userResponse = confirm("This will delete the last data entered. Proceed?");
    
    if (userResponse) {
 
      deleteDataFromFirebase();
    } else {

    }
  };
}


function clearAllTimers() {
  let database = firebase.database();
  let timersRef = database.ref('AFFV2/timers');

  timersRef.remove()
    .then(function() {
      console.log("All timers cleared successfully!");
      document.querySelector("#dataDisplay").innerHTML = "--------NULL---------";
      timersCount = 0;
    })
    .catch(function(error) {
      console.error("Error clearing timers: " + error.message);
    });
}


// Add an event listener to a button or trigger for clearing all timers
document.querySelector("#clear").onclick = function () {
  var userResponse = confirm("Are you sure you want to clear all timers?");
  
  if (userResponse) {
    clearAllTimers();
  } else {
    // User canceled the operation
  }
};


function displayDataInHtml(data) {
  var dataDisplayDiv = document.getElementById('dataDisplay');

  if (dataDisplayDiv && data) {

    dataDisplayDiv.innerHTML = "";

  
    for (var timerKey in data) {
      if (data.hasOwnProperty(timerKey)) {
        var timerData = data[timerKey];

   
        var timerInstance = new TimerData(timerKey, timerData.schedule, timerData.spin);

     
        var timerDiv = document.createElement('div');
        timerDiv.classList.add('timer');

 
        var htmlContent = "<p>Timer: " + timerInstance.timerKey + "</p>";
        htmlContent += "<p>Schedule: " + timerInstance.schedule + "</p>";

        if (timerInstance.spin) {
      
          htmlContent += "<p>Spin Property: " + timerInstance.spin + "</p>";
        }

  
        timerDiv.addEventListener('click', createDeleteHandler(timerInstance.timerKey));


        timerDiv.innerHTML = htmlContent;

        dataDisplayDiv.appendChild(timerDiv);
      }
    }
  }
}
