const grid = document.querySelector('.grid');
const optionsButton = document.getElementById('options');

optionsButton.addEventListener('click', () => {
  window.location.href = 'options.html';
});

function updateGrid() {
    const rows = localStorage.getItem('rows') || 6;
    const cols = localStorage.getItem('cols') || 6;
    const unique = localStorage.getItem('unique') || false;
    const wrapping = localStorage.getItem('wrapping') || false;

    grid.style.setProperty('--rows', rows);
    grid.style.setProperty('--cols', cols);
    // set unique and wrapping

    grid.innerHTML = '';
    for (let i = 0; i < rows * cols; i++) {
        const cell = document.createElement('div');
        cell.classList.add('cell');
        cell.textContent = ''; // Modification pour afficher le numéro
        cell.addEventListener('click', () => {
        if (cell.textContent === '') {
            cell.classList.remove('active');
            cell.textContent = '1'; // Modification pour afficher le numéro
        } else if (cell.textContent === '1') {
            cell.classList.add('active');
            cell.textContent = '0'; // Modification pour afficher le numéro
        } else {
            cell.textContent = ''; // Modification pour afficher le numéro
        }
        });
        grid.appendChild(cell);
    }
}

updateGrid();
