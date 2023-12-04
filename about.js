window.addEventListener('scroll', function () {
    const buttonContainer = document.querySelector('.button-container');
    const scrollY = window.scrollY;

    if (scrollY > 100) { // Adjust the scroll threshold as needed
        buttonContainer.style.bottom = '0';
    } else {
        buttonContainer.style.bottom = '-60px'; // Hide the button again
    }
});