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

    // Call a function to update the HTML div with the data
    displayDataInHtml(data);
  }, function(error) {
    console.error("Error reading data: " + error.code);
  });
}

// Function to display data in an HTML div
// function displayDataInHtml(data) {
//   var dataDisplayDiv = document.getElementById('dataDisplay');

//   if (dataDisplayDiv && data) {
//     // Clear previous content
//     dataDisplayDiv.innerHTML = "";

//     // Loop through each child node
//     for (var timerKey in data) {
//       if (data.hasOwnProperty(timerKey)) {
//         var timerData = data[timerKey];

//         // Create an instance of TimerData using the constructor
//         var timerInstance = new TimerData(timerKey, timerData.schedule, timerData.spin);

//         // Create a new div element
//         var timerDiv = document.createElement('div');
//         timerDiv.classList.add('timer');

//         // Create HTML content using the instance
//         var htmlContent = "<p>Timer: " + timerInstance.timerKey + "</p>";
//         htmlContent += "<p>Schedule: " + timerInstance.schedule + "</p>";

//         // Check if "spin" is present before displaying
//         if (timerInstance.spin) {
//           // Include spin-related content
//           // For example, assuming spin has a property named "someProperty"
//           htmlContent += "<p>Spin Property: " + timerInstance.spin + "</p>";
//         }

//         // Add event listener for the click event
//         timerDiv.addEventListener('click', function() {
//           // Prompt the user for confirmation
//           var userResponse = confirm("Do you want to delete or go back?");
          
//           if (userResponse) {
//             // If user chooses to delete, remove the corresponding data from Firebase
//             deleteDataFromFirebase(timerInstance.timerKey);
//           } else {
//             // If user chooses to go back, you can handle this as needed
//             // For example, navigate to a different page or perform another action
//           }
//         });

//         // Set the inner HTML of the div
//         timerDiv.innerHTML = htmlContent;

//         // Append the div to the parent container
//         dataDisplayDiv.appendChild(timerDiv);
//       }
//     }
//   }
// }




// // Function to delete data from Realtime Database
// function deleteDataFromFirebase(timerKey) {
//   var database = firebase.database();
//   var dataRef = database.ref('AFFV2/timers/' + timerKey);

//   // Remove the data from Firebase
//   dataRef.remove()
//     .then(function() {
//       console.log("Data removed successfully!");
//     })
//     .catch(function(error) {
//       console.error("Error removing data: " + error.message);
//     });
// }



function deleteDataFromFirebase(timerKey) {
  var database = firebase.database();
  var dataRef = database.ref('AFFV2/timers/' + timerKey);


  dataRef.once('value')
    .then(function(snapshot) {
      if (snapshot.exists()) {
 
        dataRef.remove()
          .then(function() {
            console.log("Data removed successfully!");
          })
          .catch(function(error) {
            console.error("Error removing data: " + error.message);
          });
      } else {
        // Data does not exist
        document.querySelector("#dataDisplay").innerHTML = "---------------";
        console.log("No data found for deletion");
        alert("No Data Please add some in the Schedule");
      }
    })
    .catch(function(error) {
      console.error("Error checking data existence: " + error.message);
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


function createDeleteHandler(timerKey) {
  return function() {

    var userResponse = confirm("Do you want to delete or go back?");
    
    if (userResponse) {
 
      deleteDataFromFirebase(timerKey);
    } else {

    }
  };
}


