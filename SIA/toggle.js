function toggleDiv(divId) {
    // Hide all divs
    const divs = document.querySelectorAll('.toggle');
    divs.forEach(div => {
      div.style.display = 'none';
    });

    // Show the selected div
    const selectedDiv = document.getElementById(divId);
    if (selectedDiv) {
      selectedDiv.style.display = 'block';
    }
  }

  function backbutton() {
    // Hide all divs
    const divs = document.querySelectorAll('.toggle');
    divs.forEach(div => {
      div.style.display = 'none';
    });
  }


