import { setupUI } from './index.js';

const boardNames = ["board1", "board2"]; // Add more board names as needed
let selectedBoard = boardNames[0]; // Initially select the first board

// Define cardContainer at the top of the script
const cardContainer = document.getElementById("card-grid");

// Function to toggle card visibility based on selected board
function toggleCards() {
  const cards = document.querySelectorAll(".card");
  cards.forEach((card) => {
    const [board, gpioPin] = card.dataset.board.split("-");
    card.style.display = board === selectedBoard ? "block" : "none";
  });
}

// Add event listeners to board selection buttons
boardNames.forEach((boardName) => {
  document
    .getElementById(`board${boardName[5]}Button`)
    .addEventListener("click", () => {
      selectedBoard = boardName;
      toggleCards();
    });
});

// Create cards for both boards
boardNames.forEach((boardName) => {
  for (let gpioPin = 2; gpioPin <= 33; gpioPin++) {
    if (
      gpioPin === 2 ||
      gpioPin === 4 ||
      gpioPin === 5 ||
      (gpioPin >= 12 && gpioPin <= 33)
    ) {
      const card = document.createElement("div");
      card.classList.add("card");
      card.dataset.board = `${boardName}-${gpioPin}`;
      card.innerHTML = `
        <p class="card-title"><i class="fas fa-lightbulb"></i> GPIO ${gpioPin} on ${boardName}</p>
        <p>
          <button class="button-on" id="btn${boardName}-${gpioPin}On">ON</button>
          <button class="button-off" id="btn${boardName}-${gpioPin}Off">OFF</button>
        </p>
        <p class="state">State:<span id="state${boardName}-${gpioPin}"></span></p>
      `;
      cardContainer.appendChild(card);
    }
  }
});

// Initially display cards for the selected board
toggleCards();

// Export setupUI function
export { setupUI };