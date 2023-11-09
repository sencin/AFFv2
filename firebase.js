
let inputTime = document.querySelector("#inputtime");
let inputSpin = document.querySelector("#inputspin");




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

var countRef = firebase.database().ref('count');
countRef.on('value', function(snapshot) {
    count = snapshot.val()
    console.log(count)
});

function feednow() {
    firebase.database().ref().update({
        feednow: 1
    });
}

function schedulefeed(){
    
}