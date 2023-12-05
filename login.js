
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
  
  

document.addEventListener('DOMContentLoaded', function () {
            // Get the modal and buttons
            var modal = document.getElementById('myModal');
            var openModalBtn = document.getElementById('openModalBtn');

            var yesBtn = document.getElementById('yesBtn');
            var noBtn = document.getElementById('noBtn');

            // Open the modal
            openModalBtn.onclick = function () {
                modal.style.display = 'block';
            };

        

            // Close the modal if the overlay is clicked
            window.onclick = function (event) {
                if (event.target === modal) {
                    modal.style.display = 'none';
                }
            };

            // Handle Yes and No buttons
            yesBtn.onclick = function () {

                let usertoken = document.querySelector("#authtoken").value;

                let authdata = firebase.database().ref('/authtoken');
  
                authdata.on('value', function(snapshot) 
                {
                 var authvalue = snapshot.val();
                 console.log(authvalue);
     // Handle the data
                 if(usertoken === authvalue)
                 {
                   window.location.href = './signup.html';
                 }
                   else{
                     alert("INVALID AUTH");
                       }
   
                });

                // Add your logic for "Yes" button click
                console.log('Creating an account!');
                
            };

            noBtn.onclick = function () {
                // Add your logic for "No" button click
                console.log('Not creating an account.');
                modal.style.display = 'none'; // Close the modal
            };
        });



        function checkCredentials() {
            // Reference to the /accounts/admin path in the database
            
          }


          document.querySelector("#loginbtn").onclick = function () {
  
            var accountsRef = firebase.database().ref('/accounts/admin');
        
            // User input (replace these with user-entered values)
            var username = document.querySelector("#username").value;
            var password = document.querySelector("#password").value;
        
            // Loop through each child in /accounts/admin and check credentials
            accountsRef.once('value')
              .then(function(snapshot) {
                snapshot.forEach(function(childSnapshot) {
                  var data = childSnapshot.val();
                  if (data && data.username === username && data.password === password) {
                    console.log('Credentials match!');
                    alert('Credentials match!');
                    window.location.href = './main.htm';

                  } else {
                    console.log('Credentials do not match.');
                  }
                });
              })
              .catch(function(error) {
                console.error('Error reading from database:', error);
              });
        
        };


