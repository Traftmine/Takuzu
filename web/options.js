const submitButton = document.getElementById('submit');

submitButton.addEventListener('click', (event) => {
  event.preventDefault();

  const rowsInput = document.getElementById('rows');
  const colsInput = document.getElementById('cols');
  const uniqueInput = document.getElementById('unique');
  const wrappingInput = document.getElementById('wrapping');
  const rows = rowsInput.value;
  const cols = colsInput.value;
  const unique = rowsInput.value;
  const wrapping = colsInput.value;

  localStorage.setItem('rows', rows);
  localStorage.setItem('cols', cols);
  localStorage.setItem('unique', unique);
  localStorage.setItem('wrapping', wrapping);

  window.location.href = 'main.html';
});

const backButton = document.getElementById('back-to-main');

backButton.addEventListener('click', () => {
  window.location.href = 'main.html';
});
