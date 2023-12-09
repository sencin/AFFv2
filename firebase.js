
// THIS IS THE CONFIG OF OUR FIREBASE REAL TIME DATABASE

const firebaseConfig = {
    apiKey: "AIzaSyC0WToORO9UG83T1MlsudbVripZ7FdGli0",
    authDomain: "frenzy-ecfc7.firebaseapp.com",
    databaseURL: "https://frenzy-ecfc7-default-rtdb.firebaseio.com",
    projectId: "frenzy-ecfc7",
    storageBucket: "frenzy-ecfc7.appspot.com",
    messagingSenderId: "783829962407",
    appId: "1:783829962407:web:35b6506ef000dd682fd582"
  };

// INITIALIZE FIREBASE
firebase.initializeApp(firebaseConfig);
  
let timerPath, numberOfChildrenAsInt;
let timersCount = 0;

readDataFromFirebase();

 
// SCHEDULE FEEDING
document.querySelector("#inputsave").onclick = function () {

  let scheduletime, spintimes;

  scheduletime = document.querySelector("#inputtime").value;
  spintimes = document.querySelector("#inputspin").value;


//
  var inputTime = document.getElementById("inputtime").value;

  // Split the input time into hours and minutes
  var timeParts = inputTime.split(":");
  var hours = parseInt(timeParts[0]);
  var minutes = timeParts[1];

  // Convert to 12-hour format
  var period = (hours >= 12) ? "PM" : "AM";
  hours = (hours % 12) || 12; // Handle midnight (00:00) as 12 AM

  // Format the result
  var convertedTime = hours + ":" + minutes + " " + period;
//
 
  // Check if either scheduletime or spintimes is empty
  if (scheduletime.trim() === "" || spintimes.trim() === "") {
    alert("Please enter both a valid schedule time and spin value (spin value should be 1-10).");
    // You may choose to handle this case in a way that fits your application
    return; // Stop further execution if one of the fields is empty
  }

  // Check if spintimes is a valid non-negative number
  if (Number.isInteger(Number(spintimes))&& spintimes >= 1 && spintimes<=10) {
    timerPath = `AFFV2/timers/timer${timersCount}`;
    timersCount++;
    firebase.database().ref(timerPath).set({
      schedule: scheduletime,
      spin: parseInt(spintimes),
      convertedtime : convertedTime
    });

    readDataFromFirebase();
    alert("Data Inserted");
  } else {
    alert("Please enter a valid spin value (1 to 10).");
    // You may choose to handle this case in a way that fits your application
  }
};








// MANUAL FEEDING FUNCTION
document.querySelector("#feedbtn").onclick = function () {
  
    let userResponse = confirm("Are you Sure to Feed The Fish?");
    
    if (userResponse) {
      
      firebase.database().ref().update({
        feednow: 1 
    });
    alert("Thank You for The Food");
    } else {

    }

};


// READ DATA FROM FIREBASE
// Function to read data from Realtime Database
function readDataFromFirebase() {
  let database = firebase.database();
  let dataRef = database.ref('AFFV2/timers');

  dataRef.once('value', function(snapshot) {
    let data = snapshot.val();

    if (data !== null && data !== undefined) {
      
      numberOfChildrenAsInt = Object.keys(data).length; // Get the number of children

      timersCount = parseInt(numberOfChildrenAsInt, 10); // Now you can use numberOfChildrenAsInt as an integer

      console.log("Current Child Node "+ timersCount);
      firebase.database().ref().update({
        node: timersCount
    });

    
    displayDataInHtml(data); // Call a function to update the HTML div with the data
      
    } else {
      console.log("Data is empty");
      timersCount = 0;
      firebase.database().ref().update({
        node: timersCount
    });
     
    }
 
  }, function(error) {
    console.error("Error reading data: " + error.code);
  });




  var distancedata = firebase.database().ref('/distance');

  distancedata.on('value', function(snapshot) {
   var data = snapshot.val();
   // Handle the data
   var formatteddistance = "Remaining : " + data + ""; // For example, you can add some text to the data

    // Update the <p> element
    document.getElementById('remainingfood').innerText = formatteddistance;
});


var recentfeeddata = firebase.database().ref('/recentfeed');

recentfeeddata.on('value', function(snapshot) {
   var recentfeedvalue = snapshot.val();
   // Handle the data
   var formattedfeedvalue = "Recent: " + recentfeedvalue + ""; // For example, you can add some text to the data

    // Update the <p> element
    document.getElementById('recentfeed').innerText = formattedfeedvalue;
});



var batterypercent = firebase.database().ref('/battery');

batterypercent.on('value', function(snapshot) {
   var batterypercent = snapshot.val();
   // Handle the data
   var batterypercentformat = "Battery: " + batterypercent + ""; // For example, you can add some text to the data

    // Update the <p> element
    document.getElementById('batper').innerText = batterypercentformat;
});
}


// DELETE FUNCTION
function deleteDataFromFirebase() {
  let database = firebase.database();
  let dataRef = database.ref('AFFV2/timers');

  // Use limitToLast(1) to get the last item
  dataRef.orderByKey().limitToLast(1).once('value')
    .then(function(snapshot) {
      snapshot.forEach(function(childSnapshot) {
        let lastTimerKey = childSnapshot.key;

        let lastTimerRef = database.ref('AFFV2/timers/' + lastTimerKey);

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
                console.log("Remaining Child Node "+ timersCount);
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
      readDataFromFirebase();
    })
    .catch(function(error) {
      console.error("Error getting last data: " + error.message);
    });
}


//PROMPT BEFORE DELETING
function createDeleteHandler() {
  return function() {

    let userResponse = confirm("This will delete the last data entered. Proceed?");
    
    if (userResponse) {
 
      deleteDataFromFirebase();
    } else {

    }
  };
}


//CLEAR ALL FUNCTION
function clearAllTimers() {
  let database = firebase.database();
  let timersRef = database.ref('AFFV2/timers');

  timersRef.remove()
    .then(function() {
      console.log("All timers cleared successfully!");
      document.querySelector("#dataDisplay").innerHTML = "--------NULL---------";
      timersCount = 0;
      readDataFromFirebase();
    })
    .catch(function(error) {
      console.error("Error clearing timers: " + error.message);
    });
    

}


// CLEAR ALL PROMPT
// Add an event listener to a button or trigger for clearing all timers
document.querySelector("#clear").onclick = function () {
  let userResponse = confirm("Are you sure you want to clear all timers?");
  
  if (userResponse) {
    
    clearAllTimers();
  } else {
    // User canceled the operation
  }
};



// USING CONSTRUCTOR TO CREATE NEW DIV
// Define a constructor for your data nodes
function TimerData(timerKey, convertedtime, spin) {
  this.timerKey = timerKey;
  this.convertedtime = convertedtime;
  this.spin = spin;
}

// ADD DATA TO MY SCHEDULE DYNAMICALLY
function displayDataInHtml(data) {
  let dataDisplayDiv = document.getElementById('dataDisplay');

  if (dataDisplayDiv && data) {

    dataDisplayDiv.innerHTML = "";

  
    for (let timerKey in data) {
      if (data.hasOwnProperty(timerKey)) {
        let timerData = data[timerKey];

   
        let timerInstance = new TimerData(timerKey, timerData.convertedtime, timerData.spin);

     
        let timerDiv = document.createElement('div');
        timerDiv.classList.add('timer');

 
        let htmlContent = "<p>Timer: " + timerInstance.timerKey + "</p>";
        htmlContent += "<p>Schedule: " + timerInstance.convertedtime + "</p>";

        if (timerInstance.spin) {
      
          htmlContent += "<p>Spin : " + timerInstance.spin + "</p>";
        }

  
        timerDiv.addEventListener('click', createDeleteHandler(timerInstance.timerKey));


        timerDiv.innerHTML = htmlContent;

        dataDisplayDiv.appendChild(timerDiv);
      }
    }
  }
}


