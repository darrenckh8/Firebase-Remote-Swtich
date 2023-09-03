const loginElement = document.querySelector("#login-form");
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector("#user-details");
const authBarElement = document.querySelector("#authentication-bar");

// Create an object to store GPIO pins, states, and buttons for each board
const boards = {};

// Function to set up GPIO pins for a specific board
const setupBoard = (boardName) => {
  boards[boardName] = {};

  // Elements for GPIO states
  boards[boardName].stateElements = {}; // Store state elements in an object

  // Button Elements
  boards[boardName].btnOn = {};
  boards[boardName].btnOff = {};

  // Database references for GPIO pins
  boards[boardName].dbPathOutputs = {}; // Store database paths in an object
  boards[boardName].dbRefOutputs = {}; // Store database references in an object

  for (let gpioPin = 2; gpioPin <= 33; gpioPin++) {
    // Create state element
    boards[boardName].stateElements[gpioPin] = document.getElementById(
      `state${boardName}-${gpioPin}`
    );

    // Create button elements
    boards[boardName].btnOn[gpioPin] = document.getElementById(
      `btn${boardName}-${gpioPin}On`
    );
    boards[boardName].btnOff[gpioPin] = document.getElementById(
      `btn${boardName}-${gpioPin}Off`
    );

    // Define database path for each GPIO pin
    boards[boardName].dbPathOutputs[
      gpioPin
    ] = `${boardName}/outputs/digital/${gpioPin}`;
    boards[boardName].dbRefOutputs[gpioPin] = firebase
      .database()
      .ref()
      .child(boards[boardName].dbPathOutputs[gpioPin]);

    // Update states depending on the database value
    boards[boardName].dbRefOutputs[gpioPin].on("value", (snap) => {
      if (snap.val() == 1) {
        boards[boardName].stateElements[gpioPin].innerText = "ON";
      } else {
        boards[boardName].stateElements[gpioPin].innerText = "OFF";
      }
    });

    // Update database upon button click
    boards[boardName].btnOn[gpioPin].onclick = () => {
      boards[boardName].dbRefOutputs[gpioPin].set(1);
    };

    boards[boardName].btnOff[gpioPin].onclick = () => {
      boards[boardName].dbRefOutputs[gpioPin].set(0);
    };
  }
};

// Initialize GPIO pins for each board
boardNames.forEach((boardName) => {
  setupBoard(boardName);
});

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    // Toggle UI elements
    loginElement.style.display = "none";
    contentElement.style.display = "block";
    authBarElement.style.display = "block";
    userDetailsElement.style.display = "block";
    userDetailsElement.innerHTML = user.email;
  } else {
    // Toggle UI elements
    loginElement.style.display = "block";
    authBarElement.style.display = "none";
    userDetailsElement.style.display = "none";
    contentElement.style.display = "none";
  }
};
