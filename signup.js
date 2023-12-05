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

let admin, numberOfChildrenAsInt;
let timersCount = 0;

readdata();
document.querySelector("#create").onclick = function () {

    let username, password;
  
    username = document.querySelector("#username").value;
    password = document.querySelector("#password").value;
    console.log(username);
    console.log(password);
      admin = `accounts/admin/admin${timersCount}`;
      timersCount++;
      firebase.database().ref(admin).set({
        username: username,
        password: password
      });
      readdata();
      alert('Account Created Succesfully');
      window.location.href = './index.html';
  };


  function readdata(){
  let database = firebase.database();
  let dataRef = database.ref('/accounts/admin');

  dataRef.once('value', function(snapshot) {
    let data = snapshot.val();

    if (data !== null && data !== undefined) {
      
      numberOfChildrenAsInt = Object.keys(data).length; // Get the number of children

      timersCount = parseInt(numberOfChildrenAsInt, 10); // Now you can use numberOfChildrenAsInt as an integer

      console.log("Current Child Node "+ timersCount);

      
    } else {
      console.log("Data is empty");
      timersCount = 0;
    }
 
  }, function(error) {
    console.error("Error reading data: " + error.code);
  });

  }