/*<script type="module">
        // Import the functions you need from the SDKs you need
        import { initializeApp } from "https://www.gstatic.com/firebasejs/10.5.2/firebase-app.js";
        // TODO: Add SDKs for Firebase products that you want to use
        // https://firebase.google.com/docs/web/setup#available-libraries
      
        // Your web app's Firebase configuration
        const firebaseConfig = {
          apiKey: "AIzaSyC0WToORO9UG83T1MlsudbVripZ7FdGli0",
          authDomain: "frenzy-ecfc7.firebaseapp.com",
          databaseURL: "https://frenzy-ecfc7-default-rtdb.firebaseio.com",
          projectId: "frenzy-ecfc7",
          storageBucket: "frenzy-ecfc7.appspot.com",
          messagingSenderId: "783829962407",
          appId: "1:783829962407:web:35b6506ef000dd682fd582"
        };
      
        // Initialize Firebase
        const app = initializeApp(firebaseConfig);
        import{getDatabase, ref, get, set, update, remove} from "https://www.gstatic.com/firebasejs/10.5.2/firebase-database.js";
        
        const db = getDatabase();

        let inputTime = document.querySelector("#inputtime");
        let inputSpin = document.querySelector("#inputspin");
        let save = document.querySelector('#inputsave');
        let feed = document.querySelector('#feedbtn');

        function addData(){
            set(ref(db, 'timers/timer'),{
                scheduletime: inputTime.value,
                spintimes: inputSpin.value
            })
            .then(()=>{
                alert("Added Successfully");
            })
            .catch((error)=>{
                alert("Unsuccessful");
                console.log(error);
            })
        }

        save.addEventListener('click',addData);
      </script>*/