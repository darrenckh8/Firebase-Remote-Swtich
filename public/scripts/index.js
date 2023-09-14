document.getElementById('darkmode-toggle').addEventListener('click', function() {
  const darkmodeLink = document.getElementById('darkmode-link');
  if (darkmodeLink.disabled) {
    darkmodeLink.disabled = false;
  } else {
    darkmodeLink.disabled = true;
  }
});

// Create cards for both boards
boardNames.forEach((boardName) => {
  ...
  cardContainer.appendChild(card);
  // Add animation
  setTimeout(function() {
    card.classList.add('show');
  }, 100);
});