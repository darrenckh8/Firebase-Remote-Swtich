const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");

// Elements for GPIO states
const stateElements = {}; // Store state elements in an object

// Button Elements
const btnOn = {};
const btnOff = {};

// Database references for GPIO pins
const dbPathOutputs = {}; // Store database paths in an object
const dbRefOutputs = {}; // Store database references in an object

// Initialize GPIO pins dynamically
for (let gpioPin = 2; gpioPin <= 33; gpioPin++) {
  // Create state element
  stateElements[gpioPin] = document.getElementById(`state${gpioPin}`);

  // Create button elements
  btnOn[gpioPin] = document.getElementById(`btn${gpioPin}On`);
  btnOff[gpioPin] = document.getElementById(`btn${gpioPin}Off`);

  // Define database path for each GPIO pin
  dbPathOutputs[gpioPin] = `board1/outputs/digital/${gpioPin}`;
  dbRefOutputs[gpioPin] = firebase.database().ref().child(dbPathOutputs[gpioPin]);

  // Update states depending on the database value
  dbRefOutputs[gpioPin].on('value', snap => {
    if (snap.val() == 1) {
      stateElements[gpioPin].innerText = "ON";
    } else {
      stateElements[gpioPin].innerText = "OFF";
    }
  });

  // Update database upon button click
  btnOn[gpioPin].onclick = () => {
    dbRefOutputs[gpioPin].set(1);
  };

  btnOff[gpioPin].onclick = () => {
    dbRefOutputs[gpioPin].set(0);
  };
}

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    // Toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;
  } else {
    // Toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
}
